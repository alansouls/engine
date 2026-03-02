using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using SSGEDotNet.AssemblyLoader.Interop;
using SSGEDotNet.AssemblyLoader.Models;

namespace SSGEDotNet.AssemblyLoader;

public static class GameAssemblyLoader
{
    private const int FunctionPtrCount = 5;
    private static IntPtr _entryPointFunctionsPtr;
    private static WeakReference? _loadContextReference;
    private const string CoreAssemblyName = "SSGEDotNet.Core.dll";
    private static GameAssemblyLoadContext? _gameAssemblyLoadContext;
    private static CallComponentDelegate? _callComponentInitDelegate;
    private static CallComponentDelegate? _callComponentUpdateDelegate;
    private static CallComponentDelegate? _callComponentGetPropertyDelegate;
    private static CallComponentDelegate? _callComponentSetPropertyDelegate;
    private static InitializeDelegate? _initializeDelegate;

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static int LoadGameAssembly(IntPtr args, int argLength)
    {
        Console.WriteLine("Loading game assembly...");
        var assemblyPath = Marshal.PtrToStringUTF8(args);

        try
        {
            LoadGameAssembly(assemblyPath);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
            Console.WriteLine(e.StackTrace);
            return -255;
        }

        return 0;
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static void LoadGameAssembly(string? assemblyPath)
    {
        if (string.IsNullOrWhiteSpace(assemblyPath))
        {
            throw new ArgumentException("Assembly path cannot be null or empty.", nameof(assemblyPath));
        }

        var assemblyFullPath = Path.GetFullPath(assemblyPath);

        var assemblyDirectory = Path.GetDirectoryName(assemblyFullPath)!;

        _gameAssemblyLoadContext = new GameAssemblyLoadContext(assemblyDirectory);
        _loadContextReference = new WeakReference(_gameAssemblyLoadContext, trackResurrection: true);

        var coreAssemblyPath = Path.Combine(assemblyDirectory, CoreAssemblyName);

        var coreAssembly = _gameAssemblyLoadContext.LoadFromAssemblyPath(coreAssemblyPath);
        var gameAssembly = _gameAssemblyLoadContext.LoadFromAssemblyPath(assemblyFullPath);

        coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("SetGameAssembly")!
            .Invoke(null, [gameAssembly]);

        Console.WriteLine($"Game assembly loaded successfully: {gameAssembly.FullName}");

        if (_entryPointFunctionsPtr != IntPtr.Zero)
        {
            Marshal.FreeHGlobal(_entryPointFunctionsPtr);
        }

        _entryPointFunctionsPtr = Marshal.AllocHGlobal(IntPtr.Size * FunctionPtrCount);
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static int UnloadGameAssembly(IntPtr args, int argLength)
    {
        Console.WriteLine("Unloading game assembly...");
        return UnloadGameAssembly();
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static int UnloadGameAssembly()
    {
        if (_gameAssemblyLoadContext is not null)
        {
            Marshal.FreeHGlobal(_entryPointFunctionsPtr);
            _entryPointFunctionsPtr = IntPtr.Zero;
            _gameAssemblyLoadContext.Unload();
            _gameAssemblyLoadContext = null;
            _callComponentInitDelegate = null;
            _callComponentUpdateDelegate = null;
            _callComponentGetPropertyDelegate = null;
            _callComponentSetPropertyDelegate = null;
            _initializeDelegate = null;
            var stopwatch = Stopwatch.StartNew();
            for (; _loadContextReference!.IsAlive && stopwatch.Elapsed.TotalSeconds <= 5;)
            {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }

            if (_loadContextReference.IsAlive)
            {
                throw new TimeoutException("Unable to unload game assembly in 5 seconds.");
            }
            else
            {
                Console.WriteLine("Game assembly unloaded successfully.");
            }

            return 0;
        }

        Console.WriteLine("No game assembly to unload.");
        return -1;
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static GameAssemblyInfo GetGameAssemblyInfoUnmanaged(string? assemblyPath)
    {
        return string.IsNullOrWhiteSpace(assemblyPath)
            ? throw new ArgumentException("Assembly path cannot be null or empty.", nameof(assemblyPath))
            : GameAssemblyReader.GetGameAssemblyInfo(assemblyPath, CoreAssemblyName);
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static int GetGameAssemblyInfo(IntPtr args, int argLength)
    {
        IntPtr strPtr = Marshal.ReadIntPtr(args);
        
        var assemblyPath = Marshal.PtrToStringUTF8(strPtr);
        
        var gameInfo =  GetGameAssemblyInfo(assemblyPath);
        
        var gameInfoPtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        
        GameAssemblyInfoInterop.WriteToPtr(gameInfo, gameInfoPtr);

        return 0;
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    public static GameAssemblyInfo GetGameAssemblyInfo(string? assemblyPath)
    {
        return string.IsNullOrWhiteSpace(assemblyPath)
            ? throw new ArgumentException("Assembly path cannot be null or empty.", nameof(assemblyPath))
            : GameAssemblyReader.GetGameAssemblyInfo(assemblyPath, CoreAssemblyName);
    }

    public delegate int CallComponentDelegate(IntPtr args, int argLength);

    public delegate void InitializeDelegate(IntPtr inputStatePtr);

    [UnmanagedCallersOnly]
    public static IntPtr GetCoreEntryPointFunctions()
    {
        if (_gameAssemblyLoadContext is null)
        {
            Console.WriteLine("Game assembly load context is not initialized.");
            return IntPtr.Zero;
        }

        var coreAssembly =
            _gameAssemblyLoadContext.Assemblies.FirstOrDefault(a =>
            {
                Console.WriteLine(a.FullName);
                return a.FullName!.Contains(CoreAssemblyName.Replace(".dll", ""));
            });

        if (coreAssembly is null)
        {
            Console.WriteLine("Core assembly is not loaded.");
            return IntPtr.Zero;
        }

        var callInit = coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("CallComponentInit")!;
        var callUpdate = coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("CallComponentUpdate")!;
        var callGetProperty =
            coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("CallComponentGetProperty")!;
        var callSetProperty =
            coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("CallComponentSetProperty")!;
        var callInitialize = coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("Initialize")!;

        _callComponentInitDelegate = callInit.CreateDelegate<CallComponentDelegate>();
        _callComponentUpdateDelegate = callUpdate.CreateDelegate<CallComponentDelegate>();
        _callComponentGetPropertyDelegate = callGetProperty.CreateDelegate<CallComponentDelegate>();
        _callComponentSetPropertyDelegate = callSetProperty.CreateDelegate<CallComponentDelegate>();
        _initializeDelegate = callInitialize.CreateDelegate<InitializeDelegate>();

        var callInitPtr = Marshal.GetFunctionPointerForDelegate(_callComponentInitDelegate);
        var callUpdatePtr = Marshal.GetFunctionPointerForDelegate(_callComponentUpdateDelegate);
        var callGetPropertyPtr = Marshal.GetFunctionPointerForDelegate(_callComponentGetPropertyDelegate);
        var callSetPropertyPtr = Marshal.GetFunctionPointerForDelegate(_callComponentSetPropertyDelegate);
        var callSetInputStatePtr = Marshal.GetFunctionPointerForDelegate(_initializeDelegate);

        IntPtr[] functionPtrs = [callInitPtr, callUpdatePtr, callGetPropertyPtr, callSetPropertyPtr,  callSetInputStatePtr];
        
        for (var offset = 0; offset < FunctionPtrCount; ++offset)
        {
            Marshal.WriteIntPtr(_entryPointFunctionsPtr, IntPtr.Size * offset, functionPtrs[offset]);   
        }

        return _entryPointFunctionsPtr;
    }
}
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace SSGEDotNet.AssemblyLoader;

public static class GameAssemblyLoader
{
    private static IntPtr _entryPointFunctionsPtr;
    private static WeakReference? _loadContextReference;
    private const string CoreAssemblyName = "SSGEDotNet.Core.dll";
    private static GameAssemblyLoadContext? _gameAssemblyLoadContext;

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

        _entryPointFunctionsPtr = Marshal.AllocHGlobal(IntPtr.Size * 2);
    }

    public static int UnloadGameAssembly(IntPtr args, int argLength)
    {
        Console.WriteLine("Unloading game assembly...");
        return UnloadGameAssembly();
    }

    public static int UnloadGameAssembly()
    {
        if (_gameAssemblyLoadContext is not null)
        {
            Marshal.FreeHGlobal(_entryPointFunctionsPtr);
            _gameAssemblyLoadContext.Unload();
            _gameAssemblyLoadContext = null;
            for (int i = 0; _loadContextReference!.IsAlive && (i < 10); i++)
            {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }

            Console.WriteLine("Game assembly unloaded successfully.");
            return 0;
        }
        else
        {
            Console.WriteLine("No game assembly to unload.");
            return -1;
        }
    }

    public delegate int CallComponentDelegate(IntPtr args, int argLength);
    public delegate void SetInputStateDelegate(IntPtr inputStatePtr);

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
        var callSetInputState = coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("SetInputState")!;

        var callInitPtr = Marshal.GetFunctionPointerForDelegate(callInit.CreateDelegate<CallComponentDelegate>());
        var callUpdatePtr = Marshal.GetFunctionPointerForDelegate(callUpdate.CreateDelegate<CallComponentDelegate>());
        var callSetInputStatePtr = Marshal.GetFunctionPointerForDelegate(callSetInputState.CreateDelegate<SetInputStateDelegate>());

        Marshal.WriteIntPtr(_entryPointFunctionsPtr, 0, callInitPtr);
        Marshal.WriteIntPtr(_entryPointFunctionsPtr, IntPtr.Size, callUpdatePtr);
        Marshal.WriteIntPtr(_entryPointFunctionsPtr, IntPtr.Size * 2, callSetInputStatePtr);

        return _entryPointFunctionsPtr;
    }
}
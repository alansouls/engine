using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace SSGEDotNet.AssemblyLoader;

public static class GameAssemblyLoader
{
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

        // coreAssembly.GetType("SSGEDotNet.Core.Scene.ScriptRunner")!.GetMethod("SetGameAssembly")!
        //     .Invoke(null, [gameAssembly]);

        Console.WriteLine($"Game assembly loaded successfully: {gameAssembly.FullName}");

        gameAssembly.GetType("SSGEDotNet.Sample.Initializer")!.GetMethod("Init")!
            .Invoke(null, null);
    }
    
    public static void UnloadGameAssembly()
    {
        if (_gameAssemblyLoadContext is not null)
        {
            _gameAssemblyLoadContext.Unload();
            _gameAssemblyLoadContext = null;
            while (_loadContextReference?.IsAlive == true)
            {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
            Console.WriteLine("Game assembly unloaded successfully.");
        }
        else
        {
            Console.WriteLine("No game assembly to unload.");
        }
    }
}
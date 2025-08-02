using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public static class ScriptRunner
{
    private static Assembly? _gameAssembly;

    public static void SetGameAssembly(Assembly? assembly)
    {
        _gameAssembly = assembly;
    }
    
    public static int CallComponentInit(IntPtr args, int argLength)
    {
        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);
        var scriptNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        var scriptName = Marshal.PtrToStringUTF8(scriptNamePtr);
        
        if (gameObjectPtr == IntPtr.Zero || string.IsNullOrWhiteSpace(scriptName))
        {
            Console.WriteLine("Invalid arguments provided to CallComponentInit.");
            return -1;
        }

        var gameObject = GameObject.FromNative(gameObjectPtr);

        return InitComponent(gameObject, scriptName);
    }

    public static int InitComponent(GameObject gameObject, string scriptName)
    {
        if (_gameAssembly is null)
        {
            Console.WriteLine("Game assembly is not set.");
            return -1;
        }
        
        Component? component = gameObject.GetOrCreateComponent(_gameAssembly, scriptName);
        
        if (component is null)
        {
            Console.WriteLine("Component could not be found in assembly");
            return -2;
        }

        component.Init();
        return 0;
    }

    public static int CallComponentUpdate(IntPtr args, int argLength)
    {
        if (_gameAssembly is null)
        {
            Console.WriteLine("Game assembly is not set.");
            return -1;
        }

        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);
        var scriptNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        var scriptName = Marshal.PtrToStringUTF8(scriptNamePtr);

        if (gameObjectPtr == IntPtr.Zero || string.IsNullOrWhiteSpace(scriptName))
        {
            Console.WriteLine("Invalid arguments provided to CallComponentUpdate.");
            return -1;
        }

        var gameObject = GameObject.FromNative(gameObjectPtr);

        Component? component = gameObject.GetOrCreateComponent(_gameAssembly, scriptName);

        if (component is null)
        {
            Console.WriteLine("Component could not be found in assembly");
            return -2;
        }

        component.Update();

        return 0;
    }
}
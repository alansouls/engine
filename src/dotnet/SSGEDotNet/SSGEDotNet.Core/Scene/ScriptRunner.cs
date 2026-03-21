using System.Reflection;
using System.Runtime.InteropServices;
using SSGEDotNet.Core.Input;

namespace SSGEDotNet.Core.Scene;

public static class ScriptRunner
{
    private static Assembly? _gameAssembly;

    public static void SetGameAssembly(Assembly? assembly)
    {
        _gameAssembly = assembly;
    }

    //TODO: eventually this ptr might have more initialization data, for now we simply pass it as it's the InputState because it's the only data.
    public static void Initialize(IntPtr initializeData)
    {
        InputState.SetNativeInputState(initializeData);
        Game.InitGameInstance();
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
    
    public static int CallComponentRemove(IntPtr args, int argLength)
    {
        if (_gameAssembly is null)
        {
            Console.WriteLine("Game assembly is not set.");
            return -1;
        }

        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);

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
    
    public static int CallComponentGetProperty(IntPtr args, int argLength)
    {
        if (_gameAssembly is null)
        {
            Console.WriteLine("Game assembly is not set.");
            return -1;
        }

        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);
        var componentNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        var propertyNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size * 2);
        var valuePtr = Marshal.ReadIntPtr(args, IntPtr.Size * 3);
        var componentName = Marshal.PtrToStringUTF8(componentNamePtr);
        var propertyName = Marshal.PtrToStringUTF8(propertyNamePtr);

        if (gameObjectPtr == IntPtr.Zero || string.IsNullOrWhiteSpace(componentName) ||
            string.IsNullOrWhiteSpace(propertyName))
        {
            Console.WriteLine("Invalid arguments provided to CallComponentGetProperty.");
            return -1;
        }

        var gameObject = GameObject.FromNative(gameObjectPtr);

        var component = gameObject.GetOrCreateComponent(_gameAssembly, componentName);

        if (component is null)
        {
            Console.WriteLine("Component could not be found in assembly");
            return -1;
        }
        
        component.GetProperty(propertyName, valuePtr);

        return 0;
    }

    public static int CallComponentSetProperty(IntPtr args, int argLength)
    {
        if (_gameAssembly is null)
        {
            Console.WriteLine("Game assembly is not set.");
            return -1;
        }

        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);
        var scriptNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        var scriptName = Marshal.PtrToStringUTF8(scriptNamePtr);
        var propertyName = Marshal.PtrToStringUTF8(Marshal.ReadIntPtr(args, IntPtr.Size * 2))!;
        var valuePtr = Marshal.ReadIntPtr(args, IntPtr.Size * 3);

        if (gameObjectPtr == IntPtr.Zero || string.IsNullOrWhiteSpace(scriptName))
        {
            Console.WriteLine("Invalid arguments provided to CallComponentSetProperty.");
            return -1;
        }

        var gameObject = GameObject.FromNative(gameObjectPtr);

        Component? component = gameObject.GetOrCreateComponent(_gameAssembly, scriptName);

        if (component is null)
        {
            Console.WriteLine("Component could not be found in assembly");
            return -1;
        }

        component.SetProperty(propertyName, valuePtr);

        return 0;
    }
}
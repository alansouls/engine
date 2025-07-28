using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public static class ScriptRunner
{
    private static string? _gameAssemblyName;
    
    public static int AttachScriptToGameObject(IntPtr args, int argLength)
    {
        if (string.IsNullOrWhiteSpace(_gameAssemblyName))
        {
            Console.WriteLine("Game assembly name is not set.");
            return -1;
        }
        
        var gameObjectPtr = Marshal.ReadIntPtr(args, 0);
        var scriptNamePtr = Marshal.ReadIntPtr(args, IntPtr.Size);
        var scriptName = Marshal.PtrToStringUTF8(scriptNamePtr);
        
        if (gameObjectPtr == IntPtr.Zero || string.IsNullOrWhiteSpace(scriptName))
        {
            Console.WriteLine("Invalid arguments provided to AttachScriptToGameObject.");
            return -1;
        }

        var component = Activator.CreateInstance(_gameAssemblyName, scriptName);
    }
}
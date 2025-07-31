using SSGEDotNet.Core.Scene;
using System.Reflection;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.AssemblyUtils;

public static class GameAssemblyLoader
{
    internal static Assembly? GameAssembly { get; private set; }

    public static int LoadGameAssembly(IntPtr args, int argLength)
    {
        Console.WriteLine("Loading game assembly...");
        var assemblyName = Marshal.PtrToStringUTF8(args);

        if (string.IsNullOrWhiteSpace(assemblyName))
        {
            Console.WriteLine("Game assembly name is not provided.");
            return 1;
        }

        GameAssembly = Assembly.LoadFrom(assemblyName);

        Console.WriteLine($"Game assembly loaded successfully {GameAssembly}.");

        return 0;
    }
}
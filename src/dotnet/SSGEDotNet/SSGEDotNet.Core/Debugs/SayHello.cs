using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Debugs;

public static class Debug
{
    public static int Hello(IntPtr arg, int argLength)
    {
        Console.WriteLine("Hi from DOTNET! We're ready to run some .net scripts! for the game.");
        return 0;
    }
    
    public static int Bye(IntPtr arg, int argLength)
    {
        Console.WriteLine("Bye from DOTNET!");
        return 0;
    }
}
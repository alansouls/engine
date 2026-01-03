using SSGEDotNet.Core.Constants;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public partial class Game
{
    private IntPtr _nativePtr = IntPtr.Zero;

    public static Game Instance { get; private set; } = null!;

    private Game(IntPtr nativePtr)
    {
        _nativePtr = nativePtr;
    }

    internal static void InitGameInstance()
    {
        Console.WriteLine("Initializing Game instance...");
        Instance = new Game(Game_GetInstance());
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial IntPtr Game_GetInstance();

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial GameProperties Game_GetProperties(IntPtr gamePtr);

    public GameProperties GetProperties()
    {
        if (_nativePtr == IntPtr.Zero)
        {
            throw new InvalidOperationException("Native game pointer is not set.");
        }

        return Game_GetProperties(_nativePtr);
    }
}

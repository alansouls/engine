using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

[StructLayout(LayoutKind.Sequential)]
public struct GameProperties
{
    public int Width { get; init; }

    public int Height { get; init; }

    public long DeltaTimeInNanoseconds { get; init; }

    public TimeSpan DeltaTime => TimeSpan.FromMicroseconds(DeltaTimeInNanoseconds / 1000);
}

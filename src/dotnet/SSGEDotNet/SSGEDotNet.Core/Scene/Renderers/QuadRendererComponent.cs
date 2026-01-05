using SSGEDotNet.Core.Constants;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public partial class QuadRendererComponent : NativeComponent
{
    internal QuadRendererComponent(IntPtr nativePtr) : base(nativePtr)
    {
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float QuadRendererComponent_GetWidth(IntPtr quadRendererComponentPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadRendererComponent_SetWidth(IntPtr quadRendererComponentPtr, float width);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float QuadRendererComponent_GetHeight(IntPtr quadRendererComponentPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadRendererComponent_SetHeight(IntPtr quadRendererComponentPtr, float height);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadRendererComponent_GetFillColor(IntPtr quadRendererComponentPtr, IntPtr ptr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadRendererComponent_SetFillColor(IntPtr quadRendererComponentPtr, float[] colorArray);

    public float Width
    {
        get => QuadRendererComponent_GetWidth(_nativePtr);
        set => QuadRendererComponent_SetWidth(_nativePtr, value);
    }

    public float Height
    {
        get => QuadRendererComponent_GetHeight(_nativePtr);
        set => QuadRendererComponent_SetHeight(_nativePtr, value);
    }

    public Vector4 FillColor
    {
        get
        {
            IntPtr colorPtr = Marshal.AllocHGlobal(sizeof(float) * 4);
            float[] colorArray = new float[4];
            QuadRendererComponent_GetFillColor(_nativePtr, colorPtr);
            Marshal.Copy(colorPtr, colorArray, 0, 4);
            Marshal.FreeHGlobal(colorPtr);
            return new Vector4(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
        }
        set
        {
            float[] colorArray = [value.X, value.Y, value.Z, value.W];
            QuadRendererComponent_SetFillColor(_nativePtr, colorArray);
        }
    }
}

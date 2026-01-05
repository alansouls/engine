using SSGEDotNet.Core.Constants;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public partial class CircleRendererComponent : NativeComponent
{
    internal CircleRendererComponent(IntPtr nativePtr) : base(nativePtr)
    {
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleRendererComponent_GetCenter(IntPtr circleRendererComponentPtr, IntPtr ptr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleRendererComponent_SetCenter(IntPtr circleRendererComponentPtr, float[] centerArray);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float CircleRendererComponent_GetRadius(IntPtr circleRendererComponentPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleRendererComponent_SetRadius(IntPtr circleRendererComponentPtr, float radius);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleRendererComponent_GetFillColor(IntPtr circleRendererComponentPtr, IntPtr ptr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleRendererComponent_SetFillColor(IntPtr circleRendererComponentPtr, float[] colorArray);

    public Vector2 Center
    {
        get
        {
            IntPtr centerPtr = Marshal.AllocHGlobal(sizeof(float) * 2);
            float[] centerArray = new float[2];
            CircleRendererComponent_GetCenter(_nativePtr, centerPtr);
            Marshal.Copy(centerPtr, centerArray, 0, 2);
            Marshal.FreeHGlobal(centerPtr);
            return new Vector2(centerArray[0], centerArray[1]);
        }
        set
        {
            float[] centerArray = [value.X, value.Y];
            CircleRendererComponent_SetCenter(_nativePtr, centerArray);
        }
    }

    public float Radius
    {
        get => CircleRendererComponent_GetRadius(_nativePtr);
        set => CircleRendererComponent_SetRadius(_nativePtr, value);
    }

    public Vector4 FillColor
    {
        get
        {
            IntPtr colorPtr = Marshal.AllocHGlobal(sizeof(float) * 4);
            float[] colorArray = new float[4];
            CircleRendererComponent_GetFillColor(_nativePtr, colorPtr);
            Marshal.Copy(colorPtr, colorArray, 0, 4);
            Marshal.FreeHGlobal(colorPtr);
            return new Vector4(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
        }
        set
        {
            float[] colorArray = [value.X, value.Y, value.Z, value.W];
            CircleRendererComponent_SetFillColor(_nativePtr, colorArray);
        }
    }
}

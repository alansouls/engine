using SSGEDotNet.Core.Constants;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene.Colliders;

public partial class QuadColliderComponent : ColliderComponent
{
    internal QuadColliderComponent(IntPtr nativePtr) : base(nativePtr)
    {
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadCollider_GetTopLeft(IntPtr colliderPtr, IntPtr ptr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadCollider_SetTopLeft(IntPtr colliderPtr, float topLeftX, float topLeftY);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float QuadCollider_GetWidth(IntPtr colliderPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float QuadCollider_GetHeight(IntPtr colliderPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void QuadCollider_SetDimensions(IntPtr colliderPtr, float width, float height);

    public Vector2 TopLeft
    {
        get
        {
            IntPtr topLeftPtr = Marshal.AllocHGlobal(sizeof(float) * 2);
            float[] topLeftArray = new float[2];
            QuadCollider_GetTopLeft(_nativePtr, topLeftPtr);
            Marshal.Copy(topLeftPtr, topLeftArray, 0, 2);
            Marshal.FreeHGlobal(topLeftPtr);
            return new Vector2(topLeftArray[0], topLeftArray[1]);
        }
        set
        {
            QuadCollider_SetTopLeft(_nativePtr, value.X, value.Y);
        }
    }

    public float Width
    {
        get => QuadCollider_GetWidth(_nativePtr);
        set => QuadCollider_SetDimensions(_nativePtr, value, Height);
    }

    public float Height
    {
        get => QuadCollider_GetHeight(_nativePtr);
        set => QuadCollider_SetDimensions(_nativePtr, Width, value);
    }

    public void SetDimensions(float width, float height)
    {
        QuadCollider_SetDimensions(_nativePtr, width, height);
    }
}

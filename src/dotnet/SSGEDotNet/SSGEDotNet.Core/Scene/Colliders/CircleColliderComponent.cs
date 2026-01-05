using SSGEDotNet.Core.Constants;
using SSGEDotNet.Core.Scene.Colliders;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene;

public partial class CircleColliderComponent : ColliderComponent
{
    internal CircleColliderComponent(IntPtr nativePtr) : base(nativePtr)
    {
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleCollider_GetCenter(IntPtr colliderPtr, IntPtr ptr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleCollider_SetCenter(IntPtr colliderPtr, float centerX, float centerY);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial float CircleCollider_GetRadius(IntPtr colliderPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void CircleCollider_SetRadius(IntPtr colliderPtr, float radius);

    public Vector2 Center
    {
        get
        {
            IntPtr centerPtr = Marshal.AllocHGlobal(sizeof(float) * 2);
            float[] centerArray = new float[2];
            CircleCollider_GetCenter(_nativePtr, centerPtr);
            Marshal.Copy(centerPtr, centerArray, 0, 2);
            Marshal.FreeHGlobal(centerPtr);
            return new Vector2(centerArray[0], centerArray[1]);
        }
        set
        {
            CircleCollider_SetCenter(_nativePtr, value.X, value.Y);
        }
    }

    public float Radius
    {
        get => CircleCollider_GetRadius(_nativePtr);
        set => CircleCollider_SetRadius(_nativePtr, value);
    }
}

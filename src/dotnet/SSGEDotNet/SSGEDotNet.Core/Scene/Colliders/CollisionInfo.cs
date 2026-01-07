using SSGEDotNet.Core.Scene.Components;
using System.Numerics;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene.Colliders;

public struct CollisionInfo
{
    public ColliderComponent Collider { get; init; }
    public ColliderComponent OtherCollider { get; init; }
    public Vector2 ContactPoint { get; init; }
    public bool Entered { get; init; }

    public static CollisionInfo FromNative(IntPtr collisionInfoPtr)
    {
        var colliderPtr = Marshal.ReadIntPtr(collisionInfoPtr);
        var otherColliderPtr = Marshal.ReadIntPtr(collisionInfoPtr, IntPtr.Size);
        var contactPointX = Marshal.PtrToStructure<float>(IntPtr.Add(collisionInfoPtr, IntPtr.Size * 2));
        var contactPointY = Marshal.PtrToStructure<float>(IntPtr.Add(collisionInfoPtr, IntPtr.Size * 2 + sizeof(float)));
        var entered = Marshal.PtrToStructure<byte>(IntPtr.Add(collisionInfoPtr, IntPtr.Size * 2 + sizeof(float) * 2)) != 0;

        return new CollisionInfo
        {
            Collider = NativeComponentFactory.Get<ColliderComponent>(colliderPtr) ?? throw new Exception("Failed to get instance of collider component"),
            OtherCollider = NativeComponentFactory.Get<ColliderComponent>(colliderPtr) ?? throw new Exception("Failed to get instance of collider component"),
            ContactPoint = new Vector2(contactPointX, contactPointY),
            Entered = entered
        };
    }
}

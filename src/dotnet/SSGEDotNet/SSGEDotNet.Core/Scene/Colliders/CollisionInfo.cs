using System.Numerics;

namespace SSGEDotNet.Core.Scene.Colliders;

public struct CollisionInfo
{
    public ColliderComponent Collider { get; init; }
    public ColliderComponent OtherCollider { get; init; }
    public Vector2 ContactPoint { get; init; }
    public bool Entered { get; init; }
}

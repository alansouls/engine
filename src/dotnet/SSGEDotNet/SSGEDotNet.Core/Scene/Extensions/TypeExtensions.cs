using SSGEDotNet.Core.Scene.Colliders;
using SSGEDotNet.Core.Scene.Enums;

namespace SSGEDotNet.Core.Scene.Extensions;

internal static class TypeExtensions
{
    extension(Type type)
    {
        public bool IsComponentType()
        {
            return type.IsSubclassOf(typeof(Component)) && !type.IsAbstract;
        }

        public bool IsNativeComponentType()
        {
            return type.IsSubclassOf(typeof(NativeComponent)) && !type.IsAbstract;
        }

        public NativeComponentType GetNativeComponentType()
        {
            if (type == typeof(QuadRendererComponent))
            {
                return NativeComponentType.QuadRenderer;
            }
            else if (type == typeof(CircleRendererComponent))
            {
                return NativeComponentType.CircleRenderer;
            }
            else if (type == typeof(QuadColliderComponent))
            {
                return NativeComponentType.QuadCollider;
            }
            else if (type == typeof(CircleColliderComponent))
            {
                return NativeComponentType.CircleCollider;
            }

            throw new Exception($"Type {type.FullName} is not a recognized native component type.");
        }
    }
}

using SSGEDotNet.Core.Scene.Enums;

namespace SSGEDotNet.Core.Scene.Extensions;

internal static class TypeExtensions
{
    extension (Type type)
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

            throw new Exception($"Type {type.FullName} is not a recognized native component type.");
        }
    }
}

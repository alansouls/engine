using SSGEDotNet.Core.Constants;
using System.Runtime.InteropServices;

namespace SSGEDotNet.Core.Scene.Colliders;

public abstract partial class ColliderComponent : NativeComponent
{
    public delegate void OnCollisionEnterDelegate(CollisionInfo info);
    public delegate void OnCollisionExitDelegate(CollisionInfo info);

    public event OnCollisionEnterDelegate? OnCollisionEnter;
    public event OnCollisionExitDelegate? OnCollisionExit;

    internal ColliderComponent(IntPtr nativePtr) : base(nativePtr)
    {
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial IntPtr Collider_GetLayer(IntPtr colliderPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll, StringMarshalling = StringMarshalling.Utf8)]
    private static partial void Collider_SetLayer(IntPtr colliderPtr, string layer);

    [LibraryImport(InteropConstants.SSGEEngineDll, StringMarshalling = StringMarshalling.Utf8)]
    private static partial void Collider_SetCollidesWith(IntPtr colliderPtr, [In]string[] layers, int layersLength);

    public string Layer
    {
        get
        {
            IntPtr layerPtr = Collider_GetLayer(_nativePtr);
            return Marshal.PtrToStringUTF8(layerPtr) ?? string.Empty;
        }
        set
        {
            Collider_SetLayer(_nativePtr, value);
        }
    }

    public void SetCollidesWith(string[] layers)
    {
        Collider_SetCollidesWith(_nativePtr, layers, layers.Length);
    }
}

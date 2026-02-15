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
        var onCollisionEnterCallbackPtr = Marshal.GetFunctionPointerForDelegate((OnCollisionEnterInternalDelegate) OnCollisionEnterInternal);
        var onCollisionExitCallbackPtr = Marshal.GetFunctionPointerForDelegate((OnCollisionEnterInternalDelegate) OnCollisionExitInternal);

        Collider_RegisterOnCollisionEnterCallback(_nativePtr, onCollisionEnterCallbackPtr);
        Collider_RegisterOnCollisionExitCallback(_nativePtr, onCollisionExitCallbackPtr);
    }

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial IntPtr Collider_GetLayer(IntPtr colliderPtr);

    [LibraryImport(InteropConstants.SSGEEngineDll, StringMarshalling = StringMarshalling.Utf8)]
    private static partial void Collider_SetLayer(IntPtr colliderPtr, string layer);

    [LibraryImport(InteropConstants.SSGEEngineDll, StringMarshalling = StringMarshalling.Utf8)]
    private static partial void Collider_SetCollidesWith(IntPtr colliderPtr, [In] string[] layers, int layersLength);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void Collider_RegisterOnCollisionEnterCallback(IntPtr colliderPtr, IntPtr callbackPointer);

    [LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial void Collider_RegisterOnCollisionExitCallback(IntPtr colliderPtr, IntPtr callbackPointer);

    public string Layer
    {
        get
        {
            IntPtr layerPtr = Collider_GetLayer(_nativePtr);
            return Marshal.PtrToStringUTF8(layerPtr) ?? string.Empty;
        }
        set =>  Collider_SetLayer(_nativePtr, value);
    }

    public void SetCollidesWith(string[] layers)
    {
        Collider_SetCollidesWith(_nativePtr, layers, layers.Length);
    }

    private delegate void OnCollisionEnterInternalDelegate(IntPtr collisionInfoPtr);
    
    private void OnCollisionEnterInternal(IntPtr collisionInfoPtr)
    {
        OnCollisionEnter?.Invoke(CollisionInfo.FromNative(collisionInfoPtr));
    }
    private void OnCollisionExitInternal(IntPtr collisionInfoPtr)
    {
        OnCollisionExit?.Invoke(CollisionInfo.FromNative(collisionInfoPtr));
    }
}

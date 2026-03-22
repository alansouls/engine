using System.Diagnostics;
using SSGEDotNet.Core.Constants;
using SSGEDotNet.Core.GraphicsUtils;
using SSGEDotNet.Core.Input;
using SSGEDotNet.Core.Scene.Components;
using SSGEDotNet.Core.Scene.Enums;
using SSGEDotNet.Core.Scene.Extensions;
using SSGEDotNet.Core.Scene.Utils;
using System.Reflection;

namespace SSGEDotNet.Core.Scene;

internal partial class GameObjectNative
{
    private readonly IntPtr _handle;

    public GameObjectNative(IntPtr handle)
    {
        _handle = handle;
    }

    // P/Invoke declarations
    [System.Runtime.InteropServices.LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial IntPtr GameObject_GetTransform(IntPtr gameObject);

    // P/Invoke declarations
    [System.Runtime.InteropServices.LibraryImport(InteropConstants.SSGEEngineDll)]
    private static partial IntPtr GameObject_GetComponent(IntPtr gameObject, NativeComponentType type);

    public IntPtr GetTransform()
    {
        if (_handle == IntPtr.Zero)
        {
            return IntPtr.Zero;
        }

        return GameObject_GetTransform(_handle);
    }

    public IntPtr GetComponent(NativeComponentType type)
    {
        if (_handle == IntPtr.Zero)
        {
            return IntPtr.Zero;
        }

        return GameObject_GetComponent(_handle, type);
    }
}

public class GameObject
{
    private readonly GameObjectNative _native;
    private Transform? _transform;
    private readonly ComponentBag _components = new();

    private GameObject(IntPtr handle)
    {
        _native = new GameObjectNative(handle);
    }

    private static readonly Dictionary<IntPtr, GameObject> _gameObjectCache = [];

    static internal GameObject FromNative(IntPtr ptr)
    {
        if (_gameObjectCache.TryGetValue(ptr, out GameObject? obj))
        {
            return obj;
        }

        obj = new GameObject(ptr);

        _gameObjectCache.Add(ptr, obj);

        return obj;
    }

    internal Component? GetOrCreateComponent(Assembly gameAssembly, string componentName)
    {
        var componentKey = gameAssembly.FullName + componentName;

        if (_components.GetComponent(componentKey) is Component cachedComponent)
        {
            return cachedComponent;
        }

        Type? type = gameAssembly.GetExportedTypes().FirstOrDefault(t => t.FullName == componentName);

        if (type is null || !typeof(Component).IsAssignableFrom(type))
        {
            Console.WriteLine($"Component type '{componentName}' not found in assembly '{gameAssembly.FullName}'.");

            foreach (var type1 in gameAssembly.GetExportedTypes())
            {
                Console.WriteLine(type1.FullName);
            }

            return null;
        }

        var componentObject = Activator.CreateInstance(type);

        if (componentObject is not Component component)
        {
            return null;
        }

        component.GameObject = this;

        _components.AddComponent(component);

        return component;
    }

    internal void RemoveComponent(Assembly gameAssembly, string componentName)
    {
        var type = gameAssembly.GetExportedTypes().FirstOrDefault(t => t.FullName == componentName);
        Debug.Assert(type is not null, "Could not  find component type '" + componentName + "'.");
        RemoveComponent(type);
    }

    public void RemoveComponent<TComponent>() where TComponent : Component
    {
        RemoveComponent(typeof(TComponent));
    }

    private void RemoveComponent(Type componentType)
    {
        var isNativeComponent = componentType.IsNativeComponentType();
        var component = _components.RemoveComponent(componentType);

        if (!isNativeComponent || component is null)
            return;

        NativeComponentFactory.Remove((component as NativeComponent)!.NativePtr);
    }

    public TComponent? GetComponent<TComponent>() where TComponent : Component
    {
        var isNativeComponent = typeof(TComponent).IsNativeComponentType();

        if (_components.GetComponent<TComponent>() is TComponent cachedComponent)
        {
            return cachedComponent;
        }

        if (!isNativeComponent)
            return null;

        var nativePtr = _native.GetComponent(typeof(TComponent).GetNativeComponentType());

        return GetNativeComponent<TComponent>(nativePtr);
    }

    internal TComponent? GetNativeComponent<TComponent>(nint nativePtr) where TComponent : Component
    {
        if (nativePtr == IntPtr.Zero)
        {
            return null;
        }

        var nativeComponent = NativeComponentFactory.GetOrCreate(typeof(TComponent), nativePtr, this);

        _components.AddComponent(nativeComponent);
        return nativeComponent as TComponent;
    }

    public Transform Transform => _transform ??= new Transform(_native.GetTransform());
}
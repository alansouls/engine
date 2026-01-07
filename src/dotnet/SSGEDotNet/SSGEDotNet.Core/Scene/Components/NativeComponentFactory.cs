using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;

namespace SSGEDotNet.Core.Scene.Components;

internal class NativeComponentFactory
{
    private static readonly Dictionary<IntPtr, NativeComponent> _components = [];

    public static NativeComponent GetOrCreate(Type componentType, IntPtr nativePtr, GameObject gameObject)
    {
        var existingComponent = _components.GetValueOrDefault(nativePtr);

        if (existingComponent is not null)
        {
            return existingComponent;
        }

        NativeComponent component = Activator.CreateInstance(componentType, bindingAttr: BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.CreateInstance,
            binder: null, args: [nativePtr], culture: null) as NativeComponent ?? throw new Exception("Failed to create native component instance.");

        component.GameObject = gameObject;
        _components[nativePtr] = (component);

        return component;
    }

    public static TComponent GetOrCreate<TComponent>(IntPtr nativePtr, GameObject gameObject) where TComponent : NativeComponent
    {
        var existingComponent = GetOrCreate(typeof(TComponent), nativePtr, gameObject);

        if (existingComponent is TComponent nativeComponent)
        {
            return nativeComponent;
        }
        else
        {
            throw new Exception("A different component is already registered with the same native pointer.");
        }
    }

    public static TComponent? Get<TComponent>(IntPtr nativePtr) where TComponent : NativeComponent
        => _components.GetValueOrDefault(nativePtr) as TComponent;
}

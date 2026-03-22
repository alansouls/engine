using System.Diagnostics;

namespace SSGEDotNet.Core.Scene.Utils;

public class ComponentBag
{
    private readonly Dictionary<string, Component> _components = [];

    private static string GetComponentKey(Type componentType)
    {
        return componentType.Assembly.FullName + componentType.FullName;
    }

    public void AddComponent(Component component)
    {
        var componentKey = GetComponentKey(component.GetType());
        _components[componentKey] = component;
    }

    public Component? GetComponent<T>() where T : Component
    {
        return _components.GetValueOrDefault(GetComponentKey(typeof(T))) as T;
    }

    public T? RemoveComponent<T>() where T : Component
    {
        return RemoveComponent(typeof(T)) as T;
    }

    public Component? RemoveComponent(Type componentType)
    {
        Debug.Assert(componentType.IsAssignableTo(typeof(Component)), "Component type is not assignable to Component");
        var componentKey = GetComponentKey(componentType);
        return _components.Remove(componentKey, out var component) ? component : null;
    }

    public Component? GetComponent(string name)
    {
        return _components.GetValueOrDefault(name);
    }
}
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

    public Component? GetComponent(string name)
    {
        return _components.GetValueOrDefault(name);
    }
}
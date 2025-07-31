using System.Reflection;

namespace SSGEDotNet.Core.Scene;


internal class GameObjectNative
{
    private readonly IntPtr _handle;

    public GameObjectNative(IntPtr handle)
    {
        _handle = handle;
    }
}

public class GameObject
{
    private readonly GameObjectNative _native;
    private readonly Dictionary<string, Component> _components = [];

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

        if (_components.TryGetValue(componentKey, out Component? value))
        {
            return value;
        }

        Type? type = gameAssembly.GetType(componentName);

        if (type is null || !typeof(Component).IsAssignableFrom(type))
        {
            Console.WriteLine($"Component type '{componentName}' not found in assembly '{gameAssembly.FullName}'.");

            foreach (var type1 in gameAssembly.GetTypes())
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

        _components[componentKey] = component;

        return component;
    }
}
using SSGEDotNet.Core.Constants;
using SSGEDotNet.Core.GraphicsUtils;
using SSGEDotNet.Core.Input;
using System.Reflection;

namespace SSGEDotNet.Core.Scene;


internal class GameObjectNative
{
    private readonly IntPtr _handle;

    public GameObjectNative(IntPtr handle)
    {
        _handle = handle;
    }

    // P/Invoke declarations
    [System.Runtime.InteropServices.DllImport(InteropConstants.SSGEEngineDll, CallingConvention = System.Runtime.InteropServices.CallingConvention.Cdecl)]
    private static extern IntPtr GameObject_GetTransform(IntPtr gameObject);

    public IntPtr GetTransform()
    {
        if (_handle == IntPtr.Zero)
        {
            return IntPtr.Zero;
        }
        return GameObject_GetTransform(_handle);
    }
}

public class GameObject
{
    private readonly GameObjectNative _native;
    private Transform? _transform;
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

        _components[componentKey] = component;

        component.GameObject = this;

        return component;
    }

    public InputState Input { get; } = InputState.Instance ?? throw new InvalidOperationException("InputState was not initalized!");

    public Transform Transform => _transform ??= new Transform(_native.GetTransform());
}
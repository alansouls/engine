using System.Reflection;
using SSGEDotNet.Core.Extensions;
using SSGEDotNet.Core.Scene.Attributes;

namespace SSGEDotNet.Core.Scene;

public abstract class Component
{
    internal void SetProperties(Dictionary<string, string?>? properties)
    {
        if (properties is null)
        {
            return;
        }
        
        var finalType = GetType();

        var editorProperties = finalType.GetProperties(BindingFlags.Public)
            .Where(p => Attribute.IsDefined(p, typeof(EditorPropertyAttribute)));

        foreach (var property in editorProperties)
        {
            if (properties.TryGetValue(property.Name, out var value))
            {
                property.SetPropertyFromString(this, value);
            }
        }
    }
    
    public abstract void Init();
    
    public abstract void Update();

    public GameObject GameObject { get; internal set; } = null!;
}
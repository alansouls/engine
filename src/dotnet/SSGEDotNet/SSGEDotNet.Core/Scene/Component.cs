using System.Numerics;
using System.Reflection;
using System.Runtime.InteropServices;
using SSGEDotNet.Core.Constants;
using SSGEDotNet.Core.Scene.Attributes;

namespace SSGEDotNet.Core.Scene;

public abstract partial class Component
{
    [LibraryImport(InteropConstants.SSGEEngineDll, StringMarshalling = StringMarshalling.Utf8)]
    private static partial void ScriptComponent_SetCurrentValueString(IntPtr nativePtr, string managedString);

    private void SetProperty(PropertyInfo propertyInfo, IntPtr valuePtr)
    {
        object? value;
        if (propertyInfo.PropertyType == typeof(int))
        {
            value = Marshal.ReadInt32(valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(float))
        {
            value = Marshal.PtrToStructure<float>(valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(bool))
        {
            value = Marshal.ReadByte(valuePtr) != 0;
        }
        else if (propertyInfo.PropertyType == typeof(string))
        {
            value = Marshal.PtrToStringUTF8(valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector2))
        {
            value = Marshal.PtrToStructure<Vector2>(valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector3))
        {
            value = Marshal.PtrToStructure<Vector3>(valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector4))
        {
            value = Marshal.PtrToStructure<Vector4>(valuePtr);
        }
        else
        {
            throw new ArgumentException($"Property {propertyInfo.Name} is not supported");
        }

        propertyInfo.SetValue(this, value);
    }

    private void WriteStructure<T>(object rawValue, IntPtr valuePtr) where T : struct
    {
        var value = (T)rawValue;
        Marshal.StructureToPtr(value, valuePtr, false);
    }

    private void WritePropertyValueToPtr(PropertyInfo propertyInfo, IntPtr valuePtr)
    {
        var propertyValue = propertyInfo.GetValue(this);

        if (propertyInfo.PropertyType == typeof(int))
        {
            var value = (int?)propertyValue ?? 0;
            Marshal.WriteInt32(valuePtr, value);
        }
        else if (propertyInfo.PropertyType == typeof(float))
        {
            WriteStructure<float>(propertyValue!, valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(bool))
        {
            var value = (bool)propertyInfo.GetValue(this)!;
            Marshal.WriteByte(valuePtr, (byte)(value ? 1 : 0));
        }
        else if (propertyInfo.PropertyType == typeof(string))
        {
            //TODO: What should I do about null strings?
            var value = (string?)propertyInfo.GetValue(this) ?? string.Empty;
            ScriptComponent_SetCurrentValueString(valuePtr, value);
        }
        else if (propertyInfo.PropertyType == typeof(Vector2))
        {
            WriteStructure<Vector2>(propertyValue!, valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector3))
        {
            WriteStructure<Vector3>(propertyValue!, valuePtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector4))
        {
            WriteStructure<Vector4>(propertyValue!, valuePtr);
        }
        else
        {
            throw new ArgumentException($"Property {propertyInfo.Name} is not supported");
        }
    }

    internal void GetProperty(string propertyName, IntPtr valuePtr)
    {
        var finalType = GetType();

        var editorProperty = finalType.GetProperties()
            .FirstOrDefault(p => p.Name == propertyName && Attribute.IsDefined(p, typeof(EditorPropertyAttribute)));

        if (editorProperty is null)
        {
            throw new ArgumentException($"Property {propertyName} not found in type {finalType.FullName}");
        }

        WritePropertyValueToPtr(editorProperty, valuePtr);
    }

    internal void SetProperty(string propertyName, IntPtr valuePtr)
    {
        var finalType = GetType();

        var editorProperty = finalType.GetProperties()
            .FirstOrDefault(p => p.Name == propertyName && Attribute.IsDefined(p, typeof(EditorPropertyAttribute)));

        if (editorProperty is null)
        {
            throw new ArgumentException($"Property {propertyName} not found in type {finalType.FullName}");
        }

        SetProperty(editorProperty, valuePtr);
    }

    public abstract void Init();

    public abstract void Update();

    public GameObject GameObject { get; internal set; } = null!;
}
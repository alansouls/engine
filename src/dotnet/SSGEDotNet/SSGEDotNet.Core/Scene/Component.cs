using System.Numerics;
using System.Reflection;
using System.Runtime.InteropServices;
using SSGEDotNet.Core.Extensions;
using SSGEDotNet.Core.Scene.Attributes;

namespace SSGEDotNet.Core.Scene;

public abstract class Component
{
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
    
    private void WritePropertyValueToPtr(PropertyInfo propertyInfo, IntPtr valuePtr)
    {
        if (propertyInfo.PropertyType == typeof(int))
        {
            var value = (int)propertyInfo.GetValue(this)!;
            Marshal.WriteInt32(valuePtr, value);
        }
        else if (propertyInfo.PropertyType == typeof(float))
        {
            var value = (float)propertyInfo.GetValue(this)!;
            Marshal.StructureToPtr(value, valuePtr, false);
        }
        else if (propertyInfo.PropertyType == typeof(bool))
        {
            var value = (bool)propertyInfo.GetValue(this)!;
            Marshal.WriteByte(valuePtr, (byte)(value ? 1 : 0));
        }
        else if (propertyInfo.PropertyType == typeof(string))
        {
            var value = (string)propertyInfo.GetValue(this)!;
            var strPtr = Marshal.StringToHGlobalAuto(value);
            Marshal.WriteIntPtr(valuePtr, strPtr);
        }
        else if (propertyInfo.PropertyType == typeof(Vector2))
        {
            var value = (int)propertyInfo.GetValue(this)!;
            Marshal.WriteInt32(valuePtr, value);
        }
        else if (propertyInfo.PropertyType == typeof(Vector3))
        {
            var value = (int)propertyInfo.GetValue(this)!;
            Marshal.WriteInt32(valuePtr, value);
        }
        else if (propertyInfo.PropertyType == typeof(Vector4))
        {
            var value = (int)propertyInfo.GetValue(this)!;
            Marshal.WriteInt32(valuePtr, value);
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
        
        SetProperty(editorProperty, valuePtr);
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
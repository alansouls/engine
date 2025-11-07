using System.Reflection;

namespace SSGEDotNet.Core.Extensions;

public static class PropertyExtensions
{
    public static void SetPropertyFromString(this PropertyInfo property, object instance, string? value)
    {
        var propertyType = property.PropertyType;

        object? convertedValue = null;
        bool success = true;

        if (propertyType == typeof(string))
        {
            convertedValue = value;
        }
        else if (propertyType == typeof(int) && int.TryParse(value, out int intValue))
        {
            convertedValue = intValue;
        }
        else if (propertyType == typeof(float) && float.TryParse(value, out float floatValue))
        {
            convertedValue = floatValue;
        }
        else if (propertyType == typeof(bool) && bool.TryParse(value, out bool boolValue))
        {
            convertedValue = boolValue;
        }
        else if (propertyType == typeof(decimal) && decimal.TryParse(value, out decimal decimalValue))
        {
            convertedValue = decimalValue;
        }
        else if (propertyType == typeof(double) && double.TryParse(value, out double doubleValue))
        {
            convertedValue = doubleValue;
        }
        else if (propertyType.IsEnum && Enum.TryParse(propertyType, value, out var enumValue))
        {
            convertedValue = enumValue;
        }
        else
        {
            success = false;
        }

        if (success)
        {
            property.SetValue(instance, convertedValue);
        }
    }
}
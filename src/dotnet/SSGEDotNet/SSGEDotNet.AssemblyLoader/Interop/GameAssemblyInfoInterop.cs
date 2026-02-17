using System.Runtime.InteropServices;
using SSGEDotNet.AssemblyLoader.Models;

namespace SSGEDotNet.AssemblyLoader.Interop;

//TODO: There must be a better way of implementing this, this is highly memory insecure
public static class GameAssemblyInfoInterop
{
    public static void WriteToPtr(GameAssemblyInfo info, IntPtr ptr)
    {
        var componentsArray = Marshal.AllocHGlobal(IntPtr.Size * info.Components.Length);
        foreach (var (index, component) in info.Components.Select((x, i) => (i, x)))
        {
            var componentPtr = Marshal.AllocHGlobal(IntPtr.Size * 3);
            WriteToPtr(component, componentPtr);
            Marshal.WriteIntPtr(componentsArray, IntPtr.Size * index, componentPtr);
        }
        
        Marshal.WriteIntPtr(ptr, 0, Marshal.StringToHGlobalUni(info.Name));
        Marshal.WriteIntPtr(ptr, IntPtr.Size, componentsArray);
    }

    public static void WriteToPtr(ScriptComponentInfo info, IntPtr ptr)
    {
        var propertiesArray =  Marshal.AllocHGlobal(IntPtr.Size * info.Properties.Length);
        foreach (var (index, property) in info.Properties.Select((x, i) => (i, x)))
        {
            var propertyPtr = Marshal.AllocHGlobal(IntPtr.Size * 2);
            WriteToPtr(property, propertyPtr);
            Marshal.WriteIntPtr(propertiesArray, IntPtr.Size * index, propertyPtr);
        }
        Marshal.WriteIntPtr(ptr, 0, Marshal.StringToHGlobalUni(info.Name));
        Marshal.WriteIntPtr(ptr, IntPtr.Size, Marshal.StringToHGlobalUni(info.Fullname));
        Marshal.WriteIntPtr(ptr, IntPtr.Size * 2, propertiesArray);
    }

    public static void WriteToPtr(ComponentPropertyInfo info, IntPtr ptr)
    {
        Marshal.WriteIntPtr(ptr, 0, Marshal.StringToHGlobalUni(info.Name));
        Marshal.WriteIntPtr(ptr, IntPtr.Size, Marshal.StringToHGlobalUni(info.Type));
    }
}
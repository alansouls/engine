using SSGEDotNet.AssemblyLoader.Models;
using SSGEDotNet.Core.Constants;
using System.Runtime.InteropServices;

namespace SSGEDotNet.AssemblyLoader.Interop;

//TODO: There must be a better way of implementing this, this is highly memory insecure
public static class GameAssemblyInfoInterop
{

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void GameAssemblyInfo_FromC_GameAssemblyInfo(IntPtr info, IntPtr destinationInfo);

    public static void WriteToPtr(GameAssemblyInfo info, IntPtr ptr)
    {
        var cInfoPtr = Marshal.AllocHGlobal((IntPtr.Size * 2) + sizeof(Int32));

        var componentsArray = Marshal.AllocHGlobal(IntPtr.Size * info.Components.Length);
        foreach (var (index, component) in info.Components.Select((x, i) => (i, x)))
        {
            var componentPtr = Marshal.AllocHGlobal(IntPtr.Size * 4);
            WriteToPtr(component, componentPtr);
            Marshal.WriteIntPtr(componentsArray, IntPtr.Size * index, componentPtr);
        }

        Marshal.WriteIntPtr(cInfoPtr, 0, Marshal.StringToHGlobalAuto(info.Name));
        Marshal.WriteIntPtr(cInfoPtr, IntPtr.Size, componentsArray);
        Marshal.WriteInt32(cInfoPtr, IntPtr.Size * 2, info.Components.Length);

        GameAssemblyInfo_FromC_GameAssemblyInfo(cInfoPtr, ptr);

        FreePtr(cInfoPtr);
    }

    public static void WriteToPtr(ScriptComponentInfo info, IntPtr ptr)
    {
        var propertiesArray = Marshal.AllocHGlobal(IntPtr.Size * info.Properties.Length);
        foreach (var (index, property) in info.Properties.Select((x, i) => (i, x)))
        {
            var propertyPtr = Marshal.AllocHGlobal(IntPtr.Size * 2);
            WriteToPtr(property, propertyPtr);
            Marshal.WriteIntPtr(propertiesArray, IntPtr.Size * index, propertyPtr);
        }
        Marshal.WriteIntPtr(ptr, 0, Marshal.StringToHGlobalAuto(info.Name));
        Marshal.WriteIntPtr(ptr, IntPtr.Size, Marshal.StringToHGlobalAuto(info.Fullname));
        Marshal.WriteIntPtr(ptr, IntPtr.Size * 2, propertiesArray);
        Marshal.WriteInt32(ptr, IntPtr.Size * 3, info.Properties.Length);
    }

    public static void WriteToPtr(ComponentPropertyInfo info, IntPtr ptr)
    {
        Marshal.WriteIntPtr(ptr, 0, Marshal.StringToHGlobalAuto(info.Name));
        Marshal.WriteIntPtr(ptr, IntPtr.Size, Marshal.StringToHGlobalAuto(info.Type));
    }

    public static void FreePtr(IntPtr ptr)
    {
        var namePtr = Marshal.ReadIntPtr(ptr, 0);
        Marshal.FreeHGlobal(namePtr);
        var componentsArrayPtr = Marshal.ReadIntPtr(ptr, IntPtr.Size);
        var componentCount = Marshal.ReadInt32(ptr, IntPtr.Size * 2);
        for (int i = 0; i < componentCount; i++)
        {
            var componentPtr = Marshal.ReadIntPtr(componentsArrayPtr, IntPtr.Size * i);
            FreeComponentPtr(componentPtr);
            Marshal.FreeHGlobal(componentPtr);
        }
        Marshal.FreeHGlobal(componentsArrayPtr);
    }

    public static void FreeComponentPtr(IntPtr ptr)
    {
        var namePtr = Marshal.ReadIntPtr(ptr, 0);
        Marshal.FreeHGlobal(namePtr);
        var fullnamePtr = Marshal.ReadIntPtr(ptr, IntPtr.Size);
        Marshal.FreeHGlobal(fullnamePtr);
        var propertiesArrayPtr = Marshal.ReadIntPtr(ptr, IntPtr.Size * 2);
        var propertyCount = Marshal.ReadInt32(ptr, IntPtr.Size * 3);
        for (int i = 0; i < propertyCount; i++)
        {
            var propertyPtr = Marshal.ReadIntPtr(propertiesArrayPtr, IntPtr.Size * i);
            FreePropertyPtr(propertyPtr);
            Marshal.FreeHGlobal(propertyPtr);
        }
        Marshal.FreeHGlobal(propertiesArrayPtr);
    }

    public static void FreePropertyPtr(IntPtr ptr)
    {
        var namePtr = Marshal.ReadIntPtr(ptr, 0);
        Marshal.FreeHGlobal(namePtr);
        var typePtr = Marshal.ReadIntPtr(ptr, IntPtr.Size);
        Marshal.FreeHGlobal(typePtr);
    }
}
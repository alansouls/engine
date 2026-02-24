using System.Runtime.InteropServices;

namespace SSGEDotNet.AssemblyLoader.Models;

[StructLayout(LayoutKind.Sequential)]
public struct ComponentPropertyInfo
{
    public string Name { get; set; }
    
    public string Type { get; set; }
}
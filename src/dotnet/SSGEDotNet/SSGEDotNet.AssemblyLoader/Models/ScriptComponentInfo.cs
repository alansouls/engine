using System.Runtime.InteropServices;

namespace SSGEDotNet.AssemblyLoader.Models;

[StructLayout(LayoutKind.Sequential)]
public struct ScriptComponentInfo
{
    public string Name { get; set; }
    public string Fullname { get; set; }
    public ComponentPropertyInfo[] Properties { get; set; }
}
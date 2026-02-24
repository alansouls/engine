using System.Runtime.InteropServices;

namespace SSGEDotNet.AssemblyLoader.Models;

[StructLayout(LayoutKind.Sequential)]
public struct GameAssemblyInfo
{
    public string Name { get; set; }
    public ScriptComponentInfo[] Components { get; set; }
}
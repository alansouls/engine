using System.Reflection;
using System.Runtime.Loader;

namespace SSGEDotNet.AssemblyLoader;

public class TempGameAssemblyLoadContext : GameAssemblyLoadContext
{
    public TempGameAssemblyLoadContext(string mainAssemblyToLoadPath) : base("TempGameAssemblyLoadContext",
        mainAssemblyToLoadPath)
    {
    }
}
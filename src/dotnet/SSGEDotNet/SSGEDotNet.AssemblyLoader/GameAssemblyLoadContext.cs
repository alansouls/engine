using System.Reflection;
using System.Runtime.Loader;

namespace SSGEDotNet.AssemblyLoader;

public class GameAssemblyLoadContext : AssemblyLoadContext
{
    private readonly AssemblyDependencyResolver _resolver;

    public GameAssemblyLoadContext(string mainAssemblyToLoadPath) : base("GameAssemblyLoadContext", isCollectible: true)
    {
        _resolver = new AssemblyDependencyResolver(mainAssemblyToLoadPath);
    }

    protected override Assembly? Load(AssemblyName assemblyName)
    {
        string? assemblyPath = _resolver.ResolveAssemblyToPath(assemblyName);

        if (assemblyPath != null)
        {
            return LoadFromAssemblyPath(assemblyPath);
        }

        return null;
    }
}
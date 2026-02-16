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

    protected GameAssemblyLoadContext(string name, string mainAssemblyToLoadPath) : base(name, isCollectible: true)
    {
        _resolver = new AssemblyDependencyResolver(mainAssemblyToLoadPath);
    }

    protected override Assembly? Load(AssemblyName assemblyName)
    {
        var assemblyPath = _resolver.ResolveAssemblyToPath(assemblyName);

        return assemblyPath != null ? LoadFromAssemblyPath(assemblyPath) : null;
    }
}
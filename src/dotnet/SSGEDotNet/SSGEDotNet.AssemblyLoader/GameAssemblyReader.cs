using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using SSGEDotNet.AssemblyLoader.Models;

namespace SSGEDotNet.AssemblyLoader;

public static class GameAssemblyReader
{
    [MethodImpl(MethodImplOptions.NoInlining)]
    public static GameAssemblyInfo GetGameAssemblyInfo(string assemblyPath, string coreAssemblyName)
    {
        var assemblyFullPath = Path.GetFullPath(assemblyPath);

        var (weakReference, gameAssemblyInfo) =
            GetGameAssemblyInfoPrivate(coreAssemblyName, assemblyFullPath);

        var stopwatch = Stopwatch.StartNew();
        for (; weakReference.IsAlive && stopwatch.Elapsed.TotalSeconds <= 5;)
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        return weakReference.IsAlive
            ? throw new TimeoutException("Unable to unload game assembly in 5 seconds.")
            : gameAssemblyInfo;
    }

    private static (WeakReference, GameAssemblyInfo) GetGameAssemblyInfoPrivate(string coreAssemblyName,
        string assemblyFullPath)
    {
        var assemblyDirectory = Path.GetDirectoryName(assemblyFullPath)!;
        var tempGameAssemblyLoadContext = new TempGameAssemblyLoadContext(assemblyDirectory);
        var coreAssemblyPath = Path.Combine(assemblyDirectory, coreAssemblyName);

        var coreAssembly = tempGameAssemblyLoadContext.LoadFromAssemblyPath(coreAssemblyPath);
        var gameAssembly = tempGameAssemblyLoadContext.LoadFromAssemblyPath(assemblyFullPath);

        var baseComponentType = coreAssembly.GetType("SSGEDotNet.Core.Scene.Component") ??
                                throw new InvalidOperationException("Cannot find component type");

        var editorPropertyAttributeType =
            coreAssembly.GetType("SSGEDotNet.Core.Scene.Attributes.EditorPropertyAttribute") ??
            throw new InvalidOperationException("Cannot find editor property attribute type");

        var gameAssemblyInfo = new GameAssemblyInfo()
        {
            Name = gameAssembly.GetName().Name!,
            Components = GetComponentsInfo(gameAssembly, baseComponentType, editorPropertyAttributeType).ToArray()
        };

        tempGameAssemblyLoadContext.Unload();

        return
            (new WeakReference(tempGameAssemblyLoadContext,
                trackResurrection: true), gameAssemblyInfo);
    }

    private static IEnumerable<ScriptComponentInfo> GetComponentsInfo(Assembly gameAssembly, Type baseComponentType,
        Type editorPropertyAttributeType)
    {
        return gameAssembly.GetTypes().Where(t => t.IsAssignableTo(baseComponentType)).Select(componentType =>
            new ScriptComponentInfo()
            {
                Name = componentType.Name,
                Fullname = componentType.FullName!,
                Properties = GetComponentPropertiesInfo(componentType, editorPropertyAttributeType).ToArray()
            });
    }

    private static IEnumerable<ComponentPropertyInfo> GetComponentPropertiesInfo(Type componentType,
        Type editorPropertyAttributeType)
    {
        foreach (var property in componentType.GetProperties(BindingFlags.Public | BindingFlags.Instance))
        {
            if (property.GetCustomAttribute(editorPropertyAttributeType) is not null)
            {
                yield return new ComponentPropertyInfo()
                {
                    Name = property.Name,
                    Type = property.PropertyType.Name
                };
            }
        }
    }
}
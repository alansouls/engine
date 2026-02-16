// See https://aka.ms/new-console-template for more information

using SSGEDotNet.AssemblyLoader;

var assemblyPath = Path.GetFullPath("../../../../SSGEDotNet.Sample/bin/Debug/SSGEDotNet.Sample.dll");

var info = GameAssemblyReader.GetGameAssemblyInfo(assemblyPath, "SSGEDotNet.Core.dll");

Console.WriteLine($"Assembly Name: {info.Name}");

foreach (var component in info.Components)
{
    Console.WriteLine($"Component: {component.Name}");
    foreach (var property in component.Properties)
    {
        Console.WriteLine($"Property: {property.Name}");
    }
}
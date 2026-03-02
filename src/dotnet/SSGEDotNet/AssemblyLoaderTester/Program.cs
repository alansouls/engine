// See https://aka.ms/new-console-template for more information

using SSGEDotNet.AssemblyLoader;

var assemblyPath = Path.GetFullPath("../../../../SSGEDotNet.Sample/bin/Debug/SSGEDotNet.Sample.dll");

var info = GameAssemblyLoader.GetGameAssemblyInfo(assemblyPath);

GameAssemblyLoader.LoadGameAssembly(assemblyPath);

GameAssemblyLoader.UnloadGameAssembly();
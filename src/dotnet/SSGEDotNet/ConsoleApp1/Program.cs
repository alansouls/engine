// See https://aka.ms/new-console-template for more information

using SSGEDotNet.AssemblyLoader;

var path = "C:\\Users\\maiaa\\Documents\\Dev\\personal\\engine\\src\\dotnet\\SSGEDotNet\\SSGEDotNet.Sample\\bin\\Debug\\SSGEDotNet.Sample.dll";

GameAssemblyLoader.LoadGameAssembly(path);

GameAssemblyLoader.UnloadGameAssembly();

Console.Read();

GameAssemblyLoader.LoadGameAssembly(path);
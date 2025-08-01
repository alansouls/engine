#include "CSharpExecutionEngine.h"

#include <cassert>
#include <hostfxr.h>
#include <iostream>
#include <nethost.h>

#ifdef WINDOWS
#include <Windows.h>

#define STR(s) L##s
#define CH(c) L##c
#define DIR_SEPARATOR L'\\'

#define string_compare wcscmp

#else
#include <dlfcn.h>
#include <limits.h>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#define string_compare strcmp

#endif

#ifdef WINDOWS

void *load_library(const char_t *path)
{
    HMODULE h = ::LoadLibraryW(path);
    assert(h != nullptr);
    return (void *)h;
}

void *get_export(void *h, const char *name)
{
    void *f = ::GetProcAddress((HMODULE)h, name);
    assert(f != nullptr);
    return f;
}
#else

void *load_library(const char_t *path)
{
    void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    assert(h != nullptr);
    return h;
}

void *get_export(void *h, const char *name)
{
    void *f = dlsym(h, name);
    assert(f != nullptr);
    return f;
}

#endif

hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
hostfxr_close_fn close_fptr = nullptr;

// Using the nethost library, discover the location of hostfxr and get exports
bool load_hostfxr()
{
    // Pre-allocate a large buffer for the path to hostfxr
    char_t buffer[4096];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0)
        return false;

    // Load hostfxr and get desired exports
    void *lib = load_library(buffer);
    init_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

    return (init_fptr && get_delegate_fptr && close_fptr);
}

// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path)
{
    // Load .NET Core
    void *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle cxt = nullptr;
    int rc = init_fptr(config_path, nullptr, &cxt);
    if (rc != 0 || cxt == nullptr)
    {
        std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
        close_fptr(cxt);
        return nullptr;
    }

    // Get the load assembly function pointer
    rc = get_delegate_fptr(cxt, hdt_load_assembly_and_get_function_pointer, &load_assembly_and_get_function_pointer);
    if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

    close_fptr(cxt);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}

SSGE::CSharpExecutionEngine::CSharpExecutionEngine(std::string projectName, std::filesystem::path dotnetProjectPath)
    : m_projectName(std::move(projectName)), m_dotnetProjectPath(std::move(dotnetProjectPath)),
      m_loadAndGetFunctionPointer()
{
}

auto SSGE::CSharpExecutionEngine::init() -> void
{
    load_hostfxr();
}

auto SSGE::CSharpExecutionEngine::compile() -> bool
{
    std::filesystem::path dotNetProjectLocation =
        m_dotnetProjectPath / m_projectName / std::format("{}.csproj", m_projectName);
    auto command = std::format("dotnet build \"{}\" -c Debug", dotNetProjectLocation.string());

    if (std::system(command.c_str()))
    {
        std::cout << "Failed to compile C# project: " << m_dotnetProjectPath.string() << std::endl;
        return false;
    }

    auto dllName = std::format("{}.dll", m_projectName);
    auto depsJsonName = std::format("{}.deps.json", m_projectName);
    auto pdbName = std::format("{}.pdb", m_projectName);

    std::filesystem::copy_file(m_dotnetProjectPath / m_projectName / "bin" / "Debug" / dllName,
                               std::format("./{}", dllName), std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(m_dotnetProjectPath / m_projectName / "bin" / "Debug" / depsJsonName,
                               std::format("./{}", depsJsonName), std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(m_dotnetProjectPath / m_projectName / "bin" / "Debug" / pdbName,
                               std::format("./{}", pdbName), std::filesystem::copy_options::overwrite_existing);

    if (int rc = execute("SSGEDotNet.Core.AssemblyUtils.GameAssemblyLoader", "LoadGameAssembly",
                         (void *)dllName.c_str(), static_cast<int32_t>(dllName.length()));
        rc != 0)
    {
        std::cerr << "Failed to load game assembly: " << std::hex << std::showbase << rc << std::endl;
        return false;
    }

    return true;
}

auto SSGE::CSharpExecutionEngine::execute(const std::string_view &entryPointClass,
                                          const std::string_view &entryPointMethod, void *data, int32_t dataLength)
    -> int
{
    auto entryPoint =
        reinterpret_cast<component_entry_point_fn>(getEntryPointFunctionPointer(entryPointClass, entryPointMethod));

    if (entryPoint == nullptr)
    {
        return ErrorGettingEntryPointFunctionPointerCode;
    }

    return entryPoint(data, dataLength);
}

auto SSGE::CSharpExecutionEngine::getEntryPointFunctionPointer(const std::string_view &entryPointClass,
                                                               const std::string_view &entryPointMethod) -> void *
{
    static constexpr std::string_view EngineDotNetDllName = "SSGEDotNet.Core";
    static constexpr std::string_view EngineDotNetDllPath = "./SSGEDotNet.Core";
    void *entryPoint = m_componentEntryPoints[std::string(entryPointClass) + std::string(entryPointMethod)];

    if (entryPoint != nullptr)
    {
        return entryPoint;
    }

    std::filesystem::path assemblyPath = std::format("{}.dll", EngineDotNetDllPath);
    std::filesystem::path runtimeConfigPath = std::format("{}.runtimeconfig.json", EngineDotNetDllPath);
    m_loadAndGetFunctionPointer =
        m_loadAndGetFunctionPointer ? m_loadAndGetFunctionPointer : get_dotnet_load_assembly(runtimeConfigPath.c_str());
    const auto fullClassName = std::format("{}, {}", entryPointClass, EngineDotNetDllName);
    const char_t *entryPointCStr;
    const char_t *entryPointMethodCStr;
#ifdef WINDOWS
    const std::wstring entryPointWStr(fullClassName.begin(), fullClassName.end());
    entryPointCStr = entryPointWStr.c_str();
    const std::wstring entryPointMethodWStr(entryPointMethod.begin(), entryPointMethod.end());
    entryPointMethodCStr = entryPointMethodWStr.c_str();
#else
    entryPointCStr = entryPointClass.c_str();
    entryPointMethodCStr = entryPointMethod.c_str();
#endif
    if (int rc = m_loadAndGetFunctionPointer(assemblyPath.c_str(), entryPointCStr, entryPointMethodCStr, nullptr,
                                             nullptr, &entryPoint);
        rc != 0)
    {
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;
        return nullptr;
    }

    m_componentEntryPoints[std::string(entryPointClass) + std::string(entryPointMethod)] = entryPoint;

    return entryPoint;
}

std::unique_ptr<SSGE::CSharpExecutionEngine> SSGE::CSharpExecutionEngine::s_instance = nullptr;

auto SSGE::CSharpExecutionEngine::GetOrInitialize(const std::string &projectName,
                                                  const std::filesystem::path &dotnetProjectPath)
    -> CSharpExecutionEngine *
{
    if (s_instance != nullptr)
    {
        return s_instance.get();
    }

    s_instance.reset(new CSharpExecutionEngine(projectName, dotnetProjectPath));

    s_instance->init();

    return s_instance.get();
}

auto SSGE::CSharpExecutionEngine::Get() -> CSharpExecutionEngine *
{
    if (s_instance == nullptr)
    {
        throw std::runtime_error("CSharpExecutionEngine is not initialized. Call GetOrInitialize first.");
    }

    return s_instance.get();
}
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
    : ScriptExecutionEngine(), m_projectName(std::move(projectName)), m_dotnetProjectPath(std::move(dotnetProjectPath)),
      m_loadAndGetFunctionPointer()
{
}

auto SSGE::CSharpExecutionEngine::init() -> void
{
    load_hostfxr();
}

auto SSGE::CSharpExecutionEngine::compile() -> bool
{
    const auto command = std::format("dotnet build \"{}\" -c Debug", m_dotnetProjectPath.string());

    if (std::system(command.c_str()))
    {
        std::cout << "Failed to compile C# project: " << m_dotnetProjectPath.string() << std::endl;
        return false;
    }

    return true;
}

auto SSGE::CSharpExecutionEngine::execute(const std::string &entryPointClass, const std::string &entryPointMethod)
    -> void
{
    component_entry_point_fn entryPoint = m_componentEntryPoints[entryPointClass + entryPointMethod];

    if (entryPoint != nullptr)
    {
        entryPoint(nullptr, 0);
        return;
    }

    auto assemblyPath = m_dotnetProjectPath / m_projectName / "bin" / "Debug" / std::format("{}.dll", m_projectName);
    auto runtimeConfigPath =
        m_dotnetProjectPath / m_projectName / "bin" / "Debug" / std::format("{}.runtimeconfig.json", m_projectName);
    m_loadAndGetFunctionPointer =
        m_loadAndGetFunctionPointer ? m_loadAndGetFunctionPointer : get_dotnet_load_assembly(runtimeConfigPath.c_str());
    const auto fullClassName = std::format("{}, {}", entryPointClass, m_projectName);
    const std::wstring entryPointWStr(fullClassName.begin(), fullClassName.end());
    const std::wstring entryPointMethodWStr(entryPointMethod.begin(), entryPointMethod.end());
    if (int rc = m_loadAndGetFunctionPointer(assemblyPath.c_str(), entryPointWStr.c_str(), entryPointMethodWStr.c_str(),
                                             nullptr, nullptr, (void **)&entryPoint);
        rc != 0)
    {
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;
        return;
    }

    m_componentEntryPoints[entryPointClass + entryPointMethod] = entryPoint;
    entryPoint(nullptr, 0);
}

std::unordered_map<SSGE::ScriptComponent::ScriptType, std::unique_ptr<SSGE::ScriptExecutionEngine>>
    SSGE::ScriptExecutionEngine::s_engines;

auto SSGE::ScriptExecutionEngine::CreateExecutionEngine(ScriptComponent::ScriptType type) -> ScriptExecutionEngine *
{
    std::unique_ptr<ScriptExecutionEngine> &instance = s_engines[type];
    if (instance != nullptr)
    {
        return instance.get();
    }
    // TODO: Make this path configurable
    ScriptExecutionEngine *engine;
    switch (type)
    {
    case ScriptComponent::CSharp:
        engine = new CSharpExecutionEngine("SSGEDotNet.Core",
                                           R"(C:\Users\maiaa\Documents\Dev\personal\engine\src\dotnet\SSGEDotNet)");
        break;
    default:
        throw std::runtime_error("Invalid script type specified for ScriptExecutionEngine creation");
    }
    engine->init();
    instance = std::unique_ptr<ScriptExecutionEngine>(engine);
    return engine;
}
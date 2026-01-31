#include "CSharpExecutionEngine.h"

#include "../input/InputState.h"
#include <cassert>
#include <format>
#include <hostfxr.h>
#include <iostream>
#include <memory>
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

void* load_library(const char_t* path)
{
    HMODULE h = ::LoadLibraryW(path);
    assert(h != nullptr);
    return (void*)h;
}

void* get_export(void* h, const char* name)
{
    void* f = ::GetProcAddress(static_cast<HMODULE>(h), name);
    assert(f != nullptr);
    return f;
}
#else

void* load_library(const char_t* path)
{
    void* h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    assert(h != nullptr);
    return h;
}

void* get_export(void* h, const char* name)
{
    void* f = dlsym(h, name);
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
    {
        std::cout << getenv("PATH") << "\nError loading hostfxr: 0x" << std::hex << rc << std::endl;
        return false;
    }

    // Load hostfxr and get desired exports
    void* lib = load_library(buffer);
    init_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

    return (init_fptr && get_delegate_fptr && close_fptr);
}

// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t* config_path)
{
    // Load .NET Core
    void* load_assembly_and_get_function_pointer = nullptr;
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

auto SSGE::CSharpExecutionEngine::GetOrInitialize() -> CSharpExecutionEngine*
{
    if (s_instance != nullptr)
    {
        return s_instance.get();
    }

    s_instance.reset(new CSharpExecutionEngine());

    s_instance->init();

    return s_instance.get();
}

auto SSGE::CSharpExecutionEngine::Get() -> CSharpExecutionEngine*
{
    if (s_instance == nullptr)
    {
        throw std::runtime_error("CSharpExecutionEngine is not initialized. Call GetOrInitialize first.");
    }

    return s_instance.get();
}

SSGE::CSharpExecutionEngine::CSharpExecutionEngine()
    : m_loadAndGetFunctionPointer(), m_compiled(false), m_setInputStateFn(nullptr)
{
}

auto SSGE::CSharpExecutionEngine::init() -> void
{
    if (!load_hostfxr())
    {
        throw std::runtime_error("Failed to load hostfxr");
    }
}

auto SSGE::CSharpExecutionEngine::loadGameAssembly(const std::string& dllName) -> bool
{
    if (int rc = execute("SSGEDotNet.AssemblyLoader.GameAssemblyLoader", "LoadGameAssembly", (void*)dllName.c_str(),
                         static_cast<int32_t>(dllName.length()));
        rc != 0)
    {
        std::cerr << "Failed to load game assembly: " << std::hex << std::showbase << rc << std::endl;
        return false;
    }

    return true;
}

auto SSGE::CSharpExecutionEngine::execute(const std::string_view& entryPointClass,
                                          const std::string_view& entryPointMethod, void* data, int32_t dataLength)
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

auto SSGE::CSharpExecutionEngine::getComponentEntryPointFunctions() -> std::array<component_entry_point_fn, 3>
{
    if (m_componentEntryPointFunctions.has_value())
    {
        return m_componentEntryPointFunctions.value();
    }

    auto entryPoint = reinterpret_cast<returns_ptr_fn>(getEntryPointFunctionPointer(
        GetCoreEntryPointFunctionsClassName, GetCoreEntryPointFunctionsMethodName, UNMANAGEDCALLERSONLY_METHOD));

    if (entryPoint == nullptr)
    {
        return std::array<component_entry_point_fn, 3>{nullptr};
    }

    void* ptr = entryPoint();

    m_componentEntryPointFunctions =
        std::array{
            reinterpret_cast<component_entry_point_fn>(static_cast<uintptr_t*>(ptr)[0]),
            reinterpret_cast<component_entry_point_fn>(static_cast<uintptr_t*>(ptr)[1]),
            reinterpret_cast<component_entry_point_fn>(static_cast<uintptr_t*>(ptr)[2])
        };

    m_setInputStateFn = reinterpret_cast<set_input_state_fn>(static_cast<uintptr_t*>(ptr)[3]);

    return m_componentEntryPointFunctions.value();
}

auto SSGE::CSharpExecutionEngine::setInputState(const InputState* inputState) -> void
{
    if (m_setInputStateFn == nullptr)
    {
        getComponentEntryPointFunctions();
    }

    if (m_setInputStateFn != nullptr)
    {
        m_setInputStateFn(const_cast<InputState*>(inputState));
    }
}

std::unique_ptr<SSGE::CSharpExecutionEngine> SSGE::CSharpExecutionEngine::s_instance = nullptr;

auto SSGE::CSharpExecutionEngine::getEntryPointFunctionPointer(const std::string_view& entryPointClass,
                                                               const std::string_view& entryPointMethod,
                                                               const char_t* delegateTypeName) -> void*
{
    static constexpr std::string_view EngineDotNetDllName = "SSGEDotNet.AssemblyLoader";
    static constexpr std::string_view EngineDotNetDllPath = "./SSGEDotNet.AssemblyLoader";
    void* entryPoint = m_componentEntryPoints[std::string(entryPointClass) + std::string(entryPointMethod)];

    if (entryPoint != nullptr)
    {
        return entryPoint;
    }

    std::filesystem::path assemblyPath = std::format("{}.dll", EngineDotNetDllPath);
    std::filesystem::path runtimeConfigPath = std::format("{}.runtimeconfig.json", EngineDotNetDllPath);
    m_loadAndGetFunctionPointer =
        m_loadAndGetFunctionPointer ? m_loadAndGetFunctionPointer : get_dotnet_load_assembly(runtimeConfigPath.c_str());
    const auto fullClassName = std::format("{}, {}", entryPointClass, EngineDotNetDllName);
    const char_t* entryPointCStr;
    const char_t* entryPointMethodCStr;
#ifdef WINDOWS
    const std::wstring entryPointWStr(fullClassName.begin(), fullClassName.end());
    entryPointCStr = entryPointWStr.c_str();
    const std::wstring entryPointMethodWStr(entryPointMethod.begin(), entryPointMethod.end());
    entryPointMethodCStr = entryPointMethodWStr.c_str();
#else
    const std::string entryPointStr(fullClassName.begin(), fullClassName.end());
    entryPointCStr = entryPointStr.c_str();
    const std::string entryPointMethodStr(entryPointMethod.begin(), entryPointMethod.end());
    entryPointMethodCStr = entryPointMethodStr.c_str();
#endif
    if (int rc = m_loadAndGetFunctionPointer(assemblyPath.c_str(), entryPointCStr, entryPointMethodCStr,
                                             delegateTypeName, nullptr, &entryPoint);
        rc != 0)
    {
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;
        return nullptr;
    }

    m_componentEntryPoints[std::string(entryPointClass) + std::string(entryPointMethod)] = entryPoint;

    return entryPoint;
}

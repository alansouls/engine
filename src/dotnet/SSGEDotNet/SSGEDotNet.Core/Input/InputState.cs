using SSGEDotNet.Core.Constants;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SSGEDotNet.Core.Input;

public enum KeyState
{
    None = 0,
    Released = 1,
    Pressed = 2,
    Held = 3
}

public struct MousePosition
{
    public double X { get; set; }
    public double Y { get; set; }
}

public class InputState
{
    private IntPtr _nativeInputStatePtr = IntPtr.Zero;

    internal static InputState? Instance { get; private set; }

    private InputState(IntPtr inputStatePtr)
    {
        _nativeInputStatePtr = inputStatePtr;
    }

    internal static void SetNativeInputState(IntPtr inputStatePtr)
    {
        Instance = new InputState(inputStatePtr);
    }

    // P/Invoke declarations
    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern int InputState_GetKeyState(IntPtr inputState, int key);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern int InputState_GetMouseButtonState(IntPtr inputState, int button);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMousePositionX(IntPtr inputState);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMousePositionY(IntPtr inputState);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseDeltaX(IntPtr inputState);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseDeltaY(IntPtr inputState);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseScrollX(IntPtr inputState);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseScrollY(IntPtr inputState);

    // Key state queries
    public bool IsKeyPressed(InputKey key)
    {
        return GetKeyState(key) == KeyState.Pressed;
    }

    public bool IsKeyReleased(InputKey key)
    {
        return GetKeyState(key) == KeyState.Released;
    }

    public bool IsKeyHeld(InputKey key)
    {
        var state = GetKeyState(key);
        return state == KeyState.Held || state == KeyState.Pressed;
    }

    public KeyState GetKeyState(InputKey key)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return KeyState.Released;
        return (KeyState)InputState_GetKeyState(_nativeInputStatePtr, (int)key);
    }

    // Mouse button state queries
    public bool IsMouseButtonPressed(InputMouseButton button)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return false;
        return InputState_GetMouseButtonState(_nativeInputStatePtr, (int)button) == (int)KeyState.Pressed;
    }

    public bool IsMouseButtonReleased(InputMouseButton button)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return true;
        return InputState_GetMouseButtonState(_nativeInputStatePtr, (int)button) == (int)KeyState.Released;
    }

    public bool IsMouseButtonHeld(InputMouseButton button)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return false;
        var state = (KeyState)InputState_GetMouseButtonState(_nativeInputStatePtr, (int)button);
        return state == KeyState.Held || state == KeyState.Pressed;
    }

    // Mouse position
    public MousePosition GetMousePosition()
    {
        if (_nativeInputStatePtr == IntPtr.Zero) 
            return new MousePosition { X = 0, Y = 0 };

        return new MousePosition
        {
            X = InputState_GetMousePositionX(_nativeInputStatePtr),
            Y = InputState_GetMousePositionY(_nativeInputStatePtr)
        };
    }

    public MousePosition GetMouseDelta()
    {
        if (_nativeInputStatePtr == IntPtr.Zero) 
            return new MousePosition { X = 0, Y = 0 };

        return new MousePosition
        {
            X = InputState_GetMouseDeltaX(_nativeInputStatePtr),
            Y = InputState_GetMouseDeltaY(_nativeInputStatePtr)
        };
    }

    // Mouse scroll
    public MousePosition GetMouseScroll()
    {
        if (_nativeInputStatePtr == IntPtr.Zero) 
            return new MousePosition { X = 0, Y = 0 };

        return new MousePosition
        {
            X = InputState_GetMouseScrollX(_nativeInputStatePtr),
            Y = InputState_GetMouseScrollY(_nativeInputStatePtr)
        };
    }
}

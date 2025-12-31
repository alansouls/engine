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

public enum MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Count = 5
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
    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int InputState_GetKeyState(IntPtr inputState, int key);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int InputState_GetMouseButtonState(IntPtr inputState, int button);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMousePositionX(IntPtr inputState);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMousePositionY(IntPtr inputState);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseDeltaX(IntPtr inputState);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseDeltaY(IntPtr inputState);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseScrollX(IntPtr inputState);

    [DllImport("SSGEEngine.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double InputState_GetMouseScrollY(IntPtr inputState);

    // Key state queries
    public bool IsKeyPressed(int key)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return false;
        return InputState_GetKeyState(_nativeInputStatePtr, key) == (int)KeyState.Pressed;
    }

    public bool IsKeyReleased(int key)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return true;
        return InputState_GetKeyState(_nativeInputStatePtr, key) == (int)KeyState.Released;
    }

    public bool IsKeyHeld(int key)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return false;
        var state = (KeyState)InputState_GetKeyState(_nativeInputStatePtr, key);
        return state == KeyState.Held || state == KeyState.Pressed;
    }

    public KeyState GetKeyState(int key)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return KeyState.Released;
        return (KeyState)InputState_GetKeyState(_nativeInputStatePtr, key);
    }

    // Mouse button state queries
    public bool IsMouseButtonPressed(MouseButton button)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return false;
        return InputState_GetMouseButtonState(_nativeInputStatePtr, (int)button) == (int)KeyState.Pressed;
    }

    public bool IsMouseButtonReleased(MouseButton button)
    {
        if (_nativeInputStatePtr == IntPtr.Zero) return true;
        return InputState_GetMouseButtonState(_nativeInputStatePtr, (int)button) == (int)KeyState.Released;
    }

    public bool IsMouseButtonHeld(MouseButton button)
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

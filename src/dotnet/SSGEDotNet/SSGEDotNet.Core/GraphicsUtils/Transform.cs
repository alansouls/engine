using System.Runtime.InteropServices;
using SSGEDotNet.Core.Math;

namespace SSGEDotNet.Core.GraphicsUtils;

internal class NativeTransform
{
    private readonly IntPtr _handle;

    public NativeTransform()
    {
        if (_allocateTransformDelegate == null)
        {
            throw new InvalidOperationException("Function pointers not initialized. Call InitializeFunctionPointers first.");
        }
        
        _handle = _allocateTransformDelegate.Invoke();
        
        if (_handle == IntPtr.Zero)
        {
            throw new InvalidOperationException("Failed to allocate native transform.");
        }
    }
    
    private delegate void TranslateDelegate(IntPtr transformPtr, IntPtr translationPtr);
    private delegate void ScaleDelegate(IntPtr transformPtr, IntPtr translationPtr);
    private delegate void RotateDelegate(IntPtr transformPtr, IntPtr rotationPtr);
    private delegate IntPtr GetMatrixDelegate(IntPtr transformPtr);
    private delegate IntPtr GetPosition(IntPtr transformPtr);
    private delegate IntPtr AllocateTransformDelegate();


    private const int FunctionsCount = 6;
    private static TranslateDelegate? _translateFunction;
    private static ScaleDelegate? _scaleFunction;
    private static RotateDelegate? _rotateFunction;
    private static GetMatrixDelegate? _getMatrixFunction;
    private static GetPosition? _getPositionFunction;
    private static AllocateTransformDelegate? _allocateTransformDelegate;
    
    public static void InitializeFunctionPointers(IntPtr arg, int argLength)
    {
        if (argLength < FunctionsCount)
        {
            throw new ArgumentException("Insufficient arguments provided to initialize function pointers.");
        }
        
        _translateFunction = Marshal.GetDelegateForFunctionPointer<TranslateDelegate>(Marshal.ReadIntPtr(arg, 0));
        _scaleFunction = Marshal.GetDelegateForFunctionPointer<ScaleDelegate>(Marshal.ReadIntPtr(arg, IntPtr.Size));
        _rotateFunction = Marshal.GetDelegateForFunctionPointer<RotateDelegate>(Marshal.ReadIntPtr(arg, IntPtr.Size * 2));
        _getMatrixFunction = Marshal.GetDelegateForFunctionPointer<GetMatrixDelegate>(Marshal.ReadIntPtr(arg, IntPtr.Size * 3));
        _getPositionFunction = Marshal.GetDelegateForFunctionPointer<GetPosition>(Marshal.ReadIntPtr(arg, IntPtr.Size * 4));
        _allocateTransformDelegate = Marshal.GetDelegateForFunctionPointer<AllocateTransformDelegate>(Marshal.ReadIntPtr(arg, IntPtr.Size * 5));
    }
    
    public void Translate(Vec3 translation)
    {
        if (_translateFunction == null)
        {
            throw new InvalidOperationException("Function pointers not initialized. Call InitializeFunctionPointers first.");
        }
        
        if (_handle == IntPtr.Zero)
        {
            throw new InvalidOperationException("Transform handle is not set.");
        }
        
        IntPtr translationPtr = Marshal.AllocHGlobal(Marshal.SizeOf(translation));
        try
        {
            Marshal.StructureToPtr(translation, translationPtr, false);
            _translateFunction.Invoke(_handle, translationPtr);
        }
        finally
        {
            Marshal.FreeHGlobal(translationPtr);
        }
    }
    
    public void Scale(Vec3 scale)
    {
        if (_scaleFunction == null)
        {
            throw new InvalidOperationException("Function pointers not initialized. Call InitializeFunctionPointers first.");
        }
        
        if (_handle == IntPtr.Zero)
        {
            throw new InvalidOperationException("Transform handle is not set.");
        }
        
        IntPtr scalePtr = Marshal.AllocHGlobal(Marshal.SizeOf(scale));
        try
        {
            Marshal.StructureToPtr(scale, scalePtr, false);
            _scaleFunction.Invoke(_handle, scalePtr);
        }
        finally
        {
            Marshal.FreeHGlobal(scalePtr);
        }
    }
    
    public void Rotate(Vec3 rotation)
    {
        if (_rotateFunction == null)
        {
            throw new InvalidOperationException("Function pointers not initialized. Call InitializeFunctionPointers first.");
        }
        
        if (_handle == IntPtr.Zero)
        {
            throw new InvalidOperationException("Transform handle is not set.");
        }
        
        IntPtr rotationPtr = Marshal.AllocHGlobal(Marshal.SizeOf(rotation));
        try
        {
            Marshal.StructureToPtr(rotation, rotationPtr, false);
            _rotateFunction.Invoke(_handle, rotationPtr);
        }
        finally
        {
            Marshal.FreeHGlobal(rotationPtr);
        }
    }
}

public class Transform
{
    private readonly NativeTransform _nativeTransform = new();

    public void Translate(Vec3 translation)
    {
        _nativeTransform.Translate(translation);
    }
    
    public void Scale(Vec3 scale)
    {
        _nativeTransform.Scale(scale);
    }
    
    public void Rotate(Vec3 rotation)
    {
        _nativeTransform.Rotate(rotation);
    }
}
using System.Runtime.InteropServices;
using SSGEDotNet.Core.Constants;
using SSGEDotNet.Core.Math;

namespace SSGEDotNet.Core.GraphicsUtils;

public class Transform : IDisposable
{
    private IntPtr _nativeTransformPtr = IntPtr.Zero;
    private bool _disposed = false;

    public Transform()
    {
        _nativeTransformPtr = Transform_Create();
        if (_nativeTransformPtr == IntPtr.Zero)
        {
            throw new InvalidOperationException("Failed to create native Transform.");
        }
    }

    internal Transform(IntPtr nativaTransformPtr)
    {
        _nativeTransformPtr = nativaTransformPtr;
        if (_nativeTransformPtr == IntPtr.Zero)
        {
            throw new InvalidOperationException("Failed to create native Transform.");
        }
    }

    // P/Invoke declarations
    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr Transform_Create();

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void Transform_Destroy(IntPtr transform);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void Transform_Translate(IntPtr transform, float x, float y, float z);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void Transform_Scale(IntPtr transform, float x, float y, float z);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern void Transform_Rotate(IntPtr transform, float angle, float axisX, float axisY, float axisZ);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern float Transform_GetPositionX(IntPtr transform);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern float Transform_GetPositionY(IntPtr transform);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern float Transform_GetPositionZ(IntPtr transform);

    [DllImport(InteropConstants.SSGEEngineDll, CallingConvention = CallingConvention.Cdecl)]
    private static extern float Transform_GetMatrixElement(IntPtr transform, int row, int col);

    // Public methods
    public void Translate(Vec3 translation)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Translate(_nativeTransformPtr, translation.X, translation.Y, translation.Z);
    }

    public void Translate(float x, float y, float z)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Translate(_nativeTransformPtr, x, y, z);
    }

    public void Scale(Vec3 scale)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Scale(_nativeTransformPtr, scale.X, scale.Y, scale.Z);
    }

    public void Scale(float x, float y, float z)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Scale(_nativeTransformPtr, x, y, z);
    }

    public void Rotate(float angle, Vec3 axis)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Rotate(_nativeTransformPtr, angle, axis.X, axis.Y, axis.Z);
    }

    public void Rotate(float angle, float axisX, float axisY, float axisZ)
    {
        if (_nativeTransformPtr == IntPtr.Zero) return;
        Transform_Rotate(_nativeTransformPtr, angle, axisX, axisY, axisZ);
    }

    public Vec3 GetPosition()
    {
        if (_nativeTransformPtr == IntPtr.Zero)
            return new Vec3(0, 0, 0);

        return new Vec3(
            Transform_GetPositionX(_nativeTransformPtr),
            Transform_GetPositionY(_nativeTransformPtr),
            Transform_GetPositionZ(_nativeTransformPtr)
        );
    }

    public float[,] GetMatrix()
    {
        if (_nativeTransformPtr == IntPtr.Zero)
            return new float[4, 4];

        float[,] matrix = new float[4, 4];
        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                matrix[row, col] = Transform_GetMatrixElement(_nativeTransformPtr, row, col);
            }
        }
        return matrix;
    }

    // IDisposable implementation
    protected virtual void Dispose(bool disposing)
    {
        if (!_disposed)
        {
            if (_nativeTransformPtr != IntPtr.Zero)
            {
                Transform_Destroy(_nativeTransformPtr);
                _nativeTransformPtr = IntPtr.Zero;
            }
            _disposed = true;
        }
    }

    ~Transform()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
}
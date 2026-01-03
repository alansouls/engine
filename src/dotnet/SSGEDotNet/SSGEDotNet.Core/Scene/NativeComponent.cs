namespace SSGEDotNet.Core.Scene;

public abstract class NativeComponent : Component
{
    protected readonly IntPtr _nativePtr;

    public NativeComponent(IntPtr nativePtr)
    {
        _nativePtr = nativePtr;
    }

    public override void Init()
    {
    }

    public override void Update()
    {
    }
}

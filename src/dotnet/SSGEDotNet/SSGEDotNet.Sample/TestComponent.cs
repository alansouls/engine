using SSGEDotNet.Core.Scene;

namespace SSGEDotNet.Sample;

public sealed class TestComponent : Component
{
    private int _frameCount = 0;
    public override void Init()
    {
        _frameCount = 1;
        Console.WriteLine("TestComponent initialized.");
    }

    public override void Update()
    {
        if (_frameCount % 60 != 0)
        {
            _frameCount++;
            return;
        }
        
        Console.WriteLine($"TestComponent Update called after 60 frames.");
        _frameCount = 1;
    }
}
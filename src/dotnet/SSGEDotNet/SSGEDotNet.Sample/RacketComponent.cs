using SSGEDotNet.Core.Scene;

namespace SSGEDotNet.Sample;

public sealed class RacketComponent : Component
{
    private int _frameCount = 0;
    public override void Init()
    {
        _frameCount = 1;
        Console.WriteLine("RacketComponent initialized.");
    }

    public override void Update()
    {
        if (_frameCount % 60 != 0) 
            return;
        
        Console.WriteLine($"RacketComponent Update called after 60 frames.");
        _frameCount = 1;
    }
}
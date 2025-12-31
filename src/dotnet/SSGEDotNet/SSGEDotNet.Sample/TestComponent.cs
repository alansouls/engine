using SSGEDotNet.Core.Input;
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
        if (GameObject.Input.IsKeyPressed(InputKey.KeySpace))
        {
            Console.WriteLine("Space key pressed!");
        }
        if (GameObject.Input.IsKeyHeld(InputKey.KeySpace))
        {
            Console.WriteLine("Space key is held");
        }
        if (GameObject.Input.IsKeyReleased(InputKey.KeySpace))
        {
            Console.WriteLine("Space key is released");
        }
    }
}
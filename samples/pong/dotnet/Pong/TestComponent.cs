using SSGEDotNet.Core.Scene;

namespace Pong;

public class TestComponent : Component
{
  public override void Init()
  {
    Console.WriteLine("TestComponent Init");
  }

  public override void Update()
  {
    Console.WriteLine("TestComponent Update");
  }
}

using SSGEDotNet.Core.Scene;
using SSGEDotNet.Core.Scene.Attributes;

namespace SSGEDotNet.Sample;

public class RacketComponent : Component
{
    [EditorProperty]
    public bool IsLeft { get; set; }
    
    public override void Init()
    {
        throw new NotImplementedException();
    }

    public override void Update()
    {
        throw new NotImplementedException();
    }
}
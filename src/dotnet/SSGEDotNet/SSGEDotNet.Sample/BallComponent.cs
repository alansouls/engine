using SSGEDotNet.Core.Scene;
using System;
using System.Collections.Generic;
using System.Text;

namespace SSGEDotNet.Sample;

public class BallComponent : Component
{
    private bool _isMoving;

    public override void Init()
    {
        _isMoving = true;
    }

    public override void Update()
    {
    }
}

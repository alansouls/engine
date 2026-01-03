using SSGEDotNet.Core.Scene;
using SSGEDotNet.Core.Scene.Attributes;
using System.Numerics;

namespace SSGEDotNet.Sample;

public class RacketComponent : Component
{
    private const float RacketWidth = 50.0f;
    private const float RacketHeightRatio = 0.15f;

    private float _originalWindowWidth;
    private float _originalWindowHeight;
    private float _lastWindowWidth;
    private float _lastWindowHeight;
    private float _width;
    private float _height;
    private DateTime m_lastTime;
    private float m_bottomLimit;
    private int m_currentStep;

    [EditorProperty]
    public bool IsLeft { get; set; }

    public override void Init()
    {
        var properties = Game.Instance.GetProperties();
        _originalWindowWidth = properties.Width;
        _originalWindowHeight = properties.Height;
        _lastWindowWidth = _originalWindowWidth;
        _lastWindowHeight = _originalWindowHeight;
        _width = RacketWidth;
        _height = RacketHeightRatio * _lastWindowHeight;
        float middle = (_lastWindowHeight - _height) / 2;
        Vector2 topLeft = new(0.0f, 0.0f);
        var quad = GameObject.GetComponent<QuadRendererComponent>() ?? throw new Exception("QuadRendererComponent not found on GameObject.");
        if (IsLeft)
        {
            GameObject.Transform.Translate(10.0f, middle, 0.0f);
            quad.Width = _width;
            quad.Height = _height;
            quad.FillColor = new Vector4(0.0f, 1.0f, 0.0f, 1.0f);
        }
        else
        {
            GameObject.Transform.Translate(_lastWindowWidth - _width - 10.0f, middle, 0.0f);
            quad.Width = _width;
            quad.Height = _height;
            quad.FillColor = new Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        //auto & collider = gameObject()->addComponent<SSGE::QuadCollider>(false, gameObject(), topLeft, _width, _height);
        //collider.setLayer("racket");
        m_lastTime = DateTime.UtcNow;
        if (IsLeft)
            m_currentStep = 0;
        else
            m_currentStep = 1;
        m_bottomLimit = properties.Height - _height - 5.0f;
    }

    public override void Update()
    {
    }
}
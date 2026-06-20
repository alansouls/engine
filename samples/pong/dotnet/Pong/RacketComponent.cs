using SSGEDotNet.Core.GraphicsUtils;
using SSGEDotNet.Core.Input;
using SSGEDotNet.Core.Scene;
using SSGEDotNet.Core.Scene.Attributes;
using SSGEDotNet.Core.Scene.Colliders;
using System.Numerics;

namespace Pong;

public class RacketComponent : Component
{
    private const float RacketWidth = 50.0f;
    private const float RacketHeightRatio = 0.15f;
    private const float _topLimit = 5.0f;

    private float _originalWindowWidth;
    private float _originalWindowHeight;
    private float _lastWindowWidth;
    private float _lastWindowHeight;
    private float _width;
    private float _height;
    private DateTime _lastTime;
    private float _bottomLimit;
    private int _currentStep;
    private int _direction = 0; // -1 for up, 1 for down, 0 for stationary
    private float[] _steps = [-1.0f, 1.0f];

    [EditorProperty]
    public bool IsLeft { get; set; }

    [EditorProperty] public string Str { get; set; } = string.Empty;

    [EditorProperty]
    public float F { get; set; }

    [EditorProperty]
    public Vector2 V2 { get; set; }

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
        var collider = GameObject.GetComponent<QuadColliderComponent>() ?? throw new Exception("QuadColliderComponent not found on GameObject.");
        collider.Width = _width;
        collider.Height = _height;
        collider.Layer = "racket";
        _lastTime = DateTime.UtcNow;
        if (IsLeft)
            _currentStep = 0;
        else
            _currentStep = 1;
        _bottomLimit = properties.Height - _height - 5.0f;
    }

    public override void Update()
    {
        MoveToBoundaries();
        HandleInput();
        var rendererItem = GameObject.GetComponent<QuadRendererComponent>()!;
        var collider = GameObject.GetComponent<QuadColliderComponent>()!;
        var transform = GameObject.Transform;
        var properties = Game.Instance.GetProperties();
        AdjustSizes(properties, transform, rendererItem, collider);
        const float speed = 2.0f;

        var stop = DateTime.UtcNow;
        var duration = (stop - _lastTime).TotalMilliseconds;

        if (_direction == -1)
             _currentStep = 1;
        else if (_direction == 1)
             _currentStep = 0;
        else
             _currentStep = 2;

        if (properties.DeltaTime.TotalMilliseconds > 5 && _currentStep <= 1)
        {
            _lastTime = DateTime.UtcNow;
            var position = transform.GetPosition();
            if (position.Y + _steps[_currentStep] > _topLimit && position.Y + _steps[_currentStep] < _bottomLimit)
            {
                transform.Translate(0.0f, _steps[_currentStep] * speed, 0.0f);
            }
        }
    }

    private void MoveToBoundaries()
    {
        var transform = GameObject.Transform;

        if (transform.GetPosition().Y < _topLimit)
        {
            transform.Translate(0.0f, _topLimit - transform.GetPosition().Y, 0.0f);
        }
        else if (transform.GetPosition().Y > _bottomLimit)
        {
            transform.Translate(0.0f, _bottomLimit - transform.GetPosition().Y, 0.0f);
        }
    }

    private void AdjustSizes(GameProperties properties, Transform transform,
                             QuadRendererComponent rendererComponent, QuadColliderComponent colliderComponent)
    {
        if (properties.Width == _lastWindowWidth && properties.Height == _lastWindowHeight)
            return;

        _lastWindowWidth = properties.Width;
        _lastWindowHeight = properties.Height;
        _height = 0.15f * _lastWindowHeight;
        _bottomLimit = _lastWindowHeight - _height - 5.0f;
        rendererComponent.Height = _height;
        colliderComponent.Height = _height;
        if (!IsLeft)
        {
            var rightRacketPos = _lastWindowWidth - _width - 10.0f;
            transform.Translate(rightRacketPos - transform.GetPosition().X, 0.0f, 0.0f);
        }
    }

    private void HandleInput()
    {
        _direction = 0;
        if (IsLeft)
        {
            if (InputState.Instance.IsKeyHeld(SSGEDotNet.Core.Input.InputKey.KeyW))
            {
                _direction = 1;
            }
            else if (InputState.Instance.IsKeyHeld(SSGEDotNet.Core.Input.InputKey.KeyS))
            {
                _direction = -1;
            }
        }
        else
        {
            if (InputState.Instance.IsKeyHeld(SSGEDotNet.Core.Input.InputKey.KeyUp))
            {
                _direction = 1;
            }
            else if (InputState.Instance.IsKeyHeld(SSGEDotNet.Core.Input.InputKey.KeyDown))
            {
                _direction = -1;
            }
        }
    }
}

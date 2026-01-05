using SSGEDotNet.Core.GraphicsUtils;
using SSGEDotNet.Core.Input;
using SSGEDotNet.Core.Scene;
using SSGEDotNet.Core.Scene.Colliders;
using System.Numerics;

namespace SSGEDotNet.Sample;

public class BallComponent : Component
{
    private const float BallRatio = 0.02f;

    private bool _isMoving;
    private float _originalWindowWidth;
    private float _originalWindowHeight;
    private float _lastWindowWidth;
    private float _lastWindowHeight;
    private float _radius;
    private Vector2 _direction;

    public override void Init()
    {
        Random random = new Random();
        _isMoving = false;

        var properties = Game.Instance.GetProperties();
        _originalWindowWidth = properties.Width;
        _originalWindowHeight = properties.Height;
        _lastWindowWidth = _originalWindowWidth;
        _lastWindowHeight = _originalWindowHeight;
        _radius = BallRatio * _lastWindowHeight;

        Vector2 center = new Vector2(_lastWindowWidth / 2.0f, _lastWindowHeight / 2.0f);
        GameObject.Transform.Translate(center.X, center.Y, 0.0f);

        var circle = GameObject.GetComponent<CircleRendererComponent>()
            ?? throw new Exception("CircleRendererComponent not found on GameObject.");
        circle.Center = new Vector2(0.0f, 0.0f);
        circle.Radius = _radius;
        circle.FillColor = new Vector4(1.0f, 1.0f, 0.0f, 1.0f);

        var collider = GameObject.GetComponent<CircleColliderComponent>()
            ?? throw new Exception("CircleColliderComponent not found on GameObject.");
        collider.Center = center;
        collider.Radius = _radius;

        collider.Layer = "ball";
        collider.SetCollidesWith(["racket"]);
        collider.OnCollisionEnter += OnCollisionEnter;
    }

    public override void Update()
    {
        var rendererItem = GameObject.GetComponent<CircleRendererComponent>()!;
        var transform = GameObject.Transform;
        var properties = Game.Instance.GetProperties();
        AdjustSizes(properties, transform, rendererItem);
    }

    private void AdjustSizes(GameProperties properties, Transform transform, CircleRendererComponent rendererComponent)
    {
        if (properties.Width == _lastWindowWidth && properties.Height == _lastWindowHeight)
            return;

        _lastWindowWidth = properties.Width;
        _lastWindowHeight = properties.Height;
        _radius = BallRatio * _lastWindowHeight;
        rendererComponent.Radius = _radius;
    }

    private void OnCollisionEnter(CollisionInfo collisionInfo)
    {
        _direction.X = -_direction.X;
    }

    private void HandleInput()
    {
        if (GameObject.Input.IsKeyPressed(InputKey.KeySpace) && !_isMoving)
        {
            _isMoving = true;
        }
    }

    private void StartGame()
    {
        _isMoving = true;
        float xDir = Random.Shared.Next(3) % 2;
        float yDir = Random.Shared.Next(10001) % 10000 / 10000.0f;

        if (xDir == 1)
        {
            xDir = 1.0f;
            yDir *= -1.0f;
        }
        else
            xDir = -1.0f;

        _direction = new Vector2 { X = xDir, Y = yDir };
    }
}

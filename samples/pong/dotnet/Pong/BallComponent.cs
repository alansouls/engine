using SSGEDotNet.Core.GraphicsUtils;
using SSGEDotNet.Core.Input;
using SSGEDotNet.Core.Scene;
using SSGEDotNet.Core.Scene.Colliders;
using System.Numerics;

namespace  Pong;

public class BallComponent : Component
{
    private const float BallRatio = 0.02f;

    private bool _gameStarted;
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
        collider.Radius = _radius;

        collider.Layer = "ball";
        collider.SetCollidesWith(["racket"]);
        collider.OnCollisionEnter += OnCollisionEnter;
    }

    public override void Update()
    {
        var rendererItem = GameObject.GetComponent<CircleRendererComponent>()!;
        var collider = GameObject.GetComponent<CircleColliderComponent>()!;
        var transform = GameObject.Transform;
        var properties = Game.Instance.GetProperties();
        AdjustSizes(properties, transform, rendererItem, collider);
        HandleInput();

        if (_isMoving)
        {
            Move();
        }
    }

    private void AdjustSizes(GameProperties properties, Transform transform, CircleRendererComponent rendererComponent, CircleColliderComponent colliderComponent)
    {
        if (properties.Width == _lastWindowWidth && properties.Height == _lastWindowHeight)
            return;

        _lastWindowWidth = properties.Width;
        _lastWindowHeight = properties.Height;
        _radius = BallRatio * _lastWindowHeight;
        rendererComponent.Radius = _radius;
        colliderComponent.Radius = _radius;

        if (!_isMoving)
        {
            var center = new Vector2(_lastWindowWidth / 2.0f - transform.GetPosition().X,
                                    _lastWindowHeight / 2.0f - transform.GetPosition().Y);
            transform.Translate(center.X, center.Y, 0.0f);
        }
    }

    private void OnCollisionEnter(CollisionInfo collisionInfo)
    {
        _direction.X = -_direction.X;
    }

    private void HandleInput()
    {
        if (InputState.Instance.IsKeyPressed(InputKey.KeySpace))
        {
            if (!_gameStarted)
                StartGame();
            else
                _isMoving = !_isMoving;
        }
    }

    private void StartGame()
    {
        _gameStarted = true;
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

    private void Move()
    {
        var properties = Game.Instance.GetProperties();
        var transform = GameObject.Transform;
        var collider = GameObject.GetComponent<CircleColliderComponent>()!;

        float halfScreen = _lastWindowWidth / 2.0f;

        // takes 2 seconds to cross half the screen
        float speed = halfScreen / 1.0f;
        speed *= (float)(properties.DeltaTime.TotalSeconds);

        var position = transform.GetPosition();

        if (position.X - _radius < 0.0f || position.X + _radius > _lastWindowWidth)
        {
            _isMoving = false;
            Vector2 center = new(_lastWindowWidth / 2.0f, _lastWindowHeight / 2.0f);
            Vector3 newPosition = new(center.X, center.Y, 0.0f);
            transform.Translate(newPosition.X - position.X, newPosition.Y - position.Y, 0.0f);
            return;
        }

        if (position.Y - _radius < 0.0f || position.Y + _radius > _lastWindowHeight)
        {
            _direction.Y = -_direction.Y;
        }

        float newX = _direction.X * speed;
        float newY = _direction.Y * speed;
        transform.Translate(newX, newY, 0.0f);
    }
}

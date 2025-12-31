namespace SSGEDotNet.Core.Math;

public class Vec3
{
    public Vec3()
    {
    }

    public Vec3(float x)
    {
        X = x;
    }

    public Vec3(float x, float y)
    {
        X = x;
        Y = y;
    }

    public Vec3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    public float X { get; set; }
    public float Y { get; set; }
    public float Z { get; set; }
}
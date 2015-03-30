package katana.math;

public class Vector3f 
{
	public float x,y,z;
	
	public Vector3f(float x,float y,float z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3f(Vector3f vector)
	{
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}
	
	public Vector3f()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	
	public void add(Vector3f add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
	}
	
	public void add(float add)
	{
		x += add;
		y += add;
		z += add;
	}
	
	public void subtract(Vector3f sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
	}
	
	public void subtract(float sub)
	{
		x -= sub;
		y -= sub;
		z -= sub;
	}
	
	public void subtract(float x,float y,float z)
	{
		this.x -= x;
		this.y -= y;
		this.z -= z;
	}
	
	public void set(float set)
	{
		x = set;
		y = set;
		z = set;
	}
	
	public void set(float x,float y,float z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public void set(Vector3f set)
	{
		x = set.x;
		y = set.y;
		z = set.z;
	}
	
	public void inverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	
	public void divide(Vector3f div)
	{
		x /= div.x;
		y /= div.y;
		z /= div.z;
	}
	
	public void divide(float div)
	{
		x /= div;
		y /= div;
		z /= div;
	}
	
	public void scale(Vector3f scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
	}
	
	public void scale(float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
	}
	
	public void scale(float x,float y,float z)
	{
		this.x *= x;
		this.y *= y;
		this.z *= z;
	}
	
	public double length()
	{
		double length = 0;
		
		length += x*x;
		length += y*y;
		length += z*z;
		
		length = Math.sqrt(length);
		
		return length;
	}
	
	public void length(Vector3f out)
	{
		double length = length();
		out.set((float) length);
	}

	public void Normalize()
	{
		float length = (float)length();
		divide(length);
	}
}

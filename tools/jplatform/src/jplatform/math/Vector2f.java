package jplatform.math;

public class Vector2f 
{
	public static final Vector2f origin = new Vector2f(0,0);
	
	public float x,y;
	
	public Vector2f(float x,float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public Vector2f(Vector2f vector)
	{
		x = vector.x;
		y = vector.y;
	}
	
	public Vector2f()
	{
		x = 0;
		y = 0;
	}
	
	public void add(Vector2f add)
	{
		x += add.x;
		y += add.y;
	}
	
	public void add(float add)
	{
		x += add;
		y += add;
	}
	
	public void subtract(Vector2f sub)
	{
		x -= sub.x;
		y -= sub.y;
	}
	
	public void subtract(float sub)
	{
		x -= sub;
		y -= sub;
	}
	
	public void subtract(float x,float y)
	{
		this.x -= x;
		this.y -= y;
	}
	
	public void set(float set)
	{
		x = set;
		y = set;
	}
	
	public void set(float x,float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public void set(Vector2f set)
	{
		x = set.x;
		y = set.y;
	}
	
	public void inverse()
	{
		x = -x;
		y = -y;
	}
	
	public void divide(Vector2f div)
	{
		x /= div.x;
		y /= div.y;
	}
	
	public void divide(float div)
	{
		x /= div;
		y /= div;
	}
	
	public void scale(Vector2f scale)
	{
		x *= scale.x;
		y *= scale.y;
	}
	
	public void scale(float scale)
	{
		x *= scale;
		y *= scale;
	}
	
	public void scale(float x,float y,float z)
	{
		this.x *= x;
		this.y *= y;
	}
	
	public double length()
	{
		double dLength = 0;
		
		dLength += x*x;
		dLength += y*y;
		
		dLength = Math.sqrt(dLength);
		
		return dLength;
	}
	
	public void length(Vector2f out)
	{
		double dLength = length();
		out.set((float) dLength);
	}

	public void normalize()
	{
		float fLength = (float)length();
		divide(fLength);
	}
}

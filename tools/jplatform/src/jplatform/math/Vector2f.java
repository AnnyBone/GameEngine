package jplatform.math;

public class Vector2f 
{
	public static final Vector2f mvOrigin = new Vector2f(0,0);
	
	public float x,y;
	
	public Vector2f(float x,float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public Vector2f(Vector2f vVector)
	{
		x = vVector.x;
		y = vVector.y;
	}
	
	public Vector2f()
	{
		x = 0;
		y = 0;
	}
	
	public void Add(Vector2f mvAdd)
	{
		x += mvAdd.x;
		y += mvAdd.y;
	}
	
	public void Add(float fAdd)
	{
		x += fAdd;
		y += fAdd;
	}
	
	public void Subtract(Vector2f mvSub)
	{
		x -= mvSub.x;
		y -= mvSub.y;
	}
	
	public void Subtract(float fSub)
	{
		x -= fSub;
		y -= fSub;
	}
	
	public void Subtract(float x,float y)
	{
		this.x -= x;
		this.y -= y;
	}
	
	public void Set(float fSet)
	{
		x = fSet;
		y = fSet;
	}
	
	public void Set(float x,float y)
	{
		this.x = x;
		this.y = y;
	}
	
	public void Set(Vector2f mvSet)
	{
		x = mvSet.x;
		y = mvSet.y;
	}
	
	public void Inverse()
	{
		x = -x;
		y = -y;
	}
	
	public void Divide(Vector2f mvDiv)
	{
		x /= mvDiv.x;
		y /= mvDiv.y;
	}
	
	public void Divide(float fDiv)
	{
		x /= fDiv;
		y /= fDiv;
	}
	
	public void Scale(Vector2f mvScale)
	{
		x *= mvScale.x;
		y *= mvScale.y;
	}
	
	public void Scale(float fScale)
	{
		x *= fScale;
		y *= fScale;
	}
	
	public void Scale(float x,float y,float z)
	{
		this.x *= x;
		this.y *= y;
	}
	
	public double Length()
	{
		double dLength = 0;
		
		dLength += x*x;
		dLength += y*y;
		
		dLength = Math.sqrt(dLength);
		
		return dLength;
	}
	
	public void Length(Vector2f mvOut)
	{
		double dLength = Length();
		mvOut.Set((float) dLength);
	}

	public void Normalize()
	{
		float fLength = (float)Length();
		Divide(fLength);
	}
}

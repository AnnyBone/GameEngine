package jplatform.math;

public class Vector3f 
{
	public static final Vector3f origin = new Vector3f(0,0,0);
	
	public static final int	MATH_PITCH = 0;
	public static final int MATH_YAW = 1;
	public static final int MATH_ROLL = 2;
	
	public static final int MATH_X = 0;
	public static final int MATH_Y = 1;
	public static final int MATH_Z = 2;
	
	public float x,y,z;
	
	public Vector3f(float x,float y,float z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3f(Vector3f vVector)
	{
		x = vVector.x;
		y = vVector.y;
		z = vVector.z;
	}
	
	public Vector3f()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	
	public void Add(Vector3f mvAdd)
	{
		x += mvAdd.x;
		y += mvAdd.y;
		z += mvAdd.z;
	}
	
	public void Add(float fAdd)
	{
		x += fAdd;
		y += fAdd;
		z += fAdd;
	}
	
	public void Subtract(Vector3f mvSub)
	{
		x -= mvSub.x;
		y -= mvSub.y;
		z -= mvSub.z;
	}
	
	public void Subtract(float fSub)
	{
		x -= fSub;
		y -= fSub;
		z -= fSub;
	}
	
	public void Subtract(float x,float y,float z)
	{
		this.x -= x;
		this.y -= y;
		this.z -= z;
	}
	
	public void Set(float fSet)
	{
		x = fSet;
		y = fSet;
		z = fSet;
	}
	
	public void Set(float x,float y,float z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public void Set(Vector3f mvSet)
	{
		x = mvSet.x;
		y = mvSet.y;
		z = mvSet.z;
	}
	
	public void Inverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	
	public void Divide(Vector3f mvDiv)
	{
		x /= mvDiv.x;
		y /= mvDiv.y;
		z /= mvDiv.z;
	}
	
	public void Divide(float fDiv)
	{
		x /= fDiv;
		y /= fDiv;
		z /= fDiv;
	}
	
	public void Scale(Vector3f mvScale)
	{
		x *= mvScale.x;
		y *= mvScale.y;
		z *= mvScale.z;
	}
	
	public void Scale(float fScale)
	{
		x *= fScale;
		y *= fScale;
		z *= fScale;
	}
	
	public void Scale(float x,float y,float z)
	{
		this.x *= x;
		this.y *= y;
		this.z *= z;
	}
	
	public double Length()
	{
		double dLength = 0;
		
		dLength += x*x;
		dLength += y*y;
		dLength += z*z;
		
		dLength = Math.sqrt(dLength);
		
		return dLength;
	}
	
	public void Length(Vector3f mvOut)
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

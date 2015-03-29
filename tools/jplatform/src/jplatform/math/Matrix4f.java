package jplatform.math;

public class Matrix4f 
{
	float	fVector[][];	// Row and column.
	
	public Matrix4f()
	{
		fVector = new float[4][4];
		
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] = 0;
	}
	
	public void Add(Matrix4f mmAdd)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] += mmAdd.fVector[i][j];
	}
	
	public void Add(float fAdd)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] += fAdd;
	}
	
	public void Subtract(Matrix4f mmSub)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] -= mmSub.fVector[i][j];
	}
	
	public void Subtract(float fSub)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] -= fSub;
	}
	
	public void Inverse()
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] = -fVector[i][j];
	}
	
	public void Divide(Matrix4f mmDiv)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] /= mmDiv.fVector[i][j];
	}
	
	public void Divide(float fDiv)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] /= fDiv;
	}
	
	public void Scale(Matrix4f mmScale)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] *= mmScale.fVector[i][j];
	}
	
	public void Scale(float fScale)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				fVector[i][j] *= fScale;
	}
}

package jplatform.math;

public class Matrix4f 
{
	float	matrix[][];	// Row and column.
	
	public Matrix4f()
	{
		matrix = new float[4][4];
		
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] = 0;
	}
	
	public void add(Matrix4f add)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] += add.matrix[i][j];
	}
	
	public void add(float add)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] += add;
	}
	
	public void subtract(Matrix4f sub)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] -= sub.matrix[i][j];
	}
	
	public void subtract(float sub)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] -= sub;
	}
	
	public void inverse()
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] = -matrix[i][j];
	}
	
	public void divide(Matrix4f div)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] /= div.matrix[i][j];
	}
	
	public void divide(float div)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] /= div;
	}
	
	public void scale(Matrix4f scale)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] *= scale.matrix[i][j];
	}
	
	public void scale(float scale)
	{
		for(int i = 0; i < 4; i++)		// Row
			for(int j = 0; j < 4; j++)	// Column
				matrix[i][j] *= scale;
	}
}

package jplatform.math;

public class Random {
	
	public static float InRange(float min,float max) 
	{
		  return (float) (Math.random() < 0.5 ? ((1-Math.random()) * (max-min) + min) : (Math.random() * (max-min) + min));
	}
	
	public static int InRange(int min,int max) 
	{
		  return (int) (Math.random() < 0.5 ? ((1-Math.random()) * (max-min) + min) : (Math.random() * (max-min) + min));
	}

}

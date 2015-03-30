/*	Copyright (C) 2015 OldTimes Software
 * 
 */
package katana.engine;

public class Engine 
{
	static native void HelloWorld();
	
	public Engine()
	{
		System.loadLibrary("engine.x86");
	}
}

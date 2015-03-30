package katana.mateditor;

import org.eclipse.swt.widgets.*;

public final class Main 
{
	static Display mainDisplay;
	
	public static void main(String[] args)
	{
		mainDisplay = new Display();
		
		Shell shell = new Shell(mainDisplay);
		shell.open();
		
		while(!shell.isDisposed())
		{
			if(!mainDisplay.readAndDispatch())
				mainDisplay.sleep();
		}
		
		mainDisplay.dispose();
	}
}

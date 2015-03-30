package katana.editor.materials;

import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;

public final class MaterialWindow 
{
	static Display mainDisplay;
	static Shell mainShell;
	
	public static void main(String[] args)
	{
		// Create the display.
		mainDisplay = new Display();
		
		// Create the shell from the display.
		mainShell = new Shell(mainDisplay);
		
		// Open the shell.
		mainShell.open();
		mainShell.setText("Katana Material Editor");
		
		// Make the editor window maximized.
		mainShell.setMaximized(true);
		mainShell.setLayout(new FillLayout());
		
		while(!mainShell.isDisposed())
		{
			if(!mainDisplay.readAndDispatch())
				mainDisplay.sleep();
		}
		
		mainDisplay.dispose();
	}
}

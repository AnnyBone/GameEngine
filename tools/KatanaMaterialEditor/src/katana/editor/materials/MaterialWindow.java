package katana.editor.materials;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;

public final class MaterialWindow 
{
	static Display mainDisplay;
	static Shell mainShell;
	
	public static void createMenu()
	{
		// Menus
		Menu mainMenu;
		Menu fileMenu;
		Menu helpMenu;
		
		mainMenu = new Menu(mainShell,SWT.BAR);
		fileMenu = new Menu(mainShell,SWT.DROP_DOWN);
		helpMenu = new Menu(mainShell,SWT.DROP_DOWN);
		
		// File menu...
		
		MenuItem fileItem;
		MenuItem exitItem;
		
		fileItem = new MenuItem(mainMenu,SWT.CASCADE);
		fileItem.setText("&File");
		fileItem.setMenu(fileMenu);
		
		exitItem = new MenuItem(fileMenu,SWT.PUSH);
		exitItem.setText("&Exit");
		exitItem.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e)
			{
				mainShell.getDisplay().dispose();
				System.exit(0);
			}
		});
		
		// Help menu...
		
		MenuItem helpItem;
		MenuItem aboutItem;
		
		helpItem = new MenuItem(mainMenu,SWT.CASCADE);
		helpItem.setText("&Help");
		helpItem.setMenu(helpMenu);
		
		aboutItem = new MenuItem(helpMenu,SWT.PUSH);
		aboutItem.setText("&About");
		
		mainShell.setMenuBar(mainMenu);
	}
	
	public static void createToolBar()
	{
		ToolBar mainBar;
		
		mainBar = new ToolBar(mainShell,SWT.BORDER);
		
		mainBar.pack();
	}
	
	public static void main(String[] args)
	{
		// Create the display.
		mainDisplay = new Display();
		
		// Create the shell from the display.
		mainShell = new Shell(mainDisplay);
		
		// Open the shell.
		mainShell.open();
		
		// Create the menu for the window.
		createMenu();
		createToolBar();

		// Set the window properties.
		mainShell.setSize(512,512);
		mainShell.setMaximized(true);
		mainShell.setLayout(new FillLayout());
		mainShell.setText("Katana Material Editor");
		
		while(!mainShell.isDisposed())
		{
			if(!mainDisplay.readAndDispatch())
				mainDisplay.sleep();
		}
		
		mainDisplay.dispose();
	}
}

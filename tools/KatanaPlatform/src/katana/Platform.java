package katana;

public final class Platform 
{
	enum CPUType
	{
		X64,
		X86,
		
		UNKNOWN
	}
	
	static CPUType currentCPU;
	
	public static void initialize()
	{
		String cpu;
		
		// Figure out what CPU we're using.
		cpu = System.getProperty("os.arch");
		if(cpu.endsWith("64") == true)
			currentCPU = CPUType.X64;
		else if(cpu.endsWith("86") == true)
			currentCPU = CPUType.X86;
		else
			currentCPU = CPUType.UNKNOWN;
	}
}

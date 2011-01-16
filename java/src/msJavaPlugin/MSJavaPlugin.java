package msJavaPlugin;

import java.io.IOException;

public class MSJavaPlugin {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception
	{
		boolean usePipes = false;
		int port = 38830;
		for (int i=0; i < args.length; i++)
		{
			if (args[i].equals("-pipe"))
			{
				usePipes = true;
				break;
			}
			if (args[i].equals("-port"))
			{
				boolean err = true;
				if (args.length >= i+2)
				{
					try
					{
						port = Integer.parseInt(args[i+1]);
						err = (port < 1) || (port > 65535);
					}
					catch (NumberFormatException e) { }
				}
				if (err)
				{
					System.err.print("Invalid port. Must be an integer between 1 and 65535.");
					return;
				}
				break;
			}
		}
		MineserverInterface mi = new MineserverInterface(
				new MSJavaPlugin(), usePipes, port);
		mi.runCommandLoop();
	}
	
	public boolean timerTest()
	{
		MineserverInterface.getInstance().logger_log(
				LogType.LOG_INFO, "msJavaPlugin-java", "Timer1000 called!");
		return false;
	}

	public void init()
	{
		MineserverInterface mi = MineserverInterface.getInstance();
		mi.logger_log(LogType.LOG_INFO, "msJavaPlugin-java",
				"msJavaPlugin java side loaded.");
		try {
			mi.plugin_addCallback("Timer1000", this, this.getClass().getDeclaredMethod("timerTest"));
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

package hModPluginLoader;

import java.io.IOException;

public class hModPluginLoader {

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
				new hModPluginLoader(), usePipes, port);
		mi.runCommandLoop();
	}

	public void init() throws IOException
	{
		MineserverInterface mi = MineserverInterface.getInstance();
		mi.logger_log(LogType.LOG_INFO, "hModPluginLoader-java",
				"hModPluginLoader java side loaded.");
	}
}

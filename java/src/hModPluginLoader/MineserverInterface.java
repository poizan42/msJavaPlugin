package hModPluginLoader;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class MineserverInterface {
	private static MineserverInterface instance;
	private DataOutputStream outStream;
	private DataInputStream inStream;
	private hModPluginLoader loader;

	public MineserverInterface(hModPluginLoader l, boolean usePipes, int port) throws IOException
	{
		instance = this;
		this.loader = l;
		
		if (usePipes)
		{
			inStream = new DataInputStream(System.in);
			outStream = new DataOutputStream(System.out);
		}
		else
		{
			ServerSocket ssock = new ServerSocket(port, 0,
				InetAddress.getByAddress(
					new byte[]{127,0,0,1}));
			System.out.println("Waiting for connection to 127.0.0.1:"+port);
			Socket sock = ssock.accept();
			System.out.println("Received connection");
			inStream = new DataInputStream(sock.getInputStream());
			outStream = new DataOutputStream(sock.getOutputStream());
		}
	}
	
	public static MineserverInterface getInstance() {
		return instance;
	}
	
	public void runCommandLoop() throws IOException
	{
		while (true)
		{
			int cmd = inStream.readInt();
			if (cmd == -1)
			{
				loader.init();//init
				intercom_return();
			}
			else if (cmd == -2)
			{
				//shutdown
				intercom_return();
				return;
			}
			else
				;//callback
		}
	}
	
	private void _intercom_return() throws IOException
	{
		outStream.writeInt(ClientCommand.intercom_return.ordinal());
	}
	
	public void intercom_return() throws IOException
	{
		_intercom_return();
		outStream.flush();
	}
	
	public void intercom_return(int retval) throws IOException
	{
		_intercom_return();
		outStream.writeInt(retval);
		outStream.flush();
	}
	
	public void intercom_return(short retval) throws IOException
	{
		_intercom_return();
		outStream.writeShort(retval);
		outStream.flush();
	}

	public void intercom_return(byte retval) throws IOException
	{
		_intercom_return();
		outStream.writeByte(retval);
		outStream.flush();
	}
	
	public void intercom_return(float retval) throws IOException
	{
		_intercom_return();
		outStream.writeFloat(retval);
		outStream.flush();
	}
	
	public void intercom_return(String retval) throws IOException
	{
		_intercom_return();
		outStream.writeUTF(retval);
		outStream.flush();
	}
	
	public void logger_log(LogType type, String source, String message) throws IOException
	{
		outStream.writeInt(ClientCommand.logger_log.ordinal());
		outStream.writeInt(type.ordinal());
		outStream.writeUTF(source);
		outStream.writeUTF(message);
		outStream.flush();
	}
}

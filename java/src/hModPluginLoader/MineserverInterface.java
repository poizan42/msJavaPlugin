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
	
	private void IOError()
	{
		System.err.println("Connection to host lost. Shutting down.");
		System.exit(-1);
	}

	private void flush() {
		try {outStream.flush();}catch (IOException e){IOError();}}
	private String readString() {
		try {return inStream.readUTF();}catch (IOException e){IOError();}
		return null;}
	private int readInt() {
		try {return inStream.readInt();}catch (IOException e){IOError();}
		return 0;}
	private short readShort() {
		try {return inStream.readShort();}catch (IOException e){IOError();}
		return 0;}
	private byte readByte() {
		try {return inStream.readByte();}catch (IOException e){IOError();}
		return 0;}
	private boolean readBool() {
		try {return inStream.readBoolean();}catch (IOException e){IOError();}
		return false;}
	private float readFloat() {
		try {return inStream.readFloat();}catch (IOException e){IOError();}
		return 0;}
	private double readDouble() {
		try {return inStream.readDouble();}catch (IOException e){IOError();}
		return 0;}
	
	private void writeString(String v) {
		try {outStream.writeUTF(v);}catch (IOException e){IOError();}}
	private void writeInt(int v) {
		try {outStream.writeInt(v);}catch (IOException e){IOError();}}
	private void writeShort(short v) {
		try {outStream.writeShort(v);}catch (IOException e){IOError();}}
	private void writeByte(byte v) {
		try {outStream.writeByte(v);}catch (IOException e){IOError();}}
	private void writeBool(boolean v) {
		try {outStream.writeBoolean(v);}catch (IOException e){IOError();}}
	private void writeFloat(Float v) {
		try {outStream.writeFloat(v);}catch (IOException e){IOError();}}
	private void writeDouble(Double v) {
		try {outStream.writeDouble(v);}catch (IOException e){IOError();}}
	
	private void _intercom_return()
	{
		writeInt(ClientCommand.intercom_return.ordinal());
	}
	
	public void intercom_return()
	{
		_intercom_return();
		flush();
	}
	
	public void intercom_return(int retval)
	{
		_intercom_return();
		writeInt(retval);
		flush();
	}
	
	public void intercom_return(short retval)
	{
		_intercom_return();
		writeShort(retval);
		flush();
	}

	public void intercom_return(byte retval)
	{
		_intercom_return();
		writeByte(retval);
		flush();
	}
	
	public void intercom_return(float retval)
	{
		_intercom_return();
		writeFloat(retval);
		flush();
	}

	public void intercom_return(double retval)
	{
		_intercom_return();
		writeDouble(retval);
		flush();
	}
	
	public void intercom_return(String retval)
	{
		_intercom_return();
		writeString(retval);
		flush();
	}
	
	public void logger_log(LogType type, String source, String message)
	{
		writeInt(ClientCommand.logger_log.ordinal());
		writeInt(type.ordinal());
		writeString(source);
		writeString(message);
		flush();
	}
}

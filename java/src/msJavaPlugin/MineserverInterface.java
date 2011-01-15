package msJavaPlugin;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class MineserverInterface {
	public class ByRef_boolean {public boolean value;}
	public class ByRef_byte {public byte value;}
	public class ByRef_short {public short value;}
	public class ByRef_int {public int value;}
	public class ByRef_float {public float value;}
	public class ByRef_double {public double value;}
	
	private static MineserverInterface instance;
	private DataOutputStream outStream;
	private DataInputStream inStream;
	private MSJavaPlugin loader;

	public MineserverInterface(MSJavaPlugin l, boolean usePipes, int port) throws IOException
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
	private long readLong() {
		try {return inStream.readLong();}catch (IOException e){IOError();}
		return 0;}
	private int readInt() {
		try {return inStream.readInt();}catch (IOException e){IOError();}
		return 0;}
	private short readShort() {
		try {return inStream.readShort();}catch (IOException e){IOError();}
		return 0;}
	private byte readByte() {
		try {return inStream.readByte();}catch (IOException e){IOError();}
		return 0;}
	private short readUnsignedByte() {
		try {return (short)inStream.readUnsignedByte();}catch (IOException e){IOError();}
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
	private void writeLong(long v) {
		try {outStream.writeLong(v);}catch (IOException e){IOError();}}
	private void writeInt(int v) {
		try {outStream.writeInt(v);}catch (IOException e){IOError();}}
	private void writeShort(int v) {
		try {outStream.writeShort(v);}catch (IOException e){IOError();}}
	private void writeByte(short v) {
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
	/*getPointer/setPointer probably not very useful
	  for java plugins. But here they are: */
	public long plugin_getPointer(String name)
	{
		writeInt(ClientCommand.plugin_getPointer.ordinal());
		writeString(name);
		flush();
		return readLong();
	}
	
	public void plugin_setPointer(String name, long pointer)
	{
		writeInt(ClientCommand.plugin_setPointer.ordinal());
		writeString(name);
		writeLong(pointer);
		flush();
	}
	
	//*************** codegen.py output below ***************
	public boolean plugin_hasPluginVersion(String name)
	{
		writeInt(ClientCommand.plugin_hasPluginVersion.ordinal());
		writeString(name);
		flush();
		return readBool();
	}

	public float plugin_getPluginVersion(String name)
	{
		writeInt(ClientCommand.plugin_getPluginVersion.ordinal());
		writeString(name);
		flush();
		return readFloat();
	}

	public void plugin_setPluginVersion(String name, float version)
	{
		writeInt(ClientCommand.plugin_setPluginVersion.ordinal());
		writeString(name);
		writeFloat(version);
		flush();
	}

	public void plugin_remPluginVersion(String name)
	{
		writeInt(ClientCommand.plugin_remPluginVersion.ordinal());
		writeString(name);
		flush();
	}

	public boolean plugin_hasPointer(String name)
	{
		writeInt(ClientCommand.plugin_hasPointer.ordinal());
		writeString(name);
		flush();
		return readBool();
	}

	public void plugin_remPointer(String name)
	{
		writeInt(ClientCommand.plugin_remPointer.ordinal());
		writeString(name);
		flush();
	}

	public boolean plugin_hasHook(String hookID)
	{
		writeInt(ClientCommand.plugin_hasHook.ordinal());
		writeString(hookID);
		flush();
		return readBool();
	}

//  Hook* (*getHook)(const char* hookID);
//struct="plugin", ret="Hook*", name="getHook", args=[('const char*', 'hookID')]
//Unknown return type: "Hook*". Not generating code!

//  void  (*setHook)(const char* hookID, Hook* hook);
//struct="plugin", ret="void", name="setHook", args=[('const char*', 'hookID'), ('Hook*', 'hook')]
//Unknown argument type: "Hook*" for argument "hook". Not generating code!

	public void plugin_remHook(String hookID)
	{
		writeInt(ClientCommand.plugin_remHook.ordinal());
		writeString(hookID);
		flush();
	}

//  bool (*hasCallback)          (const char* hookID, void* function);
//struct="plugin", ret="bool", name="hasCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  void (*addCallback)          (const char* hookID, void* function);
//struct="plugin", ret="void", name="addCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  void (*addIdentifiedCallback)(const char* hookID, void* identifier, void* function);
//struct="plugin", ret="void", name="addIdentifiedCallback", args=[('const char*', 'hookID'), ('void*', 'identifier'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "identifier". Not generating code!

//  void (*remCallback)          (const char* hookID, void* function);
//struct="plugin", ret="void", name="remCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  bool (*doUntilTrue)          (const char* hookID, ...);
//struct="plugin", ret="bool", name="doUntilTrue", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

//  bool (*doUntilFalse)         (const char* hookID, ...);
//struct="plugin", ret="bool", name="doUntilFalse", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

//  void (*doAll)                (const char* hookID, ...);
//struct="plugin", ret="void", name="doAll", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

	public boolean user_teleport(String user, double x, double y, double z)
	{
		writeInt(ClientCommand.user_teleport.ordinal());
		writeString(user);
		writeDouble(x);
		writeDouble(y);
		writeDouble(z);
		flush();
		return readBool();
	}

	public boolean user_getPosition(String user, ByRef_double x, ByRef_double y, ByRef_double z, ByRef_float yaw, ByRef_float pitch, ByRef_double stance)
	{
		writeInt(ClientCommand.user_getPosition.ordinal());
		writeString(user);
		flush();
		x.value = readDouble();
		y.value = readDouble();
		z.value = readDouble();
		yaw.value = readFloat();
		pitch.value = readFloat();
		stance.value = readDouble();
		return readBool();
	}

	public boolean user_sethealth(String user, int userHealth)
	{
		writeInt(ClientCommand.user_sethealth.ordinal());
		writeString(user);
		writeInt(userHealth);
		flush();
		return readBool();
	}

	public boolean chat_sendmsgTo(String user, String msg)
	{
		writeInt(ClientCommand.chat_sendmsgTo.ordinal());
		writeString(user);
		writeString(msg);
		flush();
		return readBool();
	}

	public boolean chat_sendmsg(String msg)
	{
		writeInt(ClientCommand.chat_sendmsg.ordinal());
		writeString(msg);
		flush();
		return readBool();
	}

	public boolean chat_sendUserlist(String user)
	{
		writeInt(ClientCommand.chat_sendUserlist.ordinal());
		writeString(user);
		flush();
		return readBool();
	}

	public void map_createPickupSpawn(int x, int y, int z, int type, int count, int health, String user)
	{
		writeInt(ClientCommand.map_createPickupSpawn.ordinal());
		writeInt(x);
		writeInt(y);
		writeInt(z);
		writeInt(type);
		writeInt(count);
		writeInt(health);
		writeString(user);
		flush();
	}

	public boolean map_setTime(int timeValue)
	{
		writeInt(ClientCommand.map_setTime.ordinal());
		writeInt(timeValue);
		flush();
		return readBool();
	}

	public void map_getSpawn(ByRef_int x, ByRef_int y, ByRef_int z)
	{
		writeInt(ClientCommand.map_getSpawn.ordinal());
		flush();
		x.value = readInt();
		y.value = readInt();
		z.value = readInt();
	}

	public boolean map_getBlock(int x, int y, int z, ByRef_short type, ByRef_short meta)
	{
		writeInt(ClientCommand.map_getBlock.ordinal());
		writeInt(x);
		writeInt(y);
		writeInt(z);
		flush();
		type.value = readUnsignedByte();
		meta.value = readUnsignedByte();
		return readBool();
	}

	public boolean map_setBlock(int x, int y, int z, short type, short meta)
	{
		writeInt(ClientCommand.map_setBlock.ordinal());
		writeInt(x);
		writeInt(y);
		writeInt(z);
		writeByte(type);
		writeByte(meta);
		flush();
		return readBool();
	}

	public void map_saveWholeMap()
	{
		writeInt(ClientCommand.map_saveWholeMap.ordinal());
		flush();
	}

//  unsigned char* (*getMapData_block)(int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_block", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_meta) (int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_meta", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_skylight)  (int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_skylight", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_blocklight)(int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_blocklight", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

	public boolean config_has(String name)
	{
		writeInt(ClientCommand.config_has.ordinal());
		writeString(name);
		flush();
		return readBool();
	}

	public int config_iData(String name)
	{
		writeInt(ClientCommand.config_iData.ordinal());
		writeString(name);
		flush();
		return readInt();
	}

	public long config_lData(String name)
	{
		writeInt(ClientCommand.config_lData.ordinal());
		writeString(name);
		flush();
		return readLong();
	}

	public float config_fData(String name)
	{
		writeInt(ClientCommand.config_fData.ordinal());
		writeString(name);
		flush();
		return readFloat();
	}

	public double config_dData(String name)
	{
		writeInt(ClientCommand.config_dData.ordinal());
		writeString(name);
		flush();
		return readDouble();
	}

	public String config_sData(String name)
	{
		writeInt(ClientCommand.config_sData.ordinal());
		writeString(name);
		flush();
		return readString();
	}

	public boolean config_bData(String name)
	{
		writeInt(ClientCommand.config_bData.ordinal());
		writeString(name);
		flush();
		return readBool();
	}

}

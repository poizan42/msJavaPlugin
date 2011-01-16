package msJavaPlugin;

import java.lang.reflect.Method;
import java.util.Dictionary;
import java.util.Hashtable;

public class CallbackInfo {
	private static int lastIndex = 0;
	private int index;
	private Object obj;
	private Method method;
	private InterprocType[] types;
	
	private static Dictionary<Class<?>, InterprocType> typemap;
	
	{
		typemap = new Hashtable<Class<?>, InterprocType>();
		typemap.put(byte.class, InterprocType.int8);
		typemap.put(short.class, InterprocType.int16);
		typemap.put(int.class, InterprocType.int32);
		typemap.put(long.class, InterprocType.int64);
		typemap.put(float.class, InterprocType.floatType);
		typemap.put(double.class, InterprocType.doubleType);
		typemap.put(boolean.class, InterprocType.boolType);
		typemap.put(String.class, InterprocType.string);
	}

	public Object getObject() {
		return obj;
	}

	public Method getMethod() {
		return method;
	}

	public int getIndex() {
		return index;
	}
	
	public InterprocType[] getTypes()
	{
		return types;
	}
	
	public CallbackInfo(Object obj, Method method)
	{
		if (method.getReturnType() != boolean.class)
			throw new IllegalArgumentException("Return type must be boolean");
		Class<?>[] args = method.getParameterTypes();
		types = new InterprocType[args.length];
		for (int i=0; i < args.length; i++)
		{
			InterprocType type = typemap.get(args[i]);
			if (type == null)
				throw new IllegalArgumentException("Unsupported type: "+args[i].getName());
			types[i] = type;
		}
		
		this.index = lastIndex++;
		this.obj = obj;
		this.method = method;
	}

}

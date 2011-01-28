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
		types = MineserverInterface.getArgTypesFromClasses(args);
		this.index = lastIndex++;
		this.obj = obj;
		this.method = method;
	}

}

package util.events;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public interface IEventDispatcher
{
	void addEventListener(String type, Object object, Method listener);
	void addEventListener(String type, Object object, Method listener, Boolean useCapture);
	void addEventListener(String type, Object object, Method listener, Boolean useCapture,int priority);
	void addEventListener(String type, Object object, Method listener, Boolean useCapture, int priority, Boolean useWeakReference);
	
	void removeEventListener(String type, Object object, Method listener);
	void removeEventListener(String type, Object object, Method listener, Boolean useCapture);
	Boolean dispatchEvent(Event event) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException;
	Boolean hasEventListener(String type);
	Boolean willTrigger(String type);
}

package util.events;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class EventListener
{
	private String type;
	private Object object;
	private Method listener;
	private Boolean useCapture;
	private int priority;
	private Boolean useWeakReference;

	public EventListener(String type, Object object, Method listener, Boolean useCapture, int priority, Boolean useWeakReference)
	{
		this.type = type;
		this.object = object;
		this.listener = listener;
		this.useCapture = useCapture;
		this.priority = priority;
		this.useWeakReference = useWeakReference;
	}
	
	public void dispatch(Event event, IEventDispatcher target) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		if(event.getType().equals(type))
		{
			event.target = target;
			listener.invoke(object, event);
		}
	}

	public String getType() {
		return type;
	}

	public Object getObject() {
		return object;
	}

	public Method getListener() {
		return listener;
	}

	public Boolean getUseCapture() {
		return useCapture;
	}

	public int getPriority() {
		return priority;
	}

	public Boolean getUseWeakReference() {
		return useWeakReference;
	}
}

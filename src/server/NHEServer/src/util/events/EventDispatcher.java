package util.events;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Vector;

public class EventDispatcher implements IEventDispatcher
{
	Vector<EventListener> prl_listeners;
	IEventDispatcher target;
	
	public EventDispatcher()
	{
		this(null);
	}
	
	public EventDispatcher(IEventDispatcher target)
	{
		if(target == null)
			target = this;
		this.prl_listeners = new Vector<EventListener>();
	}
	
	@Override
	public void addEventListener(String type, Object object, Method listener)
	{
		addEventListener(type, object, listener, false, 0, false);
	}

	@Override
	public void addEventListener(String type, Object object, Method listener,
			Boolean useCapture) {
		addEventListener(type, object, listener, useCapture, 0, false);

	}

	@Override
	public void addEventListener(String type, Object object, Method listener,
			Boolean useCapture, int priority) {
		addEventListener(type, object, listener, useCapture, priority, false);

	}

	@Override
	public void addEventListener(String type, Object object, Method listener,
			Boolean useCapture, int priority, Boolean useWeakReference) {
		EventListener eventListener = new EventListener(type, object, listener, useCapture, priority, useWeakReference);
		prl_listeners.add(eventListener);
	}

	@Override
	public Boolean dispatchEvent(Event event) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		for(int i = 0; i < prl_listeners.size(); i++)
		{
			prl_listeners.get(i).dispatch(event, target);
		}
		return true;
	}

	@Override
	public Boolean hasEventListener(String type)
	{
		for(int i = 0; i < prl_listeners.size(); i++)
		{
			if(prl_listeners.get(i).getType().equals(type)) return true;
		}
		return false;
	}

	@Override
	public void removeEventListener(String type, Object object, Method listener)
	{
		removeEventListener(type, object, listener, false);
	}

	@Override
	public void removeEventListener(String type, Object object, Method listener,
			Boolean useCapture)
	{
		for(int i = 0; i < prl_listeners.size(); i++)
		{
			if((prl_listeners.get(i).getType().equals(type) || type != null) &&
			   (prl_listeners.get(i).getObject().equals(object) || object != null) &&
			   (prl_listeners.get(i).getListener().equals(listener) || listener != null) &&
			   prl_listeners.get(i).getUseCapture().equals(useCapture))
			   prl_listeners.remove(i);
		}
	}

	@Override
	public Boolean willTrigger(String type) {
		// TODO Auto-generated method stub
		return null;
	}

}

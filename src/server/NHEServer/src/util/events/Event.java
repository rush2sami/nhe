package util.events;

public class Event
{
	public String type;
	public Boolean bubbles;
	public Boolean cancelable; 
	public IEventDispatcher target;
	
	public Event(String type)
	{
		this(type, false, false);
	}
	
	public Event(String type, Boolean bubbles, Boolean cancelable)
	{
		this.type = type;
		this.bubbles = bubbles;
		this.cancelable = cancelable;
	}

	public String getType()
	{
		return type;
	}

	public void setType(String type)
	{
		this.type = type;
	}

	public Boolean getBubbles()
	{
		return bubbles;
	}

	public Boolean getCancelable()
	{
		return cancelable;
	}
	
	public IEventDispatcher getTarget()
	{
		return target;
	}
}

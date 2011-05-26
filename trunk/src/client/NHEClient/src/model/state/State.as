package model.state
{
	import flash.display.Sprite;
	import flash.events.EventDispatcher;
	
	import model.bd.aca.Acao;
	import model.errors.StateError;
	import model.events.StateEvent;

	
	[Event(name="added", type="model.events.StateEvent")]
	[Event(name="removed", type="model.events.StateEvent")]
	[Event(name="action_changed", type="model.events.StateEvent")]
	
	public class State extends EventDispatcher
	{
		private var pro_parent:State;
		private var prl_children:Vector.<State>;
		
		private var _pao_action:Acao;
		
		public function State()
		{
			prl_children = new Vector.<State>();
		}
		
		/**
		 * Add the child to the end of state list.
		 * @param child
		 * @event StateEvent.ADDED
		 */
		public function addChild(child:State):void
		{
			if(prl_children.indexOf(child) != -1)
			{
				prl_children.splice(prl_children.indexOf(child),1);
			}
			prl_children.push(child);
			child.pro_parent = this;
			child.dispatchEvent(new StateEvent(StateEvent.ADDED));
		}
		
		/**
		 * Add a Child at a specific index
		 * @param index
		 * @event StateEvent.ADDED
		 * 
		 */
		public function addChildAt(child:State, index:Number):void
		{
			if(prl_children.indexOf(child) != -1)
			{
				prl_children.splice(prl_children.indexOf(child),1);
			}
			prl_children.splice(index,0,child);
			child.pro_parent = this;
			child.dispatchEvent(new StateEvent(StateEvent.ADDED));
		}
		
		/**
		 * Remove a Child at a specific index 
		 * @param index
		 * @throws StateError with type StateError.BEYOND_RANGE_ERROR
		 * 
		 */
		public function removeChildAt(index:Number):void
		{
			if(index >= prl_children.length) throw new StateError(StateError.BEYOND_RANGE_ERROR, "The index is beyond the range");
			else
			{
				var child:State = prl_children[index];
				prl_children.splice(index,1);
				child.pro_parent = null;
				child.dispatchEvent(new StateEvent(StateEvent.REMOVED));
			}
		}
		
		/**
		 * Remove a child state from the list of states 
		 * @param child
		 * 
		 */
		public function removeChild(child:State):void
		{
			if(prl_children.indexOf(child) == -1) throw new StateError(StateError.NOT_CHILD_ERROR, "The parameter is not child of this state");
			else
			{
				prl_children.splice(prl_children.indexOf(child),1);
				child.pro_parent = null;
				child.dispatchEvent(new StateEvent(StateEvent.REMOVED));
			}
		}
		
		/**
		 * Clone the state. If deep is true, clone all the children and so on...
		 * @param deep a Boolean value. If true, clone all the children. If false, keep linking to the same children. 
		 * @return a new state with same attributes. If <code>deep</code> is <code>true</code>, clone the children in depth
		 * 
		 * 
		 */
		public function clone(deep:Boolean):State
		{
			var newState:State = new State();
			
			if(deep)
			{
				for(var i:uint = 0; i < prl_children.length; i++)
				{
					newState.prl_children.push(prl_children[i].clone(true));
				}
			}
			else
			{
				for(var i:uint = 0; i < prl_children.length; i++)
				{
					newState.prl_children.push(prl_children[i]);
				}
			}
			return newState;
		}
		
		/**
		 * Asks if the parameter is a parent or grandfather or other ancestor 
		 * @param state
		 * @return true if the parameter ancestor is really a ancestor of the state 
		 * 
		 */
		public function isAncestor(ancestor:State):Boolean
		{
			var stateTmp:State = this;
			while(stateTmp.parent != null)
			{
				if(stateTmp.parent == ancestor)
					return true;
				else
					stateTmp = stateTmp.parent;
			}
			return false;
		}
		
		public function isDescendent(descedent:state):Boolean
		{
			if(prl_children.length == 0) return false;
			else if(prl_children.indexOf(descedent) != -1) return true;
			else
			{
				for(var i:uint = 0; i < prl_children.length; i++)
				{
					if(prl_children[i].isDescendent(descedent)) return true;
				}
				return false;
			}
		}
		
		/**
		 * Return the level of the state, relative to the root state
		 * (root has depth = 0) 
		 * @return the level of the state, from 0 to N 
		 */
		public function get depth():Number
		{
			var level:Number = 0;
			var stateTmp:State = this;
			while(stateTmp.parent != null)
			{
				level++;
				stateTmp = stateTmp.parent;
			}
			return level;
		}
		
		/**
		 * Returns the level based on a ancestor (can be root or not)
		 * @param ancestor
		 * @return 
		 * @throws StateError.NOT_ANCESTOR_ERROR
		 */
		public function depthFromAncestor(ancestor:State):Number
		{
			var level:Number = 0;
			var stateTmp:State = this;
			while(stateTmp.parent != null)
			{
				level++;
				if(stateTmp.parent == ancestor) return level;
				stateTmp = stateTmp.parent;
			}
			throw new StateError(StateError.NOT_ANCESTOR_ERROR, "You can't calculate depth if the parameter is not ancestor");
		}
		
		/**
		 * Get the root of this state
		 * @return 
		 * 
		 */
		public function get root():State
		{
			var stateTmp:State = this;
			while(stateTmp.parent != null)
			{
				stateTmp = stateTmp.parent;
			}
			return stateTmp;
		}
		
		/**
		 * Search a child (or other descendant) which has the action parameter
		 * @param action
		 * @return 
		 * 
		 */
		public function searchFromAction(action:Acao):State
		{
			if(pao_action == action) return this;
			else if(prl_children.length == 0) return null;
			else
			{
				for(var i:uint = 0; i < prl_children.length; i++)
				{
					var actionChild:Acao = prl_children[i].searchFromAction(action);
					if(actionChild != null) return actionChild; 
				}
				return null;
			}
		}

		/**
		 * Returns the state parent 
		 * @return 
		 * 
		 */
		public function get parent():State
		{
			return this.pro_parent;
		}
		
		/**
		 * Returns the number of children added to the state 
		 * @return 
		 * 
		 */
		public function get numChildren():Number
		{
			return prl_children.length;
		}
		
		/**
		 * Get the action linked to this state 
		 * @return 
		 * 
		 */
		public function get pao_action():Acao
		{
			return _pao_action;
		}

		/**
		 * 
		 * @param value
		 * 
		 */		
		public function set pao_action(value:Acao):void
		{
			_pao_action = value;
			dispatchEvent(new StateEvent(StateEvent.ACTION_CHANGED));
		}
	}
}
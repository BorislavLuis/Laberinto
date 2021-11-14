#ifndef STATES_H
#define STATES_H

namespace States
{
	template<typename T>
	bool isActive(T* states, int target)
	{
		return (*states & (1 << target))== (1<<target);
	}

	template<typename T>
	void activate(T* states, int target)
	{
		*states |= 1 << target;
	}

	template<typename T>
	void uniquelyActivate(T* states, int target)
	{
		*states |= 1 << target;
		*states &= 1 << target;
	}

	template<typename t>
	void deactivate(T* states, int target)
	{
		*states &= ~(1 << target);
	}
	template<typename t>
	void toggle(T* states, int target)
	{
		*states ^= 1 << target;
	}


}

#endif // !STATES_H


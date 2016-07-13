
//----------------------------------------------------------------------------------------
/**
 * \file       RayBuffer.h
 * \author     Petr Beránek
 * \date       2016/01/04
 * \brief      File contains RayBuffer class representing buffer of RayTasks
 *
 *
*/
//----------------------------------------------------------------------------------------


#ifndef RAYBUFFER
#define RAYBUFFER

class RayTask;

struct RayLink
{
	RayTask *task;
	RayLink *next;
};
/**
Buffer for rays on CPU
*/
class RayBuffer
{
public:
	RayLink *start;
	RayLink *end;
	int size;

	RayBuffer::RayBuffer()
	{
		start = new RayLink();
		start->next  = new RayLink();
		end = start->next;
		size = 0;
	}

	void addRayTask(RayTask& task)
	{
		end->task = &task;
		end->next = new RayLink();
		end = end->next;
		size++;
	}

	RayLink *getFront()
	{
		return start->next;
	}

	void deleteFront()
	{
		if(start->next != end)
		{
			RayLink *l = start->next;
			start->next = start->next->next;
			delete l->next;
			delete l;
			size--;
		}
	}

	void clear()
	{
		RayLink *l;
		RayLink *t;
		l=start->next;

		while(l != end)
		{
			t = l;
			l = l->next;
			delete l->task;
			delete t;
		}
		start->next = end;
		size = 0;
	}


};

#endif
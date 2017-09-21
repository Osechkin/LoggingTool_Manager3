#include "experiment_scheduler.h"


Scheduler::SchedulerObject::SchedulerObject(Command _type) 
{ 
	line = 0;
	type = _type; 
	switch (type)
	{
	case Command::Exec:				mnemonic = "EXEC"; break;
	case Command::DistanceRange:	mnemonic = "DISTANCE_RANGE"; break;
	case Command::SetDistance:		mnemonic = "SET_DISTANCE"; break;
	case Command::Loop:				mnemonic = "LOOP"; break;
	case Command::Until:			mnemonic = "UNTIL"; break;
	}
}


Scheduler::Exec::Exec(int _line)
{
	type = Scheduler::Exec;
	line = _line;
	mnemonic = "EXEC";
}


Scheduler::DistanceRange::DistanceRange(int _line)
{
	type = Scheduler::DistanceRange;
	line = _line;
	mnemonic = "DISTANCE_RANGE";
}


Scheduler::SetDistance::SetDistance(int _line)
{
	type = Scheduler::SetDistance;
	line = _line;
	mnemonic = "SET_DISTANCE";
}


Scheduler::Loop::Loop(int _line)
{
	type = Scheduler::Loop;
	line = _line;
	mnemonic = "LOOP";
	index = 1;	
}


Scheduler::Until::Until(int _line)
{
	type = Scheduler::Until;
	line = _line;
	mnemonic = "UNTIL";
	ref_obj = NULL;
}


Scheduler::NOP::NOP(int _line)
{
	type = Scheduler::NOP;
	line = _line;
	mnemonic = "NOP";
}


Scheduler::Engine::~Engine()
{
	clear();
}

void Scheduler::Engine::clear()
{
	qDeleteAll(obj_list.begin(), obj_list.end());
	obj_list.clear();
}

Scheduler::SchedulerObject* Scheduler::Engine::get(int index) 
{ 
	if (index < obj_list.count()) return obj_list.at(index);
	else return NULL; 
}
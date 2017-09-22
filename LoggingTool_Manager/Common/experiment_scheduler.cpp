#include "experiment_scheduler.h"


Scheduler::SchedulerObject::SchedulerObject(Command _type) 
{ 	
	type = _type; 
	switch (type)
	{
	case Command::Exec_Cmd:
		mnemonic = "EXEC"; 
		cell_text = "<font size=4><color=darkgreen>EXEC </font>( <font color=blue>%1</font> )</font>"; 
		break;
	case Command::DistanceRange_Cmd:
		mnemonic = "DISTANCE_RANGE"; 
		cell_text = "<font size=4><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>"; 
		break;
	case Command::SetDistance_Cmd:
		mnemonic = "SET_DISTANCE"; 
		cell_text = "<font size=4><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
		break;
	case Command::Loop_Cmd:
		mnemonic = "LOOP"; 
		cell_text = "<font size=4><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
		break;
	case Command::Until_Cmd:
		mnemonic = "UNTIL"; 
		cell_text = "<font size=4 color=darkgreen>UNTIL</font>"; 
		break;
	case Command::NoP_Cmd:
		mnemonic = "NOP";
		cell_text = "<font size=4 color=darkgreen>NOP</font>"; 
		break;
	default:
		mnemonic = "NOP";
		cell_text = "<font size=4 color=darkgreen>NOP</font>"; 
		break;
	}
}


Scheduler::Exec::Exec()
{
	type = Scheduler::Exec_Cmd;	
	mnemonic = "EXEC";
	cell_text = "<font size=4><color=darkgreen>EXEC </font>( <font color=blue>%1</font> )</font>";
}


Scheduler::DistanceRange::DistanceRange()
{
	type = Scheduler::DistanceRange_Cmd;	
	mnemonic = "DISTANCE_RANGE";
	cell_text = "<font size=4><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>";
}


Scheduler::SetDistance::SetDistance()
{
	type = Scheduler::SetDistance_Cmd;	
	mnemonic = "SET_DISTANCE";
	cell_text = "<font size=4><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
}


Scheduler::Loop::Loop()
{
	type = Scheduler::Loop_Cmd;	
	mnemonic = "LOOP";
	cell_text = "<font size=4><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
	index = 1;	
}


Scheduler::Until::Until()
{
	type = Scheduler::Until_Cmd;	
	mnemonic = "UNTIL";
	cell_text = "<font size=4 color=darkgreen>UNTIL</font>";
	ref_obj = NULL;
}


Scheduler::NOP::NOP()
{
	type = Scheduler::NoP_Cmd;	
	mnemonic = "NOP";
	cell_text = "<font size=4 color=darkgreen>NOP</font>";
}


Scheduler::Engine::~Engine()
{
	clear();
}

Scheduler::SchedulerObject* Scheduler::Engine::get(int index) 
{ 
	if (index < obj_list.count()) return obj_list.at(index);
	else return NULL; 
}

void Scheduler::Engine::remove(int index)
{
	obj_list.removeAt(index);
}

void Scheduler::Engine::clear()
{
	qDeleteAll(obj_list.begin(), obj_list.end());
	obj_list.clear();
}

Scheduler::SchedulerObject* Scheduler::Engine::take(int index)
{
	if (index < 0) return NULL;
	
	return obj_list.takeAt(index);
}


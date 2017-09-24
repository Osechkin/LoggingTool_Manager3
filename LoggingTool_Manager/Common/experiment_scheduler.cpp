#include "experiment_scheduler.h"


Scheduler::SchedulerObject::SchedulerObject(Command _type) 
{ 	
	type = _type; 
	switch (type)
	{
	case Command::Exec_Cmd:
		mnemonic = "EXEC"; 
		cell_text = "<font size=3><color=darkgreen>EXEC </font>( <font color=blue>%1</font> )</font>"; 
		break;
	case Command::DistanceRange_Cmd:
		mnemonic = "DISTANCE_RANGE"; 
		cell_text = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>"; 
		break;
	case Command::SetDistance_Cmd:
		mnemonic = "SET_DISTANCE"; 
		cell_text = "<font size=3><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
		break;
	case Command::Loop_Cmd:
		mnemonic = "LOOP"; 
		cell_text = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
		break;
	case Command::Until_Cmd:
		mnemonic = "UNTIL"; 
		cell_text = "<font size=3 color=darkgreen>UNTIL</font>"; 
		break;
	case Command::NoP_Cmd:
		mnemonic = "";
		cell_text = "<font size=3 color=darkgreen></font>"; 
		break;
	default:
		mnemonic = "";
		cell_text = "<font size=3 color=darkgreen></font>"; 
		break;
	}
}


Scheduler::Exec::Exec()
{
	type = Scheduler::Exec_Cmd;	
	mnemonic = "EXEC";
	cell_text = "<font size=3><font color=darkgreen>EXEC </font>( <font color=blue>%1</font> )</font>";
	jseq_name = "";
	jseq_path = "";
	data_file = "";


}


Scheduler::DistanceRange::DistanceRange()
{
	type = Scheduler::DistanceRange_Cmd;	
	mnemonic = "DISTANCE_RANGE";
	cell_text = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>";
	from = 0;
	to = 0;
	step = 0;
}


Scheduler::SetDistance::SetDistance()
{
	type = Scheduler::SetDistance_Cmd;	
	mnemonic = "SET_DISTANCE";
	cell_text = "<font size=3><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
	position = 0;
}


Scheduler::Loop::Loop()
{
	type = Scheduler::Loop_Cmd;	
	mnemonic = "LOOP";
	cell_text = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
	index = 0;	
	to = 1;
}


Scheduler::Until::Until()
{
	type = Scheduler::Until_Cmd;	
	mnemonic = "UNTIL";
	cell_text = "<font size=3 color=darkgreen>UNTIL</font>";
	ref_obj = NULL;
}


Scheduler::NOP::NOP()
{
	type = Scheduler::NoP_Cmd;	
	mnemonic = "";
	cell_text = "<font size=3 color=darkgreen></font>";
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
	Scheduler::SchedulerObject *obj = obj_list.takeAt(index);
	delete obj;	
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


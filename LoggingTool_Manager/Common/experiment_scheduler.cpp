#include <QComboBox>
#include <QLineEdit>

#include "experiment_scheduler.h"


Scheduler::SchedulerObject::SchedulerObject(Command _type) 
{ 	
	type = _type; 
	switch (type)
	{
	case Command::Exec_Cmd:		
		cell_text = "<font size=3><font color=darkgreen>EXEC </font>( '<font color=blue>%1</font>', '<font color=blue>%2</font>' )</font>"; 
		break;
	case Command::DistanceRange_Cmd:		
		cell_text = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>"; 
		break;
	case Command::SetDistance_Cmd:		
		cell_text = "<font size=3><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
		break;
	case Command::Loop_Cmd:		
		cell_text = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
		break;
	case Command::End_Cmd:		
		cell_text = "<font size=3 color=darkgreen>END</font>"; 
		break;
	case Command::NoP_Cmd:		
		cell_text = "<font size=3 color=darkgreen></font>"; 
		break;
	default:		
		cell_text = "<font size=3 color=darkgreen></font>"; 
		break;
	}
}


Scheduler::Exec::Exec(QStringList jseqs, QString _data_file)
{
	type = Scheduler::Exec_Cmd;			

	QComboBox *cboxJSeqs = new QComboBox;
	cboxJSeqs->addItems(jseqs);
	jseq_name = cboxJSeqs->currentText();
	connect(cboxJSeqs, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(changeJSeq(const QString&)));

	QLineEdit *ledDataFile = new QLineEdit(data_file);
	ledDataFile->setToolTip(data_file);
	connect(ledDataFile, SIGNAL(textEdited(const QString&)), this, SLOT(editFileName(const QString&)));

	data_file = _data_file;
	cell_text = QString("<font size=3><font color=darkgreen>EXEC </font>( '<font color=blue>%1</font>', '<font color=blue>%2</font>' )</font>").arg(jseq_name, data_file);

	Scheduler::SettingsItem *param_item_1 = new Scheduler::SettingsItem(tr("Sequence:"), Scheduler::ComboBox, cboxJSeqs, "");
	Scheduler::SettingsItem *param_item_2 = new Scheduler::SettingsItem(tr("Data File:"), Scheduler::LineEdit, ledDataFile, "");
	param_objects.append(param_item_1);
	param_objects.append(param_item_2);
}

Scheduler::Exec::~Exec()
{
	qDeleteAll(param_objects.begin(), param_objects.end());
	param_objects.clear();
}


Scheduler::DistanceRange::DistanceRange()
{
	type = Scheduler::DistanceRange_Cmd;		
	cell_text = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>";
	from = 0;
	to = 0;
	step = 0;
}


Scheduler::SetDistance::SetDistance()
{
	type = Scheduler::SetDistance_Cmd;		
	cell_text = "<font size=3><font color=darkgreen>SET_DISTANCE </font>( <font color=blue>%1</font> )</font>"; 
	position = 0;
}


Scheduler::Loop::Loop()
{
	type = Scheduler::Loop_Cmd;		
	cell_text = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
	index = 0;	
	to = 1;
}


Scheduler::End::End()
{
	type = Scheduler::End_Cmd;		
	cell_text = "<font size=3 color=darkgreen>END</font>";
	ref_obj = NULL;
}


Scheduler::NOP::NOP()
{
	type = Scheduler::NoP_Cmd;		
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


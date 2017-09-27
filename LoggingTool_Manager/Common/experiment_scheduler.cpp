#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QGridLayout>
#include "QFileDialog"

#include "../Dialogs/data_filename_dialog.h"

#include "experiment_scheduler.h"


Scheduler::FileBrowser::FileBrowser(QString _file_name, QWidget *parent /* = 0 */)
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	gridLayout->setHorizontalSpacing(0);
	gridLayout->setVerticalSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	ledFileName = new QLineEdit(this);
	ledFileName->setObjectName(QStringLiteral("ledFileName"));
	ledFileName->setReadOnly(true);

	gridLayout->addWidget(ledFileName, 0, 0, 1, 1);

	QToolButton *tbtDataFileName = new QToolButton(this);
	tbtDataFileName->setObjectName(QStringLiteral("tbtDataFileName"));
	tbtDataFileName->setText("...");
	tbtDataFileName->setToolTip(tr("Enter Data file Name"));

	gridLayout->addWidget(tbtDataFileName, 0, 1, 1, 1);

	connect(tbtDataFileName, SIGNAL(clicked()), this, SLOT(enterDataFileName()));

	ledFileName->setText(_file_name);

	QFileInfo file_info(_file_name);
	file_name = file_info.fileName();
	file_path = file_info.absoluteDir().path();
}

void Scheduler::FileBrowser::enterDataFileName()
{
	DataFileNameDialog dlg(file_path, file_name);
	if (dlg.exec()) file_name = dlg.getDataFileName();

	if (file_name.isEmpty()) ledFileName->setStyleSheet("QLineEdit {background-color: red; color: darkblue}");
	else ledFileName->setStyleSheet("QLineEdit {background-color: white; color: darkblue;}");

	QDir dir(file_path);
	QFileInfo file_info(dir, file_name);

	emit new_filename(file_info);

	QString canonical = file_info.absoluteFilePath();
	ledFileName->setText(canonical);
}


Scheduler::SchedulerObject::SchedulerObject(Command _type) 
{ 	
	type = _type; 
	switch (type)
	{
	case Command::Exec_Cmd:		
		cell_text_template = "<font size=3><font color=darkgreen>EXEC </font>( '<font color=blue>%1</font>', '<font color=blue>%2</font>' )</font>"; 
		cell_text = "";
		break;
	case Command::DistanceRange_Cmd:		
		cell_text_template = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>"; 
		cell_text = "";
		break;
	case Command::SetPosition_Cmd:		
		cell_text_template = "<font size=3><font color=darkgreen>SET_POSITION </font>( <font color=blue>%1</font> )</font>"; 
		cell_text = "";
		break;
	case Command::Loop_Cmd:		
		cell_text_template = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
		cell_text = "";
		break;
	case Command::End_Cmd:		
		cell_text_template = "<font size=3 color=darkgreen>END</font>"; 
		cell_text = "";
		break;
	case Command::NoP_Cmd:		
		cell_text_template = "<font size=3 color=darkgreen></font>"; 
		cell_text = "";
		break;
	default:		
		cell_text_template = "<font size=3 color=darkgreen></font>"; 
		cell_text = "";
		break;
	}
}


Scheduler::Exec::Exec(QStringList jseqs, QString jseq_file, QString _data_file)
{
	type = Scheduler::Exec_Cmd;			
	jseq_list = jseqs;
	jseq_name = jseq_file;
	if (jseq_list.isEmpty()) jseq_name = "";
	else if (!jseq_list.contains(jseq_file)) jseq_name = jseq_list.first();

	data_file = _data_file;
	cell_text_template = QString("<font size=3><font color=darkgreen>EXEC </font>( '<font color=blue>%1</font>' )</font>");
	cell_text = cell_text_template.arg(jseq_name);

	Scheduler::SettingsItem *param_item_1 = new Scheduler::SettingsItem(tr("Sequence:"), Scheduler::ComboBox, "");
	Scheduler::SettingsItem *param_item_2 = new Scheduler::SettingsItem(tr("Data File:"), Scheduler::FileBrowse, "");
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
	cell_text_template = "<font size=3><font color=darkgreen>DISTANCE_RANGE </font>( <font color=blue>%1</font> : <font color=blue>%2</font> : <font color=blue>%3</font> )</font>";
	cell_text = "";
	from = 0;
	to = 0;
	step = 0;
}


Scheduler::SetPosition::SetPosition(double _position)
{
	type = Scheduler::SetPosition_Cmd;		
	cell_text_template = "<font size=3><font color=darkgreen>SET_POSITION </font>( <font color=blue>%1</font> )</font>"; 	
	position = _position;		
	cell_text = cell_text_template.arg(position);

	Scheduler::SettingsItem *param_item_1 = new Scheduler::SettingsItem(tr("Position:"), Scheduler::DoubleSpinBox, "mm");	
	param_objects.append(param_item_1);	
}


Scheduler::Loop::Loop()
{
	type = Scheduler::Loop_Cmd;		
	cell_text_template = "<font size=3><font color=darkgreen>LOOP </font>( <font color=blue>%1</font> : <font color=blue>%2</font> )</font>"; 
	cell_text = "";
	index = 0;	
	to = 1;
}


Scheduler::End::End()
{
	type = Scheduler::End_Cmd;		
	cell_text_template = "<font size=3 color=darkgreen>END</font>";
	cell_text = "";
	ref_obj = NULL;
}


Scheduler::NOP::NOP()
{
	type = Scheduler::NoP_Cmd;		
	cell_text_template = "<font size=3 color=darkgreen></font>";
	cell_text = "";
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


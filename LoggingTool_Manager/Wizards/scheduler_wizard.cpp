#include <QMenu>
#include <QAction>
#include <QLabel>

#include "scheduler_wizard.h"


SchedulerWizard::SchedulerWizard(QSettings *settings, QWidget *parent) : QWidget(parent), ui(new Ui::SchedulerWizard)
{
	ui->setupUi(this);

	ui->tbtAdd->setIcon(QIcon(":/images/add_button.png"));
	ui->tbtRemove->setIcon(QIcon(":/images/disconnect.png"));
	ui->tbtOpen->setIcon(QIcon(":/images/open.png"));
	ui->tbtSave->setIcon(QIcon(":/images/save.png"));
		
	QStringList headlist;
	ui->treeWidgetParam->setColumnCount(3);
	headlist << tr("Parameter") << tr("Value") << tr("Units");
	ui->treeWidgetParam->setHeaderLabels(headlist);
	ui->treeWidgetParam->setColumnWidth(0,120);
	ui->treeWidgetParam->setColumnWidth(1,250);	
	ui->treeWidgetParam->header()->setFont(QFont("Arial", 10, 0, false));

	QMenu *menu_add = new QMenu(this);
	QAction *a_exec = new QAction("EXEC", this);
	QAction *a_distance_range = new QAction("DISTANCE_RANGE - END", this);
	QAction *a_set_distance = new QAction("SET_DISTANCE", this);
	QAction *a_loop = new QAction("LOOP - END", this);
	//QAction *a_until = new QAction("UNTIL", this);
	//QAction *a_nop = new QAction("NOP", this);
	menu_add->addAction(a_exec);
	menu_add->addAction(a_distance_range);
	menu_add->addAction(a_set_distance);
	menu_add->addAction(a_loop);
	//menu_add->addAction(a_until);
	//menu_add->addAction(a_nop);
	ui->tbtAdd->setMenu(menu_add);
	//ui->tbtAdd->setPopupMode(QToolButton::InstantPopup);

	QMenu *menu_remove = new QMenu(this);
	QAction *a_remove_all = new QAction("Remove All", this);
	menu_remove->addAction(a_remove_all);
	ui->tbtRemove->setMenu(menu_remove);

	QMenu *menu_save = new QMenu(this);
	QAction *a_save = new QAction(tr("Save"), this);
	QAction *a_save_as = new QAction(tr("Save As..."), this);	
	menu_save->addAction(a_save);
	menu_save->addAction(a_save_as);
	ui->tbtSave->setMenu(menu_save);
	//ui->tbtSave->setPopupMode(QToolButton::InstantPopup);

	ui->tableWidgetExp->installEventFilter(this);

	scheduler_engine.clear();

	connect(ui->tbtAdd, SIGNAL(clicked()), this, SLOT(addItemNOP()));	
	connect(a_exec, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(a_set_distance, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(a_distance_range, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(a_loop, SIGNAL(triggered()), this, SLOT(addItem()));
	//connect(a_until, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(ui->tbtRemove, SIGNAL(clicked()), this, SLOT(removeItem()));
	connect(a_remove_all, SIGNAL(triggered()), this, SLOT(removeAllItems()));
}

SchedulerWizard::~SchedulerWizard()
{
	
}

bool SchedulerWizard::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == ui->tableWidgetExp)
	{
		if (event->type() == QEvent::Resize)
		{
			int w = ui->tableWidgetExp->size().width();
			ui->tableWidgetExp->setColumnWidth(0,w);
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}


void SchedulerWizard::addItemNOP()
{
	int rows = ui->tableWidgetExp->rowCount();
	int cur_row = ui->tableWidgetExp->currentRow();	

	if (cur_row < 0 || cur_row == rows-1) 
	{
		ui->tableWidgetExp->setRowCount(rows+1);
		Scheduler::NOP *cmd_nop = new Scheduler::NOP;
		scheduler_engine.add(cmd_nop);
		showItemParameters(cmd_nop);

		QLabel *lb = new QLabel(cmd_nop->cell_text);
		ui->tableWidgetExp->setCellWidget(rows, 0, lb);
		ui->tableWidgetExp->setCurrentCell(rows, 0);		
	}
	else 
	{
		Scheduler::NOP *cmd_nop = new Scheduler::NOP;
		scheduler_engine.insert(cur_row, cmd_nop);
		insertItem(cur_row, cmd_nop->cell_text);
		showItemParameters(cmd_nop);
	}
}

void SchedulerWizard::addItem()
{	
	QAction *a = qobject_cast<QAction*>(sender());
	if (!a) return;
	QString txt = a->text();
	
	Scheduler::SchedulerObjList cmd_obj_list;	
	if (txt == "EXEC")
	{
		Scheduler::Exec *exec_obj = new Scheduler::Exec(jseq_list, data_file);
		connect(exec_obj, SIGNAL(changed()), this, SLOT(update()));
		cmd_obj_list.append(exec_obj);
	}
	else if (txt == "DISTANCE_RANGE - END")	  
	{ 
		Scheduler::DistanceRange *dist__range_obj = new Scheduler::DistanceRange;
		connect(dist__range_obj, SIGNAL(changed()), this, SLOT(update()));
		
		cmd_obj_list.append(dist__range_obj); 		
		cmd_obj_list.append(new Scheduler::End); 
	}
	else if (txt == "SET_DISTANCE")	
	{
		Scheduler::SetDistance *dist_obj = new Scheduler::SetDistance;
		connect(dist_obj, SIGNAL(changed()), this, SLOT(update()));
		cmd_obj_list.append(new Scheduler::SetDistance);
	}
	else if (txt == "LOOP - END")			  
	{ 
		Scheduler::Loop *loop_obj = new Scheduler::Loop;
		connect(loop_obj, SIGNAL(changed()), this, SLOT(update()));

		cmd_obj_list.append(loop_obj); 
		cmd_obj_list.append(new Scheduler::End); 
	}	
	if (cmd_obj_list.isEmpty()) return;

	for (int i = 0; i < cmd_obj_list.count(); i++)
	{
		int rows = ui->tableWidgetExp->rowCount();
		int cur_row = ui->tableWidgetExp->currentRow();	

		Scheduler::SchedulerObject *cmd_obj = cmd_obj_list.at(i);
		if (cur_row < 0 || cur_row == rows-1) 
		{		
			ui->tableWidgetExp->setRowCount(rows+1);
			scheduler_engine.add(cmd_obj);
			
			QLabel *lb = new QLabel(cmd_obj->cell_text);
			ui->tableWidgetExp->setCellWidget(rows, 0, lb);
			ui->tableWidgetExp->setCurrentCell(rows, 0);		
		}
		else 
		{		
			scheduler_engine.insert(cur_row, cmd_obj);			
			insertItem(cur_row, cmd_obj->cell_text);			
		}
		showItemParameters(cmd_obj);
	}	
}

void SchedulerWizard::insertItem(int row, QString cmd)
{
	if (row < 0) return;

	ui->tableWidgetExp->insertRow(row);
	
	QLabel *lb = new QLabel(cmd);
	ui->tableWidgetExp->setCellWidget(row, 0, lb);
	ui->tableWidgetExp->setCurrentCell(row, 0);
}

void SchedulerWizard::removeItem(int row)
{
	int rows = scheduler_engine.getObjectList().count();
	if (row < 0 || row >= rows) return;
	
	ui->tableWidgetExp->removeRow(row);
	scheduler_engine.remove(row);
}

void SchedulerWizard::removeItem()
{
	int cur_row = ui->tableWidgetExp->currentRow();	
	if (cur_row < 0) return;	

	Scheduler::Command cmd_type = scheduler_engine.getObjectList()[cur_row]->type;
	if (cmd_type == Scheduler::Command::Loop_Cmd || cmd_type == Scheduler::Command::DistanceRange_Cmd)
	{
		ui->tableWidgetExp->removeRow(cur_row);
		scheduler_engine.remove(cur_row);
		int rows = scheduler_engine.getObjectList().count();
		for (int i = cur_row; i < rows; i++)
		{
			Scheduler::Command cmd_type = scheduler_engine.getObjectList()[i]->type;
			if (cmd_type == Scheduler::Command::End_Cmd)
			{
				ui->tableWidgetExp->removeRow(i);
				scheduler_engine.remove(i);
				break;
			}
		}
	}
	else if (cmd_type == Scheduler::Command::End_Cmd)
	{
		ui->tableWidgetExp->removeRow(cur_row);
		scheduler_engine.remove(cur_row);
		int rows = scheduler_engine.getObjectList().count();
		if (cur_row > 0)
		{
			for (int i = cur_row-1; i >= 0; --i)
			{
				Scheduler::Command cmd_type = scheduler_engine.getObjectList()[i]->type;
				if (cmd_type == Scheduler::Command::Loop_Cmd || cmd_type == Scheduler::Command::DistanceRange_Cmd)
				{
					ui->tableWidgetExp->removeRow(i);
					scheduler_engine.remove(i);
					break;
				}
			}
		}		
	}
	else
	{
		ui->tableWidgetExp->removeRow(cur_row);
		scheduler_engine.remove(cur_row);
	}
}

void SchedulerWizard::removeAllItems()
{
	int rows = scheduler_engine.getObjectList().count();

	for (int i = 0; i < rows; i++)
	{
		removeItem(0);
	}
}

void SchedulerWizard::update()
{
	Scheduler::SchedulerObject *obj_sender = qobject_cast<Scheduler::SchedulerObject*>(sender());
	Scheduler::Command cmd_sender = obj_sender->type;

	int rows = scheduler_engine.getObjectList().count();
	for (int i = 0; i < rows; i++)
	{
		Scheduler::SchedulerObject *obj = scheduler_engine.getObjectList()[i];
		if (obj == obj_sender)
		{
			switch (cmd_sender)
			{
			case Scheduler::Command::DistanceRange_Cmd:	break;
			case Scheduler::Command::Exec_Cmd:		
				{
					QLabel *lb = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(i,0));
					Scheduler::Exec *exec_obj = qobject_cast<Scheduler::Exec*>(obj_sender);
					lb->setText(exec_obj->cell_text.arg(exec_obj->jseq_name).arg(exec_obj->data_file));
					break;
				}
			case Scheduler::Command::Loop_Cmd:			break;
			case Scheduler::Command::SetDistance_Cmd:	break;
			case Scheduler::Command::End_Cmd:			break;
			case Scheduler::Command::NoP_Cmd:			break;
			default: break;
			}
		}
	}
}

// Read text in tableWidgetExp excluding HTML QString text:
// QString  txt = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(cur_row,0))->text().remove(QRegExp("<[^>]*>"));


void SchedulerWizard::showItemParameters(Scheduler::SchedulerObject *obj)
{
	qDeleteAll(tree_items);
	tree_items.clear();

	Scheduler::Command obj_type = obj->type;
	switch (obj_type)
	{
	case Scheduler::Command::DistanceRange_Cmd:	break;
	case Scheduler::Command::Exec_Cmd:
		{
			int items_count = obj->param_objects.count();
			for (int i = 0; i < items_count; i++)
			{
				Scheduler::SettingsItem *param_item = obj->param_objects[i];
				QLabel *lbl_title = new QLabel(param_item->title);
				QPalette palette = lbl_title->palette();
				palette.setColor(QPalette::Text, (QColor(Qt::darkBlue)));		
				lbl_title->setPalette(palette);
				QWidget *widget = param_item->widget;
				QLabel *lbl_units = new QLabel(param_item->units);
				lbl_units->setPalette(palette);

				QTreeWidgetItem *twi = new QTreeWidgetItem(ui->treeWidgetParam);
				ui->treeWidgetParam->setItemWidget(twi, 0, lbl_title);
				ui->treeWidgetParam->setItemWidget(twi, 1, widget);
				ui->treeWidgetParam->setItemWidget(twi, 2, lbl_units);
				tree_items.append(twi);
			}
			break;
		}
	case Scheduler::Command::SetDistance_Cmd:	break;
	case Scheduler::Command::Loop_Cmd:			break;
	case Scheduler::Command::End_Cmd:			break;
	case Scheduler::Command::NoP_Cmd:			break;
	default: break;
	}
}
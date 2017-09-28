#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QDir>

#include "../Widgets/distance_meter_widget.h"

#include "scheduler_wizard.h"


SchedulerWizard::SchedulerWizard(QSettings *settings, DepthTemplateWizard *depth_wiz, QWidget *parent) : QWidget(parent), ui(new Ui::SchedulerWizard)
{
	ui->setupUi(this);

	depth_wizard = depth_wiz;
	app_settings = settings;
	setDataFileSettings();

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
	QAction *a_distance_range = new QAction("POSITION_LOOP - END_LOOP", this);
	QAction *a_set_position = new QAction("SET_POSITION", this);
	QAction *a_loop = new QAction("LOOP - END_LOOP", this);
	menu_add->addAction(a_exec);
	menu_add->addAction(a_distance_range);
	menu_add->addAction(a_set_position);
	menu_add->addAction(a_loop);	
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
	ui->tableWidgetExp->setStyleSheet("QTableWidget::item {padding-left: 10px }");
	QItemSelectionModel *sm = ui->tableWidgetExp->selectionModel();
	connect(sm, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentItemSelected(QModelIndex,QModelIndex)));

	scheduler_engine.clear();

	connect(ui->tbtAdd, SIGNAL(clicked()), this, SLOT(addItemNOP()));	
	connect(a_exec, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(a_set_position, SIGNAL(triggered()), this, SLOT(addItem()));
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
		
		QLabel *lb = new QLabel(cmd_nop->cell_text);
		ui->tableWidgetExp->setCellWidget(rows, 0, lb);
		ui->tableWidgetExp->setCurrentCell(rows, 0);		
	}
	else 
	{
		Scheduler::NOP *cmd_nop = new Scheduler::NOP;
		scheduler_engine.insert(cur_row, cmd_nop);
		insertItem(cur_row, cmd_nop->cell_text);		
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
		data_file = generateDataFileName();
		Scheduler::Exec *exec_obj = new Scheduler::Exec(jseq_list, jseq_file, data_file);
		connect(exec_obj, SIGNAL(changed()), this, SLOT(update()));
		cmd_obj_list.append(exec_obj);
	}
	else if (txt == "POSITION_LOOP - END_LOOP")	  
	{ 
		Scheduler::DistanceRange *dist_range_obj = new Scheduler::DistanceRange;
		AbstractDepthMeter *abs_depthmeter = depth_wizard->getCurrentDepthMeter();
		if (abs_depthmeter)
		{
			if (abs_depthmeter->getType() == AbstractDepthMeter::LeuzeDistanceMeter)
			{
				if (LeuzeDistanceMeterWidget *leuze_meter = qobject_cast<LeuzeDistanceMeterWidget*>(abs_depthmeter))
				{
					QPair<double,double> bounds = leuze_meter->getBounds();
					QPair<double,double> from_to = leuze_meter->getFromTo();
					double _step = leuze_meter->getStep();
					
					dist_range_obj->setFromToStep(from_to, _step);
					dist_range_obj->setBounds(bounds);
				}				
			}
		}

		connect(dist_range_obj, SIGNAL(changed()), this, SLOT(update()));
		
		cmd_obj_list.append(dist_range_obj); 		
		cmd_obj_list.append(new Scheduler::End); 
	}
	else if (txt == "SET_POSITION")	
	{
		Scheduler::SetPosition *dist_obj = new Scheduler::SetPosition;
		AbstractDepthMeter *abs_depthmeter = depth_wizard->getCurrentDepthMeter();
		if (abs_depthmeter)
		{
			if (abs_depthmeter->getType() == AbstractDepthMeter::LeuzeDistanceMeter)
			{
				if (LeuzeDistanceMeterWidget *leuze_meter = qobject_cast<LeuzeDistanceMeterWidget*>(abs_depthmeter))
				{
					QPair<double,double> bounds = leuze_meter->getBounds();
					double leuze_pos = 100*leuze_meter->getOrderedDepth(); // convert to [cm]
					dist_obj->changePosition(leuze_pos);
					dist_obj->setBounds(bounds);
				}				
			}
		}
		connect(dist_obj, SIGNAL(changed()), this, SLOT(update()));
		cmd_obj_list.append(dist_obj);
	}
	else if (txt == "LOOP - END_LOOP")			  
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
	int table_rows = ui->tableWidgetExp->rowCount();
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
			case Scheduler::Command::DistanceRange_Cmd:	
				{
					QLabel *lb = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(i,0));
					Scheduler::DistanceRange *dist_range_obj = qobject_cast<Scheduler::DistanceRange*>(obj_sender);
					dist_range_obj->cell_text = dist_range_obj->cell_text_template.arg(dist_range_obj->from).arg(dist_range_obj->step).arg(dist_range_obj->to);
					lb->setText(dist_range_obj->cell_text);
					break;
				}
			case Scheduler::Command::Exec_Cmd:		
				{
					QLabel *lb = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(i,0));
					Scheduler::Exec *exec_obj = qobject_cast<Scheduler::Exec*>(obj_sender);
					exec_obj->cell_text = exec_obj->cell_text_template.arg(exec_obj->jseq_name);
					lb->setText(exec_obj->cell_text);
					break;
				}
			case Scheduler::Command::Loop_Cmd:			
				{
					QLabel *lb = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(i,0));
					Scheduler::Loop *loop_obj = qobject_cast<Scheduler::Loop*>(obj_sender);
					loop_obj->cell_text = loop_obj->cell_text_template.arg(loop_obj->counts);
					lb->setText(loop_obj->cell_text);
					break;
				}
			case Scheduler::Command::SetPosition_Cmd:	
				{
					QLabel *lb = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(i,0));
					Scheduler::SetPosition *setpos_obj = qobject_cast<Scheduler::SetPosition*>(obj_sender);
					setpos_obj->cell_text = setpos_obj->cell_text_template.arg(setpos_obj->position);
					lb->setText(setpos_obj->cell_text);
					break;
				}				
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
	//ui->treeWidgetParam->clear();
	
	QWidgetList widget_params;
	Scheduler::Command obj_type = obj->type;
	switch (obj_type)
	{
	case Scheduler::Command::DistanceRange_Cmd:	
		{
			Scheduler::DistanceRange *dist_range_obj = qobject_cast<Scheduler::DistanceRange*>(obj);
			if (!dist_range_obj) return;

			QDoubleSpinBox *dsboxFrom = new QDoubleSpinBox;				
			dsboxFrom->setMinimum(dist_range_obj->lower_bound);
			dsboxFrom->setMaximum(dist_range_obj->upper_bound);
			dsboxFrom->setValue(dist_range_obj->from);	
			connect(dsboxFrom, SIGNAL(valueChanged(double)), dist_range_obj, SLOT(changeFrom(double)));

			QDoubleSpinBox *dsboxTo = new QDoubleSpinBox;				
			dsboxTo->setMinimum(dist_range_obj->lower_bound);
			dsboxTo->setMaximum(dist_range_obj->upper_bound);
			dsboxTo->setValue(dist_range_obj->to);	
			connect(dsboxTo, SIGNAL(valueChanged(double)), dist_range_obj, SLOT(changeTo(double)));

			QDoubleSpinBox *dsboxStep = new QDoubleSpinBox;					
			dsboxStep->setValue(dist_range_obj->step);	
			connect(dsboxStep, SIGNAL(valueChanged(double)), dist_range_obj, SLOT(changeStep(double)));
						
			widget_params << dsboxFrom << dsboxTo << dsboxStep;
			break;
		}
	case Scheduler::Command::Exec_Cmd:
		{
			Scheduler::Exec *exec_obj = qobject_cast<Scheduler::Exec*>(obj);
			if (!exec_obj) return;
						
			QComboBox *cboxJSeqs = new QComboBox;
			cboxJSeqs->addItems(exec_obj->jseq_list);			
			connect(cboxJSeqs, SIGNAL(currentIndexChanged(const QString&)), exec_obj, SLOT(changeJSeq(const QString&)));

			Scheduler::FileBrowser *fileBrowser = new Scheduler::FileBrowser(exec_obj->data_file);
			fileBrowser->setToolTip(exec_obj->data_file);
			connect(fileBrowser, SIGNAL(new_filename(const QFileInfo&)), exec_obj, SLOT(editFileName(const QFileInfo&)));

			//QWidgetList widget_params;
			widget_params << cboxJSeqs << fileBrowser;

			break;
		}
	case Scheduler::Command::SetPosition_Cmd:	
		{
			Scheduler::SetPosition *setpos_obj = qobject_cast<Scheduler::SetPosition*>(obj);
			if (!setpos_obj) return;

			QDoubleSpinBox *dsboxPos = new QDoubleSpinBox;				
			dsboxPos->setMinimum(setpos_obj->lower_bound);
			dsboxPos->setMaximum(setpos_obj->upper_bound);
			dsboxPos->setValue(setpos_obj->position);	
			connect(dsboxPos, SIGNAL(valueChanged(double)), setpos_obj, SLOT(changePosition(double)));

			widget_params << dsboxPos;

			break;
		}
	case Scheduler::Command::Loop_Cmd:			
		{
			Scheduler::Loop *loop_obj = qobject_cast<Scheduler::Loop*>(obj);
			if (!loop_obj) return;

			QSpinBox *sboxCounts = new QSpinBox;				
			sboxCounts->setMinimum(loop_obj->lower_bound);
			sboxCounts->setValue(loop_obj->counts);	
			connect(sboxCounts, SIGNAL(valueChanged(int)), loop_obj, SLOT(changeCounts(int)));
						
			widget_params << sboxCounts;

			break;
		}
	case Scheduler::Command::End_Cmd:			break;
	case Scheduler::Command::NoP_Cmd:			break;
	default: break;
	}

	int items_count = obj->param_objects.count();
	for (int i = 0; i < items_count; i++)
	{
		Scheduler::SettingsItem *param_item = obj->param_objects[i];
		QLabel *lbl_title = new QLabel(param_item->title);
		QPalette palette = lbl_title->palette();
		palette.setColor(QPalette::Text, (QColor(Qt::darkBlue)));		
		lbl_title->setPalette(palette);
		QWidget *widget = widget_params[i];
		QLabel *lbl_units = new QLabel(param_item->units);
		lbl_units->setPalette(palette);

		QTreeWidgetItem *twi = new QTreeWidgetItem(ui->treeWidgetParam);
		ui->treeWidgetParam->setItemWidget(twi, 0, lbl_title);
		ui->treeWidgetParam->setItemWidget(twi, 1, widget);
		ui->treeWidgetParam->setItemWidget(twi, 2, lbl_units);				
	}
}

void SchedulerWizard::currentItemSelected(QModelIndex index1, QModelIndex index2)
{
	ui->treeWidgetParam->clear();

	int row = index1.row();
	if (row < 0) return;
	Scheduler::SchedulerObject *obj = scheduler_engine.get(row);
	if (!obj) return;

	showItemParameters(obj);
}

void SchedulerWizard::setDataFileSettings()
{
	if (app_settings->contains("SaveDataSettings/Path")) datafile_path = app_settings->value("SaveDataSettings/Path").toString();
	else
	{
		QString cur_dir = QCoreApplication::applicationDirPath();		
		datafile_path = cur_dir;
		app_settings->setValue("SaveDataSettings/Path", datafile_path);
	}
		
	if (app_settings->contains("SaveDataSettings/Prefix")) datafile_prefix = app_settings->value("SaveDataSettings/Prefix").toString();
	else
	{
		datafile_prefix = "data";
		app_settings->setValue("SaveDataSettings/Prefix", datafile_prefix);
	}	
	
	if (app_settings->contains("SaveDataSettings/Postfix")) datafile_postfix = app_settings->value("SaveDataSettings/Postfix").toString();
	else
	{
		datafile_postfix = "";
		app_settings->setValue("SaveDataSettings/Postfix", datafile_postfix);
	}
		
	QStringList items;
	items << tr("dat") << tr("txt");	
	if (app_settings->contains("SaveDataSettings/Extension")) datafile_extension = app_settings->value("SaveDataSettings/Extension").toString();
	else
	{
		datafile_extension = "dat";
		app_settings->setValue("SaveDataSettings/Extension", datafile_extension);
	}	
}

QString SchedulerWizard::generateDataFileName()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dateTime_str = dateTime.toString("yyyy-MM-dd_hh-mm-ss-zzz");
	QDir dir(datafile_path);
	QString file_name = "";
	if (!datafile_prefix.isEmpty()) file_name += datafile_prefix + "_";
	file_name += dateTime_str;
	if (!datafile_postfix.isEmpty()) file_name += "_" + datafile_postfix;
	static int file_counter = 1;
	file_name += "_" + QString::number(file_counter++);
	file_name += "." + datafile_extension;
	
	QFileInfo file_info(dir, file_name);	
	return file_info.absoluteFilePath();
}
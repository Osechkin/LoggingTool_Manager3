#include <QMenu>
#include <QAction>

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
	ui->treeWidgetParam->setColumnWidth(0,250);
	ui->treeWidgetParam->setColumnWidth(1,75);	
	ui->treeWidgetParam->header()->setFont(QFont("Arial", 10, 0, false));

	QMenu *menu_add = new QMenu(this);
	QAction *a_exec = new QAction("EXEC", this);
	QAction *a_distance_range = new QAction("DISTANCE_RANGE", this);
	QAction *a_set_distance = new QAction("SET_DISTANCE", this);
	QAction *a_loop = new QAction("LOOP", this);
	QAction *a_until = new QAction("UNTIL", this);
	//QAction *a_nop = new QAction("NOP", this);
	menu_add->addAction(a_exec);
	menu_add->addAction(a_distance_range);
	menu_add->addAction(a_set_distance);
	menu_add->addAction(a_loop);
	menu_add->addAction(a_until);
	//menu_add->addAction(a_nop);
	ui->tbtAdd->setMenu(menu_add);
	//ui->tbtAdd->setPopupMode(QToolButton::InstantPopup);

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
	connect(ui->tbtRemove, SIGNAL(clicked()), this, SLOT(removeItem()));
	connect(a_distance_range, SIGNAL(clicked()), this, SLOT(addItem()));
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

}

void SchedulerWizard::insertItem(int row, QString cmd)
{
	if (row < 0) return;

	ui->tableWidgetExp->insertRow(row);
	
	QLabel *lb = new QLabel(cmd);
	ui->tableWidgetExp->setCellWidget(row, 0, lb);
	ui->tableWidgetExp->setCurrentCell(row, 0);
}

void SchedulerWizard::removeItem()
{
	int cur_row = ui->tableWidgetExp->currentRow();	
	if (cur_row < 0) return;	

	ui->tableWidgetExp->removeRow(cur_row);
	scheduler_engine.remove(cur_row);
}

// Read text in tableWidgetExp excluding HTML QString text:
// QString  txt = qobject_cast<QLabel*>(ui->tableWidgetExp->cellWidget(cur_row,0))->text().remove(QRegExp("<[^>]*>"));
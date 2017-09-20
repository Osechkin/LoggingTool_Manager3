#include "scheduler_wizard.h"


SchedulerWizard::SchedulerWizard(QSettings *settings, QWidget *parent) : QWidget(parent), ui(new Ui::SchedulerWizard)
{
	ui->setupUi(this);

	ui->tbtAdd->setIcon(QIcon(":/images/add_button.png"));
	ui->tbtRemove->setIcon(QIcon(":/images/disconnect.png"));
	ui->tbtOpen->setIcon(QIcon(":/images/open.png"));
	ui->tbtSave->setIcon(QIcon(":/images/save.png"));

	QStringList headlist;
	ui->treeWidgetPlan->setColumnCount(1);
	headlist << tr("Schedule") << tr("Value") << tr("Units");
	ui->treeWidgetPlan->setHeaderLabels(headlist);
	ui->treeWidgetPlan->header()->setFont(QFont("Arial", 10, 0, false));		

	headlist.clear();
	ui->treeWidgetParam->setColumnCount(3);
	headlist << tr("Parameter") << tr("Value") << tr("Units");
	ui->treeWidgetParam->setHeaderLabels(headlist);
	ui->treeWidgetParam->setColumnWidth(0,300);
	ui->treeWidgetParam->setColumnWidth(1,75);	
	ui->treeWidgetParam->header()->setFont(QFont("Arial", 10, 0, false));
}

SchedulerWizard::~SchedulerWizard()
{

}


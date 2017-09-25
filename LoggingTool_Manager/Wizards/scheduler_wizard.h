#ifndef SCHEDULER_WIZARD_H
#define SCHEDULER_WIZARD_H


#include <QSettings>
//#include <QList>
//#include <QDateTime>

//#include "sequence_wizard.h"
//#include "../Common/settings_tree.h"
//#include "../io_general.h"
//#include "sequence_wizard.h"

#include "../Common/settings_tree.h"
#include "../Common/experiment_scheduler.h"

#include "ui_scheduler_wizard.h"


// виджет редактирования циклограммы измерений программой сигнального процессора
class SchedulerWizard : public QWidget, public Ui::SchedulerWizard
{
	Q_OBJECT

public:
	SchedulerWizard(QSettings *settings, QWidget *parent = 0);
	~SchedulerWizard();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private:
	void insertItem(int row, QString cmd);
	void removeItem(int row);
	
	void showItemParameters(Scheduler::SchedulerObject *obj);

private slots:
	void addItem();	
	void addItemNOP();
	void removeItem();
	void removeAllItems();
	void update();

private:
	Ui::SchedulerWizard *ui;

	QList<QTreeWidgetItem*> tree_items;	
		
	Scheduler::Engine scheduler_engine;

	QStringList jseq_list;
	QString data_file;
};

#endif // SCHEDULER_WIZARD_H
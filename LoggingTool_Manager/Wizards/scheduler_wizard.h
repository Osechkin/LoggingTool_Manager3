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
#include "../Wizards/depth_template_wizard.h"
#include "../Wizards/sequence_wizard.h"

#include "ui_scheduler_wizard.h"


// виджет редактирования циклограммы измерений программой сигнального процессора
class SchedulerWizard : public QWidget, public Ui::SchedulerWizard
{
	Q_OBJECT

public:
	SchedulerWizard(QSettings *settings, SequenceWizard *seq_wiz, DepthTemplateWizard *depth_wiz, QWidget *parent = 0);
	~SchedulerWizard();

	Scheduler::Engine* getSchedulerEngine() { return &scheduler_engine; } 

	bool scheduling();
	
public slots:
	void setJSeqList(QStringList _jseq_list) { jseq_list = _jseq_list; }
	void setJSeqFile(const QString &_file_name) { jseq_file = _file_name; }
	void setDataFile(const QString &_file_name) { data_file = _file_name; }

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private:
	void setDataFileSettings();
	QString generateDataFileName();

	void insertItem(int row, QString cmd);
	void removeItem(int row);
	
	void showItemParameters(Scheduler::SchedulerObject *obj);

private slots:
	void addItem();	
	void addItemNOP();
	void removeItem();
	void removeAllItems();
	void update();

	void currentItemSelected(QModelIndex index1, QModelIndex index2);

private:
	Ui::SchedulerWizard *ui;
	
	QSettings *app_settings;
	DepthTemplateWizard *depth_wizard;
	SequenceWizard *sequence_wizard;

	QList<QTreeWidgetItem*> tree_items;	
		
	Scheduler::Engine scheduler_engine;

	QStringList jseq_list;
	QString jseq_file;
	QString data_file;

	QString datafile_path;
	QString datafile_prefix;
	QString datafile_postfix;
	QString datafile_extension;
};

#endif // SCHEDULER_WIZARD_H
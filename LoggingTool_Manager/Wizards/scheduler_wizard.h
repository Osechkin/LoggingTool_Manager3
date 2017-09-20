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

#include "ui_scheduler_wizard.h"



// ������ �������������� ����������� ��������� ���������� ����������� ����������
class SchedulerWizard : public QWidget, public Ui::SchedulerWizard
{
	Q_OBJECT

public:
	SchedulerWizard(QSettings *settings, QWidget *parent = 0);
	~SchedulerWizard();

private:
	Ui::SchedulerWizard *ui;

	QList<CTreeWidgetItem*> c_items;
	QList<CTreeWidgetItem*> c_title_items;
	CTreeWidgetItem *c_item_selected;
		
};

#endif // SCHEDULER_WIZARD_H
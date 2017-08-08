#ifndef SELECT_SEQUENCE_DIALOG_H
#define SELECT_SEQUENCE_DIALOG_H

#include <QtGui>

#include "../Wizards/experiment_manager.h"

#include "ui_select_sequence_dialog.h"


class SelectSequenceDialog : public QDialog, public Ui::SelectSequenceDialog
{
	Q_OBJECT

public:
	explicit SelectSequenceDialog(QWidget *parent = 0);	
	~SelectSequenceDialog();
		

private:
	bool findSequenceScripts(QStringList &files, QStringList &pathes);
	QSettings* initSequenceScript(QString file_name);
	

	Ui::SelectSequenceDialog *ui;
	ExperimentManager *expManager;

	QList<SeqCmdInfo> *seq_cmd_index;			// "�������" ������ ������������� �������������
	QList<SeqInstrInfo> *seq_instr_index;		// "�������" ���������� ��� ��������� ������ ���������� ����������� ������������� �������������
	QList<SeqDataTypeInfo> *seq_datatype_index;	// "�������" ����� ������ ��� ��������� ���������� ����������� ������������� �������������

	QStringList *file_list;
	QStringList *path_list;

	

private slots:
	

};

#endif // VIEW_CODE_DIALOG_H
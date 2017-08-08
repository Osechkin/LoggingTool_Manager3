#ifndef EXPERIMENT_MANAGER_H
#define EXPERIMENT_MANAGER_H


#include <QSettings>
#include <QList>
#include <QDateTime>

#include "sequence_wizard.h"
#include "../Common/settings_tree.h"
#include "../io_general.h"
#include "sequence_wizard.h"

#include "ui_experiment_manager.h"


struct ExpTask
{
	Sequence sequence;				// последовательность
	int repeat_count;				// число повторений	
};

struct Experiment
{
	QList<ExpTask> task_list;		// список последовательностей, выполняемых поочередно в течение эксперимента
	int repeat_count;				// количество повторов списка sequence_list (кол-во измерений)
	bool accumulate;				// флаг производить/не производить накопление сигнала
	int index;						// номер текущего измерения
};


// виджет редактирования циклограммы измерений программой сигнального процессора
class ExperimentManager : public QWidget, public Ui::ExperimentManager
{
	Q_OBJECT

public:
	explicit ExperimentManager(QSettings *settings, QWidget *parent = 0);
	~ExperimentManager();

	QStringList *getSeqFileList() { return file_list; }
	QStringList *getSeqPathList() { return path_list; }

	QList<SeqCmdInfo> *getSeqCmdInfo() { return seq_cmd_index; }
	QList<SeqInstrInfo> *getSeqInstrIndex() { return seq_instr_index; }
	QList<SeqDataTypeInfo> *getSeqDataTypeInfo() { return seq_datatype_index; }

	//Sequence *getCurrentSequence() { return &curSeq; }
	//bool getDSPPrg(QVector<uint8_t> &_prg, QVector<uint8_t> &_instr);

private:
	void setConnections();	
	//QSettings *initSequenceScript(QString file_name);
	//bool findSequenceScripts(QStringList &files, QStringList &pathes);
	void parseSequenceScript(QSettings *settings, Sequence &seq);
	QString getStrItemNumber(QString text, int index, int base);
	void showSeqParameters();
	void clearCTreeWidget();
	void showSequenceMemo(Sequence &seq);
	uint8_t findCmdCode(const QString &str, bool &flag);
	QString findCmdMnemonics(const uint8_t value, bool &flag);	
	QList<uint8_t> findCmdValue(const QString &str);
	QString findInstrMnemonics(const uint8_t value, bool &flag);
	uint8_t findInstrCode(const QString &str, bool &flag);
	QList<uint8_t> findInstrValue(const QString &str, bool &flag);

	Ui::ExperimentManager *ui;

	QSettings *app_settings;
	SequenceWizard *seqWizard;

	//QSettings *sequence_proc;
	QStringList *file_list;
	QStringList *path_list;
	//Sequence curSeq;
	
	QList<SeqCmdInfo> *seq_cmd_index;			// "словарь" команд интервального программатора
	QList<SeqInstrInfo> *seq_instr_index;		// "словарь" инструкций для обработки данных сигнальным процессором интервального программатора
	QList<SeqDataTypeInfo> *seq_datatype_index;	// "словарь" типов данных для обработки сигнальным процессором интервального программатора

	QList<CTreeWidgetItem*> c_items;
	QList<CTreeWidgetItem*> c_title_items;
	CTreeWidgetItem *c_item_selected;

	DataSave save_data;

private slots:		
	void descriptionLinkActivated(const QString &link);
	void treeWidgetActivated(QTreeWidgetItem* item, int index);
	bool changeCurrentSequence(const QString &text);	
	void paramValueChanged(QObject *obj, QVariant &value);
	void paramEditingFinished(QObject *obj);
	void addSequence();
	void refreshSequence();
	void refreshSequenceList();
	void viewCode();
	//void setExportSettings();

signals:
	void sequence_changed();
	void save_data_changed(DataSave &file_attrs);
};

#endif // EXPERIMENT_MANAGER
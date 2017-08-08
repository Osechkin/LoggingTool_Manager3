#include <QDir>
#include <QTextCodec>
#include <QFileDialog>
#include <QMessageBox>
#include <QScriptEngine>

#include "experiment_manager.h"
#include "../Dialogs/select_sequence_dialog.h"


ExperimentManager::ExperimentManager(QSettings *settings, QWidget *parent) : QWidget(parent), ui(new Ui::ExperimentManager)
{
	ui->setupUi(this);
	ui->tbtAdd->setIcon(QIcon(":/images/add_button.png"));
	ui->tbtRemove->setIcon(QIcon(":/images/disconnect.png"));
		
	ui->tbtAdd->setToolTip(tr("Add Sequence to the Task List"));
	ui->tbtRemove->setToolTip(tr("Remove Sequence from the Task List"));
	
	QStringList headlist_ex;
	ui->treeWidgetPlan->setColumnCount(4);
	headlist_ex << tr("Parameter") << tr("Range") << tr("Actual") << tr("Units");
	ui->treeWidgetPlan->setHeaderLabels(headlist_ex);
	ui->treeWidgetPlan->setColumnWidth(0,180);
	ui->treeWidgetPlan->setColumnWidth(1,100);	
	ui->treeWidgetPlan->setColumnWidth(2,60);
	ui->treeWidgetPlan->setColumnWidth(3,60);

	ui->treeWidgetPlan->header()->setFont(QFont("Arial", 10, 0, false));		
	
	setParent(parent);
	seqWizard = qobject_cast<SequenceWizard*>(parent);
	if (seqWizard)
	{
		seq_cmd_index = seqWizard->getSeqCmdInfo();
		seq_instr_index = seqWizard->getSeqInstrIndex();		
		seq_datatype_index = seqWizard->getSeqDataTypeInfo();		

		file_list = seqWizard->getSeqFileList();
		path_list = seqWizard->getSeqPathList();
	}
	else qFatal(tr("Cannot get Sequence Wizard data!").toLocal8Bit().data());

	setConnections();
}

ExperimentManager::~ExperimentManager()
{
	//clearCTreeWidget();

	delete ui;	
}

void ExperimentManager::setConnections()
{
	//connect(ui->lblDescription, SIGNAL(linkActivated(const QString &)), this, SLOT(descriptionLinkActivated(const QString &)));
	
	connect(ui->tbtAdd, SIGNAL(clicked()), this, SLOT(addSequence()));
	/*connect(ui->pbtViewCode, SIGNAL(clicked()), this, SLOT(viewCode()));
	connect(ui->pbtRefresh, SIGNAL(clicked()), this, SLOT(refreshSequence()));
	connect(ui->tbtRefreshSeqList, SIGNAL(clicked()), this, SLOT(refreshSequenceList()));
	connect(ui->pbtExportSettings, SIGNAL(clicked()), this, SLOT(setExportSettings()));

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeWidgetActivated(QTreeWidgetItem*,int)));
	*/
}

/*bool ExperimentManager::findSequenceScripts(QStringList &files, QStringList &pathes)
{
	files.clear();
	pathes.clear();

	QString path = QDir::currentPath() + "/Sequences";
	QDir dir(path);
	QStringList file_list = dir.entryList(QDir::Files | QDir::NoSymLinks);

	QStringList res;
	for (int i = 0; i < file_list.count(); i++)
	{
		if (file_list[i].split(".").last().toLower() == QString("seq")) 
		{
			files.append(file_list[i]);
			pathes.append(path);
		}
	}

	return !files.isEmpty();
}*/

/*QSettings* ExperimentManager::initSequenceScript(QString file_name)
{
	QSettings *settings = new QSettings(file_name, QSettings::IniFormat, this);	

#ifdef Q_OS_WIN
	QString code_page = "Windows-1251";
#elif Q_OS_LINUX
	QString code_page = "UTF-8";
#endif
	QTextCodec *codec = QTextCodec::codecForName(code_page.toUtf8());
	settings->setIniCodec(codec);

	return settings;
}*/

void ExperimentManager::parseSequenceScript(QSettings *settings, Sequence &seq)
{
	/*seq.clear();

	QStringList all_keys = settings->allKeys();

	if (settings->contains("Main/Sequence_name")) seq.name = settings->value("Main/Sequence_name").toString();
	if (settings->contains("Main/Author")) seq.author = settings->value("Main/Author").toString();		
	
	QLocale loc(QLocale::English);
	if (settings->contains("Main/Date_time")) 
	{
		QString str_dt = settings->value("Main/Date_time").toString();
		seq.date_time = loc.toDateTime(str_dt, "dd.MM.yyyy hh:mm:ss"); 
	}
	
	if (settings->contains("Main/Description")) seq.description = settings->value("Main/Description").toString();
	if (settings->contains("Main/Frequencies_parameters")) seq.freq_count = settings->value("Main/Frequencies_parameters").toInt(); else seq.freq_count = 0;
	if (settings->contains("Main/Intervals_parameters")) seq.interval_count = settings->value("Main/Intervals_parameters").toInt(); else seq.interval_count = 0;
	if (settings->contains("Main/RF_pulses_parameters")) seq.rf_pulse_count = settings->value("Main/RF_pulses_parameters").toInt(); else seq.rf_pulse_count = 0;
	if (settings->contains("Main/Phase_parameters")) seq.phase_count = settings->value("Main/Phase_parameters").toInt(); else seq.phase_count = 0;
	if (settings->contains("Main/Counter_parameters")) seq.counter_count = settings->value("Main/Counter_parameters").toInt(); else seq.counter_count = 0;
	if (settings->contains("Main/Constant_parameters")) seq.constant_count = settings->value("Main/Constant_parameters").toInt(); else seq.constant_count = 0;
	if (settings->contains("Main/Probe_parameters")) seq.probe_count = settings->value("Main/Probe_parameters").toInt(); else seq.probe_count = 0;
	if (settings->contains("Main/Processing_parameters")) seq.processing_count = settings->value("Main/Processing_parameters").toInt(); else seq.processing_count = 0;
	if (settings->contains("Main/Output_datasets")) seq.output_count = settings->value("Main/Output_datasets").toInt(); else seq.output_count = 0;
	if (settings->contains("Main/Commands_Number")) seq.commands_count = settings->value("Main/Commands_Number").toInt(); else seq.commands_count = 0;
	if (settings->contains("Main/InstrPacks_number")) seq.instr_packs_count = settings->value("Main/InstrPacks_number").toInt(); else seq.instr_packs_count = 0;


	if (seq.freq_count == 0) seq.seq_errors.append(SeqErrors(E_NoFreqs, ""));
	else if (seq.freq_count > 9999) seq.seq_errors.append(SeqErrors(E_FreqCount, ""));
	else if (settings->childGroups().contains("Frequencies"))
	{		
		int freq_count = 0;
		for (int index = 1; index <= seq.freq_count; index++)
		{
			QString par_ = "Frequencies/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Frequencies/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Frequencies/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Frequencies/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Frequencies/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Frequencies/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Frequencies/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Frequencies/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Frequencies/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Frequencies/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Frequency;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString(); 
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;		
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				freq_count++;
			}			
		}
		if (freq_count != seq.freq_count) seq.seq_errors.append(SeqErrors(E_FreqCount,""));
	}
	else seq.seq_errors.append(SeqErrors(E_NoFreqs,""));
	

	if (seq.interval_count == 0) seq.seq_errors.append(SeqErrors(E_NoIntervals,""));
	else if (seq.interval_count > 9999) seq.seq_errors.append(SeqErrors(E_IntervalCount,""));
	else if (settings->childGroups().contains("Intervals"))
	{		
		int interval_count = 0;
		for (int index = 1; index <= seq.interval_count; index++)
		{
			QString par_ = "Intervals/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Intervals/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Intervals/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Intervals/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Intervals/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Intervals/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Intervals/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Intervals/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Intervals/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Intervals/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Interval;
				seq_param->flag = true;
				bool ok;
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString(); 
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				interval_count++;
			}			
		}
		if (interval_count != seq.interval_count) seq.seq_errors.append(SeqErrors(E_IntervalCount,""));
	}
	else seq.seq_errors.append(SeqErrors(E_NoIntervals,""));


	if (seq.rf_pulse_count == 0) seq.seq_errors.append(SeqErrors(E_NoRFpulses,""));
	else if (seq.rf_pulse_count > 9999) seq.seq_errors.append(SeqErrors(E_RFpulseCount,""));
	else if (settings->childGroups().contains("RF_pulses"))
	{		
		int rf_pulse_count = 0;
		for (int index = 1; index <= seq.rf_pulse_count; index++)
		{
			QString par_ = "RF_pulses/" + getStrItemNumber("Par", index, 4);
			QString min_ = "RF_pulses/" + getStrItemNumber("Min", index, 4);
			QString max_ = "RF_pulses/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "RF_pulses/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "RF_pulses/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "RF_pulses/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "RF_pulses/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "RF_pulses/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "RF_pulses/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "RF_pulses/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = RF_Pulse;
				seq_param->flag = true;
				bool ok;
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				rf_pulse_count++;
			}			
		}
		if (rf_pulse_count != seq.rf_pulse_count) seq.seq_errors.append(SeqErrors(E_RFpulseCount,""));
	}
	else seq.seq_errors.append(SeqErrors(E_NoRFpulses,""));


	if (seq.phase_count > 9999) seq.seq_errors.append(SeqErrors(E_PhaseCount,""));
	else if (settings->childGroups().contains("Phases"))
	{		
		int ph_count = 0;
		for (int index = 1; index <= seq.phase_count; index++)
		{
			QString par_ = "Phases/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Phases/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Phases/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Phases/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Phases/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Phases/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Phases/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Phases/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Phases/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Phases/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Phase;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				ph_count++;
			}			
		}
		if (ph_count != seq.phase_count) seq.seq_errors.append(SeqErrors(E_PhaseCount,""));
	}


	if (seq.counter_count > 9999) seq.seq_errors.append(SeqErrors(E_CounterCount,""));
	else if (settings->childGroups().contains("Counters"))
	{		
		int cnt_count = 0;
		for (int index = 1; index <= seq.counter_count; index++)
		{
			QString par_ = "Counters/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Counters/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Counters/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Counters/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Counters/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Counters/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Counters/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Counters/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Counters/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Counters/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Counter;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				cnt_count++;
			}			
		}
		if (cnt_count != seq.counter_count) seq.seq_errors.append(SeqErrors(E_CounterCount,""));
	}


	if (seq.constant_count > 9999) seq.seq_errors.append(SeqErrors(E_ConstantCount,""));
	else if (settings->childGroups().contains("Constants"))
	{		
		int const_count = 0;
		for (int index = 1; index <= seq.constant_count; index++)
		{
			QString par_ = "Constants/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Constants/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Constants/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Constants/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Constants/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Constants/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Constants/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Constants/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Constants/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Constants/" + getStrItemNumber("Ron", index, 4);
			
			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Constant;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				const_count++;
			}			
		}
		if (const_count != seq.constant_count) seq.seq_errors.append(SeqErrors(E_ConstantCount,""));
	}

	if (seq.probe_count > 9999) seq.seq_errors.append(SeqErrors(E_ProbeCount,""));
	else if (settings->childGroups().contains("Probes"))
	{		
		int prob_count = 0;
		for (int index = 1; index <= seq.probe_count; index++)
		{
			QString par_ = "Probes/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Probes/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Probes/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Probes/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Probes/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Probes/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Probes/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Probes/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Probes/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Probes/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Probe;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;		
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				prob_count++;
			}			
		}
		if (prob_count != seq.probe_count) seq.seq_errors.append(SeqErrors(E_ProbeCount,""));
	}

	if (seq.processing_count > 9999) seq.seq_errors.append(SeqErrors(E_ProcessingCount,""));
	else if (settings->childGroups().contains("Processing"))
	{		
		int proc_count = 0;
		for (int index = 1; index <= seq.processing_count; index++)
		{
			QString par_ = "Processing/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Processing/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Processing/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Processing/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Processing/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Processing/" + getStrItemNumber("Des", index, 4);
			QString frm_ = "Processing/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Processing/" + getStrItemNumber("Vis", index, 4);
			QString unt_ = "Processing/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Processing/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Processing;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;		
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				proc_count++;
			}			
		}
		if (proc_count != seq.processing_count) seq.seq_errors.append(SeqErrors(E_ProcessingCount,""));
	}

	if (seq.output_count > 9999) seq.seq_errors.append(SeqErrors(E_OutputCount,""));
	else if (settings->childGroups().contains("Output"))
	{		
		int output_count = 0;
		for (int index = 1; index <= seq.output_count; index++)
		{
			QString par_ = "Output/" + getStrItemNumber("Par", index, 4);
			QString min_ = "Output/" + getStrItemNumber("Min", index, 4);
			QString max_ = "Output/" + getStrItemNumber("Max", index, 4);
			QString dfl_ = "Output/" + getStrItemNumber("Dfl", index, 4);
			QString cap_ = "Output/" + getStrItemNumber("Cap", index, 4);
			QString des_ = "Output/" + getStrItemNumber("Des", index, 4);	
			QString frm_ = "Output/" + getStrItemNumber("Frm", index, 4);
			QString vis_ = "Output/" + getStrItemNumber("Vis", index, 4);	
			QString unt_ = "Output/" + getStrItemNumber("Unt", index, 4);
			QString ron_ = "Output/" + getStrItemNumber("Ron", index, 4);

			if (settings->contains(par_))
			{
				Sequence_Param *seq_param = new Sequence_Param;
				seq_param->name = settings->value(par_).toString();
				seq_param->type = Output;
				seq_param->flag = true;
				bool ok;				
				if (settings->contains(min_)) seq_param->min_value = settings->value(min_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(max_)) seq_param->max_value = settings->value(max_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(dfl_)) seq_param->def_value = settings->value(dfl_).toInt(&ok); else seq_param->flag = false; if (!ok) seq_param->flag = false;
				if (settings->contains(cap_)) seq_param->caption = settings->value(cap_).toString();
				if (settings->contains(des_)) seq_param->description = settings->value(des_).toString(); 
				if (settings->contains(frm_)) seq_param->formula = settings->value(frm_).toString(); 
				if (settings->contains(vis_)) seq_param->visible = (bool)(settings->value(vis_).toInt(&ok)); if (!ok) seq_param->flag = false;	
				if (settings->contains(unt_)) seq_param->units = settings->value(unt_).toString();
				if (settings->contains(ron_)) seq_param->read_only = (bool)(settings->value(ron_).toInt(&ok)); if (!ok) seq_param->flag = false;
				seq_param->app_value = seq_param->def_value;
				seq_param->par_number = index;

				seq.param_list.append(seq_param);
				output_count++;
			}			
		}
		if (output_count != seq.output_count) seq.seq_errors.append(SeqErrors(E_OutputCount,""));
	}
	

	for (int i = 0; i < seq.param_list.count(); i++)
	{
		Sequence_Param *seq_param = seq.param_list[i];
		if (seq_param->formula != "" && seq_param->def_value != NAN)
		{
			QString formula = seq_param->formula;
			formula = simplifyFormula(formula, &seq);

			QScriptEngine engine;
			QScriptValue qscr_val = engine.evaluate(formula);
			if (!qscr_val.isError()) 
			{
				int res = qscr_val.toInt32();
				seq_param->app_value = res;
			}		
			else
			{
				QString par_name = seq_param->name;
				seq.seq_errors.append(SeqErrors(E_BadFormula, par_name));
			}
		}
	}


	if (seq.commands_count == 0) seq.seq_errors.append(SeqErrors(E_NoCommands,""));
	else if (seq.commands_count > 9999) seq.seq_errors.append(SeqErrors(E_CommandCount, ""));
	else if (settings->childGroups().contains("Commands"))
	{		
		int commands_count = 0;
		for (int index = 1; index <= seq.commands_count; index++)
		{
			QString com_ = "Commands/" + getStrItemNumber("Com", index, 4);
			if (settings->contains(com_))
			{
				Sequence_Cmd *seq_cmd = new Sequence_Cmd;
				seq_cmd->cmd_number = index;
				seq_cmd->flag = true;
								
				QVariant v_val = settings->value(com_);				
				QString str_val = settings->value(com_).toString(); 
				QStringList str_bytes = v_val.toStringList();
				
				bool ok;
				uint8_t byte1 = (uint8_t)str_bytes.first().toUInt(&ok);
				if (ok) 
				{
					QString str = findCmdMnemonics(byte1, ok);	
					if (ok)
					{
						seq_cmd->byte1 = byte1;
						seq_cmd->str_byte1 = str;
					}
					else seq_cmd->flag = false;					
				}
				else 
				{
					uint8_t ui8_value = findCmdCode(str_bytes.first(), ok);
					if (ok)
					{
						seq_cmd->str_byte1 = str_bytes.first();
						seq_cmd->byte1 = ui8_value;
					}
					else seq_cmd->flag = false;					
				}

				if (str_bytes.size() == 2) 
				{
					seq_cmd->str_bytes234 = str_bytes.last();
					QList<uint8_t> byte_list = findCmdValue(str_bytes.last());
					if (!byte_list.isEmpty())
					{
						seq_cmd->byte2 = byte_list[0];
						seq_cmd->byte3 = byte_list[1];
						seq_cmd->byte4 = byte_list[2];
					}
				}
				else if (str_bytes.size() == 4)
				{
					uint8_t byte2 = (uint8_t)str_bytes[1].toUInt(&ok);
					if (ok) seq_cmd->byte2 = byte2; else seq_cmd->flag = false;
					uint8_t byte3 = (uint8_t)str_bytes[2].toUInt(&ok);
					if (ok) seq_cmd->byte3 = byte3; else seq_cmd->flag = false;
					uint8_t byte4 = (uint8_t)str_bytes[3].toUInt(&ok);
					if (ok) seq_cmd->byte4 = byte4; else seq_cmd->flag = false;
				}
				else seq_cmd->flag = false;
				
				seq.cmd_list.append(seq_cmd);
				commands_count++;
			}			
		}
		if (commands_count != seq.commands_count) seq.seq_errors.append(SeqErrors(E_CommandCount,""));
	}
	else seq.seq_errors.append(SeqErrors(E_NoCommands,""));	


	if (seq.instr_packs_count > 9999) seq.seq_errors.append(SeqErrors(E_InstrPacksCount,""));
	else 
	{
		for (int index = 1; index <= seq.instr_packs_count; index++)
		{			
			QString pack_ = getStrItemNumber("Package", index, 3);
			if (settings->contains(pack_ + "/Package"))
			{
				Sequence_InstrPack *package = new Sequence_InstrPack;				
				package->pack_number = index;

				QString name_ = pack_ + "/Package";
				if (settings->contains(name_))
				{
					QVariant v_val = settings->value(name_);
					QString str_pack_name = v_val.toString();
					package->pack_name = str_pack_name;
				}
				else package->pack_name = "";

				int instr_index = 1;
				bool fin = false;
				while (!fin)
				{
					QString ins_ = pack_ + "/" + getStrItemNumber("Ins", instr_index, 4);
					if (settings->contains(ins_))
					{
						Sequence_Instr *seq_instr = new Sequence_Instr;
						seq_instr->instr_number = instr_index;
						seq_instr->flag = true;

						QVariant v_val = settings->value(ins_);						
						QStringList str_bytes = v_val.toStringList();
						
						bool ok;
						uint8_t byte1 = (uint8_t)str_bytes.first().toUInt(&ok);
						if (ok)
						{
							QString str = findInstrMnemonics(byte1, ok);	
							if (ok)
							{
								seq_instr->byte1 = byte1;
								seq_instr->str_byte1 = str;
							}
							else seq_instr->flag = false;	
						}
						else
						{
							uint8_t ui8_value = findInstrCode(str_bytes.first(), ok);
							if (ok)
							{
								seq_instr->str_byte1 = str_bytes.first();
								seq_instr->byte1 = ui8_value;
							}
							else seq_instr->flag = false;
						}

						if (str_bytes.size() > 2)
						{
							uint8_t ui8_params_value = str_bytes[1].toUInt(&ok); 
							if (ok)
							{
								seq_instr->str_byte2 = str_bytes[1];
								seq_instr->byte2 = ui8_params_value;
							}
							else seq_instr->flag = false;
							
							uint8_t ui8_value = str_bytes[2].toUInt(&ok); 
							if (ok)
							{
								seq_instr->str_byte3 = str_bytes[2];
								seq_instr->byte3 = ui8_value;
							}
							else seq_instr->flag = false;

							if (ui8_params_value == str_bytes.size()-3)
							{
								for (int k = 0; k < ui8_params_value; k++)
								{
									QString str_param = str_bytes[3+k];
									seq_instr->str_params.append(str_bytes[3+k]);
									int param = str_param.toInt(&ok);
									if (ok)
									{
										uint8_t byte1 = (uint8_t)(param & 0x000000FF); 
										uint8_t byte2 = (uint8_t)((param & 0x0000FF00) >> 8); 
										uint8_t byte3 = (uint8_t)((param & 0x00FF0000) >> 16);
										uint8_t byte4 = (uint8_t)((param & 0xFF000000) >> 24);
																				
										seq_instr->param_bytes << byte1 << byte2 << byte3 << byte4;
									}
									else 
									{
										QList<uint8_t> ui8_bytes = findInstrValue(str_bytes[3+k], ok);
										if (ok)
										{
											if (ui8_bytes.size() == 4) seq_instr->param_bytes << ui8_bytes[0] << ui8_bytes[1] << ui8_bytes[2] << ui8_bytes[3];
										}
										else seq_instr->flag = false;
									}
								}
							}
							else seq_instr->flag = false;
						}
						else seq_instr->flag = false;
												
						package->instr_list.append(seq_instr);
						
						instr_index++;
					}
					else 
					{
						fin = true;
					}
				}

				seq.instr_pack_list.append(package);
			}
		}
		if (seq.instr_pack_list.count() != seq.instr_packs_count) seq.seq_errors.append(SeqErrors(E_InstrPacksCount,""));
	}	*/
}

void ExperimentManager::showSeqParameters()
{
	/*clearCTreeWidget();
	ui->treeWidget->clear();

	// —оздание раздела "Frequencies"
	QList<CSettings> title_settings_list_freq;
	CSettings title_settings_freq("label", tr("Frequencies"));
	title_settings_freq.title_flag = true;
	title_settings_freq.text_color = QColor(Qt::darkRed);
	title_settings_freq.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_freq.append(title_settings_freq);

	CTreeWidgetItem *c_title_freq = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_freq);
	c_title_items.append(c_title_freq);
	c_title_freq->show();
	

	// —оздание раздела "RF Pulses"
	QList<CSettings> title_settings_list_rfpulses;
	CSettings title_settings_rfpulses("label", tr("RF Pulses"));
	title_settings_rfpulses.title_flag = true;
	title_settings_rfpulses.text_color = QColor(Qt::darkRed);
	title_settings_rfpulses.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_rfpulses.append(title_settings_rfpulses);

	CTreeWidgetItem *c_title_rfpulses = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_rfpulses);
	c_title_items.append(c_title_rfpulses);
	c_title_rfpulses->show();


	// —оздание раздела "Intervals"
	QList<CSettings> title_settings_list_int;
	CSettings title_settings_int("label", tr("Intervals"));
	title_settings_int.title_flag = true;
	title_settings_int.text_color = QColor(Qt::darkRed);
	title_settings_int.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_int.append(title_settings_int);

	CTreeWidgetItem *c_title_int = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_int);
	c_title_items.append(c_title_int);
	c_title_int->show();
	

	// —оздание раздела "Phases"
	QList<CSettings> title_settings_list_phases;
	CSettings title_settings_phases("label", tr("Phases"));
	title_settings_phases.title_flag = true;
	title_settings_phases.text_color = QColor(Qt::darkRed);
	title_settings_phases.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_phases.append(title_settings_phases);

	CTreeWidgetItem *c_title_phases = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_phases);
	c_title_items.append(c_title_phases);
	c_title_phases->show();


	// —оздание раздела "Counters"
	QList<CSettings> title_settings_list_counters;
	CSettings title_settings_counters("label", tr("Counters"));
	title_settings_counters.title_flag = true;
	title_settings_counters.text_color = QColor(Qt::darkRed);
	title_settings_counters.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_counters.append(title_settings_counters);
		
	CTreeWidgetItem *c_title_counters = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_counters);
	c_title_items.append(c_title_counters);
	c_title_counters->show();


	// —оздание раздела "Processing"
	QList<CSettings> title_settings_list_processing;
	CSettings title_settings_processing("label", tr("Processing"));
	title_settings_processing.title_flag = true;
	title_settings_processing.text_color = QColor(Qt::darkRed);
	title_settings_processing.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_processing.append(title_settings_processing);

	CTreeWidgetItem *c_title_processing = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_processing);
	c_title_items.append(c_title_processing);
	c_title_processing->show();


	// —оздание раздела "Output"
	QList<CSettings> title_settings_list_output;
	CSettings title_settings_output("label", tr("Output"));
	title_settings_output.title_flag = true;
	title_settings_output.text_color = QColor(Qt::darkRed);
	title_settings_output.background_color = QColor(Qt::blue).lighter(170);
	title_settings_list_output.append(title_settings_output);

	CTreeWidgetItem *c_title_output = new CTreeWidgetItem(ui->treeWidget, 0, title_settings_list_output);
	c_title_items.append(c_title_output);
	c_title_output->show();
	
	// создание отдельных пунктов внутри разделов
	int param_count = curSeq.freq_count + 
					  curSeq.interval_count + 
					  curSeq.rf_pulse_count + 
					  curSeq.phase_count + 
					  curSeq.counter_count + 
					  curSeq.constant_count + 
					  curSeq.processing_count +
					  curSeq.output_count + 
					  curSeq.probe_count;
	
	if (param_count > curSeq.param_list.count())
	{
		curSeq.seq_errors.append(SeqErrors(E_ParamsCount,""));
		return;
	}

	for (int i = 0; i < param_count; i++)
	{
		QList<CSettings> item_settings_list_freq;
		QList<CSettings> item_settings_list_int;
		QList<CSettings> item_settings_list_rfpulses;
		QList<CSettings> item_settings_list_phases;
		QList<CSettings> item_settings_list_counters;
		QList<CSettings> item_settings_list_processing;

		Sequence_Param *param = curSeq.param_list[i];
		if (!param->visible) continue;
		if (param->type == Output) continue;

		CSettings item_settings1("label", param->caption);
		item_settings1.hint = param->name;
		item_settings1.text_color = QColor(Qt::darkBlue);		
		item_settings1.background_color = QColor(Qt::blue).lighter(190);

		CSettings item_settings2("textedit", (param->app_value == NAN ? "" : QString::number(param->app_value)));
		item_settings2.hint = param->caption;
		item_settings2.text_color = QColor(Qt::darkBlue);		
		item_settings2.background_color = QColor(Qt::blue).lighter(190);		
		item_settings2.read_only = true;
		item_settings2.set_frame = true;

		CSettings item_settings3("spinbox", param->def_value);
		QString str_minmax = QString("[ %1 ... %2 ]").arg(param->min_value).arg(param->max_value);	
		item_settings3.hint = str_minmax;
		double d_min = (double)param->min_value;
		double d_max = (double)param->max_value;		
		item_settings3.min_max = QPair<double,double>(d_min,d_max);		
		item_settings3.read_only = param->read_only;
		item_settings3.text_color = QColor(Qt::darkMagenta);
		item_settings3.background_color = QColor(Qt::blue).lighter(190);

		//CSettings item_settings4("combobox", QString(""));
		//item_settings4.background_color = QColor(Qt::blue).lighter(190);		
		//item_settings4.text_color = QColor(Qt::darkMagenta);
		CSettings item_settings4("label", " " + param->units);		
		item_settings4.text_color = QColor(Qt::darkBlue);		
		item_settings4.background_color = QColor(Qt::blue).lighter(190);
				
		switch (param->type)
		{
		case Frequency:		// если параметр - частота 
			{				
				//item_settings4.value = "kHz";
				item_settings_list_freq << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_freq->getQSubTreeWidgetItem(), item_settings_list_freq);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		case Interval:		// если параметр - интервал времени 
			{
				//item_settings4.value = "mks";
				item_settings_list_int << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_int->getQSubTreeWidgetItem(), item_settings_list_int);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		case RF_Pulse:		// если параметр - радиочастотный импульс 
			{
				//item_settings4.value = "mks";
				item_settings_list_rfpulses << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_rfpulses->getQSubTreeWidgetItem(), item_settings_list_rfpulses);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		case Phase:		// если параметр - фаза 
			{
				//item_settings4.value = "";
				item_settings_list_phases << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_phases->getQSubTreeWidgetItem(), item_settings_list_phases);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		case Counter:		// если параметр - счетчик 
			{
				//item_settings4.value = "";
				item_settings_list_counters << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_counters->getQSubTreeWidgetItem(), item_settings_list_counters);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		case Processing:		// если параметр - параметр обработки 
			{
				//item_settings4.value = "";
				item_settings_list_processing << item_settings1 << item_settings2 << item_settings3 << item_settings4;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_processing->getQSubTreeWidgetItem(), item_settings_list_processing);
				connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}
		default: break;	
		}		
	}

	// создание отдельных пунктов внутри раздела Output
	//int output_count = curSeq.output_count;

	for (int i = 0; i < param_count; i++)
	{
		QList<CSettings> item_settings_list_output;
		
		Sequence_Param *param = curSeq.param_list[i];
		if (!param->visible) continue;
		if (param->type != Output) continue;

		CSettings item_settings1("label", param->caption);
		item_settings1.hint = param->description;
		item_settings1.text_color = QColor(Qt::darkBlue);		
		item_settings1.background_color = QColor(Qt::blue).lighter(190);

		CSettings item_settings2("checkbox", "");	
		item_settings2.hint = param->description;
		item_settings2.background_color = QColor(Qt::blue).lighter(190);
		item_settings2.checkable = true;
		item_settings2.check_state = param->def_value;
			
		switch (param->type)
		{
		case Output:		
			{						
				item_settings_list_output << item_settings1 << item_settings2;
				CTreeWidgetItem *c_item = new CTreeWidgetItem(ui->treeWidget, c_title_output->getQSubTreeWidgetItem(), item_settings_list_output);
				//connect(c_item, SIGNAL(editing_finished(QObject*)), this, SLOT(paramEditingFinished(QObject*)));
				connect(c_item, SIGNAL(value_changed(QObject*, QVariant&)), this, SLOT(paramValueChanged(QObject*, QVariant&)));
				c_item->linkObject(param);
				c_items.append(c_item);
				c_item->show();
				break;
			}		
		default: break;	
		}		
	} */
}

void ExperimentManager::showSequenceMemo(Sequence &seq)
{
	/*QString memo = "<font color = darkblue>Author:</font> " + seq.author + "<br>";
	memo += "<font color = darkblue>Created:</font> " + seq.date_time.toString("dd.MM.yyyy hh:mm:ss") + "<br>";
	memo += "<font color = darkblue>Description:</font> " + seq.description + "<br><br>";

	memo += "<font color = darkblue>Errors:</font> " + QString(!seq.seq_errors.isEmpty() ? "<a href=#parse_error><font color=red><b><u>Found!</u></b></font></a>" : "<font color=darkgreen>Not Found.</font>");

	ui->lblDescription->setOpenExternalLinks(false);
	ui->lblDescription->setText(memo);*/
}

void ExperimentManager::clearCTreeWidget()
{
	/*if (!c_items.isEmpty()) qDeleteAll(c_items);
	c_items.clear();

	if (!c_title_items.isEmpty()) qDeleteAll(c_title_items);
	c_title_items.clear();*/
}

void ExperimentManager::descriptionLinkActivated(const QString &link)
{
	/*if( link == "#parse_error" )
	{
		QString err_msg = "<b>" + tr("Sequence file parsing errors:") + "</b><br><br><font color=red>";
		for (int i = 0; i < curSeq.seq_errors.count(); i++)
		{
			QString text = curSeq.seq_errors[i].second;
			SeqErrorsType err = curSeq.seq_errors[i].first;
			switch (err)
			{
			case E_FreqCount:
				err_msg += tr("The number of declared parameters in 'Frequencies_parameters' does not match the number of parameters in section [Frequencies] found !<br>");
				break;
			case E_NoFreqs:
				err_msg += tr("No parameters found in section [Frequencies] or 'Frequencies_parameters' &lt;= 0 !<br>");
				break;
			case E_IntervalCount:
				err_msg += tr("The number of declared parameters in 'Intervals_parameters' does not match the number of parameters in section [Intervals] found !<br>");
				break;
			case E_NoIntervals:
				err_msg += tr("No parameters found in section [Intervals] or 'Intervals_parameters' &lt;= 0 !<br>");
				break;
			case E_RFpulseCount:
				err_msg += tr("The number of declared parameters in 'RF_pulses_parameters' does not match the number of parameters in section [RF_pulses] found !<br>");
				break;
			case E_NoRFpulses:
				err_msg += tr("No parameters found in section [RF_pulses] or 'RF_pulses_parameters' &lt;= 0 !<br>");
				break;
			case E_CommandCount:
				err_msg += tr("The number of declared parameters in 'Commands_Number' does not match the number of parameters in section [Commands] found !<br>");
				break;
			case E_NoCommands:
				err_msg += tr("No parameters found in section [Commands] or 'Commands_Number' &lt;= 0 !<br>");
				break;
			case E_PhaseCount:
				err_msg += tr("The number of declared parameters in 'Phase_parameters' does not match the number of parameters in section [Phases] found !<br>");
				break;
			case E_CounterCount:
				err_msg += tr("The number of declared parameters in 'Counter_parameters' does not match the number of parameters in section [Counters] found !<br>");
				break;
			case E_ProbeCount:
				err_msg += tr("The number of declared parameters in 'Probe_number' does not match the number of parameters in section [Probes] found !<br>");
				break;
			case E_ProcessingCount:
				err_msg += tr("The number of declared parameters in 'Processing_parameters' does not match the number of parameters in section [Processing] found !<br>");
				break;
			case E_BadFormula:
				err_msg += tr("Bad formula was found in parameter '%1' !<br>").arg(text);
				break;
			case E_InstrPacksCount:
				err_msg += tr("The number of declared instruction packages in 'InstrPacks_number' does not match the number of instruction packets found in a sequence file !<br>");
				break;
			case E_ConstantCount:
				err_msg += tr("The number of declared parameters in 'Constant_parameters' does not match the number of parameters in section [Constants] found !<br>");
				break;
			default:
				break;
			}
		}
		err_msg += "<br><br>";

		for (int i = 0; i < curSeq.param_list.count(); i++)
		{
			Sequence_Param *param = curSeq.param_list[i];
			if (!param->flag)
			{
				QString section_name = "";
				switch (param->type)				
				{
				case Frequency: section_name = "[Frequencies]"; break;
				case Interval: section_name = "[Intervals]"; break;
				case RF_Pulse: section_name = "[RF_pulses]"; break;
				case Phase: section_name = "[Phases]"; break;
				case Counter: section_name = "[Counters]"; break;
				case Constant: section_name = "[Constants]"; break;
				case Probe: section_name = "[Probes]"; break;
				case Processing: section_name = "[Processing]"; break;
				default: break;
				}
				QString param_nickname = getStrItemNumber("Par", param->par_number, 4);
				err_msg += tr("Section %1: Error parsing '%2' ('%3') parameter !<br>").arg(section_name).arg(param->name).arg(param_nickname);
			}
		}

		QList<Sequence_InstrPack*> instr_packs = curSeq.instr_pack_list;
		for (int i = 0; i < instr_packs.count(); i++)
		{
			Sequence_InstrPack *pack = instr_packs[i];
			QString pack_name = tr("Package '%1' ( #%2 ): ").arg(pack->pack_name).arg(pack->pack_number);
			
			QList<Sequence_Instr*> instr_list = pack->instr_list;
			for (int j = 0; j < instr_list.count(); j++)
			{
				Sequence_Instr *instr = instr_list[j];
				if (!instr->flag)
				{
					QString param_nickname = getStrItemNumber("Ins", instr->instr_number, 4);
					err_msg += pack_name +  tr("Error parsing '%1' ('%2') instruction !<br>").arg(instr->str_byte1).arg(param_nickname);
				}
			}
		}

		err_msg += "</font>";

		InfoDialog info_dlg(err_msg, this);
		if (info_dlg.exec()) return;
	}*/
}

bool ExperimentManager::changeCurrentSequence(const QString &text)
{
	/*int index = file_list.indexOf(text, 0);
	if (index < 0) return false;

	//curSeq.clear();
	if (sequence_proc != NULL) delete sequence_proc;
	sequence_proc = NULL;

	QString file_name = path_list[index] + "/" + file_list[index];
	sequence_proc = initSequenceScript(file_name);
	parseSequenceScript(sequence_proc, curSeq);

	bool errs = false;
	if (!curSeq.seq_errors.isEmpty()) errs = true;
	for (int i = 0; i < curSeq.param_list.count(); i++) if (!curSeq.param_list[i]->flag) errs = true;
	
	ui->ledSeqName->setText(curSeq.name);

	QString memo = "<font color = darkblue>Author:</font> " + curSeq.author + "<br>";
	QString str_datetime = curSeq.date_time.toString("dd.MM.yyyy hh:mm:ss");		
	memo += "<font color = darkblue>Created:</font> " + str_datetime + "<br>";
	memo += "<font color = darkblue>Description:</font> " + curSeq.description + "<br><br>";

	memo += "<font color = darkblue>Errors:</font> " + QString(errs ? "<a href=#parse_error><font color=red><b><u>Found!</u></b></font></a>" : "<font color=darkgreen>Not Found.</font>");

	ui->lblDescription->setOpenExternalLinks(false);
	ui->lblDescription->setText(memo);		

	showSeqParameters();	

	emit sequence_changed();
	*/
	return true;
}

void ExperimentManager::addSequence()
{

	SelectSequenceDialog dlg(this);
	if (dlg.exec());

	/*QString full_fileName = QFileDialog::getOpenFileName(this, tr("Add Sequence File"), QDir::currentPath(), tr("Sequences (*.seq)"));
	if (full_fileName.isEmpty()) return;

	QString fileName = full_fileName.split("/").last();
	QString pathName = full_fileName.split("/" + fileName).first();

	file_list.append(fileName);
	path_list.append(pathName);
	ui->cboxSequences->addItem(fileName);
	ui->cboxSequences->setCurrentText(fileName); */
}

void ExperimentManager::refreshSequence()
{
	/*QString cur_fileName = ui->cboxSequences->currentText();
	int index = file_list.indexOf(cur_fileName, 0);
	
	QString full_fileName = path_list[index] + "/" + file_list[index];
	QFile inputFile(full_fileName);
	if (!inputFile.open(QIODevice::ReadOnly))		// если файл *.seq текущей последовательности не найден или не открываетс€
	{
		QMessageBox msgBox;							// то сообщить пользователю и убрать файл *.seq из списка cboxSequencies и все данные этой последователньости
		msgBox.setText(tr("Warning !"));
		msgBox.setInformativeText(tr("Cannot open file %1 !").arg(full_fileName));
		msgBox.setStandardButtons(QMessageBox::Ok);		
		int ret = msgBox.exec();
				
		file_list.removeAt(index);
		path_list.removeAt(index);
		index = ui->cboxSequences->findText(cur_fileName);
		ui->cboxSequences->removeItem(index);				

		cur_fileName = ui->cboxSequences->currentText();
		if (!changeCurrentSequence(cur_fileName))	// выбрать в качестве текущей следующую последовательность. ≈сли такой не найдено, то очисить весь виджет и все данные
		{
			curSeq.clear();
			if (sequence_proc != NULL) delete sequence_proc;
			sequence_proc = NULL;
			
			ui->ledSeqName->clear();
			ui->lblDescription->clear();

			clearCTreeWidget();
			ui->treeWidget->clear();
		}
	}	
	else changeCurrentSequence(cur_fileName);		// обновить все данные текущей последовательности  */
}

void ExperimentManager::refreshSequenceList()
{
	/*QString cur_fileName = ui->cboxSequences->currentText();
	
	if (findSequenceScripts(file_list, path_list))	// обновить список файлов *.seq 
	{
		QString file_name = cur_fileName;
		if (file_name.isEmpty()) file_name = path_list.first() + "/" + file_list.first();	// если последовательность с именем файла cur_fileName не найдена, то выбрать первую из списка в качестве текущей
		sequence_proc = initSequenceScript(file_name);		
		parseSequenceScript(sequence_proc, curSeq);

		bool errs = false;
		if (!curSeq.seq_errors.isEmpty()) errs = true;
		for (int i = 0; i < curSeq.param_list.count(); i++) if (!curSeq.param_list[i]->flag) errs = true;

		ui->cboxSequences->clear();
		ui->cboxSequences->addItems(file_list);		
		ui->ledSeqName->setText(curSeq.name);

		//QString memo = "<font color = darkblue>Author:</font> " + curSeq.author + "<br>";
		//memo += "<font color = darkblue>Created:</font> " + curSeq.date_time.toString("dd.MM.yyyy hh:mm:ss") + "<br>";
		//memo += "<font color = darkblue>Description:</font> " + curSeq.description + "<br><br>";
		//memo += "<font color = darkblue>Errors:</font> " + QString(errs ? "<a href=#parse_error><font color=red><b><u>Found!</u></b></font></a>" : "<font color=darkgreen>Not Found.</font>");
		//ui->lblDescription->setOpenExternalLinks(false);
		//ui->lblDescription->setText(memo);

		showSequenceMemo(curSeq);

		showSeqParameters();			
	}
	else
	{
		curSeq.clear();
		if (sequence_proc != NULL) delete sequence_proc;
		sequence_proc = NULL;

		ui->ledSeqName->clear();
		ui->lblDescription->clear();

		clearCTreeWidget();
		ui->treeWidget->clear();
	}*/
}

/*void SequenceWizard::readSequenceCmdIndex()
{
	QFile inputFile(":/text_files/sequence_commands.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			if (!line.isEmpty())
			{
				QStringList str_list = line.split('\t');
				if (str_list.size() >= 2)
				{							
					bool ok;
					uint8_t code = (uint8_t)str_list[1].toUInt(&ok);
					if (ok) 
					{
						SeqCmdInfo cmd_info;
						cmd_info.cmd_name = str_list.first();
						cmd_info.cmd_code = code;
						if (str_list.size() > 2) cmd_info.comment = str_list[2];
						seq_cmd_index.append(cmd_info);
					}
				}
			}			
		}
		inputFile.close();
	}
}

void SequenceWizard::readSequenceInstrIndex()
{
	QFile inputFile(":/text_files/sequence_instructions.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			if (!line.isEmpty())
			{
				QStringList str_list = line.split('\t');
				if (str_list.size() >= 2)
				{							
					bool ok;
					uint8_t code = (uint8_t)str_list[1].toUInt(&ok);
					if (ok) 
					{
						SeqInstrInfo instr_info;
						instr_info.instr_name = str_list.first();
						instr_info.instr_code = code;
						if (str_list.size() > 2) instr_info.comment = str_list[2];
						seq_instr_index.append(instr_info);
					}
				}
			}			
		}
		inputFile.close();
	}
}

void SequenceWizard::readSequenceDataTypeIndex()
{
	QFile inputFile(":/text_files/sequence_datatypes.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			if (!line.isEmpty())
			{
				QStringList str_list = line.split('\t');
				if (str_list.size() >= 1)
				{							
					bool ok;
					uint8_t code = (uint8_t)str_list.first().toUInt(&ok);
					if (ok) 
					{
						SeqDataTypeInfo datatype_info;						
						datatype_info.datatype_code = code;
						if (str_list.size() > 1) datatype_info.comment = str_list[1];
						seq_datatype_index.append(datatype_info);
					}
				}
			}			
		}
		inputFile.close();
	}
}*/


void ExperimentManager::paramValueChanged(QObject *obj, QVariant &value)
{
	/*CTreeWidgetItem *ctwi = qobject_cast<CTreeWidgetItem*>(sender());
	if (ctwi)
	{
		Sequence_Param *cur_param = (Sequence_Param*)ctwi->getLinkedObject();
		if (cur_param)
		{
			CTextEdit *cted = qobject_cast<CTextEdit*>(obj);
			if (cted)
			{
				bool ok;
				int32_t i32_value = value.toInt(&ok);
				if (ok) 
				{
					cur_param->def_value = i32_value;
					return;
				}
			}

			CComboBox *ccbox = qobject_cast<CComboBox*>(obj);
			if (ccbox)
			{
				bool ok;
				QString str_value = value.toString();
				// ...
				return;			
			}

			CCheckBox *chbox = qobject_cast<CCheckBox*>(obj);
			if (chbox)
			{						
				int32_t bool_value = (int32_t)value.toBool();
				cur_param->def_value = bool_value;
				cur_param->app_value = bool_value;			

				for (int i = 0; i < curSeq.cmd_list.count(); i++)
				{
					Sequence_Cmd *cmd = curSeq.cmd_list[i];
					for (int j = 0; j < curSeq.param_list.count(); j++)
					{
						Sequence_Param *cur_param = curSeq.param_list[j];
						if (cmd->str_bytes234 == cur_param->name)
						{
							cmd->byte2 = (uint8_t)((cur_param->app_value & 0x00FF0000) >> 16);
							cmd->byte3 = (uint8_t)((cur_param->app_value & 0x0000FF00) >> 8);
							cmd->byte4 = (uint8_t)((cur_param->app_value & 0x000000FF));
						}
					}
				}

				for (int i = 0; i < curSeq.instr_pack_list.count(); i++)
				{
					Sequence_InstrPack *pack = curSeq.instr_pack_list[i];
					for (int k = 0; k < pack->instr_list.count(); k++)
					{
						Sequence_Instr *instr = pack->instr_list[k];
						for (int j = 0; j < curSeq.param_list.count(); j++)
						{
							Sequence_Param *cur_param = curSeq.param_list[j];
							if (!instr->str_params.isEmpty())
							{
								for (int m = 0; m < instr->str_params.count(); m++)
								{
									if (instr->str_params[m] == cur_param->name)
									{
										instr->param_bytes[4*m+3] = (uint8_t)((cur_param->app_value & 0xFF000000) >> 24);
										instr->param_bytes[4*m+2] = (uint8_t)((cur_param->app_value & 0x00FF0000) >> 16);
										instr->param_bytes[4*m+1] = (uint8_t)((cur_param->app_value & 0x0000FF00) >> 8);
										instr->param_bytes[4*m] = (uint8_t)((cur_param->app_value & 0x000000FF));
									}
								}
							}
						}
					}
				}
			}

			CSpinBox *csbox = qobject_cast<CSpinBox*>(obj);
			if (csbox)
			{
				bool ok;
				int32_t i32_value = value.toInt(&ok);
				if (ok) 
				{
					cur_param->def_value = i32_value;
					
					for (int i = 0; i < curSeq.param_list.count(); i++)
					{
						Sequence_Param *seq_param = curSeq.param_list[i];
						QString formula = seq_param->formula;
						if (seq_param->formula != "" && seq_param->def_value != NAN)
						{
							formula = simplifyFormula(formula, &curSeq);

							QScriptEngine engine;
							QScriptValue qscr_val = engine.evaluate(formula);
							if (!qscr_val.isError()) 
							{
								int res = qscr_val.toInt32();
								seq_param->app_value = res;
							}		
							else
							{
								QString par_name = seq_param->name;
								curSeq.seq_errors.append(SeqErrors(E_BadFormula, par_name));							
							}
						}
						else
						{
							seq_param->app_value = seq_param->def_value;
						}
					}
					
					showSequenceMemo(curSeq);
					showSeqParameters();
					
					for (int i = 0; i < curSeq.cmd_list.count(); i++)
					{
						Sequence_Cmd *cmd = curSeq.cmd_list[i];
						for (int j = 0; j < curSeq.param_list.count(); j++)
						{
							Sequence_Param *cur_param = curSeq.param_list[j];
							if (cmd->str_bytes234 == cur_param->name)
							{
								cmd->byte2 = (uint8_t)((cur_param->app_value & 0x00FF0000) >> 16);
								cmd->byte3 = (uint8_t)((cur_param->app_value & 0x0000FF00) >> 8);
								cmd->byte4 = (uint8_t)((cur_param->app_value & 0x000000FF));
							}
						}
					}
					return;
				}
			}
		}		
	}*/
}

void ExperimentManager::paramEditingFinished(QObject *obj)
{
	/*CTreeWidgetItem *ctwi = qobject_cast<CTreeWidgetItem*>(sender());
	if (ctwi)
	{
		Sequence_Param *cur_param = (Sequence_Param*)ctwi->getLinkedObject();
		if (cur_param)
		{
			CTextEdit *cted = qobject_cast<CTextEdit*>(obj);
			if (cted)
			{
				bool ok;
				int32_t i32_value = cted->text().toInt(&ok);
				if (ok) 
				{
					cur_param->def_value = i32_value;

					emit sequence_changed();

					return;
				}
			}

			CComboBox *ccbox = qobject_cast<CComboBox*>(obj);
			if (ccbox)
			{
				bool ok;
				QString str_value = ccbox->currentText();
				// ...
				return;			
			}

			CSpinBox *csbox = qobject_cast<CSpinBox*>(obj);
			if (csbox)
			{				
				int32_t i32_value = csbox->value();					
				cur_param->def_value = i32_value;

				for (int i = 0; i < curSeq.param_list.count(); i++)
				{
					Sequence_Param *seq_param = curSeq.param_list[i];
					QString formula = seq_param->formula;
					if (seq_param->formula != "" && seq_param->def_value != NAN)
					{
						formula = simplifyFormula(formula, &curSeq);

						QScriptEngine engine;
						QScriptValue qscr_val = engine.evaluate(formula);
						if (!qscr_val.isError()) 
						{
							int res = qscr_val.toInt32();
							seq_param->app_value = res;
						}		
						else
						{
							QString par_name = seq_param->name;
							curSeq.seq_errors.append(SeqErrors(E_BadFormula, par_name));							
						}
					}
					else
					{
						seq_param->app_value = seq_param->def_value;
					}
				}

				for (int i = 0; i < curSeq.cmd_list.count(); i++)
				{
					Sequence_Cmd *cmd = curSeq.cmd_list[i];
					for (int j = 0; j < curSeq.param_list.count(); j++)
					{
						Sequence_Param *cur_param = curSeq.param_list[j];
						if (cmd->str_bytes234 == cur_param->name)
						{
							cmd->byte2 = (uint8_t)((cur_param->app_value & 0x00FF0000) >> 16);
							cmd->byte3 = (uint8_t)((cur_param->app_value & 0x0000FF00) >> 8);
							cmd->byte4 = (uint8_t)((cur_param->app_value & 0x000000FF));
						}
					}
				}

				for (int i = 0; i < curSeq.instr_pack_list.count(); i++)
				{
					Sequence_InstrPack *pack = curSeq.instr_pack_list[i];
					for (int k = 0; k < pack->instr_list.count(); k++)
					{
						Sequence_Instr *instr = pack->instr_list[k];
						for (int j = 0; j < curSeq.param_list.count(); j++)
						{
							Sequence_Param *cur_param = curSeq.param_list[j];
							if (!instr->str_params.isEmpty())
							{
								for (int m = 0; m < instr->str_params.count(); m++)
								{
									if (instr->str_params[m] == cur_param->name)
									{
										instr->param_bytes[4*m+3] = (uint8_t)((cur_param->app_value & 0xFF000000) >> 24);
										instr->param_bytes[4*m+2] = (uint8_t)((cur_param->app_value & 0x00FF0000) >> 16);
										instr->param_bytes[4*m+1] = (uint8_t)((cur_param->app_value & 0x0000FF00) >> 8);
										instr->param_bytes[4*m] = (uint8_t)((cur_param->app_value & 0x000000FF));
									}
								}
							}
						}
					}
				}
								
				showSequenceMemo(curSeq);
				showSeqParameters();

				emit sequence_changed();

				return;
			}
		}		
	}*/
}

void ExperimentManager::treeWidgetActivated(QTreeWidgetItem* item, int index)
{
	/*for (int i = 0; i < c_items.count(); i++)
	{
		QTreeWidgetItem *sub_item = c_items[i]->getQSubTreeWidgetItem();
		if (sub_item == item)
		{
			c_item_selected = c_items[i];			
		}
	}*/
}

uint8_t ExperimentManager::findCmdCode(const QString &str, bool &flag)
{
	uint8_t res = 0;
	/*flag = false;
	for (int i = 0; i < seq_cmd_index.count(); i++)
	{
		if (str == seq_cmd_index[i].cmd_name) 
		{
			res = seq_cmd_index[i].cmd_code;		
			flag = true;
		}
	}*/
	return res;
}

QString ExperimentManager::findCmdMnemonics(const uint8_t value, bool &flag)
{
	QString res = "";
	/*flag = false;
	for (int i = 0; i < seq_cmd_index.count(); i++)
	{
		if (value == seq_cmd_index[i].cmd_code)
		{
			res = seq_cmd_index[i].cmd_name;
			flag = true;
		}
	}*/
	return res;
}

QList<uint8_t> ExperimentManager::findCmdValue(const QString &str)
{
	QList<uint8_t> res;
	/*for (int i = 0; i < curSeq.param_list.count(); i++)
	{
		if (str == curSeq.param_list[i]->name)
		{
			//int32_t new_value = curSeq.param_list[i]->def_value;
			int32_t new_value = curSeq.param_list[i]->app_value;
			uint8_t byte4 = (uint8_t)(new_value & 0xFF);
			uint8_t byte3 = (uint8_t)((new_value & 0xFF00) >> 8);
			uint8_t byte2 = (uint8_t)((new_value & 0xFF0000) >> 16);
			res << byte2 << byte3 << byte4;		

			return res;
		}
	}*/

	return res;
}

QString ExperimentManager::findInstrMnemonics(const uint8_t value, bool &flag)
{
	QString res = "";
	/*flag = false;
	for (int i = 0; i < seq_instr_index.count(); i++)
	{
		if (value == seq_instr_index[i].instr_code)
		{
			res = seq_instr_index[i].instr_name;
			flag = true;
		}
	}*/
	return res;
}

uint8_t ExperimentManager::findInstrCode(const QString &str, bool &flag)
{
	uint8_t res = 0;
	/*flag = false;
	for (int i = 0; i < seq_instr_index.count(); i++)
	{
		if (str == seq_instr_index[i].instr_name) 
		{
			res = seq_instr_index[i].instr_code;		
			flag = true;
		}
	}*/
	return res;
}

QList<uint8_t> ExperimentManager::findInstrValue(const QString &str, bool &flag)
{
	QList<uint8_t> res;
	/*flag = false;
	for (int i = 0; i < curSeq.param_list.count(); i++)
	{
		if (str == curSeq.param_list[i]->name)
		{			
			int32_t new_value = curSeq.param_list[i]->app_value;
			uint8_t byte1 = (uint8_t)(new_value & 0x000000FF);
			uint8_t byte2 = (uint8_t)((new_value & 0x0000FF00) >> 8);
			uint8_t byte3 = (uint8_t)((new_value & 0x00FF0000) >> 16);
			uint8_t byte4 = (uint8_t)((new_value & 0xFF000000) >> 24);
			res << byte1 << byte2 << byte3 << byte4;		

			flag = true;
			return res;
		}
	}*/

	return res;
}


void ExperimentManager::viewCode()
{

	//ViewCodeDialog view_code_dlg(&curSeq);
	//if (view_code_dlg.exec());
	
}
/*
void SequenceWizard::setExportSettings()
{
	ExportSettingsDialog dlg(app_settings);
	if (dlg.exec())
	{
		save_data.file_name = dlg.getFileName();
		save_data.path = dlg.getPath();
		save_data.prefix = dlg.getPrefix();
		save_data.postfix = dlg.getPostfix();
		save_data.extension = dlg.getExtension();
		save_data.to_save = dlg.isDataExported();

		emit save_data_changed(save_data);
	}
}*/

QString ExperimentManager::getStrItemNumber(QString text, int index, int base)
{
	QString res = "";

	/*QString num = QString::number(index);

	QString pre = "_";
	for (int i = 0; i < base-num.size(); i++) pre += "0";
	res += text + pre + num;*/

	return res;
}

/*bool ExperimentManager::getDSPPrg(QVector<uint8_t> &_prg, QVector<uint8_t> &_instr)
{	
	_prg.clear();

	QList<Sequence_Cmd*> cmd_list = curSeq.cmd_list;	
	for (int i = 0; i < cmd_list.count(); i++)
	{		
		if (cmd_list[i]->flag)
		{
			_prg.append(cmd_list[i]->byte1);
			_prg.append(cmd_list[i]->byte2);
			_prg.append(cmd_list[i]->byte3);
			_prg.append(cmd_list[i]->byte4);
		}
		else 
		{			
			return false;
		}
	}
		
	_instr.clear();
	if (curSeq.instr_packs_count > 0)
	{
		QList<Sequence_InstrPack*> instr_packs = curSeq.instr_pack_list;
		for (int i = 0; i < instr_packs.count(); i++)
		{		
			Sequence_InstrPack *instr_pack = instr_packs[i];
			_instr.append(instr_pack->pack_number);

			QList<Sequence_Instr*> instr_list = instr_pack->instr_list;
			for (int j = 0; j < instr_list.count(); j++)
			{
				Sequence_Instr* instr = instr_list[j];
				if (instr->flag)
				{
					_instr.append(instr->byte1);
					_instr.append(instr->byte2);
					_instr.append(instr->byte3);					

					for (int k = 0; k < instr->param_bytes.count(); k++)
					{
						_instr.append(instr->param_bytes[k]);						
					}
				}
				else 
				{					
					return false;
				}	

				if ((j == instr_list.count()-1) && (i < instr_packs.count()-1)) 
				{
					_instr.append(0xFF);					
				}
			}			
		}
	}

	return true;
}*/

/*QString ExperimentManager::simplifyFormula(QString _formula, Sequence *_seq)
{
	QList<QPair<int,int> > list1;	
	for (int i = 0; i < _seq->param_list.count(); i++)
	{
		Sequence_Param *param = _seq->param_list[i];
		int name_len = param->name.length();
		list1 << QPair<int,int>(i,name_len);
	}
	if (list1.isEmpty()) return "";

	for (int i = 0; i < list1.count(); i++)
	{
		int index = i;
		QPair<int,int> _min = QPair<int,int>(list1[i].first,list1[i].second);		
		for (int j = i+1; j < list1.count(); j++)
		{
			if (_min.second > list1[j].second) 
			{
				_min = list1[j];
				index = j;
			}
		}
		list1.removeAt(index);
		list1.push_front(_min);
	}

	for (int i = 0; i < list1.count(); i++)
	{
		int index = list1[i].first;
		Sequence_Param *param = _seq->param_list[index];		
		if (_formula.contains(param->name))
		{
			QString num_str = QString::number(param->def_value);
			_formula = _formula.replace(param->name, num_str, Qt::CaseInsensitive);
		}
	}

	return _formula;
}*/
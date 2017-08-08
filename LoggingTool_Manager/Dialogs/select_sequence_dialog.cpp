#include "select_sequence_dialog.h"
//#include "../Wizards/sequence_wizard.h"


SelectSequenceDialog::SelectSequenceDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SelectSequenceDialog)
{
	ui->setupUi(this);

	//ui->tbtAddSequence->setIcon(QIcon(":/images/add_script.png"));

	QStringList headlist;
	ui->treeWidget->setColumnCount(3);
	headlist << tr("Select") << tr("Sequence") << tr("Status");
	ui->treeWidget->setHeaderLabels(headlist);
	ui->treeWidget->setColumnWidth(0,50);
	ui->treeWidget->setColumnWidth(1,250);	
	ui->treeWidget->setColumnWidth(2,50);
	
	ui->treeWidget->header()->setFont(QFont("Arial", 10, 0, false));	

	setParent(parent);
	expManager = qobject_cast<ExperimentManager*>(parent);
	if (expManager)
	{
		seq_cmd_index = expManager->getSeqCmdInfo();
		seq_instr_index = expManager->getSeqInstrIndex();		
		seq_datatype_index = expManager->getSeqDataTypeInfo();		

		file_list = expManager->getSeqFileList();
		path_list = expManager->getSeqPathList();
	}
	else qFatal(tr("Cannot get Sequence Wizard data!").toLocal8Bit().data());

	bool res = findSequenceScripts(*file_list, *path_list);
	if (res) 
	{
		ui->lblLoading->setVisible(true);
		ui->progressBar->setVisible(true);
		ui->progressBar->setValue(0);

		for (int i = 0; i < file_list->count(); i++)
		{
			//QString file_name = path_list[i] + "/" + file_list[i];
			//QSettings *sequence_proc = initSequenceScript(file_name);
			//parseSequenceScript(sequence_proc, curSeq);


		}
		/*QString file_name = path_list.first() + "/" + file_list.first();
		sequence_proc = initSequenceScript(file_name);
		parseSequenceScript(sequence_proc, curSeq);

		bool errs = false;
		if (!curSeq.seq_errors.isEmpty()) errs = true;
		for (int i = 0; i < curSeq.param_list.count(); i++) if (!curSeq.param_list[i]->flag) errs = true;

		ui->cboxSequences->addItems(file_list);
		ui->ledSeqName->setText(curSeq.name);

		QString memo = "<font color = darkblue>Author:</font> " + curSeq.author + "<br>";
		memo += "<font color = darkblue>Created:</font> " + curSeq.date_time.toString("dd.MM.yyyy hh:mm:ss") + "<br>";
		memo += "<font color = darkblue>Description:</font> " + curSeq.description + "<br><br>";

		memo += "<font color = darkblue>Errors:</font> " + QString(errs ? "<a href=#parse_error><font color=red><b><u>Found!</u></b></font></a>" : "<font color=darkgreen>Not Found.</font>");

		ui->lblDescription->setOpenExternalLinks(false);
		ui->lblDescription->setText(memo);*/
	}

	ui->lblLoading->setVisible(false);
	ui->progressBar->setVisible(false);	
}

SelectSequenceDialog::~SelectSequenceDialog()
{
	delete ui;
}

bool SelectSequenceDialog::findSequenceScripts(QStringList &files, QStringList &pathes)
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
}

QSettings* SelectSequenceDialog::initSequenceScript(QString file_name)
{
	QSettings *settings = new QSettings(file_name, QSettings::IniFormat);	

#ifdef Q_OS_WIN
	QString code_page = "Windows-1251";
#elif Q_OS_LINUX
	QString code_page = "UTF-8";
#endif
	QTextCodec *codec = QTextCodec::codecForName(code_page.toUtf8());
	settings->setIniCodec(codec);

	return settings;
}
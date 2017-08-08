#include <QPushButton>

#include "view_code_dialog.h"

ViewCodeDialog::ViewCodeDialog(Sequence *seq, QWidget *parent /* = 0 */)
{
	setupUi(this);
	this->setParent(parent);

	this->setWindowTitle(tr("View Code"));
	
	tbtHex->setIcon(QIcon(":/images/HEX.png"));
	tbtDec->setIcon(QIcon(":/images/DEC.png"));
	tbtByteCode->setIcon(QIcon(":/images/List.png"));
	
	tbtDec->setChecked(true);

	this->seq = seq;

	mode = Dec;	
	view_bytecode = false;
	showSeqProgram();

	QPushButton * bOK = buttonBox->button(QDialogButtonBox::Ok);
	connect(bOK, SIGNAL(clicked()), this, SLOT(close()));
	connect(tbtDec, SIGNAL(clicked(bool)), this, SLOT(setDecMode(bool)));
	connect(tbtHex, SIGNAL(clicked(bool)), this, SLOT(setHexMode(bool)));	
	connect(tbtByteCode, SIGNAL(clicked(bool)), this, SLOT(viewByteCode(bool)));
}

void ViewCodeDialog::setMode(Mode _mode)
{
	mode = _mode;
	showSeqProgram();
}

void ViewCodeDialog::showSeqProgram()
{	
	QList<Sequence_Cmd*> cmd_list = seq->cmd_list;
	QString prg_text = "";
	QString byte_code = "";
	lblErrors->setText("");
	for (int i = 0; i < cmd_list.count(); i++)
	{
		prg_text += cmdToString(cmd_list[i]);
		if (view_bytecode) byte_code += cmdToByteCode(cmd_list[i]);
	}

	tedProgram->clear();

	QString str_ = "";
	if (view_bytecode) str_ = tr("<b>Byte-code:</b>");
	QString table_header = QString("<table><tr bgcolor=""#DDA0DD""><td><b>Sequence Commands:</b></td><td>%1</td></tr>").arg(str_); 
	prg_text = "<tr><td>" + prg_text + "</td>" + "<td>" + byte_code + "</td>" + "</tr>";
	QString table_footer = "</table>";
	prg_text = table_header + prg_text + table_footer;
	tedProgram->insertHtml(prg_text);
	

	QList<Sequence_InstrPack*> instr_pack_list = seq->instr_pack_list;

	QString instr_program = "";
	QString all_instr_byte_codes = "";
 	for (int i = 0; i < instr_pack_list.count(); i++)
	{
		Sequence_InstrPack *instr_pack = instr_pack_list[i];
		QString str_instr_pack = "";
		QString instr_byte_code = "";
				
		QString table_header = QString("<tr bgcolor=""#DDA0DD""><td><b>Package '%1' ( #%2 ):</b></td><td>%3</td></tr>").arg(instr_pack->pack_name).arg(instr_pack->pack_number).arg(str_); 
		QString table_footer = "<br>";	
		QList<Sequence_Instr*> instr_list = instr_pack->instr_list;
		for (int j = 0; j < instr_list.count(); j++)
		{
			QString view_str = "";
			if (view_bytecode) view_str = instrToByteCode(instr_list[j]);
			str_instr_pack += QString("<tr><td>") + instrToString(instr_list[j]) + QString("</td><td>%1</td></tr>").arg(view_str);
		}
		instr_program += table_header + str_instr_pack + table_footer;// + "<br><br>";
		
	}

	instr_program = "<table>" + instr_program + "</table>";
	
	tedProcessing->clear();
	tedProcessing->insertHtml(instr_program);
}

void ViewCodeDialog::setDecMode(bool flag)
{
	if (!flag) 
	{
		flag = true;	
		tbtDec->setChecked(flag);
		return;
	}

	tbtHex->setChecked(false);
	mode = Dec;

	showSeqProgram();
}

void ViewCodeDialog::setHexMode(bool flag)
{
	if (!flag) 
	{
		flag = true;
		tbtHex->setChecked(flag);
		return;
	}

	tbtDec->setChecked(false);
	mode = Hex;

	showSeqProgram();
}

void ViewCodeDialog::viewByteCode(bool flag)
{
	view_bytecode = flag;
	showSeqProgram();
}

QString ViewCodeDialog::cmdToString(Sequence_Cmd *cmd)
{	
	int base = 10;
	QString pre = "";
	QString str_clr = "<font color=darkblue>";	
	if (mode == Hex) 
	{
		str_clr = "<font color=darkmagenta>";
		base = 16;
		pre = "0x";
	}
	
	QString num_clr = "<font color=grey>";
	QString mnem_clr = "<font color=darkgreen>";
	QString b234_clr = "<font color=blue>";
	QString comma_clr = "<font color=black>";
	if (!cmd->flag) 
	{
		num_clr = "<font color=red>";
		mnem_clr = "<font color=red>";
		b234_clr = "<font color=red>";
		str_clr = "<font color=red>";
		comma_clr = "<font color=red>";

		lblErrors->setText(tr("<font color=red><b>Errors found !</b></font>"));
	}

	QString res = num_clr + QString::number(cmd->cmd_number) + ". </font>";	

	if (cmd->str_byte1 != "") res += mnem_clr + cmd->str_byte1 + "</font>" + '\t';
	else res += mnem_clr + pre + QString::number(cmd->byte1, base).toUpper() + "</font>" + '\t';

	if (cmd->str_bytes234 != "") res += b234_clr + cmd->str_bytes234 + "</font>";
	else
	{
		res += str_clr + pre + QString::number(cmd->byte2, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
		res += str_clr + pre + QString::number(cmd->byte3, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
		res += str_clr + pre + QString::number(cmd->byte4, base).toUpper() + "</font>";
	}

	res += "<br>";

	return res;
}

QString ViewCodeDialog::cmdToByteCode(Sequence_Cmd *cmd)
{
	int base = 10;
	QString pre = "";
	QString str_clr = "<font color=darkblue>";	
	if (mode == Hex) 
	{
		str_clr = "<font color=darkmagenta>";
		base = 16;
		pre = "0x";
	}

	QString num_clr = "<font color=grey>";
	QString mnem_clr = "<font color=darkgreen>";
	QString b234_clr = "<font color=blue>";
	QString comma_clr = "<font color=black>";
	if (!cmd->flag) 
	{
		num_clr = "<font color=red>";
		mnem_clr = "<font color=red>";
		b234_clr = "<font color=red>";
		str_clr = "<font color=red>";
		comma_clr = "<font color=red>";

		lblErrors->setText(tr("<font color=red><b>Errors found !</b></font>"));
	}

	QString res = num_clr + QString::number(cmd->cmd_number) + ". </font>";

	res += mnem_clr + pre + QString::number(cmd->byte1, base).toUpper() + "</font>" + '\t';

	res += str_clr + pre + QString::number(cmd->byte2, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
	res += str_clr + pre + QString::number(cmd->byte3, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
	res += str_clr + pre + QString::number(cmd->byte4, base).toUpper() + "</font>";

	res += "<br>";

	return res;
}

QString ViewCodeDialog::instrToString(Sequence_Instr *instr)
{
	int base = 10;
	QString pre = "";
	QString str_clr = "<font color=darkblue>";	
	if (mode == Hex) 
	{
		str_clr = "<font color=darkmagenta>";
		base = 16;
		pre = "0x";
	}

	QString num_clr = "<font color=grey>";
	QString mnem_clr = "<font color=darkgreen>";
	QString b234_clr = "<font color=blue>";
	QString comma_clr = "<font color=black>";
	if (!instr->flag) 
	{
		num_clr = "<font color=red>";
		mnem_clr = "<font color=red>";
		b234_clr = "<font color=red>";
		str_clr = "<font color=red>";
		comma_clr = "<font color=red>";

		lblErrors->setText(tr("<font color=red><b>Errors found !</b></font>"));
	}

	QString res = num_clr + QString::number(instr->instr_number) + ". </font>";	

	if (instr->str_byte1 != "") res += mnem_clr + instr->str_byte1 + "</font>" + '\t';
	else res += mnem_clr + pre + QString::number(instr->byte1, base).toUpper() + "</font>" + '\t';
	res += str_clr + pre + QString::number(instr->byte2, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
	res += str_clr + pre + QString::number(instr->byte3, base).toUpper() + "</font>";

	if (!instr->str_params.isEmpty())
	{
		for (int i = 0; i < instr->str_params.count(); i++)
		{
			res += comma_clr + ",</font>" + '\t' + b234_clr + instr->str_params[i] + "</font>";
		}
	}
	
	return res;
}

QString ViewCodeDialog::instrToByteCode(Sequence_Instr *instr)
{
	int base = 10;
	QString pre = "";
	QString str_clr = "<font color=darkblue>";	
	if (mode == Hex) 
	{
		str_clr = "<font color=darkmagenta>";
		base = 16;
		pre = "0x";
	}

	QString num_clr = "<font color=grey>";
	QString mnem_clr = "<font color=darkgreen>";
	QString b234_clr = "<font color=blue>";
	QString comma_clr = "<font color=black>";
	if (!instr->flag) 
	{
		num_clr = "<font color=red>";
		mnem_clr = "<font color=red>";
		b234_clr = "<font color=red>";
		str_clr = "<font color=red>";
		comma_clr = "<font color=red>";

		lblErrors->setText(tr("<font color=red><b>Errors found !</b></font>"));
	}

	QString res = num_clr + QString::number(instr->instr_number) + ". </font>";

	res += mnem_clr + pre + QString::number(instr->byte1, base).toUpper() + "</font>" + '\t';
	res += str_clr + pre + QString::number(instr->byte2, base).toUpper() + "</font>" + comma_clr + ",</font>" + '\t';
	res += str_clr + pre + QString::number(instr->byte3, base).toUpper() + "</font>";
	
	if (!instr->str_params.isEmpty())
	{
		for (int i = 0; i < instr->param_bytes.count(); i++)
		{
			res += comma_clr + ",</font>" + '\t' + b234_clr + pre + QString::number(instr->param_bytes[i], base).toUpper() + "</font>";
		}
	}	

	return res;
}
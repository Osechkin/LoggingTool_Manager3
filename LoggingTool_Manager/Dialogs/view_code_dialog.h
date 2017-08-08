#ifndef VIEW_CODE_DIALOG_H
#define VIEW_CODE_DIALOG_H

#include <QtGui>

#include "../Wizards/sequence_wizard.h"

#include "ui_view_code_dialog.h"


class ViewCodeDialog : public QDialog, public Ui::ViewCodeDialog
{
	Q_OBJECT

public:
	explicit ViewCodeDialog(Sequence *seq, QWidget *parent = 0);	
	
	enum Mode { Dec, Hex};
	Mode getMode() { return mode; }
	void setMode(Mode _mode);

private:
	void showSeqProgram();
	QString cmdToString(Sequence_Cmd *cmd);
	QString cmdToByteCode(Sequence_Cmd *cmd);
	QString instrToString(Sequence_Instr *instr);
	QString instrToByteCode(Sequence_Instr *instr);

	Sequence *seq;
	Mode mode;
	bool view_bytecode;

private slots:
	void setDecMode(bool);
	void setHexMode(bool);	
	void viewByteCode(bool);
};

#endif // VIEW_CODE_DIALOG_H
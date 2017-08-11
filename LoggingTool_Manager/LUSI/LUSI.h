#ifndef LUSI_H
#define LUSI_H

#include "stdarg.h"

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QScriptEngine>

#include "../Common/experiment_settings.h"


#ifndef uint8_t
typedef unsigned __int8   uint8_t;
#endif

#ifndef uint16_t
typedef unsigned __int16   uint16_t;
#endif

#ifndef uint32_t
typedef unsigned __int32   uint32_t;
#endif


typedef QVector<int>					QIntVector;
typedef QVector<QPair<int,int> >		QIntPairVector;
typedef QList<QPair<QString,QString> >	QStringPairList;
typedef QVector<uint8_t>				QByteVector;



namespace LUSI
{
	static QStringList separate(QString _str, QString _separator, QString _ignore_list, bool _trim = true);


	struct Definition
	{
		enum Type { Section, Parameter, ProcPackage, ComProgram, Condition, Argument, Unknown };

		void clear() { type = Unknown; name = ""; fields.clear(); }

		Type type;
		QString name;
		QStringPairList fields;
	};


	class Object : public QObject
	{
		Q_OBJECT

	public:
		Object() { obj_name = ""; }

		QString getObjName() const { return obj_name; }
		void setObjName(QString _obj_name) { obj_name = _obj_name; }

		Definition::Type getType() const { return type; }
		QStringList getErrorList() const { return elist; }

	public:
		QString obj_name;

	protected:
		Definition::Type type;
		QStringList elist;			// error list
	};
	

	class Section : public Object
	{
		Q_OBJECT
					
		Q_PROPERTY(QString title READ getTitle WRITE setTitle);
		Q_PROPERTY(QObject* last_parameter READ last WRITE append);
		
	public:
		Section(QString _obj_name) { setObjName(_obj_name); type = Definition::Section; }
				
		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }

		QObjectList getParameters() { return parameters; }		

		int size() const { return parameters.size(); }

		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);

	public slots:
		void append(QObject* _obj) { parameters.append(_obj); last_parameter = _obj; }
		QObject* last() const { return parameters.last(); }

	public:
		QString title;

	private:
		QObject* last_parameter;
		QObjectList parameters;
	};

	
	class Parameter : public Object
	{
		Q_OBJECT

		Q_PROPERTY(QString title READ getTitle WRITE setTitle);
		Q_PROPERTY(double min READ getMin WRITE setMin);
		Q_PROPERTY(double max READ getMax WRITE setMax);
		Q_PROPERTY(double value READ getValue WRITE setValue);
		Q_PROPERTY(QString comment READ getComment WRITE setComment);
		Q_PROPERTY(QString units READ getUnits WRITE setUnits);
		Q_PROPERTY(QString formula READ getFormula WRITE setFormula);
		Q_PROPERTY(QString uitype READ getUIType WRITE setUIType);
		Q_PROPERTY(bool ron READ getReadOnly WRITE setReadOnly);

	public:
		Parameter(QString _obj_name)
		{
			setObjName(_obj_name);
			title = ""; 
			value = 0; 
			min = -8388608;
			max = 8388607;
			comment = "";
			units = "";
			ron = false;
			app_value = (int)value; 
			type = Definition::Parameter;
			uitype = "SpinBox";
		}
		Parameter(QString _obj_name, QString _title) 
		{ 
			setObjName(_obj_name);
			title = _title; 
			value = 0; 
			min = -8388608;
			max = 8388607;
			comment = "";
			units = "";
			ron = false;
			app_value = (int)value; 
			type = Definition::Parameter;
			uitype = "SpinBox";
		}	
		Parameter(QString _obj_name, QString _title, double _value) 
		{ 
			setObjName(_obj_name);
			title = _title; 
			value = _value; 
			min = -8388608;
			max = 8388607;
			comment = "";
			units = "";
			ron = false;
			app_value = (int)value; 
			type = Definition::Parameter;
			uitype = "SpinBox";
		}		

	public slots:
		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }

		double getMin() const { return min; }
		void setMin(double _min) { min = _min; }

		double getMax() const { return max; }
		void setMax(double _max) { max = _max; }

		double getValue() const { return value; }
		void setValue(double _value) { value = _value; }

		QString getComment() const { return comment; }
		void setComment(QString _comment) { comment = _comment; }

		QString getUnits() const { return units; }
		void setUnits(QString _units) { units = _units; }

		QString getFormula() const { return formula; }
		void setFormula(QString _formula) { formula = _formula; }

		QString getUIType() const { return uitype; }
		void setUIType(QString _uitype) { uitype = _uitype; }

		bool getReadOnly() const { return ron; }
		void setReadOnly(bool _ron) { ron = _ron; }

		void exec(double _avalue) { app_value = (int)_avalue; }

	public:
		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);
		
	public:
		QString title;			// �������� ��������� (������������ � ������� �������������������) 
		double min;				// ����������� �������� value
		double max;				// ������������ �������� value
		double value;			// �������� ��������� (������������ � ������������� ��������)
		QString comment;		// ����������� (������������ � ���� ToolTip)
		QString units;			// ������� ���������
		QString formula;		// ������� ������� ������������ ��� ������������� ������������� �������� (�� ������������ �� ��������) 
		QString uitype;			// ��� ����������� �������� ����� �������� ��������� : 'SpinBox', 'CheckBox', 'ComboBox'
		bool ron;				// ����������/������������ ��������

	private:
		int app_value;			// �������� ���������, ����������� � ���������� ��� ������������� ������������� � ����������� ����������
	};


	class ProcPackage : public Object
	{
		Q_OBJECT

		Q_PROPERTY(QString title READ getTitle WRITE setTitle);	
		Q_PROPERTY(QVariantList params_array READ getParamsArray WRITE exec);
		
	public:
		ProcPackage(QString _obj_name, QList<SeqInstrInfo> _instr_list);

		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);		

		QByteVector getProcProgram() const { return proc_program; }
		QVariantList getParamsArray() const { return params_array; }
		QStringList getErrorList() const { return elist; }

	public slots:
		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }
		
		void exec(QVariantList _params_array);

	public:
		QString title;

	private:
		uint8_t findInstrCode(const QString &_str, bool &_flag);
					
	private:		
		QByteVector proc_program;
		QVariantList params_array;
		QList<SeqInstrInfo> instr_list;
	};


	class COMProgram : public Object
	{
		Q_OBJECT

		Q_PROPERTY(QString title READ getTitle WRITE setTitle);	
		Q_PROPERTY(QVariantList params_array READ getParamsArray WRITE exec);

	public:
		COMProgram(QString _obj_name, QList<SeqCmdInfo> _com_list);

		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);		

		QByteVector getComProgram() const { return com_program; }
		QVariantList getParamsArray() const { return params_array; }
		QStringList getErrorList() const { return elist; }

	public slots:
		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }

		void exec(QVariantList _params_array);

	public:
		QString title;

	private:
		uint8_t findCmdCode(const QString &_str, bool &_flag);

	private:		
		QByteVector com_program;
		QVariantList params_array;
		QList<SeqCmdInfo> com_list; 		
	};
	

	class Condition : public Object
	{
		Q_OBJECT

		Q_PROPERTY(QString title READ getTitle WRITE setTitle);
		Q_PROPERTY(QString msg READ getMsg WRITE setMsg);
		Q_PROPERTY(QString hint READ getHint WRITE setHint);
		Q_PROPERTY(bool result READ getResult WRITE exec);

	public:
		Condition(QString _obj_name) { setObjName(_obj_name); type = Definition::Condition; }

		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }

		QString getMsg() const { return msg; }
		void setMsg(QString _msg) { msg = _msg; }

		QString getHint() const { return hint; }
		void setHint(QString _hint) { hint = _hint; }

		bool getResult() const { return result; }

		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);
				
	public slots:
		void exec(double _res) { result = (_res >= 0); }

	public:
		QString title;
		QString msg;
		QString hint;

		bool result;
	};


	class Argument : public Object
	{
		Q_OBJECT

		Q_PROPERTY(QString title READ getTitle WRITE setTitle);
		Q_PROPERTY(QString comment READ getComment WRITE setComment);
		Q_PROPERTY(QString units READ getUnits WRITE setUnits);
		Q_PROPERTY(int size READ getSize WRITE setSize);
		Q_PROPERTY(QVector<double> points READ getPoints WRITE setPoints);

	public:
		Argument(QString _obj_name) { setObjName(_obj_name); type = Definition::Argument; }

		QString getTitle() const { return title; }
		void setTitle(QString _title) { title = _title; }

		QString getComment() const { return comment; }
		void setComment(QString _comment) { comment = _comment; }

		QString getUnits() const { return units; }
		void setUnits(QString _units) { units = _units; }

		int getSize() const { return size; }
		void setSize(int _size) { size = _size; }

		QVector<double> getPoints() const { return points; }

		bool setField(QString _name, QString _value);
		bool setField(QString _value, int _index);
		
	public slots:
		void setPoints(QVector<double> _points) { points = _points; }

	public:
		QString title;
		QString comment;
		QString units;
		int size;
		QVector<double> points;
	};

	

	typedef QList<LUSI::Definition>		DefinitionList;
	typedef QList<LUSI::Object*>		ObjectList;

	class Engine : public QObject
	{
		Q_OBJECT

	public:
		Engine(QObject *parent = NULL) { }
		Engine(QScriptEngine *_qsript_engine, QList<SeqCmdInfo> _cmd_list, QList<SeqInstrInfo> _instr_list, QObject *parent = NULL);

		void init(QScriptEngine *_qsript_engine, QList<SeqCmdInfo> _cmd_list, QList<SeqInstrInfo> _instr_list);
		bool evaluate(QStringList &_elist);
		void setLUSIscript(QString _script) { lusi_script = _script; }
		QString getLUSIscript() const { return lusi_script; }
		QString getJSscript() const { return js_script; }
		ObjectList *getObjList() { return &obj_list; }
		bool findLUSIDefinition(QString &_str, Definition &_def, QString &_e);
		bool findLUSIMacros(QString &_str, QString &_e);
		QStringList getErrorList() const { return error_list; }

	private:
		void findAllQuotas(QString &_seq, QIntVector &_quotas);
		void findAllApostrophes(QString &_seq, QIntVector &_apostrophes);
		void findAllSlashComments(QString &_seq, QIntPairVector &_slash_comments);
		void findAllStarComments(QString &_seq, QIntPairVector &_star_comments);
		QPair<int,int> nextQuoteZone(int const _pos, QIntVector const _quotas, int const _seq_len) const;
		QPair<int,int> nextApostropheZone(int const _pos, QIntVector const _apostrophes, int const _seq_len) const;
		QPair<int,int> nextSlashCommentZone(int const _pos, QIntPairVector const _slash_comments) const;
		QPair<int,int> nextStarCommentZone(int const _pos, QIntPairVector const _star_comments) const;
		//void findDeadZones(QString &_seq, QIntPairVector &_dead_zones);
		QString removeComments(QString &_text);
				
		void startLusiing(QString _script, QStringList &_elist, DefinitionList &definitions, QString &_js_script);
		uint8_t findInstrCode(const QString &_str, bool &_flag);
		uint8_t findCmdCode(const QString &_str, bool &_flag);
		
	private:
		QScriptEngine *qscript_engine;
		QList<SeqInstrInfo> instr_list;
		QList<SeqCmdInfo> cmd_list;
		QString lusi_script;
		QString js_script;
		ObjectList obj_list;
		QString started_package;
		bool package_was_started;
		QString started_comprg;
		bool comprg_was_started;
		QStringList error_list;
	};
	


	struct Sequence
	{
		Sequence()
		{
			clear();
		}

		~Sequence()
		{
			qDeleteAll(param_list);
			qDeleteAll(cmd_list);
			qDeleteAll(instr_pack_list);
			qDeleteAll(arg_list);
			qDeleteAll(cond_list);
		}

		void clear()
		{
			name = "";
			author = "";
			date_time = QDateTime::currentDateTime();		
			freq_count = 0;
			interval_count = 0;
			rf_pulse_count = 0;
			counter_count = 0;
			phase_count = 0;
			constant_count = 0;
			probe_count = 0;
			commands_count = 0;
			instr_packs_count = 0;
			processing_count = 0;
			output_count = 0;
			args_count = 0;
			conds_count = 0;

			seq_errors.clear();

			if (!param_list.isEmpty())
			{
				qDeleteAll(param_list);
				param_list.clear();
			}

			if (!cmd_list.isEmpty())
			{
				qDeleteAll(cmd_list);
				cmd_list.clear();
			}

			if (!arg_list.isEmpty())
			{
				qDeleteAll(arg_list);
				arg_list.clear();
			}

			if (!cond_list.isEmpty())
			{
				qDeleteAll(cond_list);
				cond_list.clear();
			}

			if (!instr_pack_list.isEmpty())
			{
				for (int i = 0; i < instr_pack_list.count(); i++)
				{
					Sequence_InstrPack* instr_pack = instr_pack_list[i];
					delete instr_pack;
				}	
				instr_pack_list.clear();
				instr_packs_count = 0;
			}
		}

		QString name;						// �������� ������������������
		QString author;						// ����� ������������������
		QDateTime date_time;				// ���� � ����� �������� ������������������	
		QString description;				// �������� ������������������
		int freq_count;						// ����� ����������-������ � ������������������
		int interval_count;					// ����� ��������� ���������� � ������������������
		int rf_pulse_count;					// ����� �������������� ���������
		int counter_count;					// ����� ���������
		int phase_count;					// ����� ������� ����������
		int constant_count;					// ����� ����������-��������
		int probe_count;					// ����� ��������
		int commands_count;					// ����� ������ (�������� 'Commands_Number' � �������) � ������������������ 
		int instr_packs_count;				// ����� ������� ���������� (�������� 'Instructions_Packs_Number' � �������) � ������������������
		int processing_count;				// ����� ���������� ��� ��������� ������� ��� 
		int output_count;					// ����� �������� ������� ������, ����������� ���������� ��������
		int args_count;						// ����� ����������-���������� ��� ���������� �������� �� ��� Ox (�������� ��������������)
		int conds_count;					// ����� ������� �������� ������������ ��������� ������������������
		QList<Sequence_Param*> param_list;	// ������ ���������� ������������������
		QList<Sequence_Cmd*> cmd_list;		// ������ ������ ������������������ (� ������ ������� ������ �� �������)
		QList<Sequence_InstrPack*> instr_pack_list;	// ������ ������� ���������� ��� ��������� ������� ��� ���������� ����������� ���������� 
		QList<Argument*> arg_list;			// ������ ����������-���������� ��� ���������� ��� �� ��� �������� ������. ���������� ��� ������������� ������ 
		QList<Condition*> cond_list;		// ������ ����������� ���������, ����������� ������������ ������� ���������� ������������������
		QList<SeqErrors> seq_errors;		// ������, ������������ ��� ������� ����� ������������������	 
	};

	struct Sequence
	{		
		Sequence();
		Sequence(Engine *_lusi_engine);

		void setObjects(ObjectList *_obj_list);
		void clear();

		QString name;						// �������� ������������������
		QString author;						// ����� ������������������
		QDateTime date_time;				// ���� � ����� �������� ������������������	
		QString description;				// �������� ������������������

		QString js_script;					// ��������� ������������������ �� JavaScript

		QList<QByteVector> com_programs;	// ������ �������� ��� ������������� ������������� (������ ����)
		QList<QByteVector> proc_program;	// ������ ������� ��������� ������ ��� ��������� NMR_Tool �� DSP
		QList<LUSI::Parameter*> param_list;	// ������ ���������� ������������������
		QList<LUSI::Section*> cmd_list;		// ������ ������ ������������������ (� ������ ������� ������ �� �������)		
		QList<LUSI::Argument*> arg_list;	// ������ ����������-���������� ��� ���������� ��� �� ��� �������� ������. ���������� ��� ������������� ������ 
		QList<LUSI::Condition*> cond_list;	// ������ ����������� ���������, ����������� ������������ ������� ���������� ������������������
		QStringList seq_errors;				// ������, ������������ ��� ������� ����� ������������������
	};
}


#endif // LUSI_H
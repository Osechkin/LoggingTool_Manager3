#ifndef EXPERIMENT_SCHEDULER_H
#define EXPERIMENT_SCHEDULER_H

#include <QObject>
#include <QComboBox>
#include <QFileInfo>


namespace Scheduler
{
	enum Command { Exec_Cmd, DistanceRange_Cmd, SetDistance_Cmd, Loop_Cmd, End_Cmd, NoP_Cmd };
	enum WidgetType { DoubleSpinBox, ComboBox, LineEdit, FileBrowse };
		
	struct SettingsItem
	{
		SettingsItem(QString _title, WidgetType _wid_type, QString _units)
		{
			title = _title;
			widget_type = _wid_type;			
			units = _units;
		}

		QString title;		
		QString units;
		WidgetType widget_type;			// тип виджета для ввода параметра команды 		
	};
	typedef QList<SettingsItem*>		SettingsItemList;


	class FileBrowser : public QWidget
	{
		Q_OBJECT
		
	public:
		FileBrowser(QString _file_name, QWidget *parent = 0);

		QString getPath() { return file_path; } 
		QString getFileName() { return file_name; }

	private slots:		
		void enterDataFileName();
	
	private:
		QLineEdit *ledFileName;

		QString file_path;
		QString file_name;
		
	signals:
		void new_filename(const QFileInfo&);
	};


	class SchedulerObject : public QObject
	{
		Q_OBJECT

	public:
		SchedulerObject(Command _type = NoP_Cmd); 
		
		Command type;				
		QString cell_text;
		QString cell_text_template;
		SettingsItemList param_objects;

	signals:
		void changed();
	};

	class Exec : public SchedulerObject
	{
		Q_OBJECT

	public:
		explicit Exec(QStringList _jseqs, QString jseq_file, QString _data_file);
		~Exec();

		QStringList jseq_list;
		QString jseq_name;		
		QString data_file;

	public slots:
		void changeJSeq(const QString &_jseq) { jseq_name = _jseq; emit changed(); }
		void editFileName(const QFileInfo &_fi) { data_file = _fi.absoluteFilePath(); emit changed(); }
	};

	class DistanceRange : public SchedulerObject
	{
		Q_OBJECT

	public: 
		explicit DistanceRange();

		double from;
		double to;
		double step;
	};

	class SetDistance : public SchedulerObject
	{
		Q_OBJECT

	public: 
		explicit SetDistance();

		double position;
	};

	class Loop : public SchedulerObject
	{
		Q_OBJECT

	public: 
		explicit Loop();
				
		int index;
		int to;
	};

	class End : public SchedulerObject
	{
		Q_OBJECT

	public: 
		explicit End();

		SchedulerObject *ref_obj;
	};

	class NOP : public SchedulerObject
	{
		Q_OBJECT

	public: 
		explicit NOP();
	};


	typedef QList<SchedulerObject*>		SchedulerObjList;

	class Engine : public QObject
	{
		Q_OBJECT

	public:
		Engine() { }
		~Engine();
		
		SchedulerObjList getObjectList() { return obj_list; }
		
		template<class X> void add(X* obj)
		{
			SchedulerObject *sch_obj = static_cast<SchedulerObject*>(obj);
			if (sch_obj) obj_list.append(obj);
		}		
		template<class X> void insert(int index, X* obj)
		{
			if (index > obj_list.count()) return;

			SchedulerObject *sch_obj = static_cast<SchedulerObject*>(obj);
			if (sch_obj) obj_list.insert(index, obj);
		}		
		SchedulerObject* get(int index);	
		void remove(int index);
		SchedulerObject* take(int index);
		void clear();
		
	private:
		SchedulerObjList obj_list;
	};
}



#endif // EXPERIMENT_SCHEDULER_H
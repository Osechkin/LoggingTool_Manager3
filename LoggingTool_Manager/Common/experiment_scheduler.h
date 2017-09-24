#ifndef EXPERIMENT_SCHEDULER_H
#define EXPERIMENT_SCHEDULER_H

#include <QObject>


namespace Scheduler
{
	enum Command { Exec_Cmd, DistanceRange_Cmd, SetDistance_Cmd, Loop_Cmd, Until_Cmd, NoP_Cmd };
	

	class SchedulerObject
	{
	public:
		SchedulerObject(Command _type = NoP_Cmd); 
		
		Command type;		
		QString mnemonic;
		QString cell_text;
		QObjectList *param_objects;
	};

	class Exec : public SchedulerObject
	{
	public:
		explicit Exec();

		QString jseq_name;
		QString jseq_path;
		QString data_file;
	};

	class DistanceRange : public SchedulerObject
	{
	public: 
		explicit DistanceRange();

		double from;
		double to;
		double step;
	};

	class SetDistance : public SchedulerObject
	{
	public: 
		explicit SetDistance();

		double position;
	};

	class Loop : public SchedulerObject
	{
	public: 
		explicit Loop();
				
		int index;
		int to;
	};

	class Until : public SchedulerObject
	{
	public: 
		explicit Until();

		SchedulerObject *ref_obj;
	};

	class NOP : public SchedulerObject
	{
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
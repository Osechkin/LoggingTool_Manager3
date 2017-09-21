#ifndef EXPERIMENT_SCHEDULER_H
#define EXPERIMENT_SCHEDULER_H

#include <QObject>


namespace Scheduler
{
	enum Command { Exec, DistanceRange, SetDistance, Loop, Until, NOP };


	class SchedulerObject
	{
	public:
		SchedulerObject(Command _type = NOP); 
		
		Command type;
		int line;			// номер линии в программе (нулеруются с единицы)
		QString mnemonic;
	};

	class Exec : public SchedulerObject
	{
	public:
		explicit Exec(int _line);
	};

	class DistanceRange : public SchedulerObject
	{
	public: 
		explicit DistanceRange(int _line);
	};

	class SetDistance : public SchedulerObject
	{
	public: 
		explicit SetDistance(int _line);
	};

	class Loop : public SchedulerObject
	{
	public: 
		explicit Loop(int _line);
				
		int index;
	};

	class Until : public SchedulerObject
	{
	public: 
		explicit Until(int _line);

		SchedulerObject *ref_obj;
	};

	class NOP : public SchedulerObject
	{
	public: 
		explicit NOP(int _line);
	};


	class Engine : public QObject
	{
		Q_OBJECT

	public:
		//Engine();
		//~Engine() { }


	};
}



#endif // EXPERIMENT_SCHEDULER_H
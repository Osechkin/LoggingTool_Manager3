#ifndef DEPTH_TEMPLATE_WIZARD_H
#define DEPTH_TEMPLATE_WIZARD_H

#include <QWidget>

#include "tools_general.h"
#include "../Common/statusbar_widgets.h"
#include "../Communication/threads.h"

#include "../Widgets/abstract_depthmeter.h"
#include "../Widgets/depth_emul_widget.h"
#include "../Widgets/depth_impulsustye_widget.h"
#include "../Widgets/depth_internal_widget.h"
#include "../Widgets/distance_meter_widget.h"

#include "ui_depth_template_wizard.h"


class MainWindow;


class DepthTemplateWizard : public QWidget, public Ui::DepthTemplateWizard
{
	Q_OBJECT

public:
	explicit DepthTemplateWizard(COM_PORT *COM_Port, Clocker *clocker, QWidget *parent = 0);
	~DepthTemplateWizard();

	//void searchDevice();
	//void setCOMPortName(QString port_name) { COMPort_Name = port_name; }
	//void setCOMPortSettings(PortSettings port_settings);

	double_b getDepthData() { return current_depth_meter->getDepth(); /*double_b(depth_flag, depth);*/ } 
	double_b getRateData() { return current_depth_meter->getRate(); /*double_b(rate_flag, rate);*/ }
	double_b getTensionData() { return current_depth_meter->getTension(); /*double_b(tension_flag, tension);*/ }

	ImpulsConnectionWidget *getConnectionWidget() { return connectionWidget; }
	bool isConnected() { return is_connected; }

	COM_PORT *getCOMPort() { return COM_Port; }

	Ui::DepthTemplateWizard *getUI() { return ui; }

private:
	void initCOMSettings(COM_PORT *com_port);
	void showData(uint8_t type, double val);

	void setConnection();
	void setDepthCommunicatorConnections();	
	bool isAvailableCOMPort(COM_PORT *_com_port, int port_num);
	QStringList availableCOMPorts();

private:
	Ui::DepthTemplateWizard *ui;
	ImpulsConnectionWidget *connectionWidget;

	QList<AbstractDepthMeter*> depth_meters;	
	AbstractDepthMeter *current_depth_meter;

	/*double depth;
	double rate;
	double tension;
	bool depth_flag;
	bool rate_flag;
	bool tension_flag;

	QStringList depth_units_list;
	QStringList rate_units_list;
	QStringList tension_units_list;
	double k_depth;
	double k_rate;
	double k_tension;*/

	Clocker *clocker;
	QTimer timer;

	//QString COMPort_Name;
	//DepthCommunicator *depth_communicator;
	COM_PORT *COM_Port;

	bool is_connected;
	bool depth_active;
	bool rate_active;
	bool tension_active;

	//bool device_is_searching;

private slots:
	//void getMeasuredData(uint32_t _uid, uint8_t _type, double val);
	//void measureTimedOut(uint32_t _uid, uint8_t _type);
	//void includeParameter(int state);
	//void changeUnits(QString str);
	//void changeCOMPort(QString str);
	void onTime();
	//void changeLocation(Qt::DockWidgetArea area);
	void changeDepthMeter(QString str);

public slots:
	//void connectDepthMeter(bool flag);

signals:
	void to_measure(uint32_t, uint8_t);		
	void connected(bool);
	void timeout();
};

#endif // DEPTH_TEMPLATE_WIZARD_H
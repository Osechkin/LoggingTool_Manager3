#include <QMessageBox>

#include "distance_meter_widget.h"


static uint32_t uid = 0;

LeuzeDistanceMeterWidget::LeuzeDistanceMeterWidget(Clocker *_clocker, COM_PORT *com_port, QWidget *parent /* = 0 */)  : ui(new Ui::LeuzeDistanceMeter)
{
	ui->setupUi(this);
	this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

	ui->ledDistance->setText("");
	ui->cboxDistance->setStyleSheet("QComboBox { color: darkblue }");
	ui->cboxSetDistance->setStyleSheet("QComboBox { color: darkblue }");
	
	ui->lblPosition->setStyleSheet("QLabel { color: darkblue }");
	ui->ledDistance->setStyleSheet("QLineEdit { color: darkblue }");
	ui->tabWidget->setStyleSheet("QTableWidget { color: darkblue }");
	ui->lblSetPosition->setStyleSheet("QLabel { color: darkblue }");

	ui->lblFrom->setStyleSheet("QLabel { color: darkblue }");
	ui->lblTo->setStyleSheet("QLabel { color: darkblue }");
	ui->lblStep->setStyleSheet("QLabel { color: darkblue }");
	ui->lblZero->setStyleSheet("QLabel { color: darkblue }");
	ui->cboxFrom->setStyleSheet("QComboBox { color: darkblue }");
	ui->cboxTo->setStyleSheet("QComboBox { color: darkblue }");
	ui->cboxStep->setStyleSheet("QComboBox { color: darkblue }");
	ui->cboxFrom->setStyleSheet("QComboBox { color: darkblue }");
	ui->cboxZero->setStyleSheet("QComboBox { color: darkblue }");

	ui->ledDistance->setStyleSheet("QLineEdit { color: darkGreen }");
	ui->ledSetDistance->setStyleSheet("QLineEdit { color: darkGreen }");

	ui->dsboxFrom->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxTo->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxStep->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxZero->setStyleSheet("QDoubleSpinBox { color: darkGreen }");

	ui->pbtBack->setIcon(QIcon(":/images/play_back.png"));
	ui->pbtBegin->setIcon(QIcon(":/images/play_begin.png"));
	ui->pbtForward->setIcon(QIcon(":/images/play.png"));
	ui->pbtEnd->setIcon(QIcon(":/images/play_end.png"));
	
	connectionWidget = new ImpulsConnectionWidget(this);
	connectionWidget->setReport(ConnectionState::State_No);

	distance = 0.001;		// always in meters
	k_distance = 1000;		// for default units ("mm")
	distance_active = true;

	set_distance = 0;
	k_set_distance = 1000;

	from_pos = 0;
	to_pos = 0;
	step_pos = 0;
	zero_pos = 0;
	k_from = 1000;
	k_to = 1000;
	k_step = 1000;
	k_zero = 1000;
	
	distance_units_list << "mm" << "m";	
		
	ui->cboxDistance->addItems(distance_units_list);
	ui->cboxDistance->setEnabled(true);
	ui->cboxSetDistance->addItems(distance_units_list);
	ui->cboxSetDistance->setEnabled(true);
	ui->cboxFrom->addItems(distance_units_list);
	ui->cboxFrom->setEnabled(true);
	ui->cboxTo->addItems(distance_units_list);
	ui->cboxTo->setEnabled(true);
	ui->cboxStep->addItems(distance_units_list);
	ui->cboxStep->setEnabled(true);
	ui->cboxZero->addItems(distance_units_list);
	ui->cboxZero->setEnabled(true);

	ui->ledDistance->setText(QString::number(distance*k_distance));	
		
	is_connected = false;
	device_is_searching = false;

	clocker = _clocker;
	COM_Port = com_port;
	depth_communicator = NULL;

	timer.start(200);	
		
	setConnection();
}

LeuzeDistanceMeterWidget::~LeuzeDistanceMeterWidget()
{
	delete ui;
	delete connectionWidget;
}


void LeuzeDistanceMeterWidget::setConnection()
{
	connect(ui->cboxDistance, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxFrom, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxTo, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxStep, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxZero, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxSetDistance, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	
	connect(ui->pbtConnect, SIGNAL(toggled(bool)), this, SLOT(connectDepthMeter(bool)));

	connect(&timer, SIGNAL(timeout()), this, SLOT(onTime()));
}


void LeuzeDistanceMeterWidget::setDepthCommunicatorConnections()
{
	connect(depth_communicator, SIGNAL(measured_data(uint32_t, uint8_t, double)), this, SLOT(getMeasuredData(uint32_t, uint8_t, double)));
	connect(depth_communicator, SIGNAL(data_timed_out(uint32_t, uint8_t)), this, SLOT(measureTimedOut(uint32_t, uint8_t)));
	connect(this, SIGNAL(to_measure(uint32_t, uint8_t)), depth_communicator, SLOT(toMeasure(uint32_t, uint8_t)));
}



void LeuzeDistanceMeterWidget::stopDepthMeter()
{
	//ui->pbtConnect->setChecked(false);

	timer.stop();

	//disconnect(clocker, SIGNAL(clock()), this, SLOT(clocked()));
}

void LeuzeDistanceMeterWidget::startDepthMeter()
{
	timer.start(200);

	//connect(clocker, SIGNAL(clock()), this, SLOT(clocked()));
}


/*
void DepthImpulsUstyeWidget::includeParameter(int state)
{
	QCheckBox *chbox = (QCheckBox*)sender();
	if (!chbox) return;

	bool flag;
	if (state == Qt::Checked) flag = true;
	else if (state == Qt::Unchecked) flag = false;

	if (chbox == ui->chboxDepth) 
	{
		ui->lblDepth->setText("");
		ui->cboxDepth->setEnabled(flag);
		depth_active = flag;
	}
	else if (chbox == ui->chboxRate)
	{
		ui->lblRate->setText("");
		ui->cboxRate->setEnabled(flag);
		rate_active = flag;
	}
	else if (chbox == ui->chboxTension)
	{
		ui->lblTension->setText("");
		ui->cboxTension->setEnabled(flag);
		tension_active = flag;
	}
}
*/

void LeuzeDistanceMeterWidget::changeUnits(QString str)
{
	QComboBox *cbox = (QComboBox*)sender();
	if (!cbox) return;

	if (cbox == ui->cboxDistance)
	{		
		if (str == distance_units_list[0]) k_distance = 1000;		// [mm]
		else if (str == distance_units_list[1]) k_distance = 1;		// [m]
		else k_distance = 1;

		ui->ledDistance->setText(QString::number(k_distance*distance));		
	}
	else if (cbox == ui->cboxSetDistance)
	{		
		if (str == distance_units_list[0]) k_set_distance = 1000;		// [mm]
		else if (str == distance_units_list[1]) k_set_distance = 1;		// [m]
		else k_set_distance = 1;

		ui->ledSetDistance->setText(QString::number(k_set_distance*set_distance));		
	}
	else if (cbox == ui->cboxFrom)
	{
		if (str == distance_units_list[0]) k_from = 1000;			// [mm]
		else if (str == distance_units_list[1]) k_from = 1;			// [m]
		else k_from = 1;

		ui->dsboxFrom->setValue(k_from*from_pos);	
	}
	else if (cbox == ui->cboxTo)
	{
		if (str == distance_units_list[0]) k_to = 1000;				// [mm]
		else if (str == distance_units_list[1]) k_to = 1;			// [m]
		else k_to = 1;

		ui->dsboxTo->setValue(k_to*to_pos);	
	}
	else if (cbox == ui->cboxStep)
	{
		if (str == distance_units_list[0]) k_step = 1000;			// [mm]
		else if (str == distance_units_list[1]) k_step = 1;			// [m]
		else k_step = 1;

		ui->dsboxStep->setValue(k_step*step_pos);	
	}
	else if (cbox == ui->cboxZero)
	{
		if (str == distance_units_list[0]) k_zero = 1000;			// [mm]
		else if (str == distance_units_list[1]) k_zero = 1;			// [m]
		else k_zero = 1;

		ui->dsboxZero->setValue(k_zero*zero_pos);	
	}
}


void LeuzeDistanceMeterWidget::connectDepthMeter(bool flag)
{
	if (!flag)
	{
		if (COM_Port->COM_port != NULL) 
		{
			COM_Port->COM_port->close();			

			//delete COM_Port->COM_port;
			//COM_Port->COM_port = NULL;		
		}	

		if (depth_communicator != NULL)
		{
			depth_communicator->exit();
			depth_communicator->wait();
			delete depth_communicator;	
			depth_communicator = NULL;
		}

		timer.stop();
		is_connected = false;
		emit connected(is_connected);

		/*
		ui->pbtConnect->setText(tr("Connect to Depth Meter"));
		ui->pbtConnect->setIcon(QIcon(":/images/add.png"));
		ui->lblDepth->setText("");
		ui->lblRate->setText("");
		ui->lblTension->setText("");
		*/
	}	
	else
	{		
		if (COM_Port->COM_port != NULL)
		{
			QString COMPort_Name = COM_Port->COM_port->portName();
			if (COMPort_Name.isEmpty()) 
			{
				int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Depth Meter!"), QMessageBox::Ok, QMessageBox::Ok);
				return;
			}

			COM_Port->COM_port->close();
			COM_Port->COM_port->setPortName(COMPort_Name);

			if (depth_communicator != NULL)
			{
				depth_communicator->exit();
				depth_communicator->wait();
				delete depth_communicator;
				depth_communicator = NULL;
			}			
			bool res = COM_Port->COM_port->open(QextSerialPort::ReadWrite);
			if (res) 	
			{				
				depth_communicator = new DepthCommunicator(COM_Port->COM_port, clocker);
				setDepthCommunicatorConnections();
				depth_communicator->start(QThread::NormalPriority);

				is_connected = true;
				emit connected(true);

				timer.start(200);

				/*
				ui->pbtConnect->setText(tr("Disconnect from Depth Meter"));
				ui->pbtConnect->setIcon(QIcon(":/images/remove.png"));
				*/
			}
			else
			{
				is_connected = false;
				emit connected(false);

				int ret = QMessageBox::warning(this, tr("Warning!"), tr("Cannot open COM-Port (%1)!").arg(COMPort_Name), QMessageBox::Ok, QMessageBox::Ok);
			}
		}
		else
		{
			int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Depth Meter!"), QMessageBox::Ok, QMessageBox::Ok);
		}			
	}	
}


void LeuzeDistanceMeterWidget::getMeasuredData(uint32_t _uid, uint8_t _type, double val)
{
	if (!is_connected) is_connected = true;
	if (device_is_searching) 
	{
		timer.start();
		device_is_searching = false;
	}

	switch (_type)
	{
	case DEPTH_DATA:	distance = val;	break;
	case DEVICE_SEARCH:	device_is_searching = false; break;
	default: break;
	}

	showData(_type, val);
}

void LeuzeDistanceMeterWidget::measureTimedOut(uint32_t _uid, uint8_t _type)
{	
	switch (_type)
	{
	case DEPTH_DATA:	ui->ledDistance->setText("");	break;
	case DEVICE_SEARCH:	
		{
			ui->ledDistance->setText("");
			connectionWidget->setReport(ConnectionState::State_No); 
			break;
		}
	default: break;
	}

	connectionWidget->setReport(ConnectionState::State_No);
}

void LeuzeDistanceMeterWidget::showData(uint8_t type, double val)
{	
	QString str = "<font color=darkGreen><b>%1</font>";

	switch (type)
	{
	case DEPTH_DATA:	
		{
			double value = val * k_distance;  
			QString str_value = QString::number(value);
			str = QString(str).arg(str_value);
			ui->ledDistance->setText(str); 
			break;
		}	
	case DEVICE_SEARCH:	
		{
			ui->ledDistance->setText(""); 			
			break;
		}
	default: break;
	}

	connectionWidget->setReport(ConnectionState::State_OK);
}

void LeuzeDistanceMeterWidget::onTime()
{	
	if (is_connected) emit to_measure(++uid, DEPTH_DATA);		
}
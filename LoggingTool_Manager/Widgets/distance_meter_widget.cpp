#include <QMessageBox>

#include "distance_meter_widget.h"


static uint32_t uid = 0;

LeuzeDistanceMeterWidget::LeuzeDistanceMeterWidget(Clocker *_clocker, COM_PORT *com_port, COM_PORT *stepmotor_com_port, QWidget *parent /* = 0 */)  : ui(new Ui::LeuzeDistanceMeter)
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
	ui->dsboxSetPosition->setStyleSheet("QDoubleSpinBox { color: darkBlue }");	

	ui->dsboxFrom->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxTo->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxStep->setStyleSheet("QDoubleSpinBox { color: darkGreen }");
	ui->dsboxZero->setStyleSheet("QDoubleSpinBox { color: darkGreen }");

	ui->pbtBack->setIcon(QIcon(":/images/play_back.png"));
	ui->pbtBegin->setIcon(QIcon(":/images/play_begin.png"));
	ui->pbtForward->setIcon(QIcon(":/images/play.png"));
	ui->pbtEnd->setIcon(QIcon(":/images/play_end.png"));
	ui->pbtSet->setIcon(QIcon(":/images/apply.png"));
	
	ui->pbtConnect->setText(tr("Connect to Rock Movement System"));
	ui->pbtConnect->setIcon(QIcon(":/images/add.png"));
	
	connectionWidget = new ImpulsConnectionWidget(this);
	connectionWidget->setReport(ConnectionState::State_No);

	distance = 0.001;		// always in meters
	k_distance = 100;		// for default units ([cm])
	distance_ok = true;

	set_distance = 1;		// [m]
	k_set_distance = 100;	// [cm]
	direction_coef = -1;	
	pos_is_set = false;

	from_pos = 0.50;		// [m]
	to_pos = 1.50;			// [m]
	step_pos = 0.01;		// [m]
	zero_pos = 0;			// [m]
	k_from = 100;			// for [cm]
	k_to = 100;				// for [cm]
	k_step = 100;			// for [cm]
	k_zero = 100;			// for [cm]
	
	distance_units_list << "mm" << "cm" << "m";	
		
	ui->cboxDistance->addItems(distance_units_list);
	ui->cboxDistance->setCurrentIndex(1);
	ui->cboxDistance->setEnabled(true);
	ui->cboxSetDistance->addItems(distance_units_list);
	ui->cboxSetDistance->setCurrentIndex(1);
	ui->cboxSetDistance->setEnabled(true);
	ui->cboxFrom->addItems(distance_units_list);
	ui->cboxFrom->setCurrentIndex(1);
	ui->cboxFrom->setEnabled(true);
	ui->cboxTo->addItems(distance_units_list);
	ui->cboxTo->setCurrentIndex(1);
	ui->cboxTo->setEnabled(true);
	ui->cboxStep->addItems(distance_units_list);
	ui->cboxStep->setCurrentIndex(1);
	ui->cboxStep->setEnabled(true);
	ui->cboxZero->addItems(distance_units_list);
	ui->cboxZero->setCurrentIndex(1);
	ui->cboxZero->setEnabled(true);

	ui->ledDistance->setText("");	
		
	ui->dsboxSetPosition->setMinimum(15);
	ui->dsboxSetPosition->setMaximum(200);
	ui->dsboxSetPosition->setSingleStep(1);
	ui->dsboxSetPosition->setValue(set_distance*k_set_distance);

	ui->dsboxFrom->setMinimum(15);
	ui->dsboxFrom->setMaximum(200);
	ui->dsboxFrom->setSingleStep(1);
	ui->dsboxFrom->setValue(from_pos*k_from);

	ui->dsboxTo->setMinimum(15);
	ui->dsboxTo->setMaximum(200);
	ui->dsboxTo->setSingleStep(1);
	ui->dsboxTo->setValue(to_pos*k_to);

	ui->dsboxStep->setMinimum(0.1);
	ui->dsboxStep->setMaximum(200);
	ui->dsboxStep->setSingleStep(0.1);
	ui->dsboxStep->setValue(step_pos*k_step);

	is_connected = false;
	ui->framePosControl->setEnabled(is_connected);
	device_is_searching = false;

	clocker = _clocker;
	COM_Port = com_port;
	stepmotor_COM_Port = stepmotor_com_port;
	leuze_communicator = NULL;
	stepmotor_communicator = NULL;

	upper_bound = 1.950;	// m
	lower_bound = 0.150;	// m	
	
	timer.start(250);	
	stepmotor_timer.start(250);
		
	setConnection();
}

LeuzeDistanceMeterWidget::~LeuzeDistanceMeterWidget()
{
	delete ui;
	delete connectionWidget;
	if (leuze_communicator != NULL) delete leuze_communicator;
	if (stepmotor_communicator != NULL) delete stepmotor_communicator;
}


void LeuzeDistanceMeterWidget::setConnection()
{
	connect(ui->cboxDistance, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxFrom, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxTo, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxStep, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxZero, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->cboxSetDistance, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeUnits(QString)));
	connect(ui->dsboxTo, SIGNAL(valueChanged(double)), this, SLOT(setNewTo(double)));
	connect(ui->dsboxFrom, SIGNAL(valueChanged(double)), this, SLOT(setNewFrom(double)));
	connect(ui->dsboxStep, SIGNAL(valueChanged(double)), this, SLOT(setNewStep(double)));

	connect(ui->pbtBack, SIGNAL(toggled(bool)), this, SLOT(moveBack(bool)));
	connect(ui->pbtForward, SIGNAL(toggled(bool)), this, SLOT(moveForward(bool)));
	connect(ui->pbtSet, SIGNAL(toggled(bool)), this, SLOT(setPosition(bool)));
	
	connect(ui->pbtConnect, SIGNAL(toggled(bool)), this, SLOT(connectAllMeters(bool)));

	connect(&timer, SIGNAL(timeout()), this, SLOT(onTime()));
}


void LeuzeDistanceMeterWidget::setDepthCommunicatorConnections()
{
	connect(leuze_communicator, SIGNAL(measured_data(uint32_t, uint8_t, double)), this, SLOT(getMeasuredData(uint32_t, uint8_t, double)));
	connect(leuze_communicator, SIGNAL(data_timed_out(uint32_t, uint8_t)), this, SLOT(measureTimedOut(uint32_t, uint8_t)));
	connect(leuze_communicator, SIGNAL(error_msg(QString)), this, SLOT(showErrorMsg(QString)));
	connect(this, SIGNAL(to_measure(uint32_t, uint8_t)), leuze_communicator, SLOT(toMeasure(uint32_t, uint8_t)));
}

void LeuzeDistanceMeterWidget::setStepMotorCommunicatorConnections()
{
	
}

void LeuzeDistanceMeterWidget::moveBack(bool flag)
{	
	if (!flag)
	{
		stepmotor_communicator->toSend("\SP*DS*");		// Stop step motor		
	}
	else
	{
		ui->pbtForward->setChecked(false);
		ui->pbtSet->setChecked(false);

		if (distance > lower_bound && distance < upper_bound)
		{
			stepmotor_communicator->toSend("\EN*DL*SD1000*MV*");		// Move step motor left
		}
		else 
		{
			stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
			ui->pbtBack->setChecked(false);			
		}
	}
}

void LeuzeDistanceMeterWidget::moveForward(bool flag)
{
	if (!flag)
	{
		stepmotor_communicator->toSend("\SP*DS*");		// Stop step motor		
	}
	else
	{
		ui->pbtBack->setChecked(false);		
		ui->pbtSet->setChecked(false);

		if (distance > lower_bound && distance < upper_bound)
		{			
			stepmotor_communicator->toSend("\EN*DR*SD1000*MV*");		// Move step motor left
		}
		else 
		{
			stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
			ui->pbtBack->setChecked(false);
			ui->pbtForward->setChecked(false);
			ui->pbtSet->setChecked(false);
		}
	}
}

void LeuzeDistanceMeterWidget::setPosition(bool flag)
{
	if (!flag)
	{
		stepmotor_communicator->toSend("\SP*DS*");		// Stop step motor		
	}
	else
	{		
		ui->pbtBack->setChecked(false);
		ui->pbtForward->setChecked(false);
		
		set_distance = ui->dsboxSetPosition->value()/k_set_distance;
		if (set_distance > distance )
		{
			pos_is_set = true;
			direction_coef = 1;
			if (set_distance - distance < 0.011) stepmotor_communicator->toSend("\EN*DR*SD50*MV*");				// Move step motor right, very low speed			
			else if (set_distance - distance < 0.03) stepmotor_communicator->toSend("\EN*DR*SD200*MV*");		// Move step motor right, low speed			
			else stepmotor_communicator->toSend("\EN*DR*SD1000*MV*");											// Move step motor right, high speed
			
		}
		else if (set_distance < distance )
		{
			pos_is_set = true;
			direction_coef = -1;
			if (distance - set_distance < 0.011) stepmotor_communicator->toSend("\EN*DL*SD50*MV*");				// Move step motor left, very low speed
			else if (distance - set_distance < 0.03) stepmotor_communicator->toSend("\EN*DL*SD200*MV*");		// Move step motor left, low speed
			else stepmotor_communicator->toSend("\EN*DL*SD1000*MV*");											// Move step motor left, high speed
		}
		else 
		{
			stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
			ui->pbtBack->setChecked(false);
			ui->pbtForward->setChecked(false);
			ui->pbtSet->setChecked(false);
		}
	}
}

void LeuzeDistanceMeterWidget::stopDepthMeter()
{
	//ui->pbtConnect->setChecked(false);

	timer.stop();

	//disconnect(clocker, SIGNAL(clock()), this, SLOT(clocked()));
}

void LeuzeDistanceMeterWidget::startDepthMeter()
{
	timer.start(250);

	//connect(clocker, SIGNAL(clock()), this, SLOT(clocked()));
}


void LeuzeDistanceMeterWidget::setNewTo(double val)
{
	to_pos = val / k_to;

	emit set_from_to_step(from_pos, to_pos, step_pos);
}

void LeuzeDistanceMeterWidget::setNewFrom(double val)
{
	from_pos = val / k_from;

	emit set_from_to_step(from_pos, to_pos, step_pos);
}

void LeuzeDistanceMeterWidget::setNewStep(double val)
{
	step_pos = val / k_step;

	emit set_from_to_step(from_pos, to_pos, step_pos);
}

void LeuzeDistanceMeterWidget::changeUnits(QString str)
{
	QComboBox *cbox = (QComboBox*)sender();
	if (!cbox) return;

	if (cbox == ui->cboxDistance)
	{		
		if (str == distance_units_list[0]) k_distance = 1000;		// [mm]
		else if (str == distance_units_list[1]) k_distance = 100;	// [cm]
		else if (str == distance_units_list[2]) k_distance = 1;		// [m]
		else k_distance = 1;

		ui->ledDistance->setText(QString::number(k_distance*distance));		
	}
	else if (cbox == ui->cboxSetDistance)
	{				
		if (str == distance_units_list[0])							// [mm]
		{
			ui->dsboxSetPosition->setMinimum(150);
			ui->dsboxSetPosition->setMaximum(2000);
			ui->dsboxSetPosition->setSingleStep(1);
			k_set_distance = 1000;		
		}
		else if (str == distance_units_list[1])						// [cm]
		{
			ui->dsboxSetPosition->setMinimum(15);
			ui->dsboxSetPosition->setMaximum(200);
			ui->dsboxSetPosition->setSingleStep(1);
			k_set_distance = 100;		
		}
		else if (str == distance_units_list[2])						// [m]
		{
			ui->dsboxSetPosition->setMinimum(0.15);
			ui->dsboxSetPosition->setMaximum(2.00);
			ui->dsboxSetPosition->setSingleStep(0.01);
			k_set_distance = 1;		
		}
		else k_set_distance = 1;

		ui->dsboxSetPosition->setValue(k_set_distance*set_distance);		
	}
	else if (cbox == ui->cboxFrom)
	{
		disconnect(ui->dsboxFrom, SIGNAL(valueChanged(double)), this, SLOT(setNewFrom(double)));
		if (str == distance_units_list[0]) 
		{
			k_from = 1000;											// [mm]
			ui->dsboxFrom->setMinimum(150);
			ui->dsboxFrom->setMaximum(2000);
			ui->dsboxFrom->setSingleStep(1);
		}
		else if (str == distance_units_list[1]) 
		{
			k_from = 100;											// [cm]
			ui->dsboxFrom->setMinimum(15);
			ui->dsboxFrom->setMaximum(200);
			ui->dsboxFrom->setSingleStep(1);
		}
		else if (str == distance_units_list[2]) 
		{
			k_from = 1;												// [m]
			ui->dsboxFrom->setMinimum(0.150);
			ui->dsboxFrom->setMaximum(2.000);
			ui->dsboxFrom->setSingleStep(0.01);
		}
		else k_from = 1;

		ui->dsboxFrom->setValue(k_from*from_pos);	
		connect(ui->dsboxFrom, SIGNAL(valueChanged(double)), this, SLOT(setNewFrom(double)));
	}
	else if (cbox == ui->cboxTo)
	{
		disconnect(ui->dsboxTo, SIGNAL(valueChanged(double)), this, SLOT(setNewTo(double)));
		if (str == distance_units_list[0]) 
		{
			k_to = 1000;											// [mm]
			ui->dsboxTo->setMinimum(150);
			ui->dsboxTo->setMaximum(2000);
			ui->dsboxTo->setSingleStep(1);
		}
		else if (str == distance_units_list[1]) 
		{
			k_to = 100;												// [cm]
			ui->dsboxTo->setMinimum(15);
			ui->dsboxTo->setMaximum(200);
			ui->dsboxTo->setSingleStep(1);
		}
		else if (str == distance_units_list[2]) 
		{
			k_to = 1;												// [m]
			ui->dsboxTo->setMinimum(0.150);
			ui->dsboxTo->setMaximum(2.000);
			ui->dsboxTo->setSingleStep(0.01);
		}
		else k_to = 1;

		ui->dsboxTo->setValue(k_to*to_pos);	
		connect(ui->dsboxTo, SIGNAL(valueChanged(double)), this, SLOT(setNewTo(double)));
	}
	else if (cbox == ui->cboxStep)
	{
		disconnect(ui->dsboxStep, SIGNAL(valueChanged(double)), this, SLOT(setNewStep(double)));
		if (str == distance_units_list[0]) 
		{
			k_step = 1000;											// [mm]
			ui->dsboxStep->setMinimum(1);
			ui->dsboxStep->setMaximum(2000);
			ui->dsboxStep->setSingleStep(1);
		}
		else if (str == distance_units_list[1]) 
		{
			k_step = 100;											// [cm]
			ui->dsboxStep->setMinimum(0.1);
			ui->dsboxStep->setMaximum(200);
			ui->dsboxStep->setSingleStep(0.1);
		}
		else if (str == distance_units_list[2]) 
		{
			k_step = 1;												// [m]
			ui->dsboxStep->setMinimum(0.001);
			ui->dsboxStep->setMaximum(2.000);
			ui->dsboxStep->setSingleStep(0.001);
		}
		else k_step = 1;

		ui->dsboxStep->setValue(k_step*step_pos);	
		connect(ui->dsboxStep, SIGNAL(valueChanged(double)), this, SLOT(setNewStep(double)));
	}
	else if (cbox == ui->cboxZero)
	{
		if (str == distance_units_list[0]) 
		{
			k_zero = 1000;											// [mm]
			ui->dsboxZero->setMinimum(150);
			ui->dsboxZero->setMaximum(2000);
			ui->dsboxZero->setSingleStep(1);
		}
		else if (str == distance_units_list[1]) 
		{
			k_zero = 100;											// [cm]
			ui->dsboxZero->setMinimum(15);
			ui->dsboxZero->setMaximum(200);
			ui->dsboxZero->setSingleStep(1);
		}
		else if (str == distance_units_list[2]) 
		{
			k_zero = 1;												// [m]
			ui->dsboxZero->setMinimum(0.150);
			ui->dsboxZero->setMaximum(2.000);
			ui->dsboxZero->setSingleStep(0.01);
		}
		else k_zero = 1;

		ui->dsboxZero->setValue(k_zero*zero_pos);	
	}
}


void LeuzeDistanceMeterWidget::connectAllMeters(bool flag)
{
	if (!flag)
	{
		if (COM_Port->COM_port != NULL) 
		{
			COM_Port->COM_port->close();		
		}	
		if (stepmotor_COM_Port->COM_port != NULL) 
		{
			stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
			stepmotor_COM_Port->COM_port->close();		
		}

		if (leuze_communicator != NULL)
		{
			leuze_communicator->exit();
			leuze_communicator->wait();
			delete leuze_communicator;	
			leuze_communicator = NULL;
		}
		if (stepmotor_communicator != NULL)
		{
			stepmotor_communicator->exit();
			stepmotor_communicator->wait();
			delete stepmotor_communicator;	
			stepmotor_communicator = NULL;
		}

		timer.stop();
		is_connected = false;		
		emit connected(is_connected);
				
		ui->pbtConnect->setText(tr("Connect to Rock Movement System"));
		ui->pbtConnect->setIcon(QIcon(":/images/add.png"));
		ui->ledDistance->setText("");		
	}	
	else
	{		
		// Leuze destance meter initialization
		if (COM_Port->COM_port != NULL)
		{
			QString COMPort_Name = COM_Port->COM_port->portName();
			if (COMPort_Name.isEmpty()) 
			{
				int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Distance Meter!"), QMessageBox::Ok, QMessageBox::Ok);
				return;
			}

			COM_Port->COM_port->close();
			COM_Port->COM_port->setPortName(COMPort_Name);

			if (leuze_communicator != NULL)
			{
				leuze_communicator->exit();
				leuze_communicator->wait();
				delete leuze_communicator;
				leuze_communicator = NULL;
			}			
			bool res = COM_Port->COM_port->open(QextSerialPort::ReadWrite);
			if (res) 	
			{				
				leuze_communicator = new LeuzeCommunicator(COM_Port->COM_port, clocker);
				setDepthCommunicatorConnections();
				leuze_communicator->start(QThread::NormalPriority);

				is_connected = true;
				emit connected(true);

				timer.start(250);
								
				ui->pbtConnect->setText(tr("Disconnect from Rock Movement System"));
				ui->pbtConnect->setIcon(QIcon(":/images/remove.png"));				
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
			int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Distance Meter!"), QMessageBox::Ok, QMessageBox::Ok);
		}	

		// Step Motor controller initialization
		if (stepmotor_COM_Port->COM_port != NULL)
		{
			QString stepmotor_COMPort_Name = stepmotor_COM_Port->COM_port->portName();
			if (stepmotor_COMPort_Name.isEmpty()) 
			{
				int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Step Motor controller!"), QMessageBox::Ok, QMessageBox::Ok);
				return;
			}

			stepmotor_COM_Port->COM_port->close();
			stepmotor_COM_Port->COM_port->setPortName(stepmotor_COMPort_Name);

			if (stepmotor_communicator != NULL)
			{
				stepmotor_communicator->exit();
				stepmotor_communicator->wait();
				delete stepmotor_communicator;
				stepmotor_communicator = NULL;
			}			
			bool res = stepmotor_COM_Port->COM_port->open(QextSerialPort::ReadWrite);
			if (res) 	
			{				
				stepmotor_communicator = new StepMotorCommunicator(stepmotor_COM_Port->COM_port, clocker);
				setStepMotorCommunicatorConnections();
				stepmotor_communicator->start(QThread::NormalPriority);

				//is_connected = true;
				//emit connected(true);
								
				ui->pbtConnect->setText(tr("Disconnect from Rock Movement System"));
				ui->pbtConnect->setIcon(QIcon(":/images/remove.png"));				
			}
			else
			{
				is_connected = false;
				emit connected(false);

				int ret = QMessageBox::warning(this, tr("Warning!"), tr("Cannot open COM-Port (%1)!").arg(stepmotor_COMPort_Name), QMessageBox::Ok, QMessageBox::Ok);
			}
		}
		else
		{
			int ret = QMessageBox::warning(this, tr("Warning!"), tr("No available COM-Port was found to connect to Step Motor controller!"), QMessageBox::Ok, QMessageBox::Ok);
		}	
	}	

	ui->framePosControl->setEnabled(is_connected);
}

void LeuzeDistanceMeterWidget::showErrorMsg(QString msg)
{
	stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor

	QPalette palette; 
	palette.setColor(QPalette::Base,Qt::red);
	palette.setColor(QPalette::Text,Qt::darkGreen);
	ui->ledDistance->setPalette(palette);

	ui->ledDistance->setText(msg);
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
	case DEPTH_DATA:	distance = val/1000;	break;
	case DEVICE_SEARCH:	device_is_searching = false; break;
	default: break;
	}

	if (distance < lower_bound || distance > upper_bound)
	{
		distance_ok = false;
		stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
		ui->pbtBack->setChecked(false);
		ui->pbtForward->setChecked(false);
	}
	else distance_ok = true;

	if (fabs(distance - set_distance) < 0.011 && pos_is_set && direction_coef == -1 && pos_is_set)
	{		
		stepmotor_communicator->toSend("\EN*DL*SD50*MV*");			// Set motion slow 		
	}
	else if (fabs(distance - set_distance) < 0.03 && pos_is_set && direction_coef == -1 && pos_is_set)
	{		
		stepmotor_communicator->toSend("\EN*DL*SD200*MV*");			// Set motion slow 		
	}
	else if (fabs(set_distance - distance) < 0.011 && pos_is_set && direction_coef == 1 && pos_is_set)
	{		
		stepmotor_communicator->toSend("\EN*DR*SD50*MV*");			// Set motion slow 		
	}
	else if (fabs(set_distance - distance) < 0.03 && pos_is_set && direction_coef == 1 && pos_is_set)
	{		
		stepmotor_communicator->toSend("\EN*DR*SD200*MV*");			// Set motion slow 		
	}
	
	if (distance <= set_distance && pos_is_set && direction_coef == -1)
	{
		distance_ok = true;
		pos_is_set = false;
		stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
		ui->pbtBack->setChecked(false);
		ui->pbtForward->setChecked(false);
		ui->pbtSet->setChecked(false);
	}
	else if (distance >= set_distance && pos_is_set && direction_coef == 1)
	{
		distance_ok = true;
		pos_is_set = false;
		stepmotor_communicator->toSend("\SP*DS*");	// Stop step motor
		ui->pbtBack->setChecked(false);
		ui->pbtForward->setChecked(false);
		ui->pbtSet->setChecked(false);
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
	switch (type)
	{
	case DEPTH_DATA:	
		{
			QPalette palette;					
			if (distance_ok)
			{
				palette.setColor(QPalette::Base,Qt::white);
				palette.setColor(QPalette::Text,Qt::darkGreen);
				 //ui->ledDistance->setStyleSheet("QLineEdit {background-color: white;}");
			}
			else
			{
				palette.setColor(QPalette::Base,Qt::red);
				palette.setColor(QPalette::Text,Qt::darkGreen);
				//ui->ledDistance->setStyleSheet("QLineEdit {background-color: red;}");
			}
			ui->ledDistance->setPalette(palette);

			double value = distance * k_distance;  			
			QString str_value = QString::number(value,'g',4);				
			ui->ledDistance->setText(str_value);
			
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
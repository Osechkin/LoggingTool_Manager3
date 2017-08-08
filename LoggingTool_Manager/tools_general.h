#ifndef TOOLS_GENERAL_H
#define TOOLS_GENERAL_H

#include "qextserialport.h"


#define KMRK					(4)				// ID ������� ����
#define NMKT					(3)				// ID ������� ����

struct ToolInfo
{	
	ToolInfo(int _id = 0, QString _type = "", QString _file_name = "")
	{
		id = _id;
		type = _type;
		file_name = _file_name;
	}

	int id;
	QString type;
	QString file_name;
};


#define NMR_SAMPLE_FREQ			(4*250000)		// ������� ������������� ��� ������� ���
#define NMR_SAMPLE_FREQ_HALF	(NMR_SAMPLE_FREQ/2)	// �������� ������� ������������� ��� ������� ���


#define DU_DATA_LEN				45				// ����� ������ ���������� (� ������), ���������� �� ���������� DU

#define DIELECTR_MSG_LEN		20				// ����� ������ ������ ���������������� �������


struct COM_PORT
{
	PortSettings COM_Settings;
	QextSerialPort *COM_port;
	bool connect_state;				// ������/������
	bool auto_search;				// ���������� �� ������������ ��������� ���������� ������� ��� �� COM-������
};


struct Communication_Settings
{
	int packet_length;				// ����� �������
	int block_length;				// ����� ������
	int errs_count;					// ���������� �������������� ������ � ������ ������
	int packet_delay;				// �������� ����� �������� (��)
	bool antinoise_coding;			// ���������/�� ��������� ���������������� �����������
	bool packlen_autoadjust;		// ����������� ����� ������� (���./����.)
	bool noise_control;				// ���������� ���������� ���������� ��������� ����������������� ����������� � ����������� �� ��������� ���������� (���./����.)
	bool interleaving;				// ����������/�� ���������� ��������� "������������" (������������� ������ � �������) 
};


#endif // IO_GENERAL_H

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTerminal.h"
#include "Console.h"
#include <QSerialPort>
#include <QFileDialog>
#include <qtimer.h>
#include <QSerialPortInfo>
#include <fstream>
#include <queue>
#include <QTimer>

#include "CRC.h"
#include <QMessageBox>

class QtTerminal : public QMainWindow
{
	Q_OBJECT

	
signals:
	void fileSelected(QString fileName);
	void sendAck(const QByteArray& ack);

public:
	QtTerminal(QWidget *parent = Q_NULLPTR);
	virtual ~QtTerminal();

	void initActionConnections();

public slots:
	void openFileDialog();
	void readFile();
	void writeFile(QString fileName);
	void initSerialPort(QAction* triggeredPortName);
	void addAvailablePorts();
	unsigned processFile(std::ifstream& file);
	QByteArray packetizeFile(std::queue<char> data);
	bool checkCRC(QByteArray receivedFrame);
	QByteArray parseDataFrame(QByteArray receivedFrame);
	char parseControlFrame(QByteArray receivedFrame);
	void packetReceived(std::string packet);
	void ackReceived(const QByteArray& ack);
	void handleError(QSerialPort::SerialPortError error);
	void handleTimeout();
	QByteArray createAckFrame();
	QByteArray createEnqFrame();

	void handleBytesWritten(qint64 bytes);

	void genericTimeout();
	void randomTimeout();

private:
	unsigned state; //1 = idle, 2 = receive, 3 = transmit
	Ui::QtTerminalClass ui;
	Console console;
	QSerialPort port;

	std::vector<QByteArray> packets;
	QTimer timer; // for timeouts
};

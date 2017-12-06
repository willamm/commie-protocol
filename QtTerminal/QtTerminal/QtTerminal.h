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
#include "CRC.h"
#include <QMessageBox>

class QtTerminal : public QMainWindow
{
	Q_OBJECT

signals:
	void fileSelected(QString fileName);
	void packetSent(std::string packet);
	void ackSent(std::string ack);

public:
	QtTerminal(QWidget *parent = Q_NULLPTR);

	void initActionConnections();

public slots:
	void openFileDialog();
	void readFile();
	void writeFile(QString fileName);
	void initSerialPort(QAction* triggeredPortName);
	void addAvailablePorts();
	unsigned processFile(std::ifstream& file);
	QByteArray packetizeFile(std::queue<char>* data);
	bool checkCRC(QByteArray receivedFrame);
	QByteArray parseFrame(QByteArray receivedFrame);
	void packetReceived(std::string packet);
	void ackReceived(std::string ack);
	void handleError(QSerialPort::SerialPortError error);
	QByteArray createAckFrame();
	QByteArray createEnqFrame();

	void handleBytesWritten(qint64 bytes);

	void genericTimeout();
	void randomTimeout();

private:
	Ui::QtTerminalClass ui;
	Console console;
	QSerialPort port;
};

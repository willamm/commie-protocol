#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTerminal.h"
#include "Console.h"
#include <QSerialPort>
#include <QFileDialog>
#include <QSerialPortInfo>
#include <fstream>
#include <queue>
#include "CRC.h"

class QtTerminal : public QMainWindow
{
	Q_OBJECT

signals:
	void fileSelected();
public:
	QtTerminal(QWidget *parent = Q_NULLPTR);

	void initActionConnections();

public slots:
	void openFileDialog();
	void readFile();
	void initSerialPort(QAction* triggeredPortName);
	void addAvailablePorts();
	unsigned processFile(std::ifstream& file);
	QByteArray packetizeFile(std::queue<char>* data);
	bool checkCRC(QByteArray receivedFrame);
	QByteArray parseFrame(QByteArray receivedFrame);

private:
	Ui::QtTerminalClass ui;
	Console console;
	QSerialPort port;
	QString sendFileName;
};

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTerminal.h"
#include "Console.h"
#include <QSerialPort>
#include <QFileDialog>
#include <QSerialPortInfo>
#include <fstream>

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

private:
	Ui::QtTerminalClass ui;
	Console console;
	QSerialPort port;
	QString sendFileName;
};

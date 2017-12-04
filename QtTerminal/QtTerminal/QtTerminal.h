#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTerminal.h"
#include "Console.h"
#include <QSerialPort>
#include <QFileDialog>

class QtTerminal : public QMainWindow
{
	Q_OBJECT

public:
	QtTerminal(QWidget *parent = Q_NULLPTR);

	void initActionConnections();
public slots:
	void openFileDialog();

private:
	Ui::QtTerminalClass ui;
	Console console;
	QSerialPort port;
	QString sendFileName;
};

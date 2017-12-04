#include "QtTerminal.h"

QtTerminal::QtTerminal(QWidget *parent)
	: QMainWindow(parent)
	, console(this)
	, port(this)
{
	ui.setupUi(this);
	setCentralWidget(&console);
	initActionConnections();
	addAvailablePorts();
}

void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::readFile);
}

void QtTerminal::initSerialPort(QAction* triggeredPortName)
{

	QString portName = triggeredPortName->text();
	if (!portName.isEmpty())
	{
		port.setPortName(portName);
	}
	port.setBaudRate(QSerialPort::Baud9600);
	port.setDataBits(QSerialPort::Data8);
	port.setParity(QSerialPort::NoParity);
	port.setStopBits(QSerialPort::OneStop);
}

void QtTerminal::addAvailablePorts()
{
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QActionGroup* portActions = new QActionGroup(this);
	for (const auto& port : ports)
	{
		QAction* temp = new QAction(port.portName(), portActions);
		temp->setCheckable(true);
		ui.menuSelect_Port->addAction(temp);
	}
	connect(portActions, &QActionGroup::triggered, this, &QtTerminal::initSerialPort);
}

void QtTerminal::openFileDialog()
{
	sendFileName = QFileDialog::getOpenFileName(this, "Send File");
	emit fileSelected();
}

void QtTerminal::readFile()
{
	if (!sendFileName.isEmpty())
	{
		std::ifstream file(sendFileName.toStdString());
		console.putData(sendFileName.toLocal8Bit());
	}
}
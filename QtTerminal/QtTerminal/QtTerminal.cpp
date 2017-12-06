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
	ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(false);
	ui.actionSend_File->setEnabled(false);
}

void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::writeFile);
	connect(this, &QtTerminal::packetSent, this, &QtTerminal::packetReceived);
	connect(&port, &QSerialPort::errorOccurred, this, &QtTerminal::handleError);
	connect(&port, &QSerialPort::readyRead, this, &QtTerminal::readFile);
	//connect(this, QtTerminal::ackSent, this, QtTerminal::ackReceived);
	connect(ui.actionConnect, &QAction::triggered, this, [this]()
	{
		if (port.open(QIODevice::ReadWrite))
		{
			ui.statusBar->showMessage("Connected", 1000);
			ui.actionConnect->setEnabled(false);
			ui.actionDisconnect->setEnabled(true);
			ui.actionSend_File->setEnabled(true);
		}
	});
	connect(ui.actionDisconnect, &QAction::triggered, this, [this]()
	{
		if (port.isOpen())
		{
			port.close();
			ui.actionConnect->setEnabled(true);
			ui.actionDisconnect->setEnabled(false);
			ui.actionSend_File->setEnabled(false);
		}
	});
}

void QtTerminal::initSerialPort(QAction* triggeredPortName)
{

	QString portName = triggeredPortName->text();
	if (!portName.isEmpty())
	{
		port.setPortName(portName);
		port.setBaudRate(QSerialPort::Baud9600);
		port.setDataBits(QSerialPort::Data8);
		port.setParity(QSerialPort::NoParity);
		port.setStopBits(QSerialPort::OneStop);
		ui.actionConnect->setEnabled(true);
		ui.actionDisconnect->setEnabled(false);
		ui.actionSelect_Port->setEnabled(false);
	}
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

void QtTerminal::packetReceived(std::string packet)
{
}

void QtTerminal::ackReceived(std::string ack)
{
}

void QtTerminal::handleError(QSerialPort::SerialPortError error)
{
}

void QtTerminal::openFileDialog()
{
	QString sendFileName = QFileDialog::getOpenFileName(this, "Send File");
	emit fileSelected(sendFileName);
}

void QtTerminal::readFile()
{
	char buf[512];
	if (port.canReadLine())
	{
		port.read(buf, 512);
		console.putData(buf);
	}
}

void QtTerminal::writeFile(QString fileName)
{
	port.write(fileName.toLocal8Bit());
}
		

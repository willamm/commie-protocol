#include "QtTerminal.h"

//Constructs the QtTerminal window
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
}

//Initial actions executed during QtTerminal instantiation
void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::readFile);
	connect(ui.actionConnect, &QAction::triggered, this, [this]()
	{
		if (port.open(QIODevice::ReadWrite))
		{
			ui.statusBar->showMessage("Connected", 1000);
			ui.actionConnect->setEnabled(false);
			ui.actionDisconnect->setEnabled(true);
		}
	});
}

//This sets up the serial port that you connect to
void QtTerminal::initSerialPort(QAction* triggeredPortName)
{
	//Store the port name from QAction
	QString portName = triggeredPortName->text();

	//If portName is not empty, set the port
	if (!portName.isEmpty())
	{
		port.setPortName(portName);
		port.setBaudRate(QSerialPort::Baud9600);
		port.setDataBits(QSerialPort::Data8);
		port.setParity(QSerialPort::NoParity);
		port.setStopBits(QSerialPort::OneStop);
		ui.actionConnect->setEnabled(true);
	}
}

//Connect to a port
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

//Creates the open file dialog
void QtTerminal::openFileDialog()
{
	sendFileName = QFileDialog::getOpenFileName(this, "Send File");
	emit fileSelected();
}

//Reads the file that the user selects in the terminal
void QtTerminal::readFile()
{
	//If a file is selected
	if (!sendFileName.isEmpty())
	{
		//Open a stream to that file
		std::ifstream file(sendFileName.toStdString());
		//If the file stream opens successfully
		if (file.is_open())
		{
			//This iterates through the file from beginning to end until the stream cuts off, and places the content in a string
			std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			//Print it out to the Qt console
			console.putData(QString::fromStdString(content).toLocal8Bit());

			//Uncomment this to use the process file
			//processFile(file);
		}
	}
}

//Processes the file by packetizing it in 512 byte chunks.
//
//Returns the number of bytes processed.
//
//Author: Angus Lam
unsigned QtTerminal::processFile(std::ifstream& file)
{
	//The file iterator
	std::istreambuf_iterator<char> file_iterator(file);
	//The queue meant to hold data from the file
	std::queue<char>* data = new std::queue<char>();
	//Keeps track of how many bytes processed
	unsigned count = 0;

	//While it is not EOF
	while (file_iterator != std::istreambuf_iterator<char>())
	{
		//Push each character into the queue
		data->push(*file_iterator);

		//Splits it into 512 bytes
		if ((count % 512) == 0)
		{
			//Pass in the 512 bytes of data in the queue and packetizes it
			packetizeFile(data);
		}

		//increment count and move the iterator one byte over
		count++;
		file_iterator++;
	}

	//Packetize the remaining data
	packetizeFile(data);

	//Return the # of processed bytes
	return count;
}

//Packetizes data.
//
//Returns the dataframe as a QByteArray.
//
//Author: Angus Lam
QByteArray QtTerminal::packetizeFile(std::queue<char>* data)
{
	//This will be the data frame
	QByteArray dataFrame;

	//Append the SYN character
	dataFrame.append(0x16);
	//Append the STX character
	dataFrame.append(0x02);

	//While there are still 512 bytes in the queue
	while (!data->empty())
	{
		//Append to the byte array
		dataFrame.append(data->front());
		//Gotta pop afterwards because c++ containers are weird
		data->pop();
	}

	//Stuffs the byte array with nulls until it is 514 bytes
	while (dataFrame.size() < 514) {
		dataFrame.append('\0');
	}

	//Append the CRC to the end of the array
	dataFrame.append(CRC::Calculate(dataFrame, sizeof(dataFrame), CRC::CRC_32()));

	//Return the frame
	return dataFrame;
}

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
	ui.actionDisconnect->setEnabled(false);
	ui.actionSend_File->setEnabled(false);
}

//Initial actions executed during QtTerminal instantiation
void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::writeFile);
	connect(this, &QtTerminal::packetSent, this, &QtTerminal::packetReceived);
	connect(&port, &QSerialPort::errorOccurred, this, &QtTerminal::handleError);
	connect(&port, &QSerialPort::readyRead, this, &QtTerminal::readFile);
	connect(&port, &QSerialPort::bytesWritten, this, &QtTerminal::handleBytesWritten);
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
		ui.actionDisconnect->setEnabled(false);
		ui.actionSelect_Port->setEnabled(false);
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
void QtTerminal::packetReceived(std::string packet)
{
}

void QtTerminal::ackReceived(std::string ack)
{
}

void QtTerminal::handleError(QSerialPort::SerialPortError error)
{
}

void QtTerminal::handleBytesWritten(qint64 bytes)
{
}

void QtTerminal::bidForLine() 
{
	char ENQ = 0x25;
	QString toSend;
	toSend = ENQ;
	console.putData("Bidding for line...");
	port.write("0x25");
}

// Creates a timeout, lasting (2000ms), then calls update()
void QtTerminal::genericTimeout()
{
	QTimer *genericTimeout = new QTimer(this);
	genericTimeout->setSingleShot(true);
	connect(genericTimeout, SIGNAL(timeout()), this, SLOT(update()));
	genericTimeout->start(2000);
}

// Creates a timeout, lasting (2000ms + (100ms * rand(1...10))), then calls update()
void QtTerminal::randomTimeout()
{
	QTimer *randomTimeout = new QTimer(this);
	randomTimeout->setSingleShot(true);
	connect(randomTimeout, SIGNAL(timeout()), this, SLOT(update()));
	randomTimeout->start(2000 + (100 * ((qrand() % 10) + 1)));
}

void QtTerminal::openFileDialog()
{
	QString sendFileName = QFileDialog::getOpenFileName(this, "Send File");
	emit fileSelected(sendFileName);
}

//Reads the file that the user selects in the terminal
void QtTerminal::readFile()
{
	//This is from will's commit
	char buf[518];
	port.read(buf, 518);
	console.putData(buf);
	//// should send an ACK here
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
	while (dataFrame.size() < 514)
	{
		dataFrame.append('\0');
	}

	//Create the crc
	//dataFrame.data() returns a char* to the beginning of the array, increment by 2 to create crc from payload
        quint32 crc = CRC::Calculate(dataFrame.right(512), sizeof(dataFrame.right(512)), CRC::CRC_32());
	//Append the crc to the end of the array, bitshifting a byte at a time
	dataFrame.append(quint8(crc >> 24));
	dataFrame.append(quint8(crc >> 16));
	dataFrame.append(quint8(crc >> 8));
	dataFrame.append(quint8(crc));

	//Return the frame
	return dataFrame;
}

//This parses the frame
//Returns nullptr if syn or stx bit is not present
//Returns the payload if successful
QByteArray QtTerminal::parseFrame(QByteArray receivedFrame)
{
	if (receivedFrame.at(0) != 0x02 || receivedFrame.at(1) != 0x16)
	{
		return nullptr;
	}

	if (checkCRC(receivedFrame)) {
            return receivedFrame.mid(2,512);
	}

	return nullptr;
}

//Checks the crc by processing the received frame's payload
//We are checking crc by recalculating at the received end. Just like a checksum
bool QtTerminal::checkCRC(QByteArray receivedFrame)
{
	//Store the payload as the array from position 2, and 512 bytes after that
        QByteArray payload = receivedFrame.mid(2,512);
        //Store just the 4 rightmost bits
        QByteArray receivedCrc = receivedFrame.right(4);
        //Stores the calculated crc
        QByteArray crc;

        //Calculate the crc
        quint32 crc_int = CRC::Calculate(payload, sizeof(payload), CRC::CRC_32());
        //Append the crc to the end of the array, bitshifting a byte at a time
        crc.append(quint8(crc_int >> 24));
        crc.append(quint8(crc_int >> 16));
        crc.append(quint8(crc_int >> 8));
        crc.append(quint8(crc_int));

	if (crc == receivedCrc) {
		return 1;
	}

	return 0;
}

void QtTerminal::writeFile(QString fileName)
{
	//Send 10 packets
	port.write(fileName.toLocal8Bit());
	console.putData("Packet sent");
	//wait for ACK
}

//Creates the ack frame and returns it
QByteArray QtTerminal::createAckFrame()
{
	QByteArray ackFrame;

	//Append SYN
	ackFrame.append(0x16);
	//Append ACK
	ackFrame.append(0x06);

	return ackFrame;
}

//Creates the enq frame and returns it
QByteArray QtTerminal::createEnqFrame()
{
	QByteArray enqFrame;

	//Append SYN
	enqFrame.append(0x16);
	//Append ENQ
	enqFrame.append(0x06);

	return enqFrame;
}
#include "QtTerminal.h"


static constexpr int PACKET_SIZE = 518;
//Constructs the QtTerminal window
QtTerminal::QtTerminal(QWidget *parent)
	: QMainWindow(parent)
	, console(this)
	, port(this)
{
	timer.setSingleShot(true);
	ui.setupUi(this);
	setCentralWidget(&console);
	initActionConnections();
	addAvailablePorts();
	ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(false);
	ui.actionSend_File->setEnabled(false);
	connect(&timer, &QTimer::timeout, this, &QtTerminal::handleTimeout);
	state = 1; //set to idle
}

QtTerminal::~QtTerminal()
{
}

//Initial actions executed during QtTerminal instantiation
void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::writeFile);
	connect(&port, &QSerialPort::errorOccurred, this, &QtTerminal::handleError);
	connect(&port, &QSerialPort::readyRead, this, &QtTerminal::readFile);
	connect(&port, &QSerialPort::bytesWritten, this, &QtTerminal::handleBytesWritten);
	connect(this, &QtTerminal::sendAck, this, &QtTerminal::ackReceived);
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

void QtTerminal::packetReceived(std::string packet)
{
}

void QtTerminal::ackReceived(const QByteArray& ack)
{

	port.write(ack);
	port.flush();
}

void QtTerminal::handleError(QSerialPort::SerialPortError error)
{
	if (error != QSerialPort::NoError)
	{
		QMessageBox msgBox;
		msgBox.setText("Error sending the file");
		msgBox.exec();		
		QCoreApplication::exit(1);
	}
}

void QtTerminal::handleBytesWritten(qint64 bytes)
{
	// When this event is fired, send ACK

}

void QtTerminal::handleTimeout()
{
	console.putData("Time out");
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
	/*char readBuffer[PACKET_SIZE];
	port.read(readBuffer, PACKET_SIZE);
	console.putData(readBuffer);
	emit sendAck(createAckFrame());
	disconnect(this, &QtTerminal::sendAck, this, &QtTerminal::ackReceived);*/


	//Angus's block of code
	switch (state)
	{
		case 1: //idle state
		{
			//Read in 2 bytes, the size of the control frame
			QByteArray controlFrame = port.read(2);
			//Parse the control frame, and receive the control character
			char controlChar = parseControlFrame(controlFrame);
			//Can use this to check the frame
			console.putData(controlFrame);

			if (controlChar == 0x05) //If you receive an ENQ
			{
				//send an ack
				//Set to a receive state
			}

			break;
		}
		case 2: //receive state
		{
			//Read in 518 bytes, the size of the data frame
			QByteArray dataFrame = port.read(518);
			//Parse the control frame, and receive the payload
			QByteArray data = parseDataFrame(dataFrame);
			//Print the payload in console
			console.putData(data);

			break;
		}
		case 3: //transmit state
		{
			//Read in 2 bytes, the size of the control frame
			QByteArray controlFrame = port.read(2);
			//Parse the control frame, and receive the control character
			char controlChar = parseControlFrame(controlFrame);
			//Can use this to check the frame
			console.putData(controlFrame);

			if (controlChar == 0x06) //If you receive an ACK
			{
				//send a packet
			}

			break;
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
	std::queue<char> data;
	//Keeps track of how many bytes processed
	unsigned count = 1;

	//While it is not EOF
	while (file_iterator != std::istreambuf_iterator<char>())
	{
		//Push each character into the queue
		data.push(*file_iterator);

		//Splits it into 512 bytes
		if ((count % 512) == 0)
		{
			//Pass in the 512 bytes of data in the queue and packetizes it
			packets.push_back(packetizeFile(data));
		}

		//increment count and move the iterator one byte over
		count++;
		++file_iterator;
	}

	//Packetize the remaining data
	packets.push_back(packetizeFile(data));

	//Return the # of processed bytes
	return count;
}

//Packetizes data.
//
//Returns the dataframe as a QByteArray.
//
//Author: Angus Lam
QByteArray QtTerminal::packetizeFile(std::queue<char> data)
{
	//This will be the data frame
	QByteArray dataFrame;

	//Append the SYN character
	dataFrame.append(0x16);
	//Append the STX character
	dataFrame.append(0x02);

	//While there are still 512 bytes in the queue
	while (!data.empty())
	{
		//Append to the byte array
		dataFrame.append(data.front());
		//Gotta pop afterwards because c++ containers are weird
		data.pop();
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
//Returns nullptr if syn or stx bit is not present, crc failed
//Returns the payload if successful
QByteArray QtTerminal::parseDataFrame(QByteArray receivedFrame)
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

//This parses a control frame
//Returns the control character if present
//Returns 0 if failed
char QtTerminal::parseControlFrame(QByteArray receivedFrame)
{
	if (receivedFrame.at(0) != 0x02)
	{
		return 0;
	}

	if (receivedFrame.at(0) == 0x05 || receivedFrame.at(0) == 0x06)
	{
		return receivedFrame.at(1);
	}

	return 0;
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

	std::ifstream fileStream(fileName.toStdString());
	processFile(fileStream);
	int packetCount = 0;
	for (const QByteArray& packet : packets)
	{
		port.write(packet);
		// wait to receive ACK
	}
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
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: QtTerminal.cpp - An application that will monitor a send a text file wirelessly using the custom data comm protocol
--
-- PROGRAM: Power to the Protocolteriat Terminal
--
-- FUNCTIONS:
-- QtTerminal::QtTerminal(QWidget *parent)
-- QtTerminal::~QtTerminal()
-- void QtTerminal::initActionConnections()
-- void QtTerminal::initSerialPort(QAction* triggeredPortName)
-- void QtTerminal::addAvailablePorts()
-- void QtTerminal::packetReceived(std::string packet)
-- void QtTerminal::ackReceived(const QByteArray& ack)
-- void QtTerminal::handleError(QSerialPort::SerialPortError error)
-- void QtTerminal::handleBytesWritten(qint64 bytes)
-- void QtTerminal::handleTimeout()
-- void QtTerminal::genericTimeout()
-- void QtTerminal::randomTimeout()
-- void QtTerminal::openFileDialog()
-- void QtTerminal::readFile()
-- void QtTerminal::writeFile()
-- unsigned QtTerminal::processFile(std::ifstream& file)
-- QByteArray QtTerminal::packetizeFile(std::queue<char> data)
-- QByteArray QtTerminal::parseDataFrame(QByteArray receivedFrame)
-- char QtTerminal::parseControlFrame(QByteArray receivedFrame)
-- bool QtTerminal::checkCRC(QByteArray receivedFrame)
-- QByteArray QtTerminal::createAckFrame()
-- QByteArray QtTerminal::createEnqFrame()
--
--
--
-- DATE December, 4, 2017
--
--
-- DESIGNER: Vafa Dehghan Saei, William Murphy, Mackenzie Craig, Angus Lam
--
-- PROGRAMMER: Vafa Dehghan Saei, William Murphy, Mackenzie Craig, Angus Lam
--
-- NOTES:
-- This program will conneect to provided wireless modem, then the user selects a file and the program processes said 
-- file and sends it through the modem using the protocol. The program also receives files and builds it up again. 
----------------------------------------------------------------------------------------------------------------------*/
#include "QtTerminal.h"
static constexpr int PACKET_SIZE = 518;



// 
// FUNCTION: QtTerminal
//
//
// DESIGNER: Mackenzie Craig, William Murphy
//
// PROGRAMMER: Vafa Dehghan Saei, Angus Lam
//
// INTERFACE: QtTerminal::QtTerminal(QWidget *parent)
//				QWidget *parent: The parent Qt ui class
//
// RETURNS: N/A Constructor
//
// NOTES: 
// The constructor of the terminal which creates the ui, and initializez menu files. 
//
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

// 
// FUNCTION: :~QtTerminal()
//
// DESIGNER: Vafa Dehghan Saei, Mackenzie Craig
//
// PROGRAMMER: Angus Lam, William Murphy
//
// INTERFACE: QtTerminal::~QtTerminal()
//
// RETURNS: Void
//
// NOTES:  The destructor, not called
//
QtTerminal::~QtTerminal()
{
}

// 
// FUNCTION: initActionConnections
//
// DESIGNER: Vafa Dehghan Saei, Angus Lam
//
// PROGRAMMER: Mackenzie Craig, William Murphy
//
// INTERFACE: void QtTerminal::initActionConnections()
//
// RETURNS: Void
//
// NOTES:  Initializez the file buttons to do the proper action when clicked on. 
//
void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
	connect(this, &QtTerminal::fileSelected, this, &QtTerminal::writeFile);
	connect(&port, &QSerialPort::errorOccurred, this, &QtTerminal::handleError);
	connect(&port, &QSerialPort::readyRead, this, &QtTerminal::readFile);
	//connect(&port, &QSerialPort::bytesWritten, this, &QtTerminal::writeFile);
	//connect(this, &QtTerminal::sendAck, this, &QtTerminal::ackReceived);
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

// 
// FUNCTION: QtTerminal::initSerialPort
//
// DESIGNER: Vafa Dehghan Saei, William Murphy
//
// PROGRAMMER: Angus Lam
//
// INTERFACE:  void QtTerminal::initSerialPort(QAction* triggeredPortName)
//
// RETURNS: void
//
// NOTES:  Initializez the serial port settings, such as baud rate. 
//
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

// 
// FUNCTION: addAvailablePorts
//
// DESIGNER: William Murphy
// 
// PROGRAMMER: Angus Lam
//
// INTERFACE:  void QtTerminal::addAvailablePorts()
//
// RETURNS: void
//
// NOTES: Searches the computer for availible com ports and adds it to the list for the user to select. 
//
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

// 
// FUNCTION: handleError
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig and Vafa Dehghan Saei
//
// PROGRAMMER: Angus Lam and Will Murphy
//
// INTERFACE: void QtTerminal::handleError(QSerialPort::SerialPortError error)
//
// RETURNS: void
//
// NOTES: 
// Outputs an error message based on a SerialPortError that is passed in.
void QtTerminal::handleError(QSerialPort::SerialPortError error)
{
	if (error != QSerialPort::NoError)
	{
		QMessageBox msgBox;
		msgBox.setText("Error sending the file\n");
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
	console.putData("Time out\n");
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
	fileName = QFileDialog::getOpenFileName(this, "Send File");
	emit fileSelected(fileName);
}

// 
// FUNCTION: readFile
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig and Vafa Dehghan Saei
//
// PROGRAMMER: Will Murphy and Angus Lam
//
// void QtTerminal::readFile()
//
// RETURNS: void
//
// NOTES: 
// Checks a switch statement and runs code blocks dependant on state.
// 1: idle 2: receive 3: bid for line 4: send
void QtTerminal::readFile()
{
	/*char readBuffer[PACKET_SIZE];
	port.read(readBuffer, PACKET_SIZE);
	console.putData(readBuffer);
	emit sendAck(createAckFrame());
	disconnect(this, &QtTerminal::sendAck, this, &QtTerminal::ackReceived);*/

	switch (state)
	{
		case 1: //idle state
		{
			//console.putData("Idle state\n");
			//Read in 2 bytes, the size of the control frame
			if (port.bytesAvailable() >= 2)
			{
				QByteArray controlFrame = port.read(1);

				//If you come across a SYN character
				if (controlFrame.at(0) == 0x16)
				{
					//Append the next byte
					controlFrame += port.read(1);

					//Parse the control frame, and receive the control character
					char controlChar = parseControlFrame(controlFrame);
					//check
					console.putData(controlFrame);

					if (controlChar == 0x05) //If you receive an ENQ
					{
						console.putData("received ENQ\n");
						//send an ack
						console.putData("sending ACK\n");
						nextTime = t.currentTime();
						nextTime.addMSecs(10000);
						console.putData("set timeout to 10 seconds\n");
						port.write(createAckFrame());
						port.clear(QSerialPort::Input);
						state = 2; //Set to a receive state
					}
				}
			}
			break;
		}
		case 2: //receive state
		{
			//console.putData("now in state receive\n");
			if (port.bytesAvailable() >= 518)
			{	
				QByteArray dataFrame = port.read(1);

				//If you come across a SYN character
				if (dataFrame.at(0) == 0x16)
				{
					dataFrame.append(port.read(1));

					if (dataFrame.at(1) == 0x02) {
						dataFrame.append(port.read(516));
						int a = dataFrame.size();
						//Parse the control frame, and receive the payload
						QByteArray data = parseDataFrame(dataFrame);
						//console.putData(dataFrame);
						//Print the payload in console
						if (data != nullptr) {
							nextTime = t.currentTime();
							nextTime.addMSecs(2000);
							console.putData("set timeout to 2 seconds\n");
							console.putData(data);
							port.write(createAckFrame());
						}
					}
				}

			}
			/*if (t.currentTime() >= nextTime)
			{
				state = 1;
				console.putData("Going back to idle from receive state\n");
			}*/
			break;
		}
		case 3: // bid for line state
		{
			console.putData("bidding for line\n");
			//Read in 2 bytes, the size of the control frame
			QByteArray controlFrame = port.read(1);

			//If you come across a SYN character
			if (controlFrame.at(0) == 0x16)
			{
				//Append the next byte
				controlFrame += port.read(1);
				//Parse the control frame, and receive the control character
				char controlChar = parseControlFrame(controlFrame);
				//Can use this to check the frame
				//console.putData(controlFrame);

				if (controlChar == 0x06) //If you receive an ACK
				{
					console.putData("ACK for bid received\n");
					nextTime = t.currentTime();
					nextTime.addMSecs(2000);
					console.putData("set timeout to 2 seconds\n");
					state = 4; // go to send state
					//Send the file, test
					std::ifstream fileStream(fileName.toStdString());
					processFile(fileStream);
					int packetCount = 0;
					for (const QByteArray& packet : packets)
					{
						console.putData(packet);
						port.write(packet);
					}
					packets.clear();
				}
			}

			QTime time = QTime::currentTime();

			if (time >= nextTime)
			{
				state = 1;
				console.putData("Going back to idle from bidding state\n");
			}
			
			break;
		}
		case 4: // send state
		{
			console.putData("now in send state\n");
			//Read in 2 bytes, the size of the control frame
			QByteArray controlFrame = port.read(2);
			//Parse the control frame, and receive the control character
			char controlChar = parseControlFrame(controlFrame);
			//Can use this to check the frame
			console.putData(controlFrame);

			if (controlChar == 0x06) //If you receive an ACK
			{
				console.putData("set timeout to 2 seconds\n");
				nextTime = t.currentTime();
				nextTime.addMSecs(2000);
				//send a packet
			}

			QTime time = QTime::currentTime();

			if (time >= nextTime)
			{
				state = 1;
				console.putData("Going back to idle from send state\n");
			}
			break;
		}
	}
}

// 
// FUNCTION: writeFile
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig and Vafa Dehghan Saei
//
// PROGRAMMER: Will Murphy and Angus Lam
//
// INTERFACE: void QtTerminal::writeFile()
//
// RETURNS: void
//
// NOTES: 
// Checks a switch statement and runs code blocks dependant on state.
// 1: idle 2: receive 3: bid for line 4: send
void QtTerminal::writeFile()
{
	// Before writing, 
	switch (state)
	{

	case 1: // idle
	{
		console.putData("Bidding for line...\n");
		console.putData("set timeout to 2 seconds\n");
		nextTime = t.currentTime();
		nextTime.addMSecs(2000);
		console.putData("starting timer\n");
		console.putData("Sending ENQ\n");
		port.write(createEnqFrame());
		state = 3; // trying to write while idling, good to bid for line.
		break;
	}
	case 2: // receive
	{
		port.write(createAckFrame());
		break;
	}
	case 3: // bid for line
	{
		console.putData("Bidding state\n");

		console.putData("sending ENQ");
		port.write(createEnqFrame());

		QTime time = QTime::currentTime();

		if (time >= nextTime)
		{
			state = 1;
			console.putData("Going back to idle from bidding state\n");
		}
		break;
	}
	case 4: // send
	{
		console.putData("ENTERED SEND STATE\n");
		//Send the file, test
		std::ifstream fileStream(fileName.toStdString());
		processFile(fileStream);
		int packetCount = 0;
		for (const QByteArray& packet : packets)
		{
 			console.putData(packet);
			port.write(packet);
		}
		break;
	}
	}
}

// 
// FUNCTION: processFile
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig and Vafa Dehghan Saei
//
// PROGRAMMER: Mackenzie Craig
//
// INTERFACE: unsigned QtTerminal::processFile(std::ifstream& file)
//
// RETURNS: the count of bits added
//
// NOTES: Processes a file by checking if the data needs to be split at 512
//
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

// 
// FUNCTION: packetizeFile
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig and Vafa Dehghan Saei
//
// PROGRAMMER: Will Murphy
//
// INTERFACE: QByteArray QtTerminal::packetizeFile(std::queue<char> data)
//
// RETURNS: the dataFrame
//
// NOTES: 
// Creates the structure of a packet by appending SYN, STX, and CRC.
//
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
	dataFrame.append(quint8(crc >> 24) & 0xFF);
	dataFrame.append(quint8(crc >> 16) & 0xFF);
	dataFrame.append(quint8(crc >> 8) & 0xFF);
	dataFrame.append(quint8(crc) & 0xFF);

	//Return the frame
	return dataFrame;
}

// 
// FUNCTION: parseDataFrame
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig & Vafa Dehghan Saei
//
// PROGRAMMER: Angus Lam
//
// INTERFACE: QByteArray QtTerminal::parseDataFrame(QByteArray receivedFrame)
//
// RETURNS: returns the data portion of the frame, or nullptr
//
// NOTES:
//This parses the frame
//Returns nullptr if syn or stx bit is not present, crc failed
//Returns the payload if successful
QByteArray QtTerminal::parseDataFrame(QByteArray receivedFrame)
{
	if (checkCRC(receivedFrame)) {
            return receivedFrame.mid(2,512);
	}

	return nullptr;
}

// 
// FUNCTION: parseControlFrame
//
// REVISIONS: N/A
//
// DESIGNER: Mackenzie Craig & Vafa Dehghan Saei
//
// PROGRAMMER: Angus Lam
//
// INTERFACE: char QtTerminal::parseControlFrame(QByteArray receivedFrame)
//
// RETURNS: 0, or the character at position 1 or 0 in the QByteArray
//
// NOTES: 
//This parses a control frame
//Returns the control character if present
//Returns 0 if failed
char QtTerminal::parseControlFrame(QByteArray receivedFrame)
{
	if (receivedFrame.at(0) != 0x16)
	{
		return 0;
	}

	if (receivedFrame.at(1) == 0x05 || receivedFrame.at(1) == 0x06)
	{
		return receivedFrame.at(1);
	}

	return 0;
}

// 
// FUNCTION: checkCRC
//
// REVISIONS: N/A
//
// DESIGNER: Angus Lam
//
// PROGRAMMER: Will Murphy
//
// INTERFACE: bool QtTerminal::checkCRC(QByteArray receivedFrame)
//
// RETURNS: 1 or 0, depending on success or fail.
//
// NOTES: 
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
    crc.append(quint8(crc_int >> 24) & 0xFF);
    crc.append(quint8(crc_int >> 16) & 0xFF);
    crc.append(quint8(crc_int >> 8) & 0xFF);
    crc.append(quint8(crc_int) & 0xFF);

	if (crc == receivedCrc) {
		return 1;
	}

	return 0;
}

// 
// FUNCTION: createAckFrame
//
// REVISIONS: N/A
//
// DESIGNER: Angus Lam
//
// PROGRAMMER: Angus Lam
//
// INTERFACE: QByteArray QtTerminal::createAckFrame()
//
// RETURNS: ackFrame
// A QByteArray of an ack.
//
// NOTES: 
//
QByteArray QtTerminal::createAckFrame()
{
	QByteArray ackFrame;

	//Append SYN
	ackFrame.append(0x16);
	//Append ACK
	ackFrame.append(0x06);

	return ackFrame;
}

// 
// FUNCTION: createEnqFrame
//
// REVISIONS: N/A
//
// DESIGNER: Angus Lam
//
// PROGRAMMER: Angus Lam
//
// INTERFACE: QByteArray QtTerminal::createEnqFrame()
//
// RETURNS: enqFrame
// A QByteArray of an enq.
//
// NOTES: 
//
QByteArray QtTerminal::createEnqFrame()
{
	QByteArray enqFrame;
	//Append SYN
	enqFrame.append(0x16);
	//Append ENQ
	enqFrame.append(0x05);

	return enqFrame;
}
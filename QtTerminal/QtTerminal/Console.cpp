/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Console.cpp - Sets the design of the Qt Terminal.
--
-- PROGRAM: Power to the Protocolteriat Terminal
--
-- FUNCTIONS:
-- Console::Console(QWidget *parent)
-- void Console::putData(const QByteArray& text)
-- Console::~Console()
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
-- This program creates a QT terminal and sets the theme of the terminal. 
----------------------------------------------------------------------------------------------------------------------*/
#include "Console.h"

// 
// FUNCTION: main
//
//
// DESIGNER: Mackenzie Craig, William Murphy
//
// PROGRAMMER: Vafa Dehghan Saei, Angus Lam
//
// INTERFACE: Console::Console(QWidget *parent)
//					QWidget *parent: a pointer to the parent QWidget class.  
//
// RETURNS: N/A Constructor
//
// NOTES: 
// The constructor creates a QT terminal and application and sets the theme. 

Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
{
	setReadOnly(true);
	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);
}

Console::~Console()
{
}


// 
// FUNCTION: putData
//
//
// DESIGNER: Mackenzie Craig, William Murphy
//
// PROGRAMMER: Vafa Dehghan Saei, Angus Lam
//
// INTERFACE: void Console::putData(const QByteArray& text)
//					const QByteArray& text: The text to display on the screen.   
//
// RETURNS: void
//
// NOTES: 
// Displays text on the screen. 
void Console::putData(const QByteArray& text)
{
	insertPlainText(QString(text));
}

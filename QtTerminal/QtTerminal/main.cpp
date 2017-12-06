/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: main.cpp - The entry point to the program. 
--
-- PROGRAM: Power to the Protocolteriat Terminal
--
-- FUNCTIONS:
-- int main(int argc, char *argv[]))
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
-- This program creates a QT terminal and application and runs it. 
----------------------------------------------------------------------------------------------------------------------*/
#include "QtTerminal.h"
#include <QtWidgets/QApplication>

// 
// FUNCTION: main
//
//
// DESIGNER: Mackenzie Craig, William Murphy
//
// PROGRAMMER: Vafa Dehghan Saei, Angus Lam
//
// INTERFACE: int main(int argc, char *argv[])
//					int argc: the number of command line arguments. 
//					char *argv[]: the command line arguments. 
//
// RETURNS: int, positive if successfull. 
//
// NOTES: 
// This program creates a QT terminal and application and runs it. 

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtTerminal w;
	w.show();
	return a.exec();
}

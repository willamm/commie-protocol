#include "QtTerminal.h"

QtTerminal::QtTerminal(QWidget *parent)
	: QMainWindow(parent)
	, console(this)
{
	ui.setupUi(this);
	setCentralWidget(&console);
	initActionConnections();
}

void QtTerminal::initActionConnections()
{
	// Opens QDialog to send file when Send File button is clicked
	connect(ui.actionSend_File, &QAction::triggered, this, &QtTerminal::openFileDialog);
}

void QtTerminal::openFileDialog()
{
	sendFileName = QFileDialog::getOpenFileName(this, "Send File");
}

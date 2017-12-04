#include "QtTerminal.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtTerminal w;
	w.show();
	return a.exec();
}

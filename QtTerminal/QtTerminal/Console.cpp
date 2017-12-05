#include "Console.h"

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


void Console::putData(const QByteArray& text)
{
	insertPlainText(QString(text));
}

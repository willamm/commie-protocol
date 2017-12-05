#include "Console.h"

/* Constructs the console that exists in the Qt terminal*/
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

//Places the text in the console
void Console::putData(const QByteArray& text)
{
	setPlainText(QString(text));
}

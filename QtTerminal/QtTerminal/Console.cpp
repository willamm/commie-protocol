#include "Console.h"

Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
{
	QPalette p = palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);
}

Console::~Console()
{
}

#pragma once

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
	Q_OBJECT

public:
	Console(QWidget* parent = nullptr);
	virtual ~Console();

	void putData(const QByteArray& text);
};

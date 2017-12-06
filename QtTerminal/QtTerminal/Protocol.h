#pragma once

#include <QObject>
#include <QStateMachine>

class Protocol : public QStateMachine
{
	Q_OBJECT

public:
	Protocol(QObject *parent);
	~Protocol();
};

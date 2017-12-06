#pragma once

#include <QObject>
#include <QStateMachine>

class Protocol : public QStateMachine
{
	Q_OBJECT

public:
	Protocol(QObject *parent);
	~Protocol();

private:
	QState idle;
	QState bidForLine;
	QState send;
	QState confirmTx;
	QState reTx;
	QState reset;
	QState ack;
	QState receive;
	QState error;
};

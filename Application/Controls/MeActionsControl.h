#pragma once
#include <QObject>
#include <QString>
#include <qqml.h>


class MeActionsControl : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	MeActionsControl() = default;
public slots:
	void parse(const QString& parserName);
	void parseInverse(const QString& parserName);
};
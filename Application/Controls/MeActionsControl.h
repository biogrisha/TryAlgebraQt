#pragma once
#include <QObject>
#include <QString>
#include <qqml.h>
#include <Models/MeActionsModel.h>

class MeActionsControl : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	MeActionsControl() = default;
public slots:
	void parse(const QString& parserName);
	void parseInverse(const QString& parserName);
	void applyFormula(MeActionData* actionData, int part);
};
#pragma once
#include <QAbstractItemModel>
#include <TRS/FormulasSystem.h>
#include <qqml.h>

class MeActionData : public QObject
{
	Q_OBJECT
public:
	explicit MeActionData(QObject* parent = nullptr);
public slots:
	QString name() const;
	QString type() const;
public:
	QString m_name;
	QString m_type;
};

class MeActionSection : public QObject
{
	Q_OBJECT
public:
	explicit MeActionSection(QObject* parent = nullptr);
public slots:
	QString name() const;
	QList<MeActionData*> actions() const;
public:
	QString m_name;
	QList<MeActionData*> m_actions;
};

class MeActionsModel : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	MeActionsModel() = default;
public slots:
	void update();
	QList<MeActionSection*> sections() const;
private:
	QList<MeActionSection*> m_sections;
};

#pragma once
#include "FreeTypeWrap.h"
#include <QObject>
#include <qqml.h>

class ToolbarControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	ToolbarControl(QObject *parent);
	ToolbarControl() = default;
	~ToolbarControl();

	FGlyphData data;
};


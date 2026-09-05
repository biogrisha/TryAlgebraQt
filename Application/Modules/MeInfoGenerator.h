#pragma once
#include <Models/MeListModel.h>
#include <Me/include/MeBase.h>
class QImage;
class MeInfoGenerator
{
public:
	void gen(MeListModel* model);
private:
	void setDrawContBackground(TryAlgebraCore::MeBase* me);
};
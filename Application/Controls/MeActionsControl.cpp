#include <Controls/MeActionsControl.h>
#include <ApplicationModel.h>
#include <AppGlobal.h>

QStringList MeActionsControl::transformers() const
{
	const auto& transformers = AppGlobal::appMod->formulasSystem()->transformers();
	QStringList res;
	for (const auto& tr : transformers)
	{
		res.push_back(QString::fromStdWString(tr.first));
	}
	return res;
}
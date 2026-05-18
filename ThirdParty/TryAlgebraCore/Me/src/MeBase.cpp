#include <Me/include/MeBase.h>

void TryAlgebraCore::MeBase::addChild(std::unique_ptr<MeBase>&& child)
{
	m_children.push_back(std::move(child));
}

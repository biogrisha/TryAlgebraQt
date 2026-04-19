#pragma once
#include <string>
#include <vector>
namespace TryAlgebraCore2
{
	class MeHelpers
	{
	public:
		std::vector FTAMeHelpers::ParseFromString(const std::wstring& InString, class FTAMathElementGenerator* Generator)
		{
			return {};
			//if (!Generator)
			//{
			//	//If cant create character
			//	return {};
			//}
			//enum class EState
			//{
			//	Default,
			//	ReadingMathElement,
			//	ReadingInfo,
			//	ExpectChildren
			//} State = EState::Default;

			//MathElementV2::FMathElements Result;
			//MathElementV2::FTAMeComposite* Parent = nullptr;
			//MathElementV2::FMathElementPtr LastElement;
			//std::wstring MeName;
			//std::wstring Info;
			//constexpr auto TermChar = L'\\';
			//constexpr auto StartChildChar = L'{';
			//constexpr auto EndChildChar = L'}';

			//for (int i = 0; i < InString.size(); i++)
			//{
			//	if (State == EState::ReadingMathElement)
			//	{
			//		//Reading Me
			//		if (InString[i] == TermChar)
			//		{
			//			//End reading Me?
			//			//start reading info
			//			Info.clear();
			//			State = EState::ReadingInfo;
			//			continue;
			//		}
			//		MeName += std::wstring(1, InString[i]);
			//		continue;
			//	}
			//	if (State == EState::ReadingInfo)
			//	{
			//		//Reading info
			//		if (InString[i] == TermChar)
			//		{
			//			//Finished info?
			//			//Try to create Me
			//			MathElementV2::FMathElementPtr Me = Generator->CreateMathElement(MeName, Info);
			//			if (!Me)
			//			{
			//				//failed to create me
			//				return {};
			//			}
			//			//Successfully created
			//			Me->SetParent(Parent);
			//			ResultOrParentArray(Result, Parent).push_back(Me);
			//			State = EState::ExpectChildren;
			//			continue;
			//		}
			//		Info += std::wstring(1, InString[i]);
			//		continue;
			//	}
			//	if (State == EState::ExpectChildren)
			//	{
			//		//Children started with combination "\{"
			//		if (i + 1 < InString.size() && InString[i] == '\\' && InString[i + 1] == StartChildChar)
			//		{
			//			//If start children
			//			//Set new parent
			//			Parent = ResultOrParentArray(Result, Parent).back()->Cast<MathElementV2::FTAMeComposite>();
			//			if (!Parent)
			//			{
			//				//If new parent not composite -> fail
			//				return {};
			//			}
			//			//Return to default state
			//			State = EState::Default;
			//			//increment to skip two chars
			//			++i;
			//			continue;
			//		}
			//		//back to default state
			//		State = EState::Default;
			//	}
			//	if (State == EState::Default)
			//	{
			//		if (InString[i] == TermChar)
			//		{
			//			if (i + 1 < InString.size() && InString[i + 1] == EndChildChar)
			//			{
			//				//If end children
			//				if (!Parent)
			//				{
			//					//If no parent -> fail
			//					return {};
			//				}
			//				//set parent one level higher
			//				Parent = Parent->GetParent().Get();
			//				//Skip "\" and continue
			//				++i;
			//				continue;
			//			}
			//			//Start reading Me
			//			State = EState::ReadingMathElement;
			//			MeName.clear();
			//			continue;
			//		}
			//		//Read character
			//		auto Me = Generator->CreateCharacter(InString[i]);
			//		Me->SetParent(Parent);
			//		ResultOrParentArray(Result, Parent).push_back(Me);
			//	}
			//}
			//if ((State != EState::Default && State != EState::ExpectChildren) || Parent != nullptr)
			//{
			//	return {};
			//}
			//return Result;
		}
	};
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/ExpressionsLibrary.h"
#include <algorithm>
#include <cwctype>
#include <map>

#include "FunctionLibraries/MeDefinition.h"

namespace
{
	std::set<std::wstring> NonMeTokens = {FTAMeDefinition::OperatorType, FTAMeDefinition::InvisibleBracketsType};
}

namespace TAExpressionsLibrary
{
	FStepUpResult FStepUpResult::Continue;
	FStepUpResult FStepUpResult::SuccessComparison(true, true);
	FStepUpResult FStepUpResult::FailedComparison(true, false);

	//Brackets to branches
	class BranchMaker
	{
	public:
		BranchMaker(FSymbols& InMathTree);

		//Index of first element in branch
		void StartBranch(const FSymbolPtr& NewParent, size_t Index);
		void AddToCurrentBranch(const FSymbolPtr& InSymbol) const;

		//Index of last element in branch, Index will point to new parent
		void EndBranch(size_t& Index);
		bool IsInBranch() const;
		int GetChildrenCount() const;

	private:
		FSymbols& MathTree;
		//Branch start index
		int StartIndex = 0;
		//Current branch
		FSymbolPtr CurrentParent;
		//Owner of MathTree elements passed in constructor
		FSymbolPtr StartParent;
		//First parent in new branch
		FSymbolPtr NewOuterParent;
	};
}


TAExpressionsLibrary::FSymbol::FSymbol(const std::wstring& InName, const std::wstring& InInfo, const FSymbols& InSymbols)
{
	ElementName = InName;
	Info = InInfo;
	Children = InSymbols;
}


void TAExpressionsLibrary::FSymbol::ParseToString(std::wstring& OutString)
{
	if (ElementName.empty())
	{
		//If it is a char
		OutString += Info;
		return;
	}
	std::wstring Sl = L"\\";
	OutString += Sl + ElementName + Sl + Info + Sl;

	if (!Children.empty())
	{
		OutString += L"\\{";


		for (FSymbolPtr& Child : Children)
		{
			Child->ParseToString(OutString);
		}

		OutString += L"\\}";
	}
}

TAExpressionsLibrary::FCharacter::FCharacter(const FSymbolPtr& InSymbolPtr)
{
	SymbolPtr = InSymbolPtr;
}

bool TAExpressionsLibrary::FCharacter::operator==(const std::wstring& Word) const
{
	if (SymbolPtr == nullptr)
	{
		return false;
	}
	return SymbolPtr->Info == Word;
}

bool TAExpressionsLibrary::FCharacter::IsA(const FSymbolPtr& InSymbolPtr)
{
	return InSymbolPtr->ElementName.empty();
}

TAExpressionsLibrary::FIndex::FIndex(const FSymbolPtr& InSymbolPtr)
{
	SymbolPtr = InSymbolPtr;
	Type = DefineType();
}

bool TAExpressionsLibrary::FIndex::IsA(const FSymbolPtr& InSymbolPtr)
{
	return InSymbolPtr->ElementName == FTAMeDefinition::IndexName;
}

TAExpressionsLibrary::FIndex::EType TAExpressionsLibrary::FIndex::DefineType() const
{
	if (!SymbolPtr)
	{
		return EType::None;
	}
	if (SymbolPtr->Info == FTAMeDefinition::SuperscriptInfo)
	{
		return EType::Superscript;
	}
	if (SymbolPtr->Info == FTAMeDefinition::SubscriptInfo)
	{
		return EType::Subscript;
	}
	return EType::Doublescript;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::FIndex::GetSubscript() const
{
	switch (Type)
	{
	case EType::Subscript:
		return SymbolPtr->Children[0]->Children;
	case EType::Doublescript:
		return SymbolPtr->Children[1]->Children;
	default:
		return FSymbols();
	}
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::FIndex::GetSuperscript() const
{
	switch (Type)
	{
	case EType::Superscript:
	case EType::Doublescript:
		return SymbolPtr->Children[0]->Children;
	default:
		return FSymbols();
	}
}

TAExpressionsLibrary::FParameter::FParameter(const FSymbolPtr& InSymbolPtr) : bMultiParam(InSymbolPtr->Info == FTAMeDefinition::MultiParamInfo), bIsValid(true)
{
	SymbolPtr = InSymbolPtr;
	if (SymbolPtr && !SymbolPtr->Children.empty())
	{
		Name = ParseToString(SymbolPtr->Children[0]->Children);
	}
}

bool TAExpressionsLibrary::FParameter::IsA(const FSymbolPtr& InSymbolPtr)
{
	return InSymbolPtr->ElementName == FTAMeDefinition::ParamName;
}

const TAExpressionsLibrary::FSymbols& TAExpressionsLibrary::FParameter::GetArguments() const
{
	return Arguments;
}

void TAExpressionsLibrary::FParameter::AddArgument(const FSymbolPtr& InSymbolPtr) const
{
	Arguments.push_back(InSymbolPtr);
}


TAExpressionsLibrary::FComparisonState::FComparisonState(const FComparisonState& Other)
{
	if (this != &Other)
	{
		*this = Other;
	}
}

TAExpressionsLibrary::FComparisonState& TAExpressionsLibrary::FComparisonState::operator=(const FComparisonState& Other)
{
	if (this != &Other)
	{
		InputInd = Other.InputInd;
		TemplInd = Other.TemplInd;
		InputCopy = Other.InputCopy;
		if (InputCopy.size() > 0)
		{
			Input = &InputCopy;
		}
		else
		{
			Input = Other.Input;
		}
		Template = Other.Template;
		TemplateParent = Other.TemplateParent;
		bFirstEnterStage = Other.bFirstEnterStage;
		bLastResult = Other.bLastResult;
		Stage = Other.Stage;
		CompareStage = Other.CompareStage;
	}
	return *this;
}

TAExpressionsLibrary::FSymbolPtr TAExpressionsLibrary::FComparisonState::GetTemplSymbol() const
{
	return (*Template)[TemplInd];
}

TAExpressionsLibrary::FSymbolPtr TAExpressionsLibrary::FComparisonState::GetInputSymbol() const
{
	return (*Input)[InputInd];
}

bool TAExpressionsLibrary::FComparisonState::TemplateEnded() const
{
	return TemplInd >= Template->size();
}

bool TAExpressionsLibrary::FComparisonState::InputEnded() const
{
	return InputInd >= Input->size();
}

TAExpressionsLibrary::FVariator::FVariator(std::vector<FComparisonState>* InStates)
{
	States = InStates;
}

bool TAExpressionsLibrary::FVariator::NextVariation()
{
	return false;
}


TAExpressionsLibrary::FOrderedMultiVariator::FOrderedMultiVariator(std::vector<FComparisonState>* InStates)
	: FVariator(InStates)
{
	auto& LastState = States->back();
	Param = LastState.GetTemplSymbol();
	//How many arguments it can capture
	AllowedArgumentsCount = LastState.Input->size() - LastState.InputInd - (LastState.Template->size() - (LastState.TemplInd + 1));
	if (LastState.TemplateParent && IsOperator(LastState.TemplateParent))
	{
		//If has parent operator -> will capture arguments into this operator, to preserve arguments context
		auto Operator = MakeSymbol(LastState.TemplateParent->ElementName, LastState.TemplateParent->Info);
		Operator->bCommute = LastState.TemplateParent->bCommute;
		Operator->Expand = LastState.TemplateParent->Expand;
		LastState.GetTemplSymbol()->Captured.push_back(Operator);
		CapturedPtr = &LastState.GetTemplSymbol()->Captured[0]->Children;
		Param->bWithContextOperator = true;
	}
	else
	{
		//If no context capture as it is
		CapturedPtr = &LastState.GetTemplSymbol()->Captured;
	}
	bCaptureAll = LastState.TemplInd == LastState.Template->size() - 1;
	LastState.TemplInd++;
}

bool TAExpressionsLibrary::FOrderedMultiVariator::NextVariation()
{
	auto& LastState = States->back();
	if (bCaptureAll)
	{
		if (!CapturedPtr->empty())
		{
			Param->Captured.clear();
			return false;
		}
		CapturedPtr->insert(CapturedPtr->begin(), LastState.Input->begin() + LastState.InputInd, LastState.Input->end());
		LastState.InputInd = LastState.Input->size();
		return true;
	}
	if (CapturedPtr->size() == AllowedArgumentsCount)
	{
		//If no variations left -> cleanup
		LastState.InputInd -= AllowedArgumentsCount;
		Param->Captured.clear();
		CapturedPtr = nullptr;
		return false;
	}
	//Add argument to parameter
	CapturedPtr->push_back(LastState.GetInputSymbol());
	LastState.InputInd++;
	return true;
}

bool TAExpressionsLibrary::FOrderedUniVariator::NextVariation()
{
	if (bCaptured)
	{
		return false;
	}
	bCaptured = true;
	//Cache last state and param
	auto& LastState = States->back();
	auto Param = LastState.GetTemplSymbol();

	if (!Param->Captured.empty())
	{
		//if param already captured something-> fail variator
		LastState.TemplInd--;
		LastState.InputInd--;
		Param->Captured.clear();
		return false;
	}
	//Capture simplified argument
	Param->Captured.push_back(LastState.GetInputSymbol());


	//Incr indices
	LastState.InputInd++;
	LastState.TemplInd++;
	return true;
}

TAExpressionsLibrary::FUnorderedUniVariator::FUnorderedUniVariator(std::vector<FComparisonState>* InStates)
	: FVariator(InStates)
{
	auto& LastState = States->back();
	//Cache input index and param
	InputIndStart = InputInd = LastState.InputInd;
	Param = LastState.GetTemplSymbol();
	//incr indices 
	LastState.TemplInd++;
	LastState.InputInd = LastState.TemplInd;
	InputCopy = *LastState.Input;
}

bool TAExpressionsLibrary::FUnorderedUniVariator::NextVariation()
{
	auto& LastState = States->back();
	Param->Captured.clear();
	if (InputInd >= LastState.Input->size())
	{
		//If all variations failed
		return false;
	}
	if (InputIndStart != InputInd)
	{
		//set next symbol in capture position
		std::swap(InputCopy[InputIndStart], InputCopy[InputInd]);
	}
	Param->Captured.push_back(InputCopy[InputIndStart]);
	*LastState.Input = InputCopy;
	InputInd++;
	return true;
}

int TAExpressionsLibrary::FChainedCounterUnit::GetValue() const
{
	return Value;
}

bool TAExpressionsLibrary::FChainedCounterUnit::Increment()
{
	if (Value == Max)
	{
		if (Left)
		{
			if (!Left->Increment())
			{
				return false;
			}
			Value = Left->Value + 1;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Value++;
	}
	return true;
}

TAExpressionsLibrary::FChainedCounter::FChainedCounter(int InUnitsCount, int InSize)
{
	Units = std::vector<FChainedCounterUnit>(InUnitsCount);
	FChainedCounterUnit* Left = nullptr;
	for (int i = 0; i < InUnitsCount; i++)
	{
		Units[i].Value = i;
		Units[i].Max = InSize - (InUnitsCount - i);
		Units[i].Left = Left;
		Left = &Units[i];
	}
}

bool TAExpressionsLibrary::FChainedCounter::Increment()
{
	return Units.back().Increment();
}

const std::vector<TAExpressionsLibrary::FChainedCounterUnit>& TAExpressionsLibrary::FChainedCounter::GetUnits()
{
	return Units;
}

TAExpressionsLibrary::FUnorderedMultiVariator::FUnorderedMultiVariator(std::vector<FComparisonState>* InStates)
	: FVariator(InStates)
{
	auto& LastState = States->back();
	Param = LastState.GetTemplSymbol();
	if (LastState.TemplateParent && IsOperator(LastState.TemplateParent))
	{
		//If has parent operator -> will capture arguments into this operator, to preserve arguments context 
		auto Operator = MakeSymbol(LastState.TemplateParent->ElementName, LastState.TemplateParent->Info);
		Operator->bCommute = LastState.TemplateParent->bCommute;
		Operator->Expand = LastState.TemplateParent->Expand;
		LastState.GetTemplSymbol()->Captured.push_back(Operator);
		CapturedPtr = &LastState.GetTemplSymbol()->Captured[0]->Children;
		Param->bWithContextOperator = true;
	}
	else
	{
		//If no context capture as it is
		CapturedPtr = &LastState.GetTemplSymbol()->Captured;
	}
	bCaptureAll = LastState.TemplInd == LastState.Template->size() - 1;
	if (!bCaptureAll)
	{
		IndexStart = LastState.InputInd;
		MaxArgsSize = LastState.Input->size() - LastState.InputInd - (LastState.Template->size() - (LastState.TemplInd + 1));
		ChainCounterSize = LastState.Input->size() - LastState.InputInd;
		ChainedCounter = FChainedCounter(Count, ChainCounterSize);
		ChildrenCopy = *LastState.Input;
	}
	LastState.TemplInd++;
}

bool TAExpressionsLibrary::FUnorderedMultiVariator::NextVariation()
{
	auto& LastState = States->back();
	if (bCaptureAll)
	{
		if (!CapturedPtr->empty())
		{
			Param->Captured.clear();
			return false;
		}
		CapturedPtr->insert(CapturedPtr->begin(), LastState.Input->begin() + LastState.InputInd, LastState.Input->end());
		LastState.InputInd = LastState.Input->size();
		return true;
	}
	if (bLastVariation)
	{
		Param->Captured.clear();
		return false;
	}
	//Cache values
	auto& CounterUnits = ChainedCounter.GetUnits();
	//Clear captured and restore original order of input
	CapturedPtr->clear();
	*LastState.Input = ChildrenCopy;
	//iterate over units
	for (int i = 0; i < CounterUnits.size(); i++)
	{
		//order with respect to new variation and capture
		std::swap(LastState.Input->at(i + IndexStart), LastState.Input->at(CounterUnits[i].GetValue() + IndexStart));
		CapturedPtr->push_back(LastState.Input->at(i + IndexStart));
	}
	LastState.InputInd = IndexStart + Count;
	if (!ChainedCounter.Increment())
	{
		//If next increment unavailable 
		if (Count < MaxArgsSize)
		{
			//We can capture more arguments
			Count++;
			ChainedCounter = FChainedCounter(Count, ChainCounterSize);
		}
		else
		{
			//We captured maximum arguments->end variator on the next variation
			bLastVariation = true;
		}
	}
	return true;
}

std::vector<TAExpressionsLibrary::FComparisonState> TAExpressionsLibrary::FVariator::GetStatesCopy() const
{
	return *States;
}

bool TAExpressionsLibrary::FSingleVariation::NextVariation()
{
	if (bFirstVariation)
	{
		bFirstVariation = false;
		return true;
	}
	return false;
}

TAExpressionsLibrary::FSymbolPtr TAExpressionsLibrary::MakeSymbol(const std::wstring& Name, const std::wstring& Info, const FSymbolPtr& Parent, const FSymbols& Children)
{
	auto Res = std::make_shared<FSymbol>();
	Res->ElementName = Name;
	Res->Info = Info;
	Res->Parent = Parent;
	Res->Children = Children;
	return Res;
}


TAExpressionsLibrary::BranchMaker::BranchMaker(FSymbols& InMathTree) : MathTree(InMathTree)
{
	if (MathTree.empty())
	{
		return;
	}
	StartParent = MathTree[0]->Parent;
	CurrentParent = StartParent;
}

void TAExpressionsLibrary::BranchMaker::StartBranch(const FSymbolPtr& NewParent, size_t Index)
{
	if (CurrentParent == StartParent)
	{
		//if we at the top level
		StartIndex = Index;
		NewOuterParent = NewParent;
	}
	else
	{
		CurrentParent->Children.push_back(NewParent);
	}
	NewParent->Parent = CurrentParent;
	CurrentParent = NewParent;
}

void TAExpressionsLibrary::BranchMaker::AddToCurrentBranch(const FSymbolPtr& InSymbol) const
{
	if (CurrentParent != StartParent)
	{
		CurrentParent->Children.push_back(InSymbol);
		InSymbol->Parent = CurrentParent;
	}
}

void TAExpressionsLibrary::BranchMaker::EndBranch(size_t& Index)
{
	//go up a level
	CurrentParent = CurrentParent->Parent;
	//if we are at the start level
	if (CurrentParent == StartParent)
	{
		//Insert new branch in array at branch start index
		MathTree.insert(MathTree.begin() + StartIndex, NewOuterParent);
		//Index points to the same element
		Index++;
		MathTree.erase(MathTree.begin() + StartIndex + 1, MathTree.begin() + Index + 1);
		//Index equals to new parent
		Index = StartIndex;
	}
}

bool TAExpressionsLibrary::BranchMaker::IsInBranch() const
{
	return CurrentParent != StartParent;
}

int TAExpressionsLibrary::BranchMaker::GetChildrenCount() const
{
	if (CurrentParent)
	{
		return CurrentParent->Children.size();
	}
	return 0;
}

bool TAExpressionsLibrary::FParametersHelper::StartParameter(const FParameter& InParameter, bool bOnlyIfAdded)
{
	//find out if parameter already here
	std::set<FParameter, FParameterComparator>::iterator it = Parameters.find(InParameter);
	//If parameter was already added
	if (it != Parameters.end())
	{
		CurrentParameterIt = it;
		bNewParameter = false;
		ArgumentIndex = 0;
	}
	else if (!bOnlyIfAdded) //Otherwise add new parameter 
	{
		auto Inserted = Parameters.insert(InParameter);
		CurrentParameterIt = Inserted.first;
		bNewParameter = true;
	}
	else
	{
		return false;
	}
	return true;
}

TAExpressionsLibrary::FParametersHelper::EAddElementStatus TAExpressionsLibrary::FParametersHelper::TryAddElement(const FSymbolPtr& Symbol)
{
	//If parameter it invalid fail
	if (CurrentParameterIt == Parameters.end())
	{
		return EAddElementStatus::Failed;
	}
	auto Copy = CopyMathTree({Symbol});
	SimplifyMathTree(Copy);

	//If new parameter we want to add elements into it
	if (bNewParameter)
	{
		if (!CurrentParameterIt->bMultiParam && !CurrentParameterIt->GetArguments().empty())
		{
			//If single param and already has captured argument -> fail
			return EAddElementStatus::Failed;
		}
		for (auto& ArgumentSymbol : Copy)
		{
			CurrentParameterIt->AddArgument(ArgumentSymbol);
		}
		if (!CurrentParameterIt->bMultiParam)
		{
			//if single param -> return Ended
			return EAddElementStatus::Ended;
		}
	}
	else //If parameter already being populated with arguments
	//we want to compare them with newly captured arguments  
	{
		if (ArgumentIndex + Copy.size() > CurrentParameterIt->GetArguments().size())
		{
			//If all elements captured were already compared
			//Or comparison failed
			return EAddElementStatus::Failed;
		}
		//Iterate over  arguments
		for (auto& ArgumentSymbol : Copy)
		{
			if (!CompareSymbolsRecursively(CurrentParameterIt->GetArguments()[ArgumentIndex], ArgumentSymbol))
			{
				//If comparison with corresponding argument in parameter failed
				return EAddElementStatus::Failed;
			}
			ArgumentIndex++;
		}
		//if comparison ended return Ended
		if (ArgumentIndex == CurrentParameterIt->GetArguments().size())
		{
			return EAddElementStatus::Ended;
		}
	}
	return EAddElementStatus::Success;
}

bool TAExpressionsLibrary::FParametersHelper::CanEndParameter()
{
	if (CurrentParameterIt->GetArguments().empty())
	{
		return false;
	}
	if (!bNewParameter && ArgumentIndex != CurrentParameterIt->GetArguments().size())
	{
		return false;
	}
	return true;
}

const std::set<TAExpressionsLibrary::FParameter, TAExpressionsLibrary::FParameterComparator>& TAExpressionsLibrary::FParametersHelper::GetParameters() const
{
	return Parameters;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::FParametersHelper::GetArguments(const FParameter& InParameter)
{
	auto It = Parameters.find(InParameter);
	if (It == Parameters.end())
	{
		return FSymbols();
	}
	return It->GetArguments();
}

void TAExpressionsLibrary::FParametersHelper::AppendParameters(const FParametersHelper& Other)
{
	Parameters.insert(Other.Parameters.begin(), Other.Parameters.end());
}


void TAExpressionsLibrary::ExpandInvisibleBrackets(int Index, const FSymbolPtr& Self)
{
	if (Self->Info.length() < 2)
	{
		return;
	}
	wchar_t OpenBracket[2] = {'\0'};
	wchar_t CloseBracket[2] = {'\0'};
	OpenBracket[0] = Self->Info[0];
	CloseBracket[0] = Self->Info[1];
	FSymbolPtr OpenBracketSymbol = MakeSymbol(L"", OpenBracket, Self);
	FSymbolPtr CloseBracketSymbol = MakeSymbol(L"", CloseBracket, Self);
	Self->Children.insert(Self->Children.begin(), std::move(OpenBracketSymbol));
	Self->Children.push_back(std::move(CloseBracketSymbol));
}

void TAExpressionsLibrary::ExpandOperator(int InIndex, const FSymbolPtr& Self)
{
	if (Self->Info.length() < 1)
	{
		return;
	}
	wchar_t Operator[2] = {'\0'};
	Operator[0] = Self->Info[0];
	if (const bool bStartFromOperator = Self->Info.length() == 2)
	{
		size_t InitialSize = Self->Children.size();
		Self->Children.reserve(InitialSize * 2);
		for (size_t Index = 0; Index < InitialSize; Index++)
		{
			auto Symbol = MakeSymbol(L"", std::wstring(1, Operator[0]), Self);
			Self->Children.push_back(std::move(Symbol));
		}
		int InitialLastIndex = InitialSize - 1;
		for (int i = 0; i < InitialSize; ++i)
		{
			std::iter_swap(Self->Children.begin() + InitialLastIndex - i, Self->Children.end() - 1 - i * 2);
		}
	}
	else
	{
		size_t InitialSize = Self->Children.size();
		Self->Children.reserve(InitialSize * 2 - 1);
		for (size_t Index = 0; Index < InitialSize - 1; Index++)
		{
			auto Symbol = MakeSymbol(L"", std::wstring(1, Operator[0]), Self);
			Self->Children.push_back(std::move(Symbol));
		}
		int InitialLastIndex = InitialSize - 1;
		for (int i = 0; i < InitialSize - 1; ++i)
		{
			iter_swap(Self->Children.begin() + InitialLastIndex - i, Self->Children.end() - 1 - i * 2);
		}
	}
}

void TAExpressionsLibrary::ExpandNegation(int InIndex, const FSymbolPtr& Self)
{
	FSymbolPtr MinusSign = MakeSymbol(L"", L"-", Self);
	Self->Children.insert(Self->Children.begin(), std::move(MinusSign));
}

void TAExpressionsLibrary::ClearExtraPlusSign(FSymbols& MathTree)
{
	if (MathTree.empty())
	{
		return;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!FParameter::IsA(MathTree[i]))
		{
			ClearExtraPlusSign(MathTree[i]->Children);
		}
	}

	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (IsCharacter(MathTree[i], L"+")
			&& MathTree.size() > i + 1
			&& IsCharacter(MathTree[i + 1], L"-"))
		{
			MathTree.erase(MathTree.begin() + i, MathTree.begin() + i + 1);
		}
	}
}

void TAExpressionsLibrary::ExpandMathTreeRecursive(FSymbols& MathTree)
{
	//Iterate over math elements
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		//if not Math element
		if (!IsMe(MathTree[i]))
		{
			//Expand if can
			if (MathTree[i]->Expand)
			{
				MathTree[i]->Expand(i, MathTree[i]);
			}
		}
	}
	//Apply recursion
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		ExpandMathTreeRecursive(MathTree[i]->Children);
	}
}

void TAExpressionsLibrary::SimplifyMathTreeRecursive(FSymbols& MathTree)
{
	SimplifyMathTreeRecursive(MathTree, [](const FSymbolPtr& InSymbol) { return !IsMe(InSymbol) || IsCustomToken(InSymbol); });
}

bool TAExpressionsLibrary::EqualsToAny(const FCharacter& Character, const std::vector<std::wstring>& Characters, const std::vector<std::wstring>& Except)
{
	for (auto& ExceptStr : Except)
	{
		if (Character == ExceptStr)
		{
			return false;
		}
	}
	for (auto& CharacterStr : Characters)
	{
		if (Character == CharacterStr)
		{
			return true;
		}
	}
	return false;
}

bool TAExpressionsLibrary::IsContainer(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::ContainerName;
}

void TAExpressionsLibrary::ReplaceParametersWithArguments(FParametersHelper* ParametersHelper, FSymbols& Template)
{
	//Iterate over template
	for (int i = Template.size() - 1; i >= 0; i--)
	{
		FParameter Parameter;
		if (To(Template[i], Parameter))
		{
			//If parameter -> replace with found arguments
			FSymbols Arguments = ParametersHelper->GetArguments(Parameter);
			if (!Arguments.empty())
			{
				Template.erase(Template.begin() + i);
				Template.insert(Template.begin() + i, Arguments.begin(), Arguments.end());
			}
		}
		else
		{
			//If not parameter -> apply this to children
			ReplaceParametersWithArguments(ParametersHelper, Template[i]->Children);
		}
	}
}

bool TAExpressionsLibrary::ExtractArgumentsSimple(const FSymbols& MathTree, const FSymbols& Template, FParametersHelper* ParametersHelper)
{
	//MathTree can't be smaller than Template
	//If one is empty, other can't be non-empty
	if (MathTree.size() < Template.size() || MathTree.empty() != Template.empty())
	{
		return false;
	}
	//If both are empty it's okay
	if (MathTree.empty() && Template.empty())
	{
		return true;
	}
	//Clear captured flag
	ClearCaptured(MathTree);
	//create pools Multi-Parameter/Terminating sequense
	std::vector<FSymbolsPool> Pools;
	if (!CreateSymbolPools(Template, Pools))
	{
		return false;
	}
	//Use pools to capture arguments
	if (!CaptureArgumentsWithPools(MathTree, Pools, ParametersHelper))
	{
		return false;
	}
	//Apply recursion
	int MathTreeInd = 0;
	int TemplateInd = 0;
	while (true)
	{
		MathTreeInd = SkipCapturedParameterIndices(MathTree, MathTreeInd);
		TemplateInd = SkipCapturedParameterIndices(Template, TemplateInd);
		if (MathTreeInd >= MathTree.size() || TemplateInd >= Template.size())
		{
			break;
		}
		if (!ExtractArgumentsSimple(MathTree[MathTreeInd]->Children, Template[TemplateInd]->Children, ParametersHelper))
		{
			return false;
		}
		MathTreeInd++;
		TemplateInd++;
	}
	return true;
}

bool TAExpressionsLibrary::CreateSymbolPools(const FSymbols& MathTree, std::vector<FSymbolsPool>& Pools)
{
	//Symbols pool is started with parameter terminated by ordinary symbols and single-params
	//since the capture only one symbol
	Pools.clear();
	//Iterate over symbols sequence
	for (auto& Symbol : MathTree)
	{
		if (IsMultiParameter(Symbol))
		{
			//If multi param
			if (!Pools.empty() && Pools.back().TerminatingSymbols.empty())
			{
				//if last terminating sequence was empty -> means that there is two consecutive multi-params
				return false;
			}
			//Create new pool started with this param
			Pools.emplace_back();
			Pools.back().MultiParameter = Symbol;
			continue;
		}
		//If first element not a multi-param
		if (Pools.empty())
		{
			Pools.emplace_back();
		}
		//If ordinary symbol or single param -> add to terminating sequence
		Pools.back().TerminatingSymbols.push_back(Symbol);
	}
	return true;
}

bool TAExpressionsLibrary::CaptureArgumentsWithPools(const FSymbols& MathTree, const std::vector<FSymbolsPool>& Pools, FParametersHelper* ParametersHelper)
{
	int PoolInd = 0;
	int MathTreeInd = 0;
	if (!Pools[0].MultiParameter)
	{
		//when started with symbols
		bool bSinglePool = Pools.size() == 1;
		if (bSinglePool && MathTree.size() != Pools[0].TerminatingSymbols.size())
		{
			return false;
		}
		//Just compare
		if (!CompareWithSequence(Pools[0].TerminatingSymbols, MathTree, 0, ParametersHelper))
		{
			return false;
		}
		if (bSinglePool)
		{
			return true;
		}
		PoolInd++;
		MathTreeInd += Pools[0].TerminatingSymbols.size();
	}
	//Iterate over pools excluding the last one
	for (; PoolInd < Pools.size() - 1; PoolInd++)
	{
		if (ParametersHelper->StartParameter(Pools[PoolInd].MultiParameter, true))
		{
			//if parameter already captured
			while (true)
			{
				//Try to add argument and incr index
				auto AddElementsResult = ParametersHelper->TryAddElement(MathTree[MathTreeInd]);
				MathTree[MathTreeInd]->bCaptured = true;
				MathTreeInd++;
				if (AddElementsResult == FParametersHelper::EAddElementStatus::Failed)
				{
					//If argument is different from that in parameter -> fail
					return false;
				}
				if (AddElementsResult == FParametersHelper::EAddElementStatus::Ended)
				{
					//If last argument -> end loop
					break;
				}
				if (MathTreeInd >= MathTree.size())
				{
					//If arguments are remained but math tree already ended -> fail
					return false;
				}
			}
			//After arguments successfully compared -> compare terminating sequences
			if (!CompareWithSequence(Pools[PoolInd].TerminatingSymbols, MathTree, MathTreeInd, ParametersHelper))
			{
				return false;
			}
			MathTreeInd += Pools[PoolInd].TerminatingSymbols.size();
		}
		else
		{
			//If not last pool
			int FirstOccurenceInd = 0;
			//Find index up to which parameter captures
			if (!FindFirstSequenceOccurence(Pools[PoolInd].TerminatingSymbols, MathTree, MathTreeInd, FirstOccurenceInd, ParametersHelper))
			{
				return false;
			}
			//Capture parameters in determined range
			ParametersHelper->StartParameter(Pools[PoolInd].MultiParameter);
			for (; MathTreeInd < FirstOccurenceInd; MathTreeInd++)
			{
				ParametersHelper->TryAddElement(MathTree[MathTreeInd]);
				MathTree[MathTreeInd]->bCaptured = true;
			}
			MathTreeInd = FirstOccurenceInd + Pools[PoolInd].TerminatingSymbols.size();
		}
	}
	//Handle last pool
	if (Pools.back().TerminatingSymbols.empty())
	{
		//If ended with parameter -> capture all remaining parameters
		ParametersHelper->StartParameter(Pools.back().MultiParameter);
		for (; MathTreeInd < MathTree.size(); MathTreeInd++)
		{
			if (ParametersHelper->TryAddElement(MathTree[MathTreeInd]) == FParametersHelper::EAddElementStatus::Failed)
			{
				return false;
			}
			MathTree[MathTreeInd]->bCaptured = true;
		}
		if (!ParametersHelper->CanEndParameter())
		{
			return false;
		}
	}
	else
	{
		//If ended with TS
		//Compare them with last symbols in MT
		int CompareFrom = MathTree.size() - Pools.back().TerminatingSymbols.size();
		if (CompareFrom <= MathTreeInd)
		{
			//If TS has too many symbols
			return false;
		}
		if (!CompareWithSequence(Pools.back().TerminatingSymbols, MathTree, CompareFrom, ParametersHelper))
		{
			return false;
		}
		//Capture remaining arguments
		ParametersHelper->StartParameter(Pools.back().MultiParameter);
		for (; MathTreeInd < CompareFrom; MathTreeInd++)
		{
			if (ParametersHelper->TryAddElement(MathTree[MathTreeInd]) == FParametersHelper::EAddElementStatus::Failed)
			{
				return false;
			}
			MathTree[MathTreeInd]->bCaptured = true;
		}
		if (!ParametersHelper->CanEndParameter())
		{
			return false;
		}
	}
	return true;
}

bool TAExpressionsLibrary::FindFirstSequenceOccurence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, int& FirstOccurrenceIndex, FParametersHelper* ParametersHelper)
{
	int To = 0;
	//Set last index to max index s.t TerminatingSequence doesn't go over the MathTree
	//012345678, 678 9-3+1 = 7  ->  i < 7
	int LastIndex = MathTree.size() - TerminatingSequence.size() + 1;
	//iterate over math tree from "From" to last reasonable index
	for (size_t i = From; i < LastIndex; i++)
	{
		//if "i" is the start of terminating sequence
		if (CompareWithSequence(TerminatingSequence, MathTree, i, ParametersHelper))
		{
			To = i;
			break;
		}
	}
	FirstOccurrenceIndex = To;
	return To > From;
}

bool TAExpressionsLibrary::CompareWithSequence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, FParametersHelper* ParametersHelper)
{
	if (MathTree.size() < TerminatingSequence.size() + From)
	{
		//If sequence exceeded MathTree
		return false;
	}
	FSymbols CapturedTemp;
	//This helper used to handle parameters that weren't added, and may be declined if function will fail
	FParametersHelper TempParameterHelper;
	//Iterate over terminating symbols
	for (size_t i = 0; i < TerminatingSequence.size(); i++)
	{
		if (TerminatingSequence[i]->Type == FSymbol::EType::UniParam)
		{
			//If parameter
			if (ParametersHelper->StartParameter(TerminatingSequence[i], true))
			{
				//If global parameter -> check that it conforms to already added parameter
				FParametersHelper::EAddElementStatus AddArgumentResult = ParametersHelper->TryAddElement(MathTree[i + From]);
				if (AddArgumentResult == FParametersHelper::EAddElementStatus::Failed)
				{
					return false;
				}
				CapturedTemp.push_back(MathTree[i + From]);
			}
			else
			{
				//If local parameter
				TempParameterHelper.StartParameter(TerminatingSequence[i]);
				FParametersHelper::EAddElementStatus AddArgumentResult = TempParameterHelper.TryAddElement(MathTree[i + From]);
				if (AddArgumentResult == FParametersHelper::EAddElementStatus::Failed)
				{
					return false;
				}
				CapturedTemp.push_back(MathTree[i + From]);
			}
			continue;
		}
		//if corresponding symbols are different -> fail
		if (!CompareSymbols(TerminatingSequence[i], MathTree[i + From]))
		{
			return false;
		}
	}
	//Append temp parameters to global
	ParametersHelper->AppendParameters(TempParameterHelper);
	//Set Captured flag
	SetCaptured(CapturedTemp, true);
	return true;
}

void TAExpressionsLibrary::CopyMathTreeImpl(const FSymbols& From, FSymbols& To, const FSymbolPtr& Parent)
{
	CopyMathTreeViewImpl(From, 0, From.size(), To, Parent);
}

void TAExpressionsLibrary::CopyMathTreeViewImpl(const FSymbols& From, int FromStart, int FromEnd, FSymbols& To, const FSymbolPtr& Parent)
{
	if (From.empty() || FromStart >= FromEnd)
	{
		return;
	}
	//Reserve To Symbols array
	To.reserve(FromEnd - FromStart);
	//Iterate over From symbols
	for (size_t i = FromStart; i < FromEnd; i++)
	{
		//Create new symbol in To
		To.push_back(std::make_shared<FSymbol>());
		FSymbolPtr& Symbol = To.back();
		//Copy properties form From symbol 
		CopySymbolProperties(From[i], Symbol);
		Symbol->Parent = Parent;
	}
	//Copy children
	for (size_t i = FromStart; i < FromEnd; i++)
	{
		CopyMathTreeImpl(From[i]->Children, To[i]->Children, To[i]);
	}
}

void TAExpressionsLibrary::CopySymbolProperties(const FSymbolPtr& From, const FSymbolPtr& To)
{
	To->ElementName = From->ElementName;
	To->Info = From->Info;
	To->Expand = From->Expand;
	To->bCommute = From->bCommute;
	To->bHasNonCommuteSuccessor = From->bHasNonCommuteSuccessor;
	To->Type = From->Type;
	To->Captured = From->Captured;
}

void TAExpressionsLibrary::SimplifyMathTreeRecursive(FSymbols& MathTree, bool (*Predicate)(const FSymbolPtr& InSymbol))
{
	//Iterate over math elements to call recursively
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		SimplifyMathTreeRecursive(MathTree[i]->Children, Predicate);
	}
	//Iterate over math elements
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		//if not Math element
		if (Predicate(MathTree[i]))
		{
			ReplaceWithChildren(MathTree, i);
		}
	}
}

void TAExpressionsLibrary::RemoveContextOperator(const std::vector<FSymbolPtr>& Params)
{
	for (auto Param : Params)
	{
		if (Param->bWithContextOperator)
		{
			auto NewArray = Param->Captured[0]->Children;
			Param->Captured = NewArray;
		}
	}
}

void TAExpressionsLibrary::RemoveSurplusOperators(FSymbols& MathTree)
{
	auto Pred = [](const FSymbolPtr& InSymbol)->bool
	{
		return IsOperator(InSymbol) && InSymbol->Parent && IsOperator(InSymbol->Parent,InSymbol->Info);
	};
	SimplifyMathTreeRecursive(MathTree, Pred);
}

void TAExpressionsLibrary::TokenizeNumeric(FSymbols& MathTree)
{
	if (MathTree.empty())
	{
		return;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			TokenizeNumeric(MathTree[i]->Children);
		}
	}

	BranchMaker BranchMaker(MathTree);
	//Iterate over symbols
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		//Check that it is not any excluding symbol and in branch
		bool bNumericPart = IsNumeric(MathTree[i]) || IsCharacter(MathTree[i], L".");
		if (!BranchMaker.IsInBranch() && bNumericPart)
		{
			//Numeric and not in branch-> start branch
			FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::CustomTokenName, FTAMeDefinition::TokenWord);
			Branch->bCommute = true;
			BranchMaker.StartBranch(Branch, i);
		}
		if (BranchMaker.IsInBranch() && !bNumericPart)
		{
			i--; //Before Spec.Symbol
			BranchMaker.EndBranch(i);
			i++; //Skip symbol that goes after number
			continue;
		}
		if (BranchMaker.IsInBranch())
		{
			BranchMaker.AddToCurrentBranch(MathTree[i]);
		}
	}
	if (BranchMaker.IsInBranch())
	{
		size_t i = MathTree.size() - 1;
		BranchMaker.EndBranch(i);
	}
}

void TAExpressionsLibrary::ClearCapturedArguments(const std::vector<FSymbolPtr>& Params)
{
	for (auto Param : Params)
	{
		Param->Captured.clear();
		Param->bOrderFixed = false;
		Param->bWithContextOperator = false;
	}
}

void TAExpressionsLibrary::ReplaceParametersWithArguments(FSymbols& MathTree, const FSymbolPtr& Parent)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (IsParameter(MathTree[i]))
		{
			auto CapturedCopy = CopyMathTree(MathTree[i]->Captured);
			SetParent(Parent, CapturedCopy);
			MathTree.insert(MathTree.begin() + i + 1, CapturedCopy.begin(), CapturedCopy.end());
			MathTree.erase(MathTree.begin() + i);
		}
		else
		{
			ReplaceParametersWithArguments(MathTree[i]->Children, MathTree[i]);
		}
	}
}

void TAExpressionsLibrary::SimplifyMathTree(FSymbols& MathTree)
{
	ExpandMathTreeRecursive(MathTree);
	SimplifyMathTreeRecursive(MathTree);
	ClearExtraPlusSign(MathTree);
}

bool TAExpressionsLibrary::IsMe(const FSymbolPtr& Symbol)
{
	return !NonMeTokens.contains(Symbol->ElementName);
}

bool TAExpressionsLibrary::IsWordWrap(const FSymbolPtr& Symbol)
{
	return IsCharacter(Symbol, L"\n");
}

bool TAExpressionsLibrary::IsCharacter(const FSymbolPtr& Symbol, const std::wstring& Character)
{
	return Symbol->ElementName.empty() && Symbol->Info == Character;
}

bool TAExpressionsLibrary::IsCharacter(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName.empty();
}

bool TAExpressionsLibrary::IsMultiParameter(const FSymbolPtr& Symbol)
{
	return Symbol->Type == FSymbol::EType::MultiParam;
}

bool TAExpressionsLibrary::IsSingleParameter(const FSymbolPtr& Symbol)
{
	return Symbol->Type == FSymbol::EType::UniParam;
}

bool TAExpressionsLibrary::IsParameter(const FSymbolPtr& Symbol)
{
	return Symbol->Type == FSymbol::EType::UniParam || Symbol->Type == FSymbol::EType::MultiParam;
}

bool TAExpressionsLibrary::IsPure(const FSymbolPtr& Symbol)
{
	return Symbol->Type == FSymbol::EType::Pure;
}

bool TAExpressionsLibrary::IsOperator(const FSymbolPtr& Symbol, const std::wstring& Character)
{
	return Symbol->ElementName == FTAMeDefinition::OperatorType && Symbol->Info == Character;
}

bool TAExpressionsLibrary::IsOperator(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::OperatorType;
}

bool TAExpressionsLibrary::IsNonCommute(const FSymbolPtr& InSymbol)
{
	return InSymbol->ElementName == FTAMeDefinition::CustomTokenName && InSymbol->Info == FTAMeDefinition::NonCommuteInfo;
}

bool TAExpressionsLibrary::IsCustomToken(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::CustomTokenName && Symbol->Info != FTAMeDefinition::NonCommuteInfo;
}

bool TAExpressionsLibrary::IsTokenWord(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::CustomTokenName && Symbol->Info == FTAMeDefinition::TokenWord;
}

bool TAExpressionsLibrary::IsTokenExpr(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::CustomTokenName && Symbol->Info == FTAMeDefinition::TokenExpression;
}

bool TAExpressionsLibrary::IsNumeric(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName.empty() && std::iswdigit(Symbol->Info[0]);
}

bool TAExpressionsLibrary::IsInvisibleBracket(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::InvisibleBracketsType;
}

bool TAExpressionsLibrary::CompareSymbols(const FSymbolPtr& First, const FSymbolPtr& Second)
{
	return First->ElementName == Second->ElementName && First->Info == Second->Info;
}

bool TAExpressionsLibrary::CompareSymbolsRecursively(const FSymbolPtr& First, const FSymbolPtr& Second)
{
	return CompareSymbols(First, Second) && CompareSymbolArraysRecursively(First->Children, Second->Children);
}

bool TAExpressionsLibrary::CompareSymbolArraysRecursively(const FSymbols& A, const FSymbols& B)
{
	if (A.size() != B.size())
	{
		return false;
	}
	for (size_t i = 0; i < A.size(); i++)
	{
		if (!CompareSymbols(A[i], B[i]))
		{
			return false;
		}
	}
	//Apply Recursion
	for (size_t i = 0; i < A.size(); i++)
	{
		if (!CompareSymbolArraysRecursively(A[i]->Children, B[i]->Children))
		{
			return false;
		}
	}
	return true;
}

bool TAExpressionsLibrary::CompareSymbolsWCommutativity(const FSymbolPtr& TemplateSymbol, const FSymbolPtr& Input)
{
	if (!CompareSymbols(TemplateSymbol, Input))
	{
		//Top comparison
		return false;
	}
	if (TemplateSymbol->Children.size() != Input->Children.size())
	{
		//Same size
		return false;
	}
	//Deep comparison
	if (TemplateSymbol->bCommute)
	{
		auto ChildrenCopy = Input->Children;
		int MaxInputInd = Input->Children.size();
		//For each template symbol
		for (int i = 0; i < TemplateSymbol->Children.size(); i++)
		{
			bool bFound = false;
			//For each input symbol
			for (int j = 0; j < MaxInputInd; j++)
			{
				if (CompareSymbolsWCommutativity(TemplateSymbol->Children[i], ChildrenCopy[j]))
				{
					//If successfully compared recursively with template symb.
					//Decrease max index and Swap with the element
					//before the last found one at the back
					MaxInputInd--;
					std::swap(ChildrenCopy[j], ChildrenCopy[MaxInputInd]);
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				//If not found any template element -> fail
				return false;
			}
		}
		return true;
	}
	//If non-commute -> compare in order
	for (int i = 0; i < TemplateSymbol->Children.size(); i++)
	{
		if (!CompareSymbolsWCommutativity(TemplateSymbol->Children[i], Input->Children[i]))
		{
			return false;
		}
	}
	return true;
}

void TAExpressionsLibrary::ClearCaptured(const FSymbols& MathTree)
{
	for (auto& Symbol : MathTree)
	{
		Symbol->bCaptured = false;
		ClearCaptured(Symbol->Children);
	}
}

void TAExpressionsLibrary::SetCaptured(const FSymbols& MathTree, bool bVal)
{
	for (auto& Symbol : MathTree)
	{
		Symbol->bCaptured = bVal;
	}
}

int TAExpressionsLibrary::SkipCapturedParameterIndices(const FSymbols& MathTree, int IndexStart)
{
	if (IndexStart >= MathTree.size())
	{
		return MathTree.size();
	}
	if (!MathTree[IndexStart]->bCaptured)
	{
		return IndexStart;
	}
	for (int i = IndexStart; i < MathTree.size(); i++)
	{
		if (!MathTree[i]->bCaptured)
		{
			return i;
		}
	}
	return MathTree.size();
}

bool TAExpressionsLibrary::CanApplyTokenization(const FSymbolPtr& Parent, bool bCanApply)
{
	if (IsTokenWord(Parent))
	{
		bCanApply = false;
	}
	else if (IsTokenExpr(Parent))
	{
		bCanApply = true;
	}
	return bCanApply;
}

bool TAExpressionsLibrary::SimplePatternMatchingImpl(const FSymbols& MathTree, const FSymbols& Template)
{
	//MathTree can't be smaller than Template
	//If one is empty, other can't be non-empty
	if (MathTree.size() < Template.size() || MathTree.empty() != Template.empty())
	{
		return false;
	}
	//If both are empty it's okay
	if (MathTree.empty() && Template.empty())
	{
		return true;
	}
	//create pools Multi-Parameter/Terminating sequense
	std::vector<FSymbolsPool> Pools;
	if (!CreateSymbolPools(Template, Pools))
	{
		return false;
	}
	//Use pools to capture arguments
	if (!CaptureArgumentsWithPools(MathTree, Pools))
	{
		return false;
	}
	//Apply recursion
	int MathTreeInd = 0;
	int TemplateInd = 0;
	while (TemplateInd < Template.size())
	{
		if (!IsParameter(Template[TemplateInd]) && !SimplePatternMatchingImpl(MathTree[MathTreeInd]->Children, Template[TemplateInd]->Children))
		{
			return false;
		}
		MathTreeInd += Template[TemplateInd]->Captured.empty() ? 1 : Template[TemplateInd]->Captured.size();
		TemplateInd++;
	}
	return true;
}

bool TAExpressionsLibrary::CaptureArgumentsWithPools(const FSymbols& MathTree, const std::vector<FSymbolsPool>& Pools)
{
	//First pool non multi
	//last pool has TerminatingSymbols
	int PoolIndex = 0;
	int PoolsSize = Pools.size() - 1;
	int From = 0;
	int To = MathTree.size();

	bool bStartedWithSymbols = !Pools[0].MultiParameter;
	bool bEndedWithSymbols = !Pools.back().TerminatingSymbols.empty();
	if (bStartedWithSymbols)
	{
		//If started with symbols
		if (!CompareWithTerminatingSequence(Pools[0].TerminatingSymbols, MathTree, 0))
		{
			return false;
		}
		From = Pools[0].TerminatingSymbols.size();
		PoolIndex = 1;
		if (Pools.size() == 1)
		{
			//If this is the only terminating sequence, its size must be equal to math tree
			return From == MathTree.size();
		}
	}
	if (bEndedWithSymbols)
	{
		//if ended with symbols
		if (!CompareWithTerminatingSequence(Pools.back().TerminatingSymbols
		                                    , MathTree, MathTree.size() - Pools.back().TerminatingSymbols.size()))
		{
			return false;
		}
		To -= Pools.back().TerminatingSymbols.size();
	}

	for (; PoolIndex < PoolsSize; PoolIndex++)
	{
		int FirstOccurence = 0;
		//From + 1 since we want at least 1 element for Mult param
		if (!FindFirstSequenceOccurence(Pools[PoolIndex].TerminatingSymbols, MathTree, From + 1, To, FirstOccurence))
		{
			return false;
		}
		auto& Captured = Pools[PoolIndex].MultiParameter->Captured;
		if (Captured.empty())
		{
			Captured.insert(Captured.begin(), MathTree.begin() + From, MathTree.begin() + FirstOccurence);
		}
		else
		{
			//If captured->compare
			if (Captured.size() != FirstOccurence - From)
			{
				return false;
			}
			for (int i = 0; i < Captured.size(); i++)
			{
				if (!CompareSymbolsRecursively(Captured[i], MathTree[From + i]))
				{
					return false;
				}
			}
		}
		From = FirstOccurence + Pools[PoolIndex].TerminatingSymbols.size();
	}
	if (From >= To)
	{
		return false;
	}
	//Last pool
	auto& Captured = Pools.back().MultiParameter->Captured;
	if (Captured.empty())
	{
		//If didn't captured-> capture remainder
		Captured.insert(Captured.begin(), MathTree.begin() + From, MathTree.begin() + To);
	}
	else
	{
		if (Captured.size() != To - From)
		{
			return false;
		}
		//If captured -> compare with remainder
		for (int i = 0; i < Captured.size(); i++)
		{
			if (!CompareSymbolsRecursively(Captured[i], MathTree[From + i]))
			{
				return false;
			}
		}
	}

	return true;
}

bool TAExpressionsLibrary::CompareWithTerminatingSequence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From)
{
	if (TerminatingSequence.size() > MathTree.size() - From)
	{
		return false;
	}
	bool bResult = true;
	int i = 0;
	for (; i < TerminatingSequence.size(); i++)
	{
		auto TemplateSymbol = TerminatingSequence[i];
		if (TemplateSymbol->Type == FSymbol::EType::UniParam)
		{
			if (IsSpecialCharacter(MathTree[i + From]))
			{
				bResult = false;
				break;
			}
			if (!TemplateSymbol->Captured.empty())
			{
				//Just compare
				if (!CompareSymbolsRecursively(TemplateSymbol->Captured[0], MathTree[i + From]))
				{
					bResult = false;
					break;
				}
			}
			else
			{
				//Capture
				TemplateSymbol->Captured.push_back(MathTree[i + From]);
				TemplateSymbol->bCaptured = true;
			}
		}
		else
		{
			if (!CompareSymbols(TemplateSymbol, MathTree[i + From]))
			{
				bResult = false;
				break;
			}
		}
	}
	if (!bResult)
	{
		for (; i >= 0; i--)
		{
			if (TerminatingSequence[i]->bCaptured)
			{
				TerminatingSequence[i]->bCaptured = false;
				TerminatingSequence[i]->Captured.clear();
			}
		}
		return false;
	}
	SetCaptured(TerminatingSequence, false);
	return true;
}

bool TAExpressionsLibrary::FindFirstSequenceOccurence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, int To, int& FirstOccurrenceIndex)
{
	int FoundIndex = 0;
	//Set last index to max index s.t TerminatingSequence doesn't go over the MathTree
	int LastIndex = To - TerminatingSequence.size();
	//iterate over math tree from "From" to last reasonable index
	for (size_t i = From; i < LastIndex; i++)
	{
		//if "i" is the start of terminating sequence
		if (CompareWithTerminatingSequence(TerminatingSequence, MathTree, i))
		{
			FoundIndex = i;
			break;
		}
	}
	FirstOccurrenceIndex = FoundIndex;
	return FoundIndex >= From;
}

bool TAExpressionsLibrary::ParseRewritingRules(const std::wstring& Content, FRewritingRules& OutRewritingRules, void (*LogicalParsePtr)(FSymbols& MathTree))
{
	OutRewritingRules.clear();
	if (Content.empty())
	{
		return false;
	}
	auto MathTree = ParseToMathTree(Content);
	RemoveCharacter(MathTree, L" ");
	OutRewritingRules.emplace_back();
	bool bFirst = true;
	for (auto& Symbol : MathTree)
	{
		if (IsCharacter(Symbol, L"="))
		{
			bFirst = false;
			if (OutRewritingRules.back().First.empty())
			{
				return false;
			}
			continue;
		}
		if (IsWordWrap(Symbol))
		{
			bFirst = true;
			if (OutRewritingRules.back().Second.empty())
			{
				return false;
			}
			OutRewritingRules.emplace_back();
			continue;
		}
		auto& SymbolsPool = bFirst ? OutRewritingRules.back().First : OutRewritingRules.back().Second;
		SymbolsPool.push_back(Symbol);
	}
	if (OutRewritingRules.back().First.empty())
	{
		OutRewritingRules.pop_back();
	}
	for (auto& RewritingRule : OutRewritingRules)
	{
		SimplifyTokens(RewritingRule.First);
		SimplifyTokens(RewritingRule.Second);
		LogicalParsePtr(RewritingRule.First);
		LogicalParsePtr(RewritingRule.Second);
		CacheParameters(RewritingRule.First);
		CacheParameters(RewritingRule.Second);
		ReplaceParamCopies(RewritingRule.First, RewritingRule.Parameters);
		ReplaceParamCopies(RewritingRule.Second, RewritingRule.Parameters);
	}
	return true;
}

void TAExpressionsLibrary::LogicalParse0(FSymbols& MathTree)
{
	BracketsToBranch(MathTree, {
		                 {L"(", L")"},
		                 {L"[", L"]"},
		                 {L"{", L"}"},
	                 });
	TokenizeNumeric(MathTree);
	IndicesToBranches(MathTree);
}

void TAExpressionsLibrary::LogicalParse1(FSymbols& MathTree)
{
	BracketsToBranch(MathTree, {
		                 {L"(", L")"},
		                 {L"[", L"]"},
		                 {L"{", L"}"},
	                 });
	TokenizeNumeric(MathTree);
	IndicesToBranches(MathTree);
	MultiplicationToBranches(MathTree);
}

void TAExpressionsLibrary::ApplyRewritingRule1(const FRewritingRule& RewritingRule, FSymbols& MathTree, const FSymbolPtr& Parent, bool bCanRewrite)
{
	if (MathTree.empty())
	{
		return;
	}
	for (auto& Symbol : MathTree)
	{
		if (!LogicParseAvoid(Symbol))
		{
			ApplyRewritingRule1(RewritingRule, Symbol->Children, Symbol, CanApplyTokenization(Symbol, bCanRewrite));
		}
	}

	if (!bCanRewrite || IsContainer(MathTree[0]))
	{
		return;
	}
	for (int i = MathTree.size() - RewritingRule.First.size(); i >= 0; i--)
	{
		for (int j = RewritingRule.First.size(); j <= MathTree.size() - i; j++)
		{
			ClearCapturedArguments(RewritingRule.Parameters);
			if (SimplePatternMatching(MathTree, i, i + j, RewritingRule.First))
			{
				FSymbols FormCopy = CopyMathTree(RewritingRule.Second);
				ReplaceParametersWithArguments(FormCopy);
				SetParent(Parent, FormCopy);
				MathTree.erase(MathTree.begin() + i, MathTree.begin() + i + j);
				MathTree.insert(MathTree.begin() + i, FormCopy.begin(), FormCopy.end());
				break;
			}
		}
	}
}

void TAExpressionsLibrary::ApplyRewritingRule2(const FRewritingRule& RewritingRule, FSymbols& MathTree, const FSymbolPtr& Parent, bool bCanRewrite)
{
	if (MathTree.empty())
	{
		return;
	}

	if (bCanRewrite && !IsContainer(MathTree[0]))
	{
		for (int i = MathTree.size() - RewritingRule.First.size(); i >= 0; i--)
		{
			for (int j = MathTree.size() - i; j >= RewritingRule.First.size(); j--)
			{
				ClearCapturedArguments(RewritingRule.Parameters);
				if (SimplePatternMatching(MathTree, i, i + j, RewritingRule.First))
				{
					FSymbols FormCopy = CopyMathTree(RewritingRule.Second);
					ReplaceParametersWithArguments(FormCopy);
					SetParent(Parent, FormCopy);
					//Refresh multiplication tokenization on FormCopy
					SimplifyMathTreeRecursive(FormCopy, [](const FSymbolPtr& Symbol) { return IsOperator(Symbol, L"*"); });
					MultiplicationToBranches(FormCopy);
					OperatorToBranches(FormCopy, L"*", true);

					MathTree.erase(MathTree.begin() + i, MathTree.begin() + i + j);
					MathTree.insert(MathTree.begin() + i, FormCopy.begin(), FormCopy.end());
					break;
				}
			}
		}
	}

	for (auto& Symbol : MathTree)
	{
		if (!LogicParseAvoid(Symbol))
		{
			ApplyRewritingRule2(RewritingRule, Symbol->Children, Symbol, CanApplyTokenization(Symbol, bCanRewrite));
		}
	}
}

bool TAExpressionsLibrary::SimplePatternMatching(const FSymbols& MathTree, int From, int To, const FSymbols& Template)
{
	FSymbols MathTreeView;
	MathTreeView.insert(MathTreeView.begin(), MathTree.begin() + From, MathTree.begin() + To);
	return SimplePatternMatchingImpl(MathTreeView, Template);
}

void TAExpressionsLibrary::SimplifyTokens(FSymbols& MathTree)
{
	auto Predicate = [](const FSymbolPtr& Symbol)
	{
		return IsContainer(Symbol) && Symbol->Parent && IsCustomToken(Symbol->Parent);
	};
	SimplifyMathTreeRecursive(MathTree, Predicate);
}

bool TAExpressionsLibrary::IsSpecialCharacter(const FSymbolPtr& Symbol, const std::vector<std::wstring>& Except)
{
	static std::vector<std::wstring> Brackets = {L"(", L")", L"[", L"]", L"{", L"}", L"<", L">"};
	static std::vector<std::wstring> Operators = {L"*",L"·", L"/", L"+", L"-", L",", L"="};
	FCharacter Character;
	return To<FCharacter>(Symbol, Character)
		&& (EqualsToAny(Character, Brackets, Except) || EqualsToAny(Character, Operators, Except));
}

TAExpressionsLibrary::FSymbols& ResultOrParentArray(TAExpressionsLibrary::FSymbols& Result, const TAExpressionsLibrary::FSymbolPtr& Parent)
{
	if (Parent)
	{
		return Parent->Children;
	}
	return Result;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::ParseToMathTree(const std::wstring& InData)
{
	enum class EState
	{
		Default,
		ReadingMathElement,
		ReadingInfo,
		ExpectChildren
	} State = EState::Default;

	FSymbols Result;
	FSymbolPtr Parent = nullptr;
	std::wstring MeName;
	std::wstring Info;
	constexpr auto TermChar = L'\\';
	constexpr auto StartChildChar = L'{';
	constexpr auto EndChildChar = L'}';

	for (int i = 0; i < InData.size(); i++)
	{
		if (State == EState::ReadingMathElement)
		{
			//Reading Me
			if (InData[i] == TermChar)
			{
				//End reading Me?
				//start reading info
				Info.clear();
				State = EState::ReadingInfo;
				continue;
			}
			MeName += std::wstring(1, InData[i]);
			continue;
		}
		if (State == EState::ReadingInfo)
		{
			//Reading info
			if (InData[i] == TermChar)
			{
				//Finished info?
				//Create Symbol
				auto Me = MakeSymbol(MeName, Info, Parent);

				ResultOrParentArray(Result, Parent).push_back(Me);
				State = EState::ExpectChildren;
				continue;
			}
			Info += std::wstring(1, InData[i]);
			continue;
		}
		if (State == EState::ExpectChildren)
		{
			//Children started with combination "\{"
			if (i + 1 < InData.size() && InData[i] == '\\' && InData[i + 1] == StartChildChar)
			{
				//If start children
				//Set new parent
				Parent = ResultOrParentArray(Result, Parent).back();
				//Return to default state
				State = EState::Default;
				//increment to skip two chars
				++i;
				continue;
			}
			//back to default state
			State = EState::Default;
		}
		if (State == EState::Default)
		{
			if (InData[i] == TermChar)
			{
				if (i + 1 < InData.size() && InData[i + 1] == EndChildChar)
				{
					//If end children
					if (!Parent)
					{
						//If no parent -> fail
						return {};
					}
					//set parent one level higher
					Parent = Parent->Parent;
					//Skip "\" and continue
					++i;
					continue;
				}
				//Start reading Me
				State = EState::ReadingMathElement;
				MeName.clear();
				continue;
			}
			//Read character
			auto Me = std::make_shared<FSymbol>();
			Me->Info = std::wstring(1, InData[i]);
			Me->Parent = Parent;
			ResultOrParentArray(Result, Parent).push_back(Me);
		}
	}
	if ((State != EState::Default && State != EState::ExpectChildren) || Parent != nullptr)
	{
		return {};
	}
	return Result;
}

std::wstring TAExpressionsLibrary::ParseToString(const FSymbols& MathTree)
{
	std::wstring Result;
	for (const FSymbolPtr& MathEl : MathTree)
	{
		MathEl->ParseToString(Result);
	}
	return Result;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::SetParents(FSymbols&& MathTree)
{
	for (auto& MathEl : MathTree)
	{
		SetParentRecursive(MathEl, MathEl->Children);
	}
	return std::move(MathTree);
}

void TAExpressionsLibrary::SetParentRecursive(const FSymbolPtr& Parent, const FSymbols& InMathTree)
{
	for (auto& MathEl : InMathTree)
	{
		MathEl->Parent = Parent;
		SetParentRecursive(MathEl, MathEl->Children);
	}
}

void TAExpressionsLibrary::SetParent(const FSymbolPtr& Parent, const FSymbols& InMathTree)
{
	for (auto& MathEl : InMathTree)
	{
		MathEl->Parent = Parent;
	}
}

bool TAExpressionsLibrary::BracketsToBranch(FSymbols& MathTree, const std::vector<std::pair<std::wstring, std::wstring>>& StartEndBrackets)
{
	if (MathTree.empty())
	{
		return true;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			BracketsToBranch(MathTree[i]->Children, StartEndBrackets);
		}
	}
	//Skip logic for containers
	if (IsContainer(MathTree[0]))
	{
		return true;
	}
	BranchMaker BranchMaker(MathTree);
	std::vector<std::wstring> RightBracketsStack;
	//Iterate over symbols
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		FCharacter Character;

		//If symbol is character
		bool bIsBracket = false;
		if (To<FCharacter>(MathTree[i], Character))
		{
			//Compare it with left brackets
			for (size_t j = 0; j < StartEndBrackets.size(); j++)
			{
				//if it equals to one of left bracket
				if (Character == StartEndBrackets[j].first)
				{
					//Add R Bracket to stack
					RightBracketsStack.push_back(StartEndBrackets[j].second);
					//Create branch symbol
					FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::InvisibleBracketsType, StartEndBrackets[j].first + StartEndBrackets[j].second);
					//Add expanding function
					Branch->Expand = &ExpandInvisibleBrackets;
					//Start branch
					BranchMaker.StartBranch(Branch, i);
					bIsBracket = true;
					break;
				}
			}
			//Check for right bracket
			if (!bIsBracket && BranchMaker.IsInBranch())
			{
				//If equals to the last closing bracket in stack
				if (!RightBracketsStack.empty() && Character == RightBracketsStack.back())
				{
					//if closes already closed branch
					if (!BranchMaker.IsInBranch())
					{
						return false;
					}
					//Remove R bracket from stack
					RightBracketsStack.pop_back();
					bIsBracket = true;
					BranchMaker.EndBranch(i);
				}
			}
		}
		//if still is not any bracket and in branch
		if (!bIsBracket && BranchMaker.IsInBranch())
		{
			BranchMaker.AddToCurrentBranch(MathTree[i]);
		}
	}
	if (BranchMaker.IsInBranch())
	{
		return false;
	}
	return true;
}

bool TAExpressionsLibrary::MultiplicationToBranches(FSymbols& MathTree, bool bCanApply)
{
	if (MathTree.empty())
	{
		return true;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			MultiplicationToBranches(MathTree[i]->Children, CanApplyTokenization(MathTree[i], bCanApply));
		}
	}
	//Skip logic for containers, indices and tokens
	if (IsContainer(MathTree[0]) ||
		(MathTree[0]->Parent && IsOperator(MathTree[0]->Parent, L"^")) ||
		!bCanApply)
	{
		return true;
	}

	BranchMaker BranchMaker(MathTree);
	//Iterate over symbols
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		//Check that it is not any excluding symbol and in branch
		bool bIsSpecialChar = IsSpecialCharacter(MathTree[i]);
		if (!BranchMaker.IsInBranch() && !bIsSpecialChar
			&& IsValidIndex(MathTree, i + 1) && !IsSpecialCharacter(MathTree[i + 1]))
		{
			//Start branch
			FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::OperatorType, L"*");
			Branch->bCommute = true;
			BranchMaker.StartBranch(Branch, i);
		}
		if (BranchMaker.IsInBranch() && bIsSpecialChar)
		{
			i--; //Before Spec.Symbol
			BranchMaker.EndBranch(i);
			i++; //To spec.symbol
			continue;
		}
		if (BranchMaker.IsInBranch())
		{
			BranchMaker.AddToCurrentBranch(MathTree[i]);
		}
	}
	if (BranchMaker.IsInBranch())
	{
		size_t i = MathTree.size() - 1;
		BranchMaker.EndBranch(i);
	}
	return true;
}

bool TAExpressionsLibrary::OperatorToBranches(FSymbols& MathTree, const std::wstring& Operator, bool bCommute, bool bCanApply)
{
	if (MathTree.empty())
	{
		return true;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			OperatorToBranches(MathTree[i]->Children, Operator, bCommute, CanApplyTokenization(MathTree[i], bCanApply));
		}
	}
	//Skip logic for containers
	if (IsContainer(MathTree[0]) || !bCanApply)
	{
		return true;
	}
	BranchMaker BranchMaker(MathTree);
	//Iterate over symbols
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		//(|-a|)
		//(|b-a|)
		//(c+|b-a-d|)
		//(c+|b-a-d|+k)
		bool bIsOperator = false;
		FCharacter Character;
		if (To<FCharacter>(MathTree[i], Character))
		{
			bIsOperator = Character == Operator;
			if (!BranchMaker.IsInBranch() && bIsOperator)
			{
				//Example : if Operator "-", then if tree = "(-a-b)", "-a-b" -> to branch
				//if "(a-b-c+k)" -> start from a, "a-b-c" to branch
				if (!IsValidIndex(MathTree, i - 1) || IsSpecialCharacter(MathTree[i - 1]))
				{
					FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::OperatorType, Operator + Operator);
					Branch->bCommute = bCommute;
					Branch->Expand = &ExpandOperator;
					//Start branch at operator
					BranchMaker.StartBranch(Branch, i);
				}
				else
				{
					//start brunch at symbol before operator
					i--;
					FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::OperatorType, Operator);
					Branch->bCommute = bCommute;
					Branch->Expand = &ExpandOperator;
					BranchMaker.StartBranch(Branch, i);
					bIsOperator = false;
				}
			}
			else if (BranchMaker.IsInBranch() && IsSpecialCharacter(MathTree[i], {Operator}))
			{
				i--; //Before Spec.Symbol
				BranchMaker.EndBranch(i);
				i++; //To spec.symbol
				continue;
			}
		}
		//Add all symbols in branch except operator
		if (BranchMaker.IsInBranch() && !bIsOperator)
		{
			BranchMaker.AddToCurrentBranch(MathTree[i]);
		}
	}
	if (BranchMaker.IsInBranch())
	{
		size_t i = MathTree.size() - 1;
		BranchMaker.EndBranch(i);
	}
	return true;
}

bool TAExpressionsLibrary::IndicesToBranches(FSymbols& MathTree)
{
	if (MathTree.empty())
	{
		return true;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			IndicesToBranches(MathTree[i]->Children);
		}
	}
	//Skip logic for containers
	if (IsContainer(MathTree[0]))
	{
		return true;
	}
	BranchMaker BranchMaker(MathTree);
	//Iterate over symbols
	for (size_t i = 1; i < MathTree.size(); i++)
	{
		bool bIsIndex = FIndex::IsA(MathTree[i]);
		//Haven't started branch yet and is an index
		if (!BranchMaker.IsInBranch() && bIsIndex)
		{
			//start brunch at symbol before index
			i--;
			BranchMaker.StartBranch(MakeSymbol(FTAMeDefinition::OperatorType, L"^"), i);
		}
		//if already in branch and faced non-index
		else if (BranchMaker.IsInBranch() && !bIsIndex)
		{
			//end branch
			i--;
			BranchMaker.EndBranch(i);
			i++;
		}
		if (BranchMaker.IsInBranch())
		{
			BranchMaker.AddToCurrentBranch(MathTree[i]);
		}
	}
	if (BranchMaker.IsInBranch())
	{
		size_t i = MathTree.size() - 1;
		BranchMaker.EndBranch(i);
	}
	return true;
}

bool TAExpressionsLibrary::EncapsulateMinusSign(FSymbols& MathTree)
{
	if (MathTree.empty())
	{
		return true;
	}
	for (size_t i = 0; i < MathTree.size(); i++)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			EncapsulateMinusSign(MathTree[i]->Children);
		}
	}

	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (IsCharacter(MathTree[i], L"-"))
		{
			if (MathTree.size() == i + 1)
			{
				return false;
			}
			//cache negated symbol
			std::shared_ptr<FSymbol> NegativeSymbol = MathTree[i + 1];
			//Create branch and add "-S" into it
			FSymbolPtr Branch = MakeSymbol(FTAMeDefinition::OperatorType, L"-", MathTree[0]->Parent);
			Branch->Children.push_back(NegativeSymbol);
			Branch->Expand = &ExpandNegation;
			//Set branch as a parent
			NegativeSymbol->Parent = Branch;
			MathTree[i]->Parent = Branch;
			//erase "-S" from previous parent
			MathTree.erase(MathTree.begin() + i, MathTree.begin() + i + 2);
			//insert "+Branch"
			MathTree.insert(MathTree.begin() + i, Branch);
			if (i > 0)
			{
				//Do not add plus sign for unary minus
				MathTree.insert(MathTree.begin() + i, MakeSymbol(L"", L"+", MathTree[0]->Parent));
			}
		}
	}
	return true;
}

void TAExpressionsLibrary::CacheParameters(const FSymbols& MathTree)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (MathTree[i]->ElementName == FTAMeDefinition::ParamName)
		{
			if (MathTree[i]->Info == FTAMeDefinition::MultiParamInfo)
			{
				MathTree[i]->Type = FSymbol::EType::MultiParam;
			}
			else if (MathTree[i]->Info == FTAMeDefinition::SingleParamInfo)
			{
				MathTree[i]->Type = FSymbol::EType::UniParam;
			}
			MathTree[i]->bCaptured = true;
		}
	}
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		CacheParameters(MathTree[i]->Children);
	}
}

void TAExpressionsLibrary::LogicalParse(FSymbols& MathTree, const std::vector<FRewritingRules>& RewritingRules)
{
	RemoveCharacter(MathTree, L" ");
	RemoveCharacter(MathTree, L"\n");
	RemoveCharacter(MathTree, L"\t");

	//Parsing level 1
	BracketsToBranch(MathTree, {
		                 {L"(", L")"},
		                 {L"[", L"]"},
		                 {L"{", L"}"},
	                 });
	TokenizeNumeric(MathTree);
	IndicesToBranches(MathTree);
	//Apply Tokenization 1
	if (!RewritingRules.empty())
	{
		for (auto& RewritingRule : RewritingRules[0])
		{
			ApplyRewritingRule1(RewritingRule, MathTree);
		}
	}

	//Parsing level 2
	MultiplicationToBranches(MathTree);
	//Apply Tokenization 2
	if (!RewritingRules.empty())
	{
		for (auto& RewritingRule : RewritingRules[1])
		{
			ApplyRewritingRule2(RewritingRule, MathTree);
		}
	}


	//Parsing level 3
	OperatorToBranches(MathTree, L"*", true);
	OperatorToBranches(MathTree, L"·", true);
	EncapsulateMinusSign(MathTree);
	OperatorToBranches(MathTree, L"+", true);
}

bool TAExpressionsLibrary::LogicParseAvoid(const FSymbolPtr& Symbol)
{
	return Symbol->Type == FSymbol::EType::MultiParam
		|| Symbol->Type == FSymbol::EType::UniParam
		|| Symbol->ElementName == FTAMeDefinition::TextBlockName;
}

void TAExpressionsLibrary::ReplaceWithChildren(FSymbols& MathTree, int Index)
{
	//iterate over its children and set parent one level higher
	for (auto& Child : MathTree[Index]->Children)
	{
		Child->Parent = MathTree[Index]->Parent;
	}
	//Insert children next to parent
	MathTree.insert(MathTree.begin() + Index + 1, MathTree[Index]->Children.begin(), MathTree[Index]->Children.end());
	//Remove parent
	MathTree.erase(MathTree.begin() + Index);
}

void TAExpressionsLibrary::RemoveFromParent(const FSymbolPtr& Symbol, int Index)
{
	if (!Symbol || !Symbol->Parent)
	{
		return;
	}
	//If index is known
	if (Index >= 0 && Index < Symbol->Parent->Children.size())
	{
		Symbol->Parent->Children.erase(Symbol->Parent->Children.begin() + Index);
		return;
	}
	//Otherwise find index
	for (int i = Symbol->Parent->Children.size() - 1; i >= 0; i--)
	{
		if (Symbol->Parent->Children[i] == Symbol)
		{
			Symbol->Parent->Children.erase(Symbol->Parent->Children.begin() + i);
		}
	}
}

void TAExpressionsLibrary::RemoveCharacter(FSymbols& MathTree, const std::wstring& Character)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (IsCharacter(MathTree[i], Character))
		{
			MathTree.erase(MathTree.begin() + i);
		}
	}
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			RemoveCharacter(MathTree[i]->Children, Character);
		}
	}
}

void TAExpressionsLibrary::ReplaceSymbol(FSymbols& MathTree, const FSymbolPtr& InWhatReplace, const std::wstring& InReplaceWith)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (CompareSymbols(MathTree[i], InWhatReplace))
		{
			MathTree.erase(MathTree.begin() + i);
			FSymbols ReplaceWithTree = ParseToMathTree(InReplaceWith);
			MathTree.insert(MathTree.begin() + i, ReplaceWithTree.begin(), ReplaceWithTree.end());
		}
		else
		{
			ReplaceSymbol(MathTree[i]->Children, InWhatReplace, InReplaceWith);
		}
	}
}

void TAExpressionsLibrary::RemoveAllSymbols(FSymbols& MathTree, const FSymbolPtr& InWhatRemove)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (CompareSymbols(MathTree[i], InWhatRemove))
		{
			MathTree.erase(MathTree.begin() + i);
		}
		else
		{
			RemoveAllSymbols(MathTree[i]->Children, InWhatRemove);
		}
	}
}

bool TAExpressionsLibrary::FindPathToFirst(const FSymbols& MathTree, const FSymbolPtr& InFindSymbol, std::vector<int>& OutPath)
{
	OutPath.push_back(0);
	int DepthIndex = OutPath.size() - 1;
	for (; OutPath[DepthIndex] < MathTree.size(); OutPath[DepthIndex]++)
	{
		if (CompareSymbols(MathTree[OutPath[DepthIndex]], InFindSymbol))
		{
			return true;
		}
		if (FindPathToFirst(MathTree[OutPath[DepthIndex]]->Children, InFindSymbol, OutPath))
		{
			return true;
		}
	}
	OutPath.pop_back();
	return false;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::CopyMathTree(const FSymbols& MathTree)
{
	FSymbols Result;
	CopyMathTreeImpl(MathTree, Result, nullptr);
	return Result;
}

TAExpressionsLibrary::FSymbols TAExpressionsLibrary::CopyMathTreeView(const FSymbols& MathTree, int From, int To)
{
	FSymbols Result;
	CopyMathTreeViewImpl(MathTree, From, To, Result, nullptr);
	return Result;
}

bool TAExpressionsLibrary::SizeCheck(const FSymbols& MathTree, const FSymbols& Template)
{
	if (MathTree.size() < Template.size())
	{
		return false;
	}
	if (!HasMultiParam(Template) && MathTree.size() != Template.size())
	{
		return false;
	}
	return true;
}

void TAExpressionsLibrary::SortCommutativeNodes(FSymbols& MathTree, const FSymbolPtr& Parent)
{
	if (MathTree.empty())
	{
		return;
	}
	for (auto& Symbol : MathTree)
	{
		if (!LogicParseAvoid(Symbol))
		{
			SortCommutativeNodes(Symbol->Children, Symbol);
		}
	}
	if (Parent && Parent->bCommute)
	{
		CacheChildNodesCount(MathTree);
		std::ranges::sort(MathTree, SortingPredicate);
	}
}

bool TAExpressionsLibrary::SortingPredicate(const FSymbolPtr& First, const FSymbolPtr& Second)
{
	if (IsParameter(First) != IsParameter(Second))
	{
		//If one of them is parameter and another is not
		return !IsParameter(First);
	}
	if (!IsParameter(First))
	{
		//If both aren't parameters
		if (IsPure(First) != IsPure(Second))
		{
			//Pure goes first
			return IsPure(First);
		}
		if (IsPure(First))
		{
			return First->bHasNonCommuteSuccessor;
		}
		return First->NonParamCount > Second->NonParamCount;
	}
	//If both of them parameters
	return First->Type == FSymbol::EType::UniParam;
}

void TAExpressionsLibrary::CacheChildNodesCount(FSymbols& MathTree)
{
	for (auto& Symbol : MathTree)
	{
		if (Symbol->Type == FSymbol::EType::NonPure)
		{
			//if non param and not pure
			int ChildNodesCount = 0;
			FindChildNodesCountRecursive(Symbol, ChildNodesCount);
			Symbol->NonParamCount = ChildNodesCount;
		}
	}
}

void TAExpressionsLibrary::FindChildNodesCountRecursive(const FSymbolPtr& InSymbol, int& Count)
{
	for (auto& Symbol : InSymbol->Children)
	{
		if (!IsParameter(Symbol))
		{
			Count++;
			FindChildNodesCountRecursive(Symbol, Count);
		}
	}
}

void TAExpressionsLibrary::ReplaceParamCopies(FSymbols& MathTree, std::vector<FSymbolPtr>& OriginalParams)
{
	for (auto& Symbol : MathTree)
	{
		if (IsParameter(Symbol))
		{
			//If a parameter
			//Find it in OriginalParams
			bool bFound = false;
			for (auto& OriginalParam : OriginalParams)
			{
				if (CompareSymbolsRecursively(Symbol, OriginalParam))
				{
					Symbol = OriginalParam;
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				//If not found->add to array
				OriginalParams.push_back(Symbol);
			}
		}
		else
		{
			ReplaceParamCopies(Symbol->Children, OriginalParams);
		}
	}
}

void TAExpressionsLibrary::MarkNodesWParamSuccessor(FSymbols& MathTree)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (!IsParameter(MathTree[i]))
		{
			MarkNodesWParamSuccessor(MathTree[i]);
		}
	}
}

void TAExpressionsLibrary::MarkNodesWParamSuccessor(FSymbolPtr Parent)
{
	auto& Children = Parent->Children;
	for (int i = Children.size() - 1; i >= 0; i--)
	{
		if (!IsParameter(Children[i]))
		{
			MarkNodesWParamSuccessor(Children[i]);
		}
		else
		{
			while (Parent)
			{
				Parent->Type = FSymbol::EType::NonPure;
				Parent = Parent->Parent;
			}
		}
	}
}

void TAExpressionsLibrary::MarkNodesWNonCommuteSuccessor(FSymbols& MathTree)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			MarkNodesWNonCommuteSuccessor(MathTree[i]->Children);
			if (IsNonCommute(MathTree[i]))
			{
				FSymbolPtr Parent = MathTree[i]->Parent;
				while (Parent)
				{
					Parent->bHasNonCommuteSuccessor = true;
					Parent = Parent->Parent;
				}
			}
		}
	}
}

void TAExpressionsLibrary::ExpandNonCommutative(FSymbols& MathTree)
{
	for (int i = MathTree.size() - 1; i >= 0; i--)
	{
		if (!LogicParseAvoid(MathTree[i]))
		{
			ExpandNonCommutative(MathTree[i]->Children);
			if (IsNonCommute(MathTree[i]))
			{
				auto Container = MathTree[i]->Children[0];
				MathTree.insert(MathTree.begin() + i + 1, Container->Children.begin(), Container->Children.end());
				MathTree.erase(MathTree.begin() + i);
				if (IsOperator(MathTree[i]))
				{
					MathTree[i]->bCommute = false;
				}
			}
		}
	}
}

bool TAExpressionsLibrary::HasMultiParam(const FSymbols& MathTree)
{
	for (auto& Symbol : MathTree)
	{
		if (Symbol->Type == FSymbol::EType::MultiParam)
		{
			return true;
		}
	}
	return false;
}


bool TAExpressionsLibrary::FComparatorTest::Compare(FSymbols& MathTree, FSymbols& Template)
{
	if (MathTree.size() < Template.size() || MathTree.empty())
	{
		return false;
	}
	FSymbols Parameters;
	{
		// Initialize the first state (scope is important since FirstState is a reference to an array element)
		//Add FStatesAndVariator
		StatesAndVariators.push_back(std::make_shared<FStatesAndVariator>());
		CurrentStatesAndVariator = StatesAndVariators.back();
		//Add State
		CurrentStatesAndVariator->States.emplace_back();
		auto& FirstState = CurrentStatesAndVariator->States.back();
		//Set its properties
		FirstState.Input = &MathTree;
		FirstState.Template = &Template;
		FirstState.Stage = EComparisonStages::OrderedTop;
		FirstState.CompareStage = &FComparatorTest::OrderedTopComparison;
	}
	EComparatorResult Result = EComparatorResult::Continue;
	int LoopsCount = 0;
	while (true)
	{
		LoopsCount++;
		if (LoopsCount > 100000)
		{
			return false;
		}
		auto CurrSt = CurrentStatesAndVariator->States.back();
		auto CompareStageResult = CurrSt.CompareStage(this);
		if (CompareStageResult == EComparatorResult::Continue)
		{
			continue;
		}
		bool bStepUpArg = CompareStageResult == EComparatorResult::Success;

		if (auto StepUpResult = StepUp(bStepUpArg))
		{
			return StepUpResult.bEndComparisonResult;
		}
	}
	return true;
}

TAExpressionsLibrary::FStepUpResult TAExpressionsLibrary::FComparatorTest::StepUp(bool bResult)
{
	CurrentStatesAndVariator->States.pop_back();
	if (!CurrentStatesAndVariator->States.empty() && !(CurrentStatesAndVariator->States.size() < CurrentStatesAndVariator->MinStateSize && !bResult))
	{
		CurrentStatesAndVariator->MinStateSize = (std::min)(CurrentStatesAndVariator->MinStateSize, static_cast<int>(CurrentStatesAndVariator->States.size()));
		CurrentStatesAndVariator->States.back().bLastResult = bResult;
		//If successfully returned to previous state
		return FStepUpResult::Continue;
	}
	//If there is no state left
	if (!bResult)
	{
		//If step up with fail -> advancing the last variator:
		//Remove last states/variator 
		StatesAndVariators.pop_back();
		if (StatesAndVariators.empty())
		{
			CurrentStatesAndVariator = nullptr;
			//If there are no more states/variators -> fail comparison
			return FStepUpResult::FailedComparison;
		}
		//If there is a variator-> try to advance it
		CurrentStatesAndVariator = StatesAndVariators.back();
		if (CurrentStatesAndVariator->Variator->NextVariation())
		{
			//If Successfully advanced:
			//Cache Variator's state
			auto& StateOfLastVariator = StatesAndVariators.back()->States;
			//Create new states/variators
			StatesAndVariators.emplace_back(std::make_shared<FStatesAndVariator>());
			//Cache it
			CurrentStatesAndVariator = StatesAndVariators.back();
			//Set Variator's state here
			CurrentStatesAndVariator->States = StateOfLastVariator;
			return FStepUpResult::Continue;
		}
		//If failed to advance variator -> Step up with fail
		return StepUp(false);
	}

	return FStepUpResult::SuccessComparison;
}

bool TAExpressionsLibrary::FComparatorTest::StepDown()
{
	//Cache last state
	auto& LastState = CurrentStatesAndVariator->States.back();
	//Cache templ/Input symb
	auto TemplSymbol = LastState.GetTemplSymbol();
	auto InputSymb = LastState.GetInputSymbol();
	if (InputSymb->Children.size() < TemplSymbol->Children.size())
	{
		//If Input less than Template
		return false;
	}
	if (TemplSymbol->Children.size() == 0 && InputSymb->Children.size() == 0)
	{
		//If both empty
		LastState.bLastResult = true;
		return true;
	}
	//Create new state
	FComparisonState NewState;
	NewState.Stage = TemplSymbol->bCommute ? EComparisonStages::UnorderedPure : EComparisonStages::OrderedTop;
	NewState.CompareStage = TemplSymbol->bCommute ? &FComparatorTest::UnorderedComparisonPure : &FComparatorTest::OrderedTopComparison;
	NewState.Template = &TemplSymbol->Children;
	NewState.TemplateParent = TemplSymbol;
	if (TemplSymbol->bCommute)
	{
		//If commute -> use children copy buffer, so that we could reorder input symbols
		NewState.InputCopy = InputSymb->Children;
		NewState.Input = &NewState.InputCopy;
	}
	else
	{
		//If ordered->use children directly
		NewState.Input = &LastState.GetInputSymbol()->Children;
	}
	//Add new state (invalidates pointers to states from this array)
	CurrentStatesAndVariator->States.push_back(std::move(NewState));
	return true;
}


TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::OrderedTopComparison(FComparatorTest* Cmp)
{
	//Get current state
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	if (CurrSt->bFirstEnterStage)
	{
		CurrSt->bFirstEnterStage = false;
	}
	//Iterate over template
	while (CurrSt->TemplInd < CurrSt->Template->size())
	{
		if (CurrSt->GetTemplSymbol()->Type == FSymbol::EType::Pure)
		{
			//If Pure subtree
			if (!CompareSymbolsWCommutativity(CurrSt->GetTemplSymbol(), CurrSt->GetInputSymbol()))
			{
				return EComparatorResult::Failed;
			}
			CurrSt->InputInd++;
			CurrSt->TemplInd++;
		}
		else if (CurrSt->GetTemplSymbol()->Type == FSymbol::EType::NonPure)
		{
			//If Non Pure subtree
			if (!CompareSymbols(CurrSt->GetTemplSymbol(), CurrSt->GetInputSymbol()))
			{
				return EComparatorResult::Failed;
			}
			CurrSt->GetTemplSymbol()->CompareIndex = CurrSt->InputInd;
			CurrSt->InputInd++;
			CurrSt->TemplInd++;
		}
		else if (CurrSt->GetTemplSymbol()->Type == FSymbol::EType::UniParam)
		{
			//If UniParam
			if (CurrSt->GetTemplSymbol()->Captured.empty())
			{
				//If the parameter hasn't captured anything yet
				//Create variator
				if (!Cmp->Checkpoint<FOrderedUniVariator>())
				{
					return EComparatorResult::Failed;
				}
				CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
			}
			else
			{
				//If already captured something
				if (!CompareSymbolsWCommutativity(CurrSt->GetTemplSymbol()->Captured[0], CurrSt->GetInputSymbol()))
				{
					return EComparatorResult::Failed;
				}
				CurrSt->InputInd++;
				CurrSt->TemplInd++;
			}
		}
		else if (CurrSt->GetTemplSymbol()->Type == FSymbol::EType::MultiParam)
		{
			//If MultiParam
			if (CurrSt->GetTemplSymbol()->Captured.empty())
			{
				//If the parameter hasn't captured anything yet
				//Create variator
				Cmp->Checkpoint<FOrderedMultiVariator>();
				CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
			}
			else
			{
				bool bWillFixOrder = !CurrSt->GetTemplSymbol()->bOrderFixed;
				if (CompareWithParamOrdered(CurrSt))
				{
					if (bWillFixOrder)
					{
						Cmp->Checkpoint<FSingleVariation>();
						CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
					}
				}
				else
				{
					return EComparatorResult::Failed;
				}
			}
		}
		if (CurrSt->TemplInd == CurrSt->Template->size() && CurrSt->InputInd != CurrSt->Input->size())
		{
			//If Template iteration finished before Input
			return EComparatorResult::Failed;
		}
	}
	//Request deep comparison as a next step
	CurrSt->TemplInd = -1;
	CurrSt->InputInd = -1;
	CurrSt->bFirstEnterStage = true;
	CurrSt->Stage = EComparisonStages::OrderedDepth;
	CurrSt->CompareStage = &FComparatorTest::OrderedDeepComparison;
	return EComparatorResult::Continue;
}

TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::OrderedDeepComparison(FComparatorTest* Cmp)
{
	//Get current state
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	if (!CurrSt->bFirstEnterStage && !CurrSt->bLastResult)
	{
		return EComparatorResult::Failed;
	}
	CurrSt->bFirstEnterStage = false;
	//Iterate over template
	while (CurrSt->TemplInd < static_cast<int>(CurrSt->Template->size()) - 1)
	{
		CurrSt->TemplInd++;
		//Cache Templ/Input symbols
		auto TemplateSymb = CurrSt->GetTemplSymbol();
		if (TemplateSymb->Type == FSymbol::EType::NonPure)
		{
			CurrSt->InputInd = TemplateSymb->CompareIndex;
			//Step down invalidates CurrSt
			if (!Cmp->StepDown())
			{
				return EComparatorResult::Failed;
			}
			return EComparatorResult::Continue;
		}
	}
	return EComparatorResult::Success;
}

TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::UnorderedComparisonPure(FComparatorTest* Cmp)
{
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	//For each template pure symbol
	for (; CurrSt->TemplInd < CurrSt->Template->size(); CurrSt->TemplInd++, CurrSt->InputInd++)
	{
		if (CurrSt->GetTemplSymbol()->Type != FSymbol::EType::Pure)
		{
			break;
		}
		//Try each Input symbol
		bool bResult = false;
		for (int i = CurrSt->TemplInd; i < CurrSt->Input->size(); i++)
		{
			bResult = CompareSymbolsWCommutativity(CurrSt->GetTemplSymbol(), CurrSt->Input->at(i));
			if (bResult)
			{
				//If they are same->swap s.t. template and input pure symbols are in the same order
				std::swap((*CurrSt->Input)[CurrSt->TemplInd], (*CurrSt->Input)[i]);
				break;
			}
		}
		if (!bResult)
		{
			return EComparatorResult::Failed;
		}
	}
	CurrSt->bFirstEnterStage = true;
	CurrSt->Stage = EComparisonStages::UnorderedNonPure;
	CurrSt->CompareStage = &FComparatorTest::UnorderedComparisonNonPure;
	return EComparatorResult::Continue;
}

TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::UnorderedComparisonNonPure(FComparatorTest* Cmp)
{
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	if (!CurrSt->bFirstEnterStage)
	{
		//If we returned from nodes comparison 
		if (CurrSt->bLastResult)
		{
			//If comparison succeeded
			//Put Input symbol at the template's symbol index
			std::swap((*CurrSt->Input)[CurrSt->TemplInd], (*CurrSt->Input)[CurrSt->InputInd]);
			//Next template symb.
			CurrSt->TemplInd++;
			CurrSt->InputInd = CurrSt->TemplInd;
			if (CurrSt->TemplateEnded())
			{
				//If this was last symbol
				return CurrSt->InputEnded() ? EComparatorResult::Success : EComparatorResult::Failed;
			}
		}
		else
		{
			CurrSt->InputInd++;
		}
	}
	else
	{
		//If we step here for the first time
		//Suppose that prev. stage set indices s.t. TempInd == InputInd
		CurrSt->bFirstEnterStage = false;
	}
	if (CurrSt->GetTemplSymbol()->Type != FSymbol::EType::NonPure)
	{
		//If successfully reached the end or the parameter
		CurrSt->bFirstEnterStage = true;
		CurrSt->InputInd = CurrSt->TemplInd;
		CurrSt->Stage = EComparisonStages::UnorderedUniParam;
		CurrSt->CompareStage = &FComparatorTest::UnorderedComparisonUniParam;
		return EComparatorResult::Continue;
	}

	for (; CurrSt->InputInd < CurrSt->Input->size(); CurrSt->InputInd++)
	{
		if (CompareSymbols(CurrSt->GetInputSymbol(), CurrSt->GetTemplSymbol()) && Cmp->StepDown())
		{
			//If symbols are same and can step down
			return EComparatorResult::Continue;
		}
	}
	return EComparatorResult::Failed;
}

TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::UnorderedComparisonUniParam(FComparatorTest* Cmp)
{
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	while (true)
	{
		if (CurrSt->TemplateEnded())
		{
			//If this was last symbol
			return CurrSt->InputEnded() ? EComparatorResult::Success : EComparatorResult::Failed;
		}
		auto TemplateSymb = CurrSt->GetTemplSymbol();
		if (TemplateSymb->Type == FSymbol::EType::UniParam)
		{
			if (TemplateSymb->Captured.empty())
			{
				//If the parameter hasn't captured anything yet
				//Create variator
				Cmp->Checkpoint<FUnorderedUniVariator>();
				CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
				continue;
			}
			bool bResult = false;
			//If parameter already captured
			//find same symbol among remaining symbols
			for (int i = CurrSt->InputInd; i < CurrSt->Input->size(); i++)
			{
				if (CompareSymbolsWCommutativity(CurrSt->GetTemplSymbol()->Captured[0], CurrSt->Input->at(i)))
				{
					bResult = true;
					std::swap((*CurrSt->Input)[CurrSt->TemplInd], (*CurrSt->Input)[i]);
					break;
				}
			}
			if (!bResult)
			{
				return EComparatorResult::Failed;
			}
			CurrSt->TemplInd++;
			CurrSt->InputInd = CurrSt->TemplInd;
		}
		else
		{
			break;
		}
	}
	//If successfully reached the end or the parameter
	CurrSt->bFirstEnterStage = true;
	CurrSt->InputInd = CurrSt->TemplInd;
	CurrSt->Stage = EComparisonStages::UnorderedMultiParam;
	CurrSt->CompareStage = &FComparatorTest::UnorderedComparisonMultiParam;
	return EComparatorResult::Continue;
}

TAExpressionsLibrary::EComparatorResult TAExpressionsLibrary::FComparatorTest::UnorderedComparisonMultiParam(FComparatorTest* Cmp)
{
	FComparisonState* CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
	while (true)
	{
		if (CurrSt->TemplateEnded())
		{
			//If this was last symbol
			return CurrSt->InputEnded() ? EComparatorResult::Success : EComparatorResult::Failed;
		}
		auto TemplateSymb = CurrSt->GetTemplSymbol();
		if (TemplateSymb->Type == FSymbol::EType::MultiParam)
		{
			if (TemplateSymb->Captured.empty())
			{
				//If the parameter hasn't captured anything yet
				//Create variator
				Cmp->Checkpoint<FUnorderedMultiVariator>();
				CurrSt = &Cmp->CurrentStatesAndVariator->States.back();
				continue;
			}
			if (!CompareWithParamUnordered(CurrSt))
			{
				return EComparatorResult::Failed;
			}
		}
		else
		{
			break;
		}
	}
	return EComparatorResult::Success;
}

bool TAExpressionsLibrary::FComparatorTest::CompareWithParamOrdered(FComparisonState* State)
{
	auto Parameter = State->GetTemplSymbol();
	FSymbols* CapturedPtr = nullptr;
	CapturedPtr = &Parameter->Captured;
	if (Parameter->Captured.size() == 1 && IsOperator(Parameter->Captured[0]))
	{
		//If only one argument and is operator
		if (Parameter->Captured[0]->Children.size() == 1
			|| (IsOperator(State->TemplateParent) && CompareSymbols(State->TemplateParent, Parameter->Captured[0])))
		{
			//If operator has one element -> Use its child
			//If this is the same operator as that of state -> use children of operator
			CapturedPtr = &Parameter->Captured[0]->Children;
		}
	}
	int FirstInputInd = State->InputInd;
	int LastInputInd = FirstInputInd + CapturedPtr->size();
	if (LastInputInd > State->Input->size())
	{
		return false;
	}
	//Clear captured flag
	for (auto& Symbol : *CapturedPtr)
	{
		Symbol->bCaptured = false;
	}
	if (!Parameter->bOrderFixed)
	{
		Parameter->bOrderFixed = true;
		//If captured without order
		//Sort to have same order as input
		for (; State->InputInd < LastInputInd; State->InputInd++)
		{
			bool bFound = false;
			auto InputSymb = State->GetInputSymbol();
			for (int i = 0; i < CapturedPtr->size(); i++)
			{
				if (CompareSymbolsWCommutativity(CapturedPtr->at(i), InputSymb))
				{
					CapturedPtr->at(i)->bCaptured = true;
					std::swap(CapturedPtr->at(i), CapturedPtr->at(State->InputInd - FirstInputInd));
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				return false;
			}
		}
	}
	else
	{
		//If order already fixed -> compare in order
		for (; State->InputInd < LastInputInd; State->InputInd++)
		{
			if (!CompareSymbolsWCommutativity(State->GetInputSymbol(), CapturedPtr->at(State->InputInd - FirstInputInd)))
			{
				return false;
			}
		}
	}
	State->TemplInd++;
	return true;
}

bool TAExpressionsLibrary::FComparatorTest::CompareWithParamUnordered(FComparisonState* State)
{
	auto Parameter = State->GetTemplSymbol();
	FSymbols* CapturedPtr = nullptr;
	CapturedPtr = &Parameter->Captured;
	if (Parameter->Captured.size() == 1 && IsOperator(Parameter->Captured[0]))
	{
		//If only one argument and is operator
		if (Parameter->Captured[0]->Children.size() == 1
			|| (IsOperator(State->TemplateParent) && CompareSymbols(State->TemplateParent, Parameter->Captured[0])))
		{
			//If operator has one element -> Use its child
			//If this is the same operator as that of state -> use children of operator
			CapturedPtr = &Parameter->Captured[0]->Children;
		}
	}

	if (State->Input->size() - State->InputInd < CapturedPtr->size())
	{
		return false;
	}
	for (auto& Symbol : *CapturedPtr)
	{
		bool bFound = false;
		for (int i = State->InputInd; i < State->Input->size(); i++)
		{
			if (CompareSymbolsWCommutativity(Symbol, State->Input->at(i)))
			{
				std::swap(State->Input->at(State->InputInd), State->Input->at(i));
				State->InputInd++;
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			return false;
		}
	}
	State->TemplInd++;
	return true;
}

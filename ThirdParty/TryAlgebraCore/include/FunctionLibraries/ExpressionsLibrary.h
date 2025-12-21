// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace TAExpressionsLibrary
{
	struct FSymbol;
	using FSymbolPtr = std::shared_ptr<FSymbol>;
	using FSymbols = std::vector<FSymbolPtr>;
	struct FRewritingRule
	{
		FSymbols First;
		FSymbols Second;
		std::vector<FSymbolPtr> Parameters;
	};
	using FRewritingRules = std::vector<FRewritingRule>;
	struct FSymbol
	{
		FSymbol() = default;
		FSymbol(const std::wstring& InName, const std::wstring& InInfo, const FSymbols& InSymbols);
		void ParseToString(std::wstring& OutString);
		std::wstring ElementName;
		std::wstring Info;
		FSymbolPtr Parent;
		FSymbols Children;
		bool bCaptured = false;

		enum class EType
		{
			MultiParam,
			UniParam,
			Pure,
			NonPure
		} Type = EType::Pure;

		void (*Expand)(int Index, const FSymbolPtr& Self) = nullptr;

		//For commutativity
		
		int NonParamCount = 0;
		bool bCommute = false;
		bool bHasNonCommuteSuccessor = false;
		int CompareIndex = -1;
		FSymbols Captured;
		bool bWithContextOperator = false;
		bool bOrderFixed = false;
	};

	class FCharacter
	{
	public:
		FCharacter()
		{
		}

		FCharacter(const FSymbolPtr& InSymbolPtr);
		bool operator ==(const std::wstring& Word) const;
		const FSymbolPtr& GetSymbol() const { return SymbolPtr; }
		static bool IsA(const FSymbolPtr& InSymbolPtr);

	private:
		FSymbolPtr SymbolPtr;
	};

	class FIndex
	{
	public:
		enum class EType
		{
			None,
			Superscript,
			Subscript,
			Doublescript
		};

		FIndex() = default;
		FIndex(const FSymbolPtr& InSymbolPtr);
		static bool IsA(const FSymbolPtr& InSymbolPtr);
		FSymbols GetSubscript() const;
		FSymbols GetSuperscript() const;
		EType GetType() const { return Type; }

	private:
		EType DefineType() const;
		EType Type = EType::None;
		FSymbolPtr SymbolPtr;
	};

	class FParameter
	{
	public:
		FParameter()
		{
		}

		FParameter(const FSymbolPtr& InSymbolPtr);
		const std::wstring& GetName() const { return Name; }
		static bool IsA(const FSymbolPtr& InSymbolPtr);
		const FSymbols& GetArguments() const;
		bool IsValid() const { return bIsValid; }
		bool IsMultiParam() const { return bMultiParam; }

	private:
		friend class FParametersHelper;
		void AddArgument(const FSymbolPtr& InSymbolPtr) const;
		std::wstring Name;
		FSymbolPtr SymbolPtr;
		mutable FSymbols Arguments;
		bool bMultiParam = false;
		bool bIsValid = false;
	};

	//Formula templates
	struct FParameterComparator
	{
		bool operator()(const FParameter& A, const FParameter& B) const
		{
			return A.GetName() < B.GetName();
		}
	};

	class FParametersHelper
	{
	public:
		enum class EAddElementStatus
		{
			Success,
			Failed,
			Ended
		};

		bool StartParameter(const class FParameter& InParameter, bool bOnlyIfAdded = false);
		EAddElementStatus TryAddElement(const FSymbolPtr& Symbol);
		bool CanEndParameter();
		const std::set<FParameter, FParameterComparator>& GetParameters() const;
		FSymbols GetArguments(const FParameter& InParameter);
		void AppendParameters(const FParametersHelper& Other);

	private:
		std::set<FParameter, FParameterComparator> Parameters;
		std::set<FParameter, FParameterComparator>::iterator CurrentParameterIt;
		bool bNewParameter = false;
		//Index used in comparison
		int ArgumentIndex = 0;
	};

	struct FSymbolsPool
	{
		FSymbolPtr MultiParameter;
		FSymbols TerminatingSymbols;
	};

	struct FNodeAnalysisData
	{
		//No param successors and not a param itself
		int PureNodeCount = 0;
		//Has successor params, but not a param itself
		int NonPureNodeCount = 0;
		int SingleParamCount = 0;
		int MultiParamCount = 0;
	};

	enum class EComparisonStages
	{
		OrderedTop,
		OrderedDepth,
		UnorderedPure,
		UnorderedNonPure,
		UnorderedUniParam,
		UnorderedMultiParam,
	};
	enum class EComparatorResult
	{
		Continue,
		Variator,
		Failed,
		Success,
	};
	struct FComparisonState
	{
		FComparisonState()=default;
		FComparisonState(const FComparisonState& Other);

		FComparisonState& operator = (const FComparisonState& Other);
		int InputInd = 0;
		int TemplInd = 0;
		FSymbols* Input;
		FSymbols InputCopy;
		FSymbols* Template;
		FSymbolPtr TemplateParent;
		bool bFirstEnterStage = true;
		bool bLastResult = true;
		EComparisonStages Stage = EComparisonStages::OrderedTop;
		EComparatorResult (*CompareStage)(class FComparatorTest* Comparator) = nullptr;

		FSymbolPtr GetTemplSymbol() const;
		FSymbolPtr GetInputSymbol() const;
		bool TemplateEnded() const;
		bool InputEnded() const;

	};

	class FVariator
	{
	public:
		virtual ~FVariator() = default;
		FVariator(std::vector<FComparisonState>* InStates);
		virtual bool NextVariation();
		std::vector<FComparisonState> GetStatesCopy() const;

	protected:
		std::vector<FComparisonState>* States;
	};

	class FSingleVariation : public FVariator
	{
	public:
		using FVariator::FVariator;
		virtual bool NextVariation() override;
	private:
		bool bFirstVariation = true;
	};

	class FOrderedMultiVariator : public FVariator
	{
	public:
		FOrderedMultiVariator(std::vector<FComparisonState>* InStates);
		virtual bool NextVariation() override;
		int AllowedArgumentsCount;
		FSymbols* CapturedPtr = nullptr;
		FSymbolPtr Param;
		bool bFirstVariation = true;
		bool bCaptureAll = false;
	};

	class FOrderedUniVariator : public FVariator
	{
	public:
		using FVariator::FVariator;
		virtual bool NextVariation() override;
	private:
		bool bCaptured = false;
	};

	class FUnorderedUniVariator : public FVariator
	{
	public:
		FUnorderedUniVariator(std::vector<FComparisonState>* InStates);
		virtual bool NextVariation() override;
	private:
		int InputInd = 0;
		int InputIndStart = 0;
		FSymbols InputCopy;
		FSymbolPtr Param;
	};

	class FChainedCounterUnit
	{
		friend class FChainedCounter;
	public:
		int GetValue() const;

	private:
		bool Increment();
		int Value = 0;
		int Max = 0;
		FChainedCounterUnit* Left = nullptr;
	};

	class FChainedCounter
	{
	public:
		FChainedCounter() = default;
		FChainedCounter(int InUnitsCount, int InSize);

		bool Increment();

		const std::vector<FChainedCounterUnit>& GetUnits();

	private:

		std::vector<FChainedCounterUnit> Units;
	};

	class FUnorderedMultiVariator : public FVariator
	{
	public:
		FUnorderedMultiVariator(std::vector<FComparisonState>* InStates);
		virtual bool NextVariation() override;
	private:
		FSymbolPtr Param;
		int IndexStart = 0;
		int MaxArgsSize = 0;
		int Count = 1;
		int ChainCounterSize = 0;
		FChainedCounter ChainedCounter;
		bool bLastVariation = false;
		FSymbols ChildrenCopy;
		FSymbols* CapturedPtr;
		bool bCaptureAll;
	};
	
	struct FStatesAndVariator
	{
		int MinStateSize = 0;
		std::vector<FComparisonState> States;
		std::shared_ptr<FVariator> Variator;
	};

	struct FStepUpResult
	{
		//Should end comparison algorithm
		bool bFinishComparison = false;
		
		bool bEndComparisonResult = false;

		static FStepUpResult Continue;
		static FStepUpResult SuccessComparison;
		static FStepUpResult FailedComparison;

		//Should finish comparison
		operator bool() const { return bFinishComparison; }
	};
	
	class FComparatorTest
	{
	public:
		//Returns unique parameters
		bool Compare(FSymbols& MathTree, FSymbols& Template);
		FStepUpResult StepUp(bool bResult);
		bool StepDown();
		template<class VariatorT>
		bool Checkpoint();
		static EComparatorResult OrderedTopComparison(FComparatorTest* Cmp);
		static EComparatorResult OrderedDeepComparison(FComparatorTest* Cmp);
		static EComparatorResult UnorderedComparisonPure(FComparatorTest* Cmp);
		static EComparatorResult UnorderedComparisonNonPure(FComparatorTest* Cmp);
		static EComparatorResult UnorderedComparisonUniParam(FComparatorTest* Cmp);
		static EComparatorResult UnorderedComparisonMultiParam(FComparatorTest* Cmp);
		static bool CompareWithParamOrdered(FComparisonState* State);
		static bool CompareWithParamUnordered(FComparisonState* State);
		std::vector<std::shared_ptr<FStatesAndVariator>> StatesAndVariators;
		std::shared_ptr<FStatesAndVariator> CurrentStatesAndVariator;
		int CurrentStateIndex = 0;
	};

	template <class VariatorT>
	bool FComparatorTest::Checkpoint()
	{
		//Create variator
		auto PrevStatesPtr = &StatesAndVariators.back()->States;
		auto Variator = std::make_shared<VariatorT>(PrevStatesPtr);
		StatesAndVariators.back()->Variator = Variator;
		//Make variator step
		bool bResult = Variator->NextVariation();
		//Save state
		CurrentStatesAndVariator = std::make_shared<FStatesAndVariator>();
		StatesAndVariators.emplace_back(CurrentStatesAndVariator);
		CurrentStatesAndVariator->States = *PrevStatesPtr;
		CurrentStatesAndVariator->MinStateSize = PrevStatesPtr->size();
		return bResult;
	}

	FSymbolPtr MakeSymbol(const std::wstring& Name, const std::wstring& Info = L"", const FSymbolPtr& Parent = nullptr, const FSymbols& Children = FSymbols());

	template <typename T>
	bool IsValidIndex(const T& Array, int Index)
	{
		return Index < Array.size() && Index >= 0;
	}
	FSymbols CopyMathTree(const FSymbols& MathTree);
	FSymbols CopyMathTreeView(const FSymbols& MathTree, int From, int To);
	void CopyMathTreeImpl(const FSymbols& From, FSymbols& To, const FSymbolPtr& Parent);
	void CopyMathTreeViewImpl(const FSymbols& From, int FromStart, int FromEnd, FSymbols& To, const FSymbolPtr& Parent);
	void CopySymbolProperties(const FSymbolPtr& From, const FSymbolPtr& To);

	bool CompareSymbols(const FSymbolPtr& First, const FSymbolPtr& Second);
	bool CompareSymbolsRecursively(const FSymbolPtr& First, const FSymbolPtr& Second);
	bool CompareSymbolArraysRecursively(const FSymbols& A, const FSymbols& B);
	bool CompareSymbolsWCommutativity(const FSymbolPtr& TemplateSymbol, const FSymbolPtr& Input);

	bool EqualsToAny(const FCharacter& Character, const std::vector<std::wstring>& Characters, const std::vector<std::wstring>& Except = {});
	bool IsContainer(const FSymbolPtr& Symbol);
	bool IsMe(const FSymbolPtr& Symbol);
	bool IsWordWrap(const FSymbolPtr& Symbol);
	bool IsCharacter(const FSymbolPtr& Symbol, const std::wstring& Character);
	bool IsCharacter(const FSymbolPtr& Symbol);
	bool IsMultiParameter(const FSymbolPtr& Symbol);
	bool IsSingleParameter(const FSymbolPtr& Symbol);
	bool IsParameter(const FSymbolPtr& Symbol);
	bool IsPure(const FSymbolPtr& Symbol);
	bool IsOperator(const FSymbolPtr& Symbol, const std::wstring& Character);
	bool IsOperator(const FSymbolPtr& Symbol);
	bool IsNonCommute(const FSymbolPtr& InSymbol);
	bool IsCustomToken(const FSymbolPtr& Symbol);
	bool IsTokenWord(const FSymbolPtr& Symbol);
	bool IsTokenExpr(const FSymbolPtr& Symbol);
	bool IsNumeric(const FSymbolPtr& Symbol);
	bool IsInvisibleBracket(const FSymbolPtr& Symbol);
	//Expressions
	void ReplaceParametersWithArguments(FParametersHelper* ParametersHelper, FSymbols& Template);
	bool ExtractArgumentsSimple(const FSymbols& MathTree, const FSymbols& Template, FParametersHelper* ParametersHelper);
	bool CreateSymbolPools(const FSymbols& MathTree, std::vector<FSymbolsPool>& Pools);
	bool CaptureArgumentsWithPools(const FSymbols& MathTree, const std::vector<FSymbolsPool>& Pools, FParametersHelper* ParametersHelper);
	bool FindFirstSequenceOccurence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, int& FirstOccurrenceIndex, FParametersHelper* ParametersHelper);
	bool CompareWithSequence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, FParametersHelper* ParametersHelper);
	void ClearCaptured(const FSymbols& MathTree);
	void SetCaptured(const FSymbols& MathTree, bool bVal);
	int SkipCapturedParameterIndices(const FSymbols& MathTree, int IndexStart);


	
	//Custom Tokenization
	bool CanApplyTokenization(const FSymbolPtr& Parent, bool bCanApply);
	void ApplyRewritingRule1(const FRewritingRule& RewritingRule, FSymbols& MathTree, const FSymbolPtr& Parent = nullptr, bool bCanRewrite = true);
	void ApplyRewritingRule2(const FRewritingRule& RewritingRule, FSymbols& MathTree, const FSymbolPtr& Parent = nullptr, bool bCanRewrite = true);
	bool SimplePatternMatching(const FSymbols& MathTree, int From, int To, const FSymbols& Template);
	bool SimplePatternMatchingImpl(const FSymbols& MathTree, const FSymbols& Template);
	bool CaptureArgumentsWithPools(const FSymbols& MathTree, const std::vector<FSymbolsPool>& Pools);
	bool CompareWithTerminatingSequence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From); 
	bool FindFirstSequenceOccurence(const FSymbols& TerminatingSequence, const FSymbols& MathTree, int From, int To, int& FirstOccurrenceIndex);
	bool ParseRewritingRules(const std::wstring& Content, FRewritingRules& OutRewritingRules, void(*LogicalParsePtr)(FSymbols& MathTree));
	void LogicalParse0(FSymbols& MathTree);
	void LogicalParse1(FSymbols& MathTree);
	void SimplifyTokens(FSymbols& MathTree);
	
	bool IsSpecialCharacter(const FSymbolPtr& Symbol, const std::vector<std::wstring>& Except = {});
	FSymbols ParseToMathTree(const std::wstring& InData);
	std::wstring ParseToString(const FSymbols& MathTree);
	FSymbols SetParents(FSymbols&& MathTree);
	void SetParentRecursive(const FSymbolPtr& Parent, const FSymbols& InMathTree);
	void SetParent(const FSymbolPtr& Parent, const FSymbols& InMathTree);
	

	template <class T>
	bool To(const FSymbolPtr& MathElement, T& OutCharacter)
	{
		if (T::IsA(MathElement))
		{
			OutCharacter = T(MathElement);
			return true;
		}
		return false;
	}

	//Adds brackets to the front and back in IB children
	void ExpandInvisibleBrackets(int Index, const FSymbolPtr& Self);
	void ExpandOperator(int InIndex, const FSymbolPtr& Self);
	void ExpandNegation(int InIndex, const FSymbolPtr& Self);
	void ClearExtraPlusSign(FSymbols& MathTree);
	void ClearCapturedArguments(const std::vector<FSymbolPtr>& Params);
	void ReplaceParametersWithArguments(FSymbols& MathTree, const FSymbolPtr& Parent = nullptr);
	
	void SimplifyMathTree(FSymbols& MathTree);
	void ExpandMathTreeRecursive(FSymbols& MathTree);
	void SimplifyMathTreeRecursive(FSymbols& MathTree);
	void SimplifyMathTreeRecursive(FSymbols& MathTree, bool(*Predicate)(const FSymbolPtr& InSymbol));
	void RemoveContextOperator(const std::vector<FSymbolPtr>& Params);
	void RemoveSurplusOperators(FSymbols& MathTree);
	
	//Tokenization
	void TokenizeNumeric(FSymbols& MathTree);
	bool BracketsToBranch(FSymbols& MathTree, const std::vector<std::pair<std::wstring, std::wstring>>& StartEndBrackets);
	bool MultiplicationToBranches(FSymbols& MathTree, bool bCanApply = true);
	bool OperatorToBranches(FSymbols& MathTree, const std::wstring& Operator, bool bCommute = false, bool bCanApply = true);
	bool IndicesToBranches(FSymbols& MathTree);
	bool EncapsulateMinusSign(FSymbols& MathTree);
	void CacheParameters(const FSymbols& MathTree);
	void ReplaceParameterCopies(FSymbols& MathTree);
	void LogicalParse(FSymbols& MathTree, const std::vector<FRewritingRules>& RewritingRules);
	bool LogicParseAvoid(const FSymbolPtr& Symbol);

	void ReplaceWithChildren(FSymbols& MathTree, int Index);
	void RemoveFromParent(const FSymbolPtr& Symbol, int Index = -1);
	void RemoveCharacter(FSymbols& MathTree, const std::wstring& Character);
	void ReplaceSymbol(FSymbols& MathTree, const FSymbolPtr& InWhatReplace, const std::wstring& InReplaceWith);
	void RemoveAllSymbols(FSymbols& MathTree, const FSymbolPtr& InWhatRemove);
	bool FindPathToFirst(const FSymbols& MathTree, const FSymbolPtr& InFindSymbol, std::vector<int>& OutPath);

	//New expressions system
	bool SizeCheck(const FSymbols& MathTree, const FSymbols& Template);

	void SortCommutativeNodes(FSymbols& MathTree, const FSymbolPtr& Parent = nullptr);
	bool SortingPredicate(const FSymbolPtr& First, const FSymbolPtr& Second);

	void CacheChildNodesCount(FSymbols& MathTree);
	void EnumerateNodes(FSymbols& MathTree);
	void FindChildNodesCountRecursive(const FSymbolPtr& InSymbol, int& Count);
	void ReplaceParamCopies(FSymbols& MathTree, std::vector<FSymbolPtr>& OriginalParams);

	void MarkNodesWParamSuccessor(FSymbols& MathTree);
	void MarkNodesWParamSuccessor(FSymbolPtr Parent);
	void MarkNodesWNonCommuteSuccessor(FSymbols& MathTree);
	void ExpandNonCommutative(FSymbols& MathTree);
	
	bool HasMultiParam(const FSymbols& MathTree);
}

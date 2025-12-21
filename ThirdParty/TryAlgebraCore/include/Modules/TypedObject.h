// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <memory>
#include <set>

#define TYPED_MAKE_SHARED(ThisClass) \
template<typename ... Args>\
static std::shared_ptr<ThisClass> MakeTypedShared(Args&& ... args)\
{\
	std::shared_ptr<ThisClass> SharedThis = std::shared_ptr<ThisClass>(new ThisClass(std::forward<Args>(args) ...));\
	SharedThis->SetupType();\
	return SharedThis;\
}\

#define TYPED_CLASS_META(ThisClass,BaseClass) \
friend FTATypedObject;\
public:\
virtual ~ThisClass() = default;\
private:\
typedef BaseClass Super;\
public:\
TYPED_MAKE_SHARED(ThisClass) \
TTypedWeak<ThisClass> GetSelfWeak() const\
{\
return TTypedWeak<ThisClass>(std::weak_ptr(SelfShared));\
}\
protected:\
virtual void SetupType()\
{\
if(Type == 0)\
{\
Type = StaticType();\
}\
Types.insert(StaticType());\
Super::SetupType();\
}\
public:\
static FType StaticType()\
{\
static FType Id = 0;\
if(Id == 0)\
{\
TypeLastIndex++;\
Id = TypeLastIndex;\
}\
return Id;\
}\
private:\

class FTATypedObject;
template<typename T>
class TTypedWeak
{
public:
	TTypedWeak() = default;
	template <class _Ty2, std::enable_if_t<std::_SP_pointer_compatible<_Ty2, T>::value, int> = 0>
	TTypedWeak(const TTypedWeak<_Ty2>& Other)
	{
		if(Other.Get())
		{
			ObjectWeak = Other.GetInnerWeak();
			ObjectPtr = ObjectWeak.lock().get();
		}
	}
	bool operator==(const TTypedWeak&) const;
	T* operator->() const;
	T* Get() const;
	const std::weak_ptr<FTATypedObject>& GetInnerWeak() const;
	bool IsValid() const;
private:
	friend T;
	TTypedWeak(const std::weak_ptr<FTATypedObject>& InObjectWeak)
	{
		ObjectWeak = InObjectWeak;
		ObjectPtr = ObjectWeak.lock().get();
	}
	std::weak_ptr<FTATypedObject> ObjectWeak;
	FTATypedObject* ObjectPtr = nullptr;
};

template <typename T>
bool TTypedWeak<T>::operator==(const TTypedWeak& Right) const
{
	return ObjectPtr == Right.ObjectPtr; 
}

template <typename T>
T* TTypedWeak<T>::operator->()const
{
	if (!ObjectWeak.expired())
	{
		return static_cast<T*>(ObjectPtr);
	}
	return nullptr;
}

template <typename T>
T* TTypedWeak<T>::Get() const
{
	if (!ObjectWeak.expired())
	{
		return static_cast<T*>(ObjectPtr);
	}
	return nullptr;
}

template <typename T>
const std::weak_ptr<FTATypedObject>& TTypedWeak<T>::GetInnerWeak() const
{
	return ObjectWeak;
}

template <typename T>
bool TTypedWeak<T>::IsValid() const
{
	return !ObjectWeak.expired();
}


class FTATypedObject
{
public:
	using FType = unsigned int;
	virtual ~FTATypedObject() = default;
protected:
	FTATypedObject();
public:	
	TTypedWeak<FTATypedObject> GetSelfWeak() const
	{
		return TTypedWeak<FTATypedObject>(std::weak_ptr(SelfShared));
	}

	bool IsOfType(FType InType) const;
	FType GetType() const { return Type; }
protected:
	std::shared_ptr<FTATypedObject> SelfShared;
	std::set<FType> Types;
	FType Type = 0;
	inline static FType TypeLastIndex = 0;
	
	virtual void SetupType()
	{
		if(Type == 0)
		{
			Type = StaticType();
		}
		Types.insert(StaticType());
	}
public:
	static FType StaticType()
	{
		static FType Id = 0;
		if(Id == 0)
		{
			TypeLastIndex++;
			Id = TypeLastIndex;
		}
		return Id;
	}
	static std::shared_ptr<FTATypedObject> MakeTypedShared()
	{
		std::shared_ptr<FTATypedObject> SharedThis = std::shared_ptr<FTATypedObject>(new FTATypedObject());
		SharedThis->SetupType();
		return SharedThis;
	}
	template<typename U>
	U* Cast()
	{
		if(IsOfType(U::StaticType()))
		{
			return static_cast<U*>(this);
		}
		return nullptr;
	}
};

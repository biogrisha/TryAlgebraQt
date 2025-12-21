// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ActionHistory/ActionBase.h"

FTAActionBase::FTAActionBase(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument)
{
	MeDocument = InMeDocument;
}

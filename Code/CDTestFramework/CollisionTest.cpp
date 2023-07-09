#include "stdafx.h"
#include "CollisionTest.h"

OpcodeSettings::OpcodeSettings() :
		mPrimitiveTests	(true),
		mFirstContact	(false),
		mUseCache		(false)
{
}

void OpcodeSettings::AddToTweakBar(TwBar* tbar)
{
	TwAddVarRW(tbar, "Primitive tests", TW_TYPE_BOOLCPP, &mPrimitiveTests, " group='Opcode' ");
	TwAddVarRW(tbar, "First contact", TW_TYPE_BOOLCPP, &mFirstContact, " group='Opcode' ");
	TwAddVarRW(tbar, "Temporal coherence", TW_TYPE_BOOLCPP, &mUseCache, " group='Opcode' ");
}

void OpcodeSettings::SetupCollider(Collider& collider) const
{
	collider.SetFirstContact(mFirstContact);
	collider.SetPrimitiveTests(mPrimitiveTests);
	collider.SetTemporalCoherence(mUseCache);
}
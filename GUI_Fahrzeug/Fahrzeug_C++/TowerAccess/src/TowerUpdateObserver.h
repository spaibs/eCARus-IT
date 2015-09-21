////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

#ifndef TOWER_UPDATE_OBSERVER_H_INCLUDED
#define TOWER_UPDATE_OBSERVER_H_INCLUDED

#include <GtfTypes/GtfTypesC99.h> // for int32_t

// Just an interface
class TowerUpdateObserver
{
public:
	virtual ~TowerUpdateObserver(){};
	
	virtual void onLightStateUpdated(bool lowBeamIsOn, bool highBeamIsOn, bool indicatorRightIsOn, bool indicatorLeftIsOn) = 0;
	virtual void onGestureDetected(int32_t gestureType) = 0;
};

#endif // TOWER_UPDATE_OBSERVER_H_INCLUDED

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
#include <GtfDataPool/GtfDataPool.h>

// Just an interface
class MusicServiceUpdateObserver
{
public:
	virtual ~MusicServiceUpdateObserver(){};
	
    virtual uint32_t getTrackPropertyId(GtfDataPool** pDataPool, GtfTypesDP::contextId_t* ctxId) = 0;
	
};

#endif // TOWER_UPDATE_OBSERVER_H_INCLUDED

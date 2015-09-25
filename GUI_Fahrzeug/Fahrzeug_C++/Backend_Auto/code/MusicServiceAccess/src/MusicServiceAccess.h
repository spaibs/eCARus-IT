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

#ifndef MUSICSERVICE_ACCESS_H_INCLUDED
#define MUSICSERVICE_ACCESS_H_INCLUDED

#include <GtfTypes/GtfTypesC99.h>
#include <GtfPluginLoader/GtfMainWorkLoop.h>
#include <GtfCoreRuntime/GtfCoreModel.h>
#include <GtfEventSystem/GtfExternalEventSystem.h>
#include <GtfDataPool/GtfDataPool.h>
#include <GtfResourcer/GtfResourcer.h>
#include <GtfOSAL/GtfFunctorTask.h>

#include "MusicServiceUpdateObserver.h"

class MusicServiceAccess : public MusicServiceUpdateObserver
{
public:
    MusicServiceAccess();
    ~MusicServiceAccess();

    bool Startup(GtfCoreModel& rCoreModel,
                   GtfMainWorkLoop& rWorkLoop,
                   const GtfFunctor0<void>& c_rCallback);
    void Shutdown(const GtfFunctor0<void>& c_rCallback);
	
	// Interface: MusicServiceUpdateObserver
	void addTitle(bool dummy1, bool dummy2, bool dummy3);

private:
    GtfMainWorkLoop* m_pWorkLoop;
    GtfExternalEventSystem* m_pEvSystem;
    GtfDataPool* m_pDataPool;
    GtfResourcer* m_pResourcer;

    bool m_fRunning;
    GtfFunctorTask m_dpTask;
    GtfFunctorTask m_evTask;
    GtfFunctorTask m_loopTask;
    GtfFunctorTask m_shutdownTask;

    void ProcessEvUpdate();
    void ProcessDpUpdate();
    void ProcessShutdown();
    void ProcessLoop();

    void ProcessEvent(GtfEventHandle handle);
    void ProcessDatapool(const GtfTypesDP::itemId_t* const c_pItemId,
                         const uint32_t c_itemCount);
};

#endif // MUSICSERVICE_ACCESS_H_INCLUDED

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

#include <stdio.h>
#include <GtfTypes/GtfStdDef.h>
#include <GtfFunctor/FunctorBind.h>
#include <GtfResources/GtfImageResource.h>
#include <GtfResources/GtfImageDecoder.h>
#include <GtfResourcer/GtfResourcer.h>
#include <GtfCommon/GtfCommon_version.h>
#include <GtfDataPool/GtfVariant.h>
#include <android/log.h>
#include "LogCatLogger.h"

#include "datapool_pdal42.h"

#include "events_0x10005.h"

#include "MusicServiceAccessJNIManager.h"
#include "MusicServiceAccess.h"
#include <string.h>

MusicServiceAccess::MusicServiceAccess()
 : m_pWorkLoop(NULL)
 , m_pEvSystem(NULL)
 , m_pDataPool(NULL)
 , m_pResourcer(NULL)
 , m_fRunning(false)
{
}

MusicServiceAccess::~MusicServiceAccess()
{
}

bool MusicServiceAccess::Startup(GtfCoreModel& rCoreModel,
                           GtfMainWorkLoop& rWorkLoop,
                           const GtfFunctor0<void>& c_rCallback)
{
    bool fSuccess = false;

    if (m_fRunning == false)
    {
        m_fRunning = true;
        m_pWorkLoop = &rWorkLoop;
        m_pDataPool = rCoreModel.getDatapool();
        m_pEvSystem = rCoreModel.getExternalEventSystem();
        m_pResourcer = rCoreModel.getResourcer();

        // check fingerprint and report on failure
        m_pDataPool->CheckFingerprint(DP_CONTEXT_ID, DP_CONTEXT_FINGERPRINT);

        // load the default datapool values
        rCoreModel.getVariant()->Restore(DP_CONTEXT_ID);

        // setup the event processing task
        m_evTask.SetExecuteCallback(gtf_bind(&MusicServiceAccess::ProcessEvUpdate, this));

        // setup the datapool processing task
        m_dpTask.SetExecuteCallback(gtf_bind(&MusicServiceAccess::ProcessDpUpdate, this));

        // register the event processing task
        fSuccess = m_pEvSystem->SetInvoker(DP_CONTEXT_ID,
                                           gtf_bind(&GtfMainWorkLoop::PerformTask,
                                           m_pWorkLoop,
                                           gtf_ref(m_evTask),
                                           gtf::Task::ePriorityValueDefault));

        //
		// TUM - SUBSCRIBE TO EVENTS HERE
		//
			//////////////////////////////////////////////////////////////////////////////
			fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Media_PauseTitle,
            EVENT_MSG_ID_ExtOut_Media_PauseTitle,
            SubscribeToEvent);
			
			fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Media_PlayTitle,
            EVENT_MSG_ID_ExtOut_Media_PlayTitle,
            SubscribeToEvent);
			//////////////////////////////////////////////////////////////////////////////
        // register the datapool processing task
        fSuccess = fSuccess && m_pDataPool->SetInvoker(DP_CONTEXT_ID,
                                                       gtf_bind(&GtfMainWorkLoop::PerformTask,
                                                       m_pWorkLoop,
                                                       gtf_ref(m_dpTask),
                                                       gtf::Task::ePriorityValueDefault));

        if (fSuccess == false)
        {
            Shutdown(c_rCallback);
        }
        else
        {
            // the datapool delivers notifications only if the property was read before
            m_pDataPool->ForEach(DP_CONTEXT_ID,
                                 true,
                                 false,
                                 gtf_bind(&GtfDataPool::MarkAsRead, m_pDataPool));

            // commit all changed properties
            m_pDataPool->Commit(DP_CONTEXT_ID);
			
			// invoke the datapool processing task once at startup
            m_pWorkLoop->PerformTask(m_dpTask);

			//
			// TUM - DO ADDITIONAL INITIALIZATIONS HERE (IF REQUIRED)
			//
			MusicServiceAccessJNIManager::getInstance()->registerObserver(this);
			
            c_rCallback();
        }
    }
    else
    {
        c_rCallback();
    }

    return fSuccess;
}

void MusicServiceAccess::Shutdown(const GtfFunctor0<void>& c_rCallback)
{
    LOGCAT_DEBUG("MusicServiceAccess shutdown");

    if (m_fRunning != false)
    {
        m_fRunning = false;

        // invoke the final shutdown tasks
        m_shutdownTask.SetExecuteCallback(gtf_bind(&MusicServiceAccess::ProcessShutdown, this));
        m_shutdownTask.SetDisposeCallback(c_rCallback);
        m_pWorkLoop->PerformTask(m_shutdownTask,
                                 gtf::Task::ePriorityValueDefault);
    }
    else
    {
        c_rCallback();
    }
}

void MusicServiceAccess::ProcessEvUpdate()
{
    // fetch all events
    m_pEvSystem->Fetch(DP_CONTEXT_ID,
                       gtf_bind(&MusicServiceAccess::ProcessEvent, this));
}

void MusicServiceAccess::ProcessEvent(GtfEventHandle handle)
{
    GtfEvent ev(handle);
    uint32_t grpId;
    uint32_t evId;
	
	LOGCAT_ERROR("process event....");
	
    if((true == ev.GetGroupId(grpId)) && (true == ev.GetEventId(evId)))
    {
        //
		// TUM - EVALUATE EVENTS HERE
		//
		__android_log_print(ANDROID_LOG_DEBUG, "MusicServiceAccess", "grpId : %i evId %i",grpId , evId);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if ((grpId == EVENT_GRP_ID_ExtOut_Media_PlayTitle) && (evId == EVENT_MSG_ID_ExtOut_Media_PlayTitle))
        {
            char title[99] = "";
            bool success = ev.ReadValue(0, GtfTypeId::eTypeId_string, title, 100);

			LOGCAT_DEBUG("play title");
			
            if(success)
            {
				MusicServiceAccessJNIManager::getInstance()->playTitle(title);
            }
        }
        if ((grpId == EVENT_GRP_ID_ExtOut_Media_PauseTitle) && (evId == EVENT_MSG_ID_ExtOut_Media_PauseTitle))
        {
			LOGCAT_DEBUG("pause title");
			MusicServiceAccessJNIManager::getInstance()->pauseTitle();
        }
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

void MusicServiceAccess::ProcessDpUpdate()
{
    // update the datapool properties
    m_pDataPool->Update(DP_CONTEXT_ID);

    // fetch all datapool notifications
    m_pDataPool->Fetch(DP_CONTEXT_ID,
                       gtf_bind(&MusicServiceAccess::ProcessDatapool, this));

    // commit all changed properties
    m_pDataPool->Commit(DP_CONTEXT_ID);
}

void MusicServiceAccess::ProcessDatapool(const GtfTypesDP::itemId_t* const c_pItemId,
                                 const uint32_t c_itemCount)
{
    GTF_UNUSED_PARAM(c_pItemId)
    GTF_UNUSED_PARAM(c_itemCount)
    
    //
    // TUM - READ AND PROCESS DP PROPERTIES HERE
    //
}

void MusicServiceAccess::ProcessShutdown()
{
    //
    // TUM - DO ADDITIONAL DEINITIALIZATIONS HERE (IF REQUIRED)
    //
	MusicServiceAccessJNIManager::getInstance()->unregisterObserver(this);

    if (m_pEvSystem != NULL)
    {
		//
		// TUM - UNSUBSCRIBE ALL SUBSCRIBED EVENTS HERE
		//
        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Media_PauseTitle,
            EVENT_MSG_ID_ExtOut_Media_PauseTitle,
            SubscribeToEvent);

        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Media_PlayTitle,
            EVENT_MSG_ID_ExtOut_Media_PlayTitle,
            SubscribeToEvent);

        // unregister the event processing task
        m_pEvSystem->SetInvoker(DP_CONTEXT_ID,
                                GtfFunctor0<void>());

        // cancel invoked event processing task
        m_pWorkLoop->Cancel(m_evTask);
    }

    // unsubscribe to data pool
    if (m_pDataPool != NULL)
    {
        // unregister the datapool processing task
        m_pDataPool->SetInvoker(DP_CONTEXT_ID,
                                GtfFunctor0<void>());

        // cancel invoked datapool processing task
        m_pWorkLoop->Cancel(m_dpTask);
    }

    // cancel invoked loop processing task
    m_pWorkLoop->Cancel(m_loopTask);

    m_pWorkLoop = NULL;
    m_pEvSystem = NULL;
    m_pDataPool = NULL;
}

void MusicServiceAccess::addTitle(bool dummy1, bool dummy2, bool dummy3)
{
	
	//m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_Extdummy1, GtfTypeId::eTypeId_bool, &dummy1, sizeof(bool));
	//m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_Extdummy2, GtfTypeId::eTypeId_bool, &dummy2, sizeof(bool));
	//m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_Extdummy3, GtfTypeId::eTypeId_bool, &idummy3, sizeof(bool));
	
	m_pDataPool->Commit(DP_CONTEXT_ID);
}

	
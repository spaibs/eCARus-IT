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
#include "events_0x10000.h"
#include "datapool_pdal45.h"

#include "NavigationServiceAccess.h"
#include "NavigationJNIAccess.h"

NavigationServiceAccess::NavigationServiceAccess()
 : m_pWorkLoop(NULL)
 , m_pEvSystem(NULL)
 , m_pDataPool(NULL)
 , m_pResourcer(NULL)
 , m_fRunning(false)
{
}

NavigationServiceAccess::~NavigationServiceAccess()
{
}

bool NavigationServiceAccess::Startup(GtfCoreModel& rCoreModel,
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
        m_evTask.SetExecuteCallback(gtf_bind(&NavigationServiceAccess::ProcessEvUpdate, this));

        // setup the datapool processing task
        m_dpTask.SetExecuteCallback(gtf_bind(&NavigationServiceAccess::ProcessDpUpdate, this));

        fSuccess = true;

        // register the event processing task
        fSuccess = fSuccess && m_pEvSystem->SetInvoker(DP_CONTEXT_ID,
            gtf_bind(&GtfMainWorkLoop::PerformTask,
            m_pWorkLoop,
            gtf_ref(m_evTask),
            gtf::Task::ePriorityValueDefault));

		//
		// TUM - SUBSCRIBE TO EVENTS HERE
		//
        fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_DoubleTap,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_DoubleTap,
            SubscribeToEvent);

        fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Move,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Move,
            SubscribeToEvent);

        fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Rotate,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Rotate,
            SubscribeToEvent);

        fSuccess = fSuccess && m_pEvSystem->Subscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Zoom,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Zoom,
            SubscribeToEvent);

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

            c_rCallback();
        }
    }
    else
    {
        c_rCallback();
    }

    return fSuccess;
}

void NavigationServiceAccess::Shutdown(const GtfFunctor0<void>& c_rCallback)
{
    LOGCAT_DEBUG("NavigationServiceAccess shutdown");

    if (m_fRunning != false)
    {
        m_fRunning = false;

        // invoke the final shutdown tasks
        m_shutdownTask.SetExecuteCallback(gtf_bind(&NavigationServiceAccess::ProcessShutdown, this));
        m_shutdownTask.SetDisposeCallback(c_rCallback);
        m_pWorkLoop->PerformTask(m_shutdownTask, gtf::Task::ePriorityValueDefault);
    }
    else
    {
        c_rCallback();
    }
}

void NavigationServiceAccess::ProcessEvUpdate()
{
    // fetch all events
    m_pEvSystem->Fetch(DP_CONTEXT_ID,
        gtf_bind(&NavigationServiceAccess::ProcessEvent, this));
}

void NavigationServiceAccess::ProcessEvent(GtfEventHandle handle)
{
    GtfEvent ev(handle);
    uint32_t grpId;
    uint32_t evId;

    if((true == ev.GetGroupId(grpId)) && (true == ev.GetEventId(evId)))
    { 
        //
	    // TUM - EVALUATE EVENTS HERE
	    //
        if ((grpId == EVENT_GRP_ID_ExtOut_Navigation_Touch_Move) && (evId == EVENT_MSG_ID_ExtOut_Navigation_Touch_Move))
        {
            int32_t deltaX = 0;
            int32_t deltaY = 0;

            // Read event parameters and store to local variables
            bool success = ev.ReadValue(0, GtfTypeId::eTypeId_int32, &deltaX, sizeof(int32_t));
            success = success && ev.ReadValue(1, GtfTypeId::eTypeId_int32, &deltaY, sizeof(int32_t));

            if(success) // If reading was successful...
            {
                // Forward touch parameters to JNI access class
                NavigationJNIAccess::getInstance().onTouchMove(deltaX, deltaY);
            }
        }
        else if ((grpId == EVENT_GRP_ID_ExtOut_Navigation_Touch_Rotate) && (evId == EVENT_MSG_ID_ExtOut_Navigation_Touch_Rotate))
        {
            float rotationAngle = 0.0f;

            bool success = ev.ReadValue(0, GtfTypeId::eTypeId_float, &rotationAngle, sizeof(float));
            if(success)
            {
                NavigationJNIAccess::getInstance().onTouchRotate(rotationAngle);
            }
        }
        else if ((grpId == EVENT_GRP_ID_ExtOut_Navigation_Touch_Zoom) && (evId == EVENT_MSG_ID_ExtOut_Navigation_Touch_Zoom))
        {
            float zoomFactor = 0.0f;

            bool success = ev.ReadValue(0, GtfTypeId::eTypeId_float, &zoomFactor, sizeof(float));
            if(success)
            {
                NavigationJNIAccess::getInstance().onTouchZoom(zoomFactor);
            }
        }
        else if ((grpId == EVENT_GRP_ID_ExtOut_Navigation_Touch_DoubleTap) && (evId == EVENT_MSG_ID_ExtOut_Navigation_Touch_DoubleTap))
        {
            int32_t tapX = 0;
            int32_t tapY = 0;

            bool success = ev.ReadValue(0, GtfTypeId::eTypeId_int32, &tapX, sizeof(int32_t));
            success = success && ev.ReadValue(1, GtfTypeId::eTypeId_int32, &tapY, sizeof(int32_t));

            if(success)
            {
                NavigationJNIAccess::getInstance().onDoubleTap(tapX, tapY);
            }
        }
    }
}

void NavigationServiceAccess::ProcessDpUpdate()
{
    // update the datapool properties
    m_pDataPool->Update(DP_CONTEXT_ID);

    // fetch all datapool notifications
    m_pDataPool->Fetch(DP_CONTEXT_ID,
                       gtf_bind(&NavigationServiceAccess::ProcessDatapool, this));

    // commit all changed properties
    m_pDataPool->Commit(DP_CONTEXT_ID);
}

void NavigationServiceAccess::ProcessDatapool(const GtfTypesDP::itemId_t* const c_pItemId,
                                 const uint32_t c_itemCount)
{
    GTF_UNUSED_PARAM(c_pItemId)
    GTF_UNUSED_PARAM(c_itemCount)

    //
    // TUM - READ AND PROCESS DP PROPERTIES HERE
    //
}

void NavigationServiceAccess::ProcessShutdown()
{
    
    // Deinitialize JNI stuff
    NavigationJNIAccess::getInstance().deinitialize();
    	
    //
    // TUM - DO ADDITIONAL DEINITIALIZATIONS HERE (IF REQUIRED)
    //

    if (m_pEvSystem != NULL)
    {
		//
		// TUM - UNSUBSCRIBE ALL SUBSCRIBED EVENTS HERE
		//

        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_DoubleTap,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_DoubleTap,
            SubscribeToEvent);

        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Move,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Move,
            SubscribeToEvent);

        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Rotate,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Rotate,
            SubscribeToEvent);

        m_pEvSystem->Unsubscribe(DP_CONTEXT_ID,
            EVENT_GRP_ID_ExtOut_Navigation_Touch_Zoom,
            EVENT_MSG_ID_ExtOut_Navigation_Touch_Zoom,
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

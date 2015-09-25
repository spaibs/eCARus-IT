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

// Set plugin name for static library build
#ifdef USE_STATIC_PLUGINS
#define GTF_OVERLOADED_SYMBOL_PREFIX myapp_
#endif

#include <GtfPluginLoader/GtfPluginSymbols.h>
#include <GtfPluginLoader/GtfPluginLoader.h>
#include <GtfPluginLoader/GtfPluginFactory.h>
#include <GtfRunlevel/GtfRunlevel.h>
#include <GtfMessageId/GtfMessageId.h>
#include "MusicServiceAccessSO.h"

#define EMPTY_APP_SERVICE_STARTUP_RUNLEVEL (GTF_RUNLEVEL_CORE_LOAD_MODEL + 2)

MusicServiceAccessMsgReceiver::MusicServiceAccessMsgReceiver()
: m_currentDirection(0)
, m_currentRunlevel(GTF_RUNLEVEL_MIN)
, m_validRunlevel(GTF_RUNLEVEL_MAX)
, m_pCoreModel(NULL)
, m_pWorkLoop(NULL)
{
}

void MusicServiceAccessMsgReceiver::startup(GtfPluginLoader& rLoader)
{
    GtfMsgExtReceiver::Init(rLoader);

    if (m_pWorkLoop == NULL)
    {
        m_pWorkLoop = static_cast<GtfMainWorkLoop*>(
            rLoader.getInterface(
            static_cast<uint32_t>(GtfMainWorkLoop::cm_gtfInterfaceId),
            static_cast<uint32_t>(GtfMainWorkLoop::cm_gtfInterfaceVersion)));
    }

    // subscribe to system messages
    if (GetMessenger() != NULL)
    {
        GetMessenger()->SubscribeSysMsg(*this);
    }
}

void MusicServiceAccessMsgReceiver::shutdown()
{
    // unsubscribe to system messages
    if (GetMessenger() != NULL)
    {
        GetMessenger()->UnsubscribeSysMsg(*this);
    }
}

void MusicServiceAccessMsgReceiver::HandleInterface(const uint32_t c_interfaceId,
                                            const uint32_t c_version,
                                            const uint16_t c_validRunlevel,
                                            void * const c_pObj)
{
    if ((c_interfaceId == static_cast<uint32_t>(GtfCoreModel::cm_gtfInterfaceId))
        && (c_version == static_cast<uint32_t>(GtfCoreModel::cm_gtfInterfaceVersion))
        && (c_pObj != NULL)
        && (m_pCoreModel == NULL))
    {
        m_validRunlevel = c_validRunlevel;
        m_pCoreModel = static_cast<GtfCoreModel*>(c_pObj);
    }
}

uint16_t MusicServiceAccessMsgReceiver::HandleRunlevel(const uint16_t c_runlevel,
                                               const int8_t c_direction)
{
    uint16_t requestRunlevel = c_runlevel;

    m_currentRunlevel = c_runlevel;
    m_currentDirection = c_direction;

    if (c_direction > 0)
    {
        if (c_runlevel < EMPTY_APP_SERVICE_STARTUP_RUNLEVEL)
        {
            requestRunlevel = EMPTY_APP_SERVICE_STARTUP_RUNLEVEL;
        }
        else
        if (c_runlevel == EMPTY_APP_SERVICE_STARTUP_RUNLEVEL)
        {
            bool fSuccess = false;

            if ((m_pWorkLoop != NULL)
                && (m_pCoreModel != NULL)
                && (m_validRunlevel <= c_runlevel))
            {
                LockRunlevel();
                fSuccess = m_service.Startup(*m_pCoreModel,
                                         *m_pWorkLoop,
                                         gtf_bind(&MusicServiceAccessMsgReceiver::UnlockRunlevel, this));
            }
            if (fSuccess == false)
            {
                RequestShutdownOnFailure();
            }
        }
    }
    else
    if (c_direction < 0)
    {
        if (c_runlevel > m_validRunlevel)
        {
            requestRunlevel = m_validRunlevel;
        }
        else
        if (c_runlevel == m_validRunlevel)
        {
            LockRunlevel();
            m_service.Shutdown(gtf_bind(&MusicServiceAccessMsgReceiver::UnlockRunlevel, this));
            m_pCoreModel = NULL;
        }
    }

    return requestRunlevel;
}

void MusicServiceAccessMsgReceiver::HandleMessage(const GtfMsgHandle c_msg)
{
    GTF_UNUSED_PARAM(c_msg);
}

void MusicServiceAccessMsgReceiver::LockRunlevel()
{
    GtfMsgHandle msg = GetMessenger()->CreateMsg(GTF_MID_RUN_RUNLEVEL_LOCK);
    GetMessenger()->PublishMsg(msg);
    GetMessenger()->DestroyMsg(msg);
}

void MusicServiceAccessMsgReceiver::UnlockRunlevel()
{
    GtfMsgHandle msg = GetMessenger()->CreateMsg(GTF_MID_RUN_RUNLEVEL_UNLOCK);
    GetMessenger()->PublishMsg(msg);
    GetMessenger()->DestroyMsg(msg);
}

void MusicServiceAccessMsgReceiver::RequestShutdownOnFailure()
{
    // send the system failure message
    GtfMsgHandle handle = GetMessenger()->CreateMsg(GTF_MID_SYSTEM_FAILURE);
    GetMessenger()->PublishMsg(handle);
    GetMessenger()->DestroyMsg(handle);

    // request shutdown
    handle = GetMessenger()->CreateMsg(GTF_MID_RUN_SHUTDOWN);
    GetMessenger()->PublishMsg(handle);
    GetMessenger()->DestroyMsg(handle);
}

// *** GTF plugin interface ****************************************************

GTF_PLUGIN_SO_SYMBOL GtfPlugin * GTF_CREATE_PLUGIN_SYMBOL(uint32_t argc, char** argv)
{
    GTF_UNUSED_PARAM(argc);
    GTF_UNUSED_PARAM(argv);
    return GtfPluginFactory<MusicServiceAccessMsgReceiver>::create();
}

GTF_PLUGIN_SO_SYMBOL void GTF_DESTROY_PLUGIN_SYMBOL(GtfPlugin * const c_pPlugin)
{
    GtfPluginFactory<MusicServiceAccessMsgReceiver>::destroy(c_pPlugin);
}

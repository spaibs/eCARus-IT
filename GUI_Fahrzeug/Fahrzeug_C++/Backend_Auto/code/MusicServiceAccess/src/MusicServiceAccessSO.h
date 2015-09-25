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

#ifndef MUSICSERVICE_ACCESS_SO_H_INCLUDED
#define MUSICSERVICE_ACCESS_SO_H_INCLUDED

#include <GtfPluginLoader/GtfMainWorkLoop.h>
#include <GtfMessenger/GtfMsgExtReceiver.h>
#include <GtfCoreRuntime/GtfCoreModel.h>
#include "MusicServiceAccess.h"

class MusicServiceAccessMsgReceiver : public GtfMsgExtReceiver
{
public:
    MusicServiceAccessMsgReceiver();

    void startup(GtfPluginLoader& rLoader);

    void shutdown();

    void HandleInterface(const uint32_t c_interfaceId,
                         const uint32_t c_version,
                         const uint16_t c_validRunlevel,
                         void * const c_pObj);

    uint16_t HandleRunlevel(const uint16_t c_runlevel,
                            const int8_t c_direction);

    void HandleMessage(const GtfMsgHandle c_msg);
private:
    int8_t m_currentDirection;
    uint16_t m_currentRunlevel;
    uint16_t m_validRunlevel;
    GtfCoreModel* m_pCoreModel;
    GtfMainWorkLoop* m_pWorkLoop;
    MusicServiceAccess m_service;

    void LockRunlevel();
    void UnlockRunlevel();
    void RequestShutdownOnFailure();
};

#endif // MUSICSERVICE_ACCESS_SO_H_INCLUDED

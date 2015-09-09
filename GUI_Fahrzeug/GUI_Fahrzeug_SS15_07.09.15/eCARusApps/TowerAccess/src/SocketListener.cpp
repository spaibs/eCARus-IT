#ifdef ANDROID
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <errno.h>
#endif

#include <GtfEventSystem/GtfExternalEventSystem.h>
#include <assert.h>
#include "LogCatLogger.h"
#include "datapool_pdal42.h"
#include "events_0x10004.h"
#include "SocketListener.h"

#define NR_OF_BATTERY_CELLS 16

#ifdef ANDROID
	struct sockaddr_in _listenerTowerAddr;  	/* talker address */
	struct sockaddr_in _listenerMyAddr;			/* my address */
    pthread_t ListenerThread;

    void* run_adapter_listener(void* pObj);
#endif

void SocketListener::init(GtfDataPool* pDataPool, GtfExternalEventSystem* pEventSystem)
{
	m_pDataPool = pDataPool;
	assert(m_pDataPool);

    m_pEventSystem = pEventSystem;
    assert(m_pEventSystem);

    initListProperties();

#ifdef ANDROID
    pthread_create(&ListenerThread, NULL, run_adapter_listener, this);
#endif
}

bool SocketListener::open(uint16_t port)
{
	bool success = true;

#ifdef ANDROID
	ListenerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if ( ListenerSocket < 0 )
	{
        LOGCAT_WARN("Listener socket creation failed. Errno: %d", errno);
		success = false;
	}
	else
	{
		memset((char *)&_listenerMyAddr, 0, sizeof(_listenerMyAddr));
		_listenerMyAddr.sin_family = AF_INET;
		_listenerMyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_listenerMyAddr.sin_port = htons(port);
		
		if (bind( ListenerSocket, (struct sockaddr *)&_listenerMyAddr, sizeof(_listenerMyAddr)) < 0)
        {
		    perror("bind failed");
            LOGCAT_WARN("Listener socket bind failed. Errno: %d", errno);
		    success = false;
		}
	}
#else
    GTF_UNUSED_PARAM(port)
#endif // ANDROID

	return success;
}

void SocketListener::run()
{
#ifdef ANDROID
	if (m_pDataPool)
	{
		for (;;) 
        {
			socklen_t addrlen = sizeof(_listenerTowerAddr);
			LOGCAT_DEBUG("Listener waiting on port %d\n", _listenerMyAddr.sin_port);
            
			recvlen = recvfrom(ListenerSocket, buf, BUFSIZE, 0, (struct sockaddr *)&_listenerTowerAddr, &addrlen);
			LOGCAT_DEBUG("Listener received %d bytes\n", recvlen);
            
			if (recvlen > 0)
            {
                buf[recvlen] = 0;
				// LOGCAT_DEBUG("Listener received message: \"%s\"\n", buf);
			}

			if(buf[0]==48) //ID für Gesten
            {
				if(buf[1]==1)
                {
                    GtfEvent gestureEvent(*m_pEventSystem, EVENT_GRP_ID_ExtIn_Tower_Gesture_NS, EVENT_MSG_ID_ExtIn_Tower_Gesture_NS);
                    gestureEvent.Publish();
				}
				if(buf[2]==1)
                {
                    GtfEvent gestureEvent(*m_pEventSystem, EVENT_GRP_ID_ExtIn_Tower_Gesture_SN, EVENT_MSG_ID_ExtIn_Tower_Gesture_SN);
                    gestureEvent.Publish();
				}
				if(buf[3]==1)
                {
                    GtfEvent gestureEvent(*m_pEventSystem, EVENT_GRP_ID_ExtIn_Tower_Gesture_OW, EVENT_MSG_ID_ExtIn_Tower_Gesture_OW);
                    gestureEvent.Publish();
				}
				if(buf[4]==1)
                {
                    GtfEvent gestureEvent(*m_pEventSystem, EVENT_GRP_ID_ExtIn_Tower_Gesture_WO, EVENT_MSG_ID_ExtIn_Tower_Gesture_WO);
                    gestureEvent.Publish();
				}
			}
			else if(buf[0]==49) //ID nur für die Lichtanzeigen
            { 	
                bool normalLichtOn = buf[1]==1;
                bool fernLichtOn = buf[2]==1;
                bool blinkerLinksOn = buf[3]==1;
                bool blinkerRechtsOn = buf[4]==1;
                bool errorOn = buf[5]==1;

				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Indicator_Light_IsOn, GtfTypeId::eTypeId_bool, &normalLichtOn, sizeof(normalLichtOn));		
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Indicator_Fern_IsOn, GtfTypeId::eTypeId_bool, &fernLichtOn, sizeof(fernLichtOn));						
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Indicator_Left_IsOn, GtfTypeId::eTypeId_bool, &blinkerLinksOn, sizeof(blinkerLinksOn));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Indicator_Right_IsOn, GtfTypeId::eTypeId_bool, &blinkerRechtsOn, sizeof(blinkerRechtsOn));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Indicator_Warning_IsOn, GtfTypeId::eTypeId_bool, &errorOn, sizeof(errorOn));
						
				m_pDataPool->Commit(DP_CONTEXT_ID);
			}
			else if(buf[0]==50) //ID nur für die BMS
            {	
                // Start positions for our description strings in the buffer
                static char *stackSpannung = &buf[1];
                static char *durchschnittSpannung = &buf[7];
                static char *niedrigsteZelle = &buf[13];
                static char *hoechsteZelle = &buf[19];
                static char *temperatur = &buf[25];

                // Current (byte = char) length of the data entries
                static int entryLen = 6 * sizeof(char);

                // Please note, that GTF internally will copy the data from the buffer adding a '\0' char at the end
                // therefore we do not need to store the strings locally. Be sure the length is specified correctly!
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_StackSpannung, GtfTypeId::eTypeId_string, stackSpannung, entryLen);
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_HoechsteZelle, GtfTypeId::eTypeId_string, hoechsteZelle, entryLen);
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_NiedrigsteZelle, GtfTypeId::eTypeId_string, niedrigsteZelle, entryLen);
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_DurchschnittSpannung, GtfTypeId::eTypeId_string, durchschnittSpannung, entryLen);
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_Temperatur, GtfTypeId::eTypeId_string, temperatur, entryLen);
						
				m_pDataPool->Commit(DP_CONTEXT_ID);
		    }
		    else if(buf[0]==51) //ID nur für die Zellen
            {
                static int cellInfoLength = 6 * sizeof(char);

                char *currentInfoString = NULL;
                int32_t currentCellState = 0;

                for(int i=0; i < NR_OF_BATTERY_CELLS; ++i)
                {
                    // Read info and convert to model-specific value
                    currentInfoString = &buf[7*i + 1];
                    switch((int) buf[7*i + 7])
                    {
                        case 50:
                            currentCellState = 1; // "Low/Empty"
                            break;
                        case 51:
                            currentCellState = 2; // "Medium"
                            break;
                        case 52:
                            currentCellState = 3; // "Full"
                            break;
                        default:
                            currentCellState = 0; // "Unknown"
                            break;
                    }

                    // Write received data to list properties
                    m_pDataPool->List_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeInfo, i, GtfTypeId::eTypeId_string, currentInfoString, cellInfoLength);
                    m_pDataPool->List_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeState, i, GtfTypeId::eTypeId_int32, &currentCellState, sizeof(currentCellState));
                }
		    }
        }
	}
#endif // ANDROID
}

void SocketListener::initListProperties()
{
    char initString[2] = {}; // Default info value: Empty string
    int32_t initValue = 0; // Default cell state value: 0

    // We need to ensure, that our list properties have the required amount of list elements...
    uint32_t currentListLength = 0;
    int32_t elementsToAdd = 0;

    // Get list length for battery charge info
    if(m_pDataPool->List_GetLength(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeInfo, currentListLength) == GtfTypesDP::eErrorOK)
    {
        elementsToAdd = NR_OF_BATTERY_CELLS - currentListLength;
        while(elementsToAdd > 0)
        {
            m_pDataPool->List_AppendValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeInfo, GtfTypeId::eTypeId_string, initString, 1*sizeof(char));
            --elementsToAdd;
        }
    }

    // Get list length for charge state
    if(m_pDataPool->List_GetLength(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeState, currentListLength) == GtfTypesDP::eErrorOK)
    {
        elementsToAdd = NR_OF_BATTERY_CELLS - currentListLength;
        while(elementsToAdd > 0)
        {
            m_pDataPool->List_AppendValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeState, GtfTypeId::eTypeId_int32, &initValue, sizeof(int));
            --elementsToAdd;
        }
    }

    // Commit all changes from above (appending list values)
    m_pDataPool->Commit(DP_CONTEXT_ID);

    // Initialize all properties to default values
    for(int i=0; i < NR_OF_BATTERY_CELLS; ++i)
    {
        m_pDataPool->List_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeInfo, i, GtfTypeId::eTypeId_string, initString, 1*sizeof(char));
        m_pDataPool->List_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Battery_ChargeState, i, GtfTypeId::eTypeId_int32, &initValue, sizeof(int));
    }

    // Commit all changes from above (writing default values to lists)
    m_pDataPool->Commit(DP_CONTEXT_ID);
}

#ifdef ANDROID
void* run_adapter_listener(void* pObj)
{		
	SocketListener* tSocketListener = (SocketListener*)pObj;
	assert(tSocketListener);
	
	while(true)
    {
		if (tSocketListener->open(32788))
		{
		    LOGCAT_DEBUG("Listener socket created and waiting");
		    tSocketListener->run();
		}
	}

	return NULL;
}
#endif // ANDROID

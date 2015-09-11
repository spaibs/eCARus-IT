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

#include <assert.h>
#include "LogCatLogger.h"
#include "datapool_pdal42.h"
#include "SocketTalker.h"

#ifdef ANDROID
	struct sockaddr_in _talkerTowerAddr;    /* Android Address */
	struct sockaddr_in _talkerMyAddr;		/* My Address */
    pthread_t TalkerThread;

    void* run_adapter_talker(void* pObj);
#endif

void SocketTalker::init(GtfDataPool* pDataPool)
{
	m_pDataPool = pDataPool;
	assert(m_pDataPool);
	lokalUpdatedIn = false;

#ifdef ANDROID
    pthread_create(&TalkerThread, NULL, run_adapter_talker, this);
#endif
}

bool SocketTalker::open(uint16_t port)
{
	bool success = true;

#ifdef ANDROID
	for (int i=0;i<14;i++)
	{
        message[i]=1;
	}
	message[2]=25;
	message[3]=4;
	
	talkerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (talkerSocket < 0)
	{
	    LOGCAT_WARN("Talker socket creation failed. Errno: %d", errno);
		success = false;
	}
	
	else
	{
		memset((char *)&_talkerMyAddr, 0, sizeof(_talkerMyAddr));
		_talkerMyAddr.sin_family = AF_INET;
		_talkerMyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_talkerMyAddr.sin_port = htons(32001);
		
		if (bind(talkerSocket, (struct sockaddr *)&_talkerMyAddr, sizeof(_talkerMyAddr)) < 0)
        {
            LOGCAT_WARN("Talker socket bind failed. Errno: %d", errno);
            perror("bind failed");
	        success = false;
		}
		
		memset((char*)&_talkerTowerAddr, 0, sizeof(_talkerTowerAddr));
		_talkerTowerAddr.sin_family = AF_INET;
		_talkerTowerAddr.sin_port = htons(port);

		struct hostent* localhost = gethostbyname("localhost");
		if (localhost)
		{
            memcpy(&_talkerTowerAddr.sin_addr.s_addr, localhost->h_addr_list[0], sizeof(_talkerTowerAddr.sin_addr.s_addr));
		}
	}
#else
    GTF_UNUSED_PARAM(port)
#endif // ANDROID

	return success;
}

void SocketTalker::run()
{
#ifdef ANDROID
	if (m_pDataPool)
	{	
        /*
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID, DP_ID_ExtOut_Mode_Updated, GtfTypeId::eTypeId_bool, &lokalUpdatedOut, sizeof(lokalUpdatedOut));
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID, DP_ID_ExtOut_Mode_EcoActive, GtfTypeId::eTypeId_bool, &lokalEcoActive, sizeof(lokalEcoActive));			  
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID, DP_ID_ExtOut_Mode_RecActive, GtfTypeId::eTypeId_bool, &lokalRecActive, sizeof(lokalRecActive));			  
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID, DP_ID_ExtOut_Mode_SportActive, GtfTypeId::eTypeId_bool, &lokalSportActive, sizeof(lokalSportActive));
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID, DP_ID_ExtOut_Mode_BoostActive, GtfTypeId::eTypeId_bool, &lokalBoostActive, sizeof(lokalBoostActive));
		
		if(lokalUpdatedIn==lokalUpdatedOut)
		{
		    message[5]=2;

		    lokalUpdatedIn=!lokalUpdatedOut;
    		
		    m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID, DP_ID_ExtIn_Mode_Updated, GtfTypeId::eTypeId_bool, &lokalUpdatedIn, sizeof(lokalUpdatedIn));	 
		    m_pDataPool->Commit(DP_CONTEXT_ID);
		}
		
		if(lokalBoostActive) message[6]=2;
		if(lokalRecActive) message[7]=2;
		if(lokalSportActive) message[8]=2;
		if(lokalEcoActive) message[9]=2;

		if (sendto(talkerSocket, message, strlen(message), 0, (struct sockaddr *)&_talkerTowerAddr, sizeof(_talkerTowerAddr)) < 0) 
		{
			perror("sendto failed");
			return;
		}
		*/

        // TODO

		close(talkerSocket);
	}
#endif // ANDROID
}

void SocketTalker::updateVehicleMode(int32_t requestedMode)
{
    message[5] = 2;

    switch(requestedMode)
    {
        case 1: // ECO Mode
            message[9] = 2;
            break;
        case 2: // Sport Mode
            message[8] = 2;
            break;
        case 3: // Recuperation Mode
            message[7] = 2;
            break;
        case 4: // Boost Mode
            message[6]=2;
            break;
        default:
			LOGCAT_WARN("Unknown vehicle mode: %d", requestedMode);
            break;
    }

#ifdef ANDROID
	if (open(32787))
	{ 
	    if (sendto(talkerSocket, message, strlen(message), 0, (struct sockaddr *)&_talkerTowerAddr, sizeof(_talkerTowerAddr)) < 0) 
	    {
            LOGCAT_ERROR("Sending vehicle mode update failed. Errno: %d", errno);
	    }

        close(talkerSocket);
    }
#endif
}

void SocketTalker::requestBatteryValuesUpdate()
{
    // TODO: Forward requested to tower app
}

#ifdef ANDROID
void* run_adapter_talker(void* pObj)
{		
	SocketTalker* pSocketTalker = (SocketTalker*)pObj;
	assert(pSocketTalker);
	
	while(true)
    {
		if (pSocketTalker->open(32787))
		{ 
		    LOGCAT_DEBUG("Talker socket created");
		    pSocketTalker->run();
		}
		sleep(1);
	}

	return NULL;
}
#endif // ANDROID

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>
#include <string>
using namespace std;

// Set plugin name for static library build
#ifdef USE_STATIC_PLUGINS
#define GTF_OVERLOADED_SYMBOL_PREFIX myapp_
#endif

#include <GtfTypes/GtfStdDef.h>
#include <GtfPluginLoader/GtfPluginSymbols.h>
#include <GtfPluginLoader/GtfPluginLoader.h>
#include <GtfPluginLoader/GtfMainWorkLoop.h>
#include <GtfPluginLoader/GtfPluginFactory.h>
#include <GtfRunlevel/GtfRunlevel.h>
#include <GtfMessenger/GtfMsgExtReceiver.h>
#include <GtfCoreRuntime/GtfCoreModel.h>
#include <GtfDataPool/GtfDataPool.h>
#include <GtfDataPool/GtfVariant.h>
#include <GtfFunctor/FunctorBind.h>
#include <GtfFunctor/Functor.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

#ifndef ASSERT
#include <assert.h>
#define ASSERT(x) assert(x)
#endif

#include "datapool_pdal42.h"

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "myapp", __VA_ARGS__))
#define BUFSIZE 256
//********* talkerSocket ******************

class SocketTalker
{
public:

	SocketTalker()
		: m_pDataPool(NULL)
		, talkerSocket(-1)
	{
	
	}

	~SocketTalker()
	{}

	void init(GtfDataPool* pDataPool);
	bool open(uint16_t port);
	void run();

private:

	GtfDataPool* m_pDataPool;
	int talkerSocket;
	struct sockaddr_in AndroidAddr;    /* Android Address */
	struct sockaddr_in MyAddr;		   /* My Address */
				
	bool localLamp_1Soll,localLamp_2Soll,localLamp_3Soll,localLamp_4Soll;
	bool localLamp_1SollAlt,localLamp_2SollAlt,localLamp_3SollAlt,localLamp_4SollAlt;
	bool localLampAutomodeSoll;
	bool localLampAutomodeSollAlt;
	char message[14];
	
};

void SocketTalker::init(GtfDataPool* pDataPool)
{
	m_pDataPool = pDataPool;
	assert(m_pDataPool);
}

bool SocketTalker::open(uint16_t port)
{
	bool success = true;
		
	for (int i=0;i<14;i++)
	{
	 message[i]=1;
	}
	message[2]=25;
	message[3]=4;
	
	talkerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (talkerSocket < 0)
	{
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT rec", "talker socket creation failed");
		success = false;
	}
	
	else
	{
		memset((char *)&MyAddr, 0, sizeof(MyAddr));
		MyAddr.sin_family = AF_INET;
		MyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		MyAddr.sin_port = htons(32001);
		
		if (bind(talkerSocket, (struct sockaddr *)&MyAddr, sizeof(MyAddr)) < 0) {
		
		perror("bind failed");
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT send", "bind failed");
		success = false;
		
		}
		
		memset((char*)&AndroidAddr, 0, sizeof(AndroidAddr));
		AndroidAddr.sin_family = AF_INET;
		AndroidAddr.sin_port = htons(port);

		struct hostent* localhost = gethostbyname("localhost");
		if (localhost)
		{
		memcpy(&AndroidAddr.sin_addr.s_addr, localhost->h_addr_list[0], sizeof(localhost->h_addr_list[0]));
		}
		
	}

	return success;
}

void SocketTalker::run()
{
	while(true){
		
		if (m_pDataPool)
		{	
				m_pDataPool->Commit(DP_CONTEXT_ID);
				m_pDataPool->Update(DP_CONTEXT_ID);
				
			m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_lamp_1Soll,GtfTypeId::eTypeId_bool,&localLamp_1Soll,sizeof(localLamp_1Soll));
			m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_lamp_2Soll,GtfTypeId::eTypeId_bool,&localLamp_2Soll,sizeof(localLamp_2Soll));
			m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_lamp_3Soll,GtfTypeId::eTypeId_bool,&localLamp_3Soll,sizeof(localLamp_3Soll));
			m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_lamp_4Soll,GtfTypeId::eTypeId_bool,&localLamp_4Soll,sizeof(localLamp_4Soll));
			
			m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_lampAutomodeSoll,GtfTypeId::eTypeId_bool,&localLampAutomodeSoll,sizeof(localLampAutomodeSoll));

			if(localLampAutomodeSollAlt != localLampAutomodeSoll)
			{
				localLampAutomodeSollAlt = localLampAutomodeSoll;
				message[0]= 't';
				message[1]= '0';
				message[2]= '1';
				
				message[3]= '0';
				message[4]= '0';
				message[5]= '0';
				message[6]= '0';
				message[7]= '0';
				message[8]= '0';
				message[9]= '0';
				message[10]='0';
				if(localLampAutomodeSoll){message[10]= '1';}
				
				__android_log_write(ANDROID_LOG_INFO, "ElektroBIT send", "send message...");
				if (sendto(talkerSocket, message, strlen(message), 0, (struct sockaddr *)&AndroidAddr, sizeof(AndroidAddr)) < 0) 
				{
					perror("sendto failed");
					return;
				}
			}
			
			if(localLamp_1Soll != localLamp_1SollAlt || localLamp_2Soll != localLamp_2SollAlt || localLamp_3Soll != localLamp_3SollAlt || localLamp_4Soll != localLamp_4SollAlt)
			{
				localLamp_1SollAlt = localLamp_1Soll;
				localLamp_2SollAlt = localLamp_2Soll;
				localLamp_3SollAlt = localLamp_3Soll;
				localLamp_4SollAlt = localLamp_4Soll;

				
				message[0]= 't';
				message[1]= '0';
				message[2]= '2';
				
				message[3]= '0';
				message[4]= '0';
				message[5]= '0';
				message[6]= '0';
				message[7]= '0';
				message[8]= '0';
				message[9]= '0';
				message[10]= '0';
				
				if(localLamp_1Soll) message[7]='1';
				if(localLamp_2Soll) message[8]='1';
				if(localLamp_3Soll) message[9]='1';
				if(localLamp_4Soll) message[10]='1';
				
				if (sendto(talkerSocket, message, strlen(message), 0, (struct sockaddr *)&AndroidAddr, sizeof(AndroidAddr)) < 0) 
				{
					perror("sendto failed");
					return;
				}
			}
			
		  
			//close(talkerSocket);
			usleep(500);
		}
	}
	close(talkerSocket);
}

void* run_adapter_talker(void* pObj)
{		

	SocketTalker* pSocketTalker = (SocketTalker*)pObj;
	assert(pSocketTalker);
	
	//while(true){
		
		if (pSocketTalker->open(32702))
		{ 
		//__android_log_print(ANDROID_LOG_INFO, "ElektroBIT send", "socket created");
		pSocketTalker->run();
		}
		//usleep(500);
	//}
	return NULL;
}



//*********************************************

//**********ListenerSocket******************
 
 class SocketListener
{
public:

	SocketListener()
		: m_pDataPool(NULL)
		, ListenerSocket(-1)
	{
	
	}

	~SocketListener()
	{}

	void init(GtfDataPool* pDataPool);
	bool open(uint16_t port);
	void run();

private:

	GtfDataPool* m_pDataPool;
	int ListenerSocket;
	int recvlen;
	struct sockaddr_in SenderAddr;  	/* talker address */
	struct sockaddr_in MyAddr;			/* my address */
	unsigned char buf[BUFSIZE];     	/* receive buffer */
	
	// Info about the current status of the car
	char voltage[9];
	char current[9];
	char ID[2];
	char direction;
	char message[8];
	
	bool lamp_1Ist,lamp_2Ist,lamp_3Ist,lamp_4Ist;
	bool lampAutomodeIst;
	
};

void SocketListener::init(GtfDataPool* pDataPool)
{
	m_pDataPool = pDataPool;
	assert(m_pDataPool);
}

bool SocketListener::open(uint16_t port)
{
	bool success = true;

	
	ListenerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if ( ListenerSocket < 0 )
	{
	    __android_log_write(ANDROID_LOG_INFO, "ElektroBIT rec", "listener socket creation failed");
		success = false;
	}
	else
	{
		memset((char *)&MyAddr, 0, sizeof(MyAddr));
		MyAddr.sin_family = AF_INET;
		MyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		MyAddr.sin_port = htons(port);
		
		if (bind( ListenerSocket, (struct sockaddr *)&MyAddr, sizeof(MyAddr)) < 0) {
		perror("bind failed");
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT rec", "bind failed");
		success = false;
		}
	
	}

	return success;
}

void SocketListener::run()
{
	if (m_pDataPool)
	{	
		
		for (;;) {
			
			socklen_t addrlen = sizeof(SenderAddr);
			__android_log_print(ANDROID_LOG_DEBUG, "ElektroBIT rec", "waiting on port %d\n", MyAddr.sin_port);
            
			recvlen = recvfrom(ListenerSocket, buf, BUFSIZE, 0, (struct sockaddr *)&SenderAddr, &addrlen);
			__android_log_print(ANDROID_LOG_DEBUG, "ElektroBIT rec","received %d bytes\n", recvlen);
            
			if (recvlen > 0) {
                    buf[recvlen] = 0;
					__android_log_write(ANDROID_LOG_DEBUG, "ElektroBIT rec", "received message");
			}
			direction = buf[0];
			ID[0] = buf[1];
			ID[1] = buf[2];
					
		
			for(int y=0;y<8;y++)
			{
				message[y]= buf[y+3];
			}
				
			if(direction == 'g')
			{
				if(ID[0] == '0' && ID[1] == '1')
				{
					__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "received voltage");
					
					voltage[0] = message[0];
					voltage[1] = message[1];
					voltage[2] = message[2];
					voltage[3] = message[3];
					voltage[4] = message[4];
					voltage[5] = message[5];
					voltage[6] = message[6];
					voltage[7] = message[7];
					voltage[8] = '\0'; // TERMINATE
					int voltageTemp;
					voltageTemp = atoi(voltage);
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_voltage_raw,GtfTypeId::eTypeId_int32,&voltageTemp,sizeof(voltageTemp));
				}
				
				if(ID[0] == '0' && ID[1] == '2')
				{
					__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "received current");
					current[0] = message[0];
					current[1] = message[1];
					current[2] = message[2];
					current[3] = message[3];
					current[4] = message[4];
					current[5] = message[5]; 
					current[6] = message[6];
					current[7] = message[7];
					current[8] = '\0'; // TERMINATE
					int currentTemp;
					currentTemp = atoi(current);
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_current_raw,GtfTypeId::eTypeId_int32,&currentTemp,sizeof(currentTemp));
				}
					
				if(ID[0] == '0' && ID[1] == '3')
				{
					__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "received automode status");
					if(message[7] == '0'){lampAutomodeIst = false;}else{lampAutomodeIst = true;}
					
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_lampAutomodeIst,GtfTypeId::eTypeId_bool,&lampAutomodeIst,sizeof(lampAutomodeIst));
					
				}

				if(ID[0] == '0' && ID[1] == '4')
				{
					__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "received lamp status");
					if(message[4] == '0'){lamp_1Ist = false;}else{lamp_1Ist = true;}
					if(message[5] == '0'){lamp_2Ist = false;}else{lamp_2Ist = true;}
					if(message[6] == '0'){lamp_3Ist = false;}else{lamp_3Ist = true;}
					if(message[7] == '0'){lamp_4Ist = false;}else{lamp_4Ist = true;}
					
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_lamp_1Ist,GtfTypeId::eTypeId_bool,&lamp_1Ist,sizeof(lamp_1Ist));
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_lamp_2Ist,GtfTypeId::eTypeId_bool,&lamp_2Ist,sizeof(lamp_2Ist));
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_lamp_3Ist,GtfTypeId::eTypeId_bool,&lamp_3Ist,sizeof(lamp_3Ist));
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_lamp_4Ist,GtfTypeId::eTypeId_bool,&lamp_4Ist,sizeof(lamp_4Ist));
				}
				m_pDataPool->Commit(DP_CONTEXT_ID);				
			}
		}
			
	}
	
}

void* run_adapter_listener(void* pObj)
{		

	SocketListener* tSocketListener = (SocketListener*)pObj;
	assert(tSocketAdapter);
	
	while(true){
		
		if (tSocketListener->open(32788))
		{
		__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "socket created andwaiting");
		tSocketListener->run();
		}
		
	}
	return NULL;
}


//***************************************************

//***********original projekt***************************

class MyApp : public gtf::Task
{
private:
    GtfDataPool* m_pDataPool;
    GtfMainWorkLoop* m_pWorkLoop;
	
	SocketTalker socketTalker;
	SocketListener socketListener;
	pthread_t TalkerThread;
	pthread_t ListenerThread;

public:
    MyApp()
    : m_pDataPool(NULL)
    , m_pWorkLoop(NULL)
	
    {}

    void Startup(GtfDataPool* pDataPool,
                 GtfMainWorkLoop* pWorkLoop)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "MyApp Startup");
        m_pDataPool = pDataPool;
        m_pWorkLoop = pWorkLoop;

        // check fingerprint and report on failure
        m_pDataPool->CheckFingerprint(DP_CONTEXT_ID, DP_CONTEXT_FINGERPRINT);

        // register the invoker
        m_pDataPool->SetInvoker
            ( DP_CONTEXT_ID,
              gtf_bind(&GtfMainWorkLoop::PerformTask,
                       m_pWorkLoop,
                       gtf_ref(*this),
                       gtf::Task::ePriorityValueDefault)
            );

        // the datapool delivers notifications only if the property was read before
        m_pDataPool->ForEach(DP_CONTEXT_ID,
                             true,
                             false,
                             gtf_bind(&GtfDataPool::MarkAsRead, m_pDataPool));

			socketTalker.init(m_pDataPool);
			pthread_create(&TalkerThread, NULL, run_adapter_talker, &socketTalker);
		
			socketListener.init(m_pDataPool);
			pthread_create(&ListenerThread, NULL, run_adapter_listener, &socketListener);
		
        // invoke the update processing task once at startup
        m_pWorkLoop->PerformTask(*this);
    }

    void Shutdown()
    {
	__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "MyApp Shutdown");
	}

    virtual void Execute()
    {
	
		// update the datapool properties
		m_pDataPool->Update(DP_CONTEXT_ID);
		

        // fetch all datapool notifications
        m_pDataPool->Fetch(DP_CONTEXT_ID,
                           gtf_bind(&MyApp::ProcessDatapool, this));
						   
        // commit all changed properties
        m_pDataPool->Commit(DP_CONTEXT_ID);
		
	}	

    virtual void Dispose()
    {
	__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "MyApp Dispose");
	}

    void ProcessDatapool(const GtfTypesDP::itemId_t* const c_pItemId,
                         const uint32_t c_itemCount)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "processdatavalue");
        for (uint32_t index = 0; index < c_itemCount; index++)
        {
            switch(c_pItemId[index])
            {
                
                    break;
            }
        }
    }
};

class MyReceiver : public GtfMsgExtReceiver
{
public:
    MyReceiver()
    : m_pCoreModel(NULL)
    , m_pWorkLoop(NULL)
    {}

    void startup(GtfPluginLoader& rLoader)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "myreceiver startup");
        GtfMsgExtReceiver::Init(rLoader);

        if (m_pWorkLoop == NULL)
        {
            m_pWorkLoop = static_cast<GtfMainWorkLoop*>(
                    rLoader.getInterface(
                        static_cast<uint32_t>(GtfMainWorkLoop::cm_gtfInterfaceId),
                        static_cast<uint32_t>(GtfMainWorkLoop::cm_gtfInterfaceVersion)));
        }

        if(GetMessenger())
        {
            GetMessenger()->SubscribeSysMsg(*this);
        }
    }

    void shutdown()
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "receiver shutdown");
        if(GetMessenger())
        {
            GetMessenger()->UnsubscribeSysMsg(*this);
        }
    }

    virtual void HandleMessage(const GtfMsgHandle c_MsgHandle)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "receiver handlemessage");
        GTF_UNUSED_PARAM(c_MsgHandle)
    }

    virtual uint16_t HandleRunlevel(const uint16_t c_runlevel,
                                    const int8_t c_direction)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "receiver handlerunlevel");
        if (m_pCoreModel)
        {
            if(0 < c_direction && GTF_RUNLEVEL_MAX == c_runlevel)
            {
                // restore the default values of global properties
                m_pCoreModel->getVariant()->Restore(DP_CONTEXT_ID);

                // startup the application
                m_myApp.Startup(m_pCoreModel->getDatapool(), m_pWorkLoop);
            }

            if(0 > c_direction && GTF_RUNLEVEL_MAX == c_runlevel)
            {
                // shutdown the application
                m_myApp.Shutdown();
            }
        }

        return c_runlevel;
    }

    virtual void HandleInterface(const uint32_t c_interfaceId,
                                 const uint32_t c_version,
                                 const uint16_t c_validRunlevel,
                                 void * const c_pObj)
    {
		__android_log_write(ANDROID_LOG_INFO, "ElektroBIT", "receiver handleinterface");
        GTF_UNUSED_PARAM(c_validRunlevel)

        if (c_pObj != NULL)
        {
            switch(c_interfaceId)
            {
                case static_cast<uint32_t>(GtfCoreModel::cm_gtfInterfaceId):
                    if( c_version == static_cast<uint32_t>(GtfCoreModel::cm_gtfInterfaceVersion) )
                    {
                        m_pCoreModel = static_cast<GtfCoreModel*>(c_pObj);
                    }
                    break;
            }
        }
    }
private:
    GtfCoreModel* m_pCoreModel;
    GtfMainWorkLoop* m_pWorkLoop;
    MyApp m_myApp;
	
};

// *** GTF plugin interface ****************************************************

GTF_PLUGIN_SO_SYMBOL GtfPlugin * GTF_CREATE_PLUGIN_SYMBOL(uint32_t argc, char** argv)
{
    GTF_UNUSED_PARAM(argc);
    GTF_UNUSED_PARAM(argv);
    return GtfPluginFactory<MyReceiver>::create();
}

GTF_PLUGIN_SO_SYMBOL void GTF_DESTROY_PLUGIN_SYMBOL(GtfPlugin * const c_pPlugin)
{
    GtfPluginFactory<MyReceiver>::destroy(c_pPlugin);
}
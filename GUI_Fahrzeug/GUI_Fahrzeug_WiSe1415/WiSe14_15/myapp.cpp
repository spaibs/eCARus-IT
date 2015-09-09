// file: myapp.cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>

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
				
	bool lokalUpdatedOut,lokalUpdatedIn;

	bool lokalBoostActive,lokalRecActive;
	bool lokalSportActive,lokalEcoActive;
	
	char message[14];
	
};

void SocketTalker::init(GtfDataPool* pDataPool)
{
	m_pDataPool = pDataPool;
	assert(m_pDataPool);
	lokalUpdatedIn=false;
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
	    __android_log_write(ANDROID_LOG_INFO, "ElektroBIT send", "socket creation failed");
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
	if (m_pDataPool)
	{	
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_UpdatedOut,GtfTypeId::eTypeId_bool,&lokalUpdatedOut,sizeof(lokalUpdatedOut));
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_EcoActive,GtfTypeId::eTypeId_bool,&lokalEcoActive,sizeof(lokalEcoActive));			  
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_RecActive,GtfTypeId::eTypeId_bool,&lokalRecActive,sizeof(lokalRecActive));			  
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_SportActive,GtfTypeId::eTypeId_bool,&lokalSportActive,sizeof(lokalSportActive));
		m_pDataPool->Scalar_ReadValue(DP_CONTEXT_ID,DP_ID_BoostActive,GtfTypeId::eTypeId_bool,&lokalBoostActive,sizeof(lokalBoostActive));
		
		if(lokalUpdatedIn==lokalUpdatedOut)
		{
		
		message[5]=2;

		lokalUpdatedIn=!lokalUpdatedOut;
		
		m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_UpdatedIn,GtfTypeId::eTypeId_bool,&lokalUpdatedIn,sizeof(lokalUpdatedIn));	 
		m_pDataPool->Commit(DP_CONTEXT_ID);

		}
		
		if(lokalBoostActive) message[6]=2;
		if(lokalRecActive) message[7]=2;
		if(lokalSportActive) message[8]=2;
		if(lokalEcoActive) message[9]=2;

		if (sendto(talkerSocket, message, strlen(message), 0, (struct sockaddr *)&AndroidAddr, sizeof(AndroidAddr)) < 0) 
		{
			perror("sendto failed");
			return;
		}
				  
		close(talkerSocket);
	}
}

void* run_adapter_talker(void* pObj)
{		

	SocketTalker* pSocketTalker = (SocketTalker*)pObj;
	assert(pSocketTalker);
	
	while(true){
		
		if (pSocketTalker->open(32787))
		{ 
		__android_log_print(ANDROID_LOG_INFO, "ElektroBIT send", "socket created");
		pSocketTalker->run();
		}
		sleep(1);
	}
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
	
	bool gestureN_S, gestureS_N;
	bool gestureO_W, gestureW_O;
	
	bool normalLichtOn, fernLichtOn;
	bool blinkerLinksOn, blinkerRechtsOn;
	bool errorOn;
	
	char StackSpannung[7];
	char Strom[7];
	char NiedrigsteZelle[7];
	char HochsteZelle[7];
	char Temperatur[7];

	char Zelle1[8],Zelle2[8],Zelle3[8],Zelle4[8];
	char Zelle5[8],Zelle6[8],Zelle7[8],Zelle8[8];
	char Zelle9[8],Zelle10[8],Zelle11[8],Zelle12[8];
	char Zelle13[8],Zelle14[8],Zelle15[8],Zelle16[8];
	bool ZustandON,ZustandOFF;
	int ZelleZustand[16];
	
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
	    __android_log_write(ANDROID_LOG_INFO, "ElektroBIT rec", "socket creation failed");
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
			__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec", "waiting on port %d\n", MyAddr.sin_port);
            
			recvlen = recvfrom(ListenerSocket, buf, BUFSIZE, 0, (struct sockaddr *)&SenderAddr, &addrlen);
			__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec","received %d bytes\n", recvlen);
            
			if (recvlen > 0) {
                    buf[recvlen] = 0;
					//__android_log_print(ANDROID_LOG_INFO, "ElektroBIT rec","received message: \"%s\"\n", buf);
			}

			if(buf[0]==48) { //ID für Gesten
				if(buf[1]==1){ // fire Event N-S
					gestureN_S = true;
				} else gestureN_S = false;
				if(buf[2]==1){ // fire Event S-N
					gestureS_N = true;
				} else gestureS_N = false;
				if(buf[3]==1){ // fire Event O-W
					gestureO_W = true;
				} else gestureO_W = false;
				if(buf[4]==1){ // fire Event W-O
					gestureW_O = true;
				} else gestureW_O = false;
				
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_gestureN_S,GtfTypeId::eTypeId_bool,&gestureN_S,sizeof(gestureN_S)); // TODO EB VARIABLEN EINFUEGEN
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_gestureS_N,GtfTypeId::eTypeId_bool,&gestureS_N,sizeof(gestureS_N));						
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_gestureO_W,GtfTypeId::eTypeId_bool,&gestureO_W,sizeof(gestureO_W));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_gestureW_O,GtfTypeId::eTypeId_bool,&gestureW_O,sizeof(gestureW_O));
				
				m_pDataPool->Commit(DP_CONTEXT_ID);
			}
			
			if(buf[0]==49) { //ID nur für die Lichtanzeigen
							
				if(buf[1]==1)	{normalLichtOn=true;}
				else 			{normalLichtOn=false;}
				if(buf[2]==1) 	{fernLichtOn=true;}
				else 			{fernLichtOn=false;}
				if(buf[3]==1)	{blinkerLinksOn=true;}
				else 			{blinkerLinksOn=false;}
				if(buf[4]==1) 	{blinkerRechtsOn=true;}
				else 			{blinkerRechtsOn=false;}
				if(buf[5]==1) 	{errorOn=true;}
				else 			{errorOn=false;}

					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_NormalesLichtOn,GtfTypeId::eTypeId_bool,&normalLichtOn,sizeof(normalLichtOn));		
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_FernLichtOn,GtfTypeId::eTypeId_bool,&fernLichtOn,sizeof(fernLichtOn));						
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_BlinkerLeftOn,GtfTypeId::eTypeId_bool,&blinkerLinksOn,sizeof(blinkerLinksOn));
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_BlinkerRechtsOn,GtfTypeId::eTypeId_bool,&blinkerRechtsOn,sizeof(blinkerRechtsOn));
					m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_errorOn,GtfTypeId::eTypeId_bool,&errorOn,sizeof(errorOn));
						
				m_pDataPool->Commit(DP_CONTEXT_ID);
			}
		
			if(buf[0]==50) { //ID nur für die BMS
		
				for(int y=0;y<6;y++)
				{
					StackSpannung[y]=buf[1+y];
					Strom[y]=buf[7+y];
					NiedrigsteZelle[y]=buf[13+y];
					HochsteZelle[y]=buf[19+y];
					Temperatur[y]=buf[25+y];
				}
					StackSpannung[6]=0;
					Strom[6]=0;
					NiedrigsteZelle[6]=0;
					HochsteZelle[6]=0;
					Temperatur[6]=0;
		
						m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_StackSpg,GtfTypeId::eTypeId_string,StackSpannung,sizeof(StackSpannung));
						m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_HchsteZelle,GtfTypeId::eTypeId_string,HochsteZelle,sizeof(HochsteZelle));
						m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_NiederZelle,GtfTypeId::eTypeId_string,NiedrigsteZelle,sizeof(NiedrigsteZelle));
						m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_DurchschnittSpg,GtfTypeId::eTypeId_string,Strom,sizeof(Strom));
						m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Temperatur,GtfTypeId::eTypeId_string,Temperatur,sizeof(Temperatur));
						
				m_pDataPool->Commit(DP_CONTEXT_ID);
		}

		if(buf[0]==51) { //ID nur für die Zellen
		
			ZustandOFF	=	false;
			ZustandON 	=	true;

			for(int y=0;y<7;y++)
			{
				Zelle1[y]=buf[1+y];	Zelle2[y]=buf[8+y];
				Zelle3[y]=buf[15+y]; Zelle4[y]=buf[22+y];
				Zelle5[y]=buf[29+y]; Zelle6[y]=buf[36+y];
				Zelle7[y]=buf[43+y]; Zelle8[y]=buf[50+y];
				Zelle9[y]=buf[57+y]; Zelle10[y]=buf[64+y];
				Zelle11[y]=buf[71+y]; Zelle12[y]=buf[78+y];
				Zelle13[y]=buf[85+y]; Zelle14[y]=buf[92+y];
				Zelle15[y]=buf[99+y]; Zelle16[y]=buf[106+y];
			}

			for(int t=0;t<16;t++)
			{
				ZelleZustand[t]=(int)buf[7*t+7];
			}

				Zelle1[6]=0;	Zelle2[6]=0;	Zelle3[6]=0;	Zelle4[6]=0;
				Zelle5[6]=0;	Zelle6[6]=0;	Zelle7[6]=0;	Zelle8[6]=0;
				Zelle9[6]=0;	Zelle10[6]=0;	Zelle11[6]=0;	Zelle12[6]=0;
				Zelle13[6]=0;	Zelle14[6]=0;	Zelle15[6]=0;	Zelle16[6]=0;

			for(int x=0;x<16;x++){

				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_fully_charged1+x,GtfTypeId::eTypeId_bool,&ZustandOFF,sizeof(ZustandOFF));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_partially_charged1+x,GtfTypeId::eTypeId_bool,&ZustandOFF,sizeof(ZustandOFF));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_low_charge1+x,GtfTypeId::eTypeId_bool,&ZustandOFF,sizeof(ZustandOFF));

				switch(ZelleZustand[x])
				{
					case 50: 	m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_low_charge1+x,GtfTypeId::eTypeId_bool,&ZustandON,sizeof(ZustandON));break;
					case 51: 	m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_partially_charged1+x,GtfTypeId::eTypeId_bool,&ZustandON,sizeof(ZustandON));break;
					case 52:	m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_fully_charged1+x,GtfTypeId::eTypeId_bool,&ZustandON,sizeof(ZustandON));break;
					default: 	break;
				}

			}	
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle1,GtfTypeId::eTypeId_string,Zelle1,sizeof(Zelle1));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle2,GtfTypeId::eTypeId_string,Zelle2,sizeof(Zelle2));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle3,GtfTypeId::eTypeId_string,Zelle3,sizeof(Zelle3));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle4,GtfTypeId::eTypeId_string,Zelle4,sizeof(Zelle4));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle5,GtfTypeId::eTypeId_string,Zelle5,sizeof(Zelle5));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle6,GtfTypeId::eTypeId_string,Zelle6,sizeof(Zelle6));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle7,GtfTypeId::eTypeId_string,Zelle7,sizeof(Zelle7));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle8,GtfTypeId::eTypeId_string,Zelle8,sizeof(Zelle8));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_Zelle9,GtfTypeId::eTypeId_string,Zelle9,sizeof(Zelle9));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA10,GtfTypeId::eTypeId_string,Zelle10,sizeof(Zelle10));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA11,GtfTypeId::eTypeId_string,Zelle11,sizeof(Zelle11));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA12,GtfTypeId::eTypeId_string,Zelle12,sizeof(Zelle12));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA13,GtfTypeId::eTypeId_string,Zelle13,sizeof(Zelle13));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA14,GtfTypeId::eTypeId_string,Zelle14,sizeof(Zelle14));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA15,GtfTypeId::eTypeId_string,Zelle15,sizeof(Zelle15));
				m_pDataPool->Scalar_WriteValue(DP_CONTEXT_ID,DP_ID_ZelleA16,GtfTypeId::eTypeId_string,Zelle16,sizeof(Zelle16));

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

// end of file: myapp.cpp

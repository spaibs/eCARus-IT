#ifndef SOCKET_LISTENER_H_INCLUDED
#define SOCKET_LISTENER_H_INCLUDED

#include <GtfDataPool/GtfDataPool.h>
#include <GtfCoreRuntime/GtfCoreModel.h>

#define BUFSIZE 256

class SocketListener
{
public:

	SocketListener()
		: m_pDataPool(NULL)
		, ListenerSocket(-1)
	{}

	~SocketListener()
	{}

	void init(GtfDataPool* pDataPool, GtfExternalEventSystem* pEventSystem);
	bool open(uint16_t port);
	void run();

private:

	GtfDataPool* m_pDataPool;
    GtfExternalEventSystem* m_pEventSystem;
	int ListenerSocket;
	int recvlen;
	char buf[BUFSIZE];     	/* receive buffer */

    void initListProperties();
};

#endif // SOCKET_LISTENER_H_INCLUDED

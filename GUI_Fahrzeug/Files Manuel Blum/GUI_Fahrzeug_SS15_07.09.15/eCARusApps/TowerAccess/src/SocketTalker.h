#ifndef SOCKET_TALKER_H_INCLUDED
#define SOCKET_TALKER_H_INCLUDED

#include <GtfDataPool/GtfDataPool.h>

class SocketTalker
{
public:

	SocketTalker()
		: m_pDataPool(NULL)
		, talkerSocket(-1)
    {}

	~SocketTalker()	
    {}

	void init(GtfDataPool* pDataPool);
	bool open(uint16_t port);
	void run();

    void updateVehicleMode(int32_t requestedMode);
    void requestBatteryValuesUpdate();

private:

	GtfDataPool* m_pDataPool;
	int talkerSocket;
				
	bool lokalUpdatedOut,lokalUpdatedIn;

	bool lokalBoostActive,lokalRecActive;
	bool lokalSportActive,lokalEcoActive;
	
	char message[14];
	
};

#endif // SOCKET_TALKER_H_INCLUDED

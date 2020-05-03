#ifndef _PLUGINPACKET_H_
#define _PLUGINPACKET_H_

#include "plugininterface.h"
	
class PluginPacket : public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OPENJABNAB_PLUGIN_IFACE_VERSION_STRING)
	Q_INTERFACES(PluginInterface)

public:
	PluginPacket();
	virtual ~PluginPacket() {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SendPacket);
	PLUGIN_BUNNY_API_CALL(Api_SendMessage);
};

#endif

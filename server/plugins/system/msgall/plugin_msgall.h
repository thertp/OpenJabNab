#ifndef _PLUGINMSGALL_H_
#define _PLUGINMSGALL_H_

#include "plugininterface.h"

class PluginMsgall : public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OPENJABNAB_PLUGIN_IFACE_VERSION_STRING)
	Q_INTERFACES(PluginInterface)

public:
	PluginMsgall();
	virtual ~PluginMsgall();
void InitApiCalls();
	PLUGIN_API_CALL(Api_Say);
};

#endif

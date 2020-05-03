#ifndef _PLUGINEARS_H_
#define _PLUGINEARS_H_

#include "plugininterface.h"
	
class PluginEars : public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OPENJABNAB_PLUGIN_IFACE_VERSION_STRING)
	Q_INTERFACES(PluginInterface)

public:
	PluginEars();
	virtual ~PluginEars();
	bool OnEarsMove(Bunny *, int, int);

	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_getFriend);
	PLUGIN_BUNNY_API_CALL(Api_setFriend);
	
};

#endif

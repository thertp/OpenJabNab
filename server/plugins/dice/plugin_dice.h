#ifndef _PLUGINDICE_H_
#define _PLUGINDICE_H_

#include "plugininterface.h"
	
class PluginDice : public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OPENJABNAB_PLUGIN_IFACE_VERSION_STRING)
	Q_INTERFACES(PluginInterface)

public:
	PluginDice();
	virtual ~PluginDice();
	bool OnClick(Bunny *, PluginInterface::ClickType);

};

#endif

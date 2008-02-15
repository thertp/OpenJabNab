#include "bunny.h"
#include "log.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include "stdio.h"

Bunny::Bunny(QByteArray const& bunnyID)
{
	// Check bunnies folder
	QDir bunniesDir = QDir(QCoreApplication::applicationDirPath());
	if (!bunniesDir.cd("bunnies"))
	{
		if (!bunniesDir.mkdir("bunnies"))
		{
			Log::Error("Unable to create bunnies directory !\n");
			exit(-1);
		}
		bunniesDir.cd("bunnies");
	}
	id = bunnyID;
	state = Disconnected;
	configFileName = bunniesDir.absoluteFilePath(bunnyID.toHex()+".dat");
	xmppHandler = 0;
	
	// Check if config file exists and load it
	if (QFile::exists(configFileName))
		LoadConfig();
		
	saveTimer = new QTimer(this);
	connect(saveTimer, SIGNAL(timeout()), this, SLOT(SaveConfig()));
	saveTimer->start(10000);
}

Bunny::~Bunny()
{
	SaveConfig();
}

void Bunny::LoadConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		Log::Error("Cannot open config file for reading : " + configFileName);
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_3);
	in >> GlobalSettings >> PluginsSettings;
	if (in.status() != QDataStream::Ok)
	{
		Log::Warning("Problem when loading config file for bunny : " + id.toHex());
	}
}

void Bunny::SaveConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		Log::Error("Cannot open config file for reading : " + configFileName);
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_3);
	out << GlobalSettings << PluginsSettings;
}

void Bunny::SetXmppHandler(XmppHandler * x)
{ 
	state = Connected;
	xmppHandler = x;
}

void Bunny::SendPacket(Packet const& p)
{
	if (xmppHandler)
		xmppHandler->WritePacketToBunny(p);
}

QVariant Bunny::GetGlobalSetting(QString const& key, QVariant const& defaultValue)
{
	if (GlobalSettings.contains(key))
		return GlobalSettings.value(key);
	else
		return defaultValue;
}

void Bunny::SetGlobalSetting(QString const& key, QVariant & value)
{
	GlobalSettings.insert(key, value);
}

QVariant Bunny::GetPluginSetting(QString const& pluginName, QString const& key, QVariant const& defaultValue)
{
	if (PluginsSettings[pluginName].contains(key))
		return PluginsSettings[pluginName].value(key);
	else
		return defaultValue;
}

void Bunny::SetPluginSetting(QString const& pluginName, QString const& key, QVariant & value)
{
	PluginsSettings[pluginName].insert(key, value);
}

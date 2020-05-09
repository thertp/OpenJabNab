#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QDir>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QDebug>
#include "global.h"

class OJN_EXPORT GlobalSettings
{
public:
	~GlobalSettings();
	static void Init();
	static void Close();

	// Without default value
	static QVariant Get(QString const& key);
	static QString GetString(QString const& key);
	static int GetInt(QString const& key);

	// With default value
	static QVariant Get(QString const& key, QVariant const& defaultValue);
	static QString GetString(QString const& key, QString const& defaultValue);
	static int GetInt(QString const& key, int defaultValue);
	
	static bool HasKey(QString const& key);
	
	static QDir AccountsDir();
	static QDir BunniesDir();
	static QDir ZtampDir();
	static QDir LogDir();
	static QDir TTSSettingsDir();
	static QDir PluginSettingsDir();
	static QDir SettingsDir();
	static QDir PluginDir();
	static QDir TTSPluginDir();

private:
	GlobalSettings();
	static GlobalSettings * instance;
	QSettings * settings;
	static QDir CheckSettingsDir(QString subdir);
	static QDir CheckPluginsDir(QString subdir);
};

inline void GlobalSettings::Init()
{
	if (!instance)
		instance = new GlobalSettings();
}

inline void GlobalSettings::Close()
{
	if (instance)
		delete instance;
}

inline QString GlobalSettings::GetString(QString const& key)
{
	return Get(key).toString();
}

inline int GlobalSettings::GetInt(QString const& key)
{
	return Get(key).toInt();
}

inline QString GlobalSettings::GetString(QString const& key, QString const& defaultValue)
{
	return Get(key, defaultValue).toString();
}

inline int GlobalSettings::GetInt(QString const& key, int defaultValue)
{
	return Get(key, defaultValue).toInt();
}


inline QDir GlobalSettings::CheckSettingsDir(QString subdir)
{
	QString base = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QStringList baselist = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
	QDir basedir = QDir(base);
	QDir compat = QDir(QCoreApplication::applicationDirPath());

	/*
	 * to no break current deployments, if the "compat"
	 * directory exists, use it
	 */
	if (compat.exists() && (compat.absolutePath() != "/usr/bin")) {
		if ((subdir != "") && !compat.cd(subdir)) {
			if (!compat.mkdir(subdir)) {
				LogError("Unable to create " + subdir + " settings directory !\n");
				exit(-1);
			}
			compat.cd(subdir);
		}
		return compat;
	}

	if ((baselist.length() == 0) && (base.length() == 0)) {
		LogError("Unable to determine settings directory to use\n");
		exit(-1);
	}

	/*
         * Try to find configuration directory
         */
	for (int i = 0; i < baselist.length(); ++i) {
		QDir d = QDir(baselist.at(i));
		if (d.exists() && d.cd(subdir))
			return  d;
	}

	/*
	 * Configuration directory not found, so use writable configuration dir
	 */
	if (!basedir.exists()) {
		if (!basedir.mkpath(".")) {
			LogError("Unable to create " + basedir.path() + "settings directory !\n");
			exit(-1);
		}
	}
	if ((subdir != "") && !basedir.cd(subdir)) {
		if (!basedir.mkpath(subdir)) {
			LogError("Unable to create " + subdir + " settings directory !\n");
			exit(-1);
		}
		basedir.cd(subdir);
	}

	return basedir;
}

inline QDir GlobalSettings::CheckPluginsDir(QString subdir)
{
	QString sysplugpath, appplugpath;
	QDir sysplugpathdir, appplugpathdir, appdir;
	QDir ret = QDir("/");

	sysplugpath = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
	sysplugpath += QDir::separator();
	sysplugpath += "openjabnab";
	sysplugpath += QDir::separator()+subdir;
	sysplugpathdir = QDir(sysplugpath);

	appdir = QDir(QCoreApplication::applicationDirPath());
	appplugpath = QCoreApplication::applicationDirPath();
	appplugpath += QDir::separator()+subdir;
	appplugpathdir = QDir(appplugpath);

	if (sysplugpathdir.exists())
		ret = sysplugpathdir;
	// Compat with existing setups.
	if (appplugpathdir.exists() && (appdir.absolutePath() != "/usr/bin"))
		ret = appplugpathdir;

	if (ret.absolutePath() == "/") {
		LogError("Unable to find plugin directory\n");
		exit(-1);
	}

	return ret;
}

inline QDir GlobalSettings::AccountsDir()
{
	return CheckSettingsDir("accounts");
}

inline QDir GlobalSettings::BunniesDir()
{
	return CheckSettingsDir("bunnies");
}

inline QDir GlobalSettings::ZtampDir()
{
	return CheckSettingsDir("ztamps");
}

inline QDir GlobalSettings::LogDir()
{
	QDir logdir = QDir("/var/log/openjabnab");
	QDir compat = QDir(QCoreApplication::applicationDirPath());

	/*
	 * to no break current deployments, if the "compat"
	 * directory exists, use it
	 */
	if (compat.absolutePath() != "/usr/bin")
		return compat;

	if (!logdir.exists())
		qWarning() << "Log directory doesnt exist\n";

	return logdir;
}

inline QDir GlobalSettings::TTSSettingsDir()
{
	return CheckSettingsDir("tts");
}

inline QDir GlobalSettings::PluginSettingsDir()
{
	return CheckSettingsDir("plugins");
}

inline QDir GlobalSettings::SettingsDir()
{
	return CheckSettingsDir("");
}

inline QDir GlobalSettings::PluginDir()
{
	return CheckPluginsDir("plugins");
}

inline QDir GlobalSettings::TTSPluginDir()
{
	return CheckPluginsDir("tts");
}

#endif

#ifndef _LOG_H_
#define _LOG_H_

#include <QString>
#include <QTextStream>

class Log
{
private:
	enum LogLevel { Log_None = 0, Log_Info, Log_Error, Log_Warn, Log_Debug};

public:
	static void LogToFile(QString const&, LogLevel);
	static void Debug(QString const& data) { LogToFile(data, Log_Debug); }
	static void Warning(QString const& data) { LogToFile(data, Log_Warn); }
	static void Error(QString const& data) { LogToFile(data, Log_Error); }
	static void Info(QString const& data) { LogToFile(data, Log_Info); }
	
private:
	Log();

	static Log * instance;
	QTextStream logStream;
	LogLevel maxFileLogLevel;
	LogLevel maxScreenLogLevel;
};

#endif

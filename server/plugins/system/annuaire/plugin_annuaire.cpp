#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QUrl>
#include <QXmlStreamReader>
#include "bunny.h"
#include "account.h"
#include "plugin_annuaire.h"

PluginAnnuaire::PluginAnnuaire():PluginInterface("annuaire", "Register the bunny on the central directory", SystemPlugin)
{
}

PluginAnnuaire::~PluginAnnuaire() {}

void PluginAnnuaire::OnBunnyConnect(Bunny * b)
{
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QString url = GetSettings("global/URL", "").toString();

	if(url != "") {
		QString api = b->GetGlobalSetting("VApiEnable", false).toBool() ? "1" : "0";
		QString pub = b->GetGlobalSetting("VApiPublic", false).toBool() ? "1" : "0";

	        connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(quit()));

		url += "/nabconnection.php?m="+ b->GetID();
		url += "&n=" + b->GetBunnyName();
		url += "&s=" + GlobalSettings::GetString("OpenJabNabServers/PingServer");
		url += "&ip=" + b->GetGlobalSetting("LastIP", QString("")).toString();
		url += "&api=" + api;
		url += "&public=" + pub;
        	manager->get(QNetworkRequest(url));
	}
}

QList<BunnyInfos> PluginAnnuaire::SearchBunny(QString endurl)
{
	QEventLoop loop;
	QNetworkReply *reply;
	QNetworkRequest req;
	QString url;
	QXmlStreamReader xml;
	QString currentTag;
	BunnyInfos currentBunny;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);

	QObject::connect(manager, SIGNAL(finished( QNetworkReply* )), &loop, SLOT(quit()));
	url = GetSettings("global/URL", "").toString() + endurl;
	req.setUrl(QUrl(url));
	loop.exec();
	reply = manager->get(req);
	reply->deleteLater();

	xml.clear();
	xml.addData(reply->readAll());
	QList<BunnyInfos> whois = QList<BunnyInfos>();

	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() || xml.isEndElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if(currentTag == "name")
			{
				currentBunny.name = xml.text().toString();
			}
			else if(currentTag == "macaddress")
			{
				currentBunny.ID = xml.text().toString().toLatin1();
			}
			else if(currentTag == "server")
			{
				currentBunny.server = xml.text().toString();
			}
		}
		if(xml.isEndElement() && currentTag == "bunny")
		{
			whois.append(currentBunny);
		}
	}

	return whois;
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByName(QString name)
{
	return SearchBunny("/whois.php?n=" + QUrl::toPercentEncoding(name));
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByMac(QByteArray ID)
{
	return SearchBunny("/whois.php?nm" + QUrl::toPercentEncoding(QString(ID)));
}
/*******/
/* API */
/*******/
void PluginAnnuaire::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setURL(url)", PluginAnnuaire, Api_setURL);
	DECLARE_PLUGIN_API_CALL("getURL()", PluginAnnuaire, Api_getURL);
	DECLARE_PLUGIN_API_CALL("searchbunnybymac(mac)", PluginAnnuaire, Api_SearchBunnyByMac);
	DECLARE_PLUGIN_API_CALL("searchbunnybyname(name)", PluginAnnuaire, Api_SearchBunnyByName);
	DECLARE_PLUGIN_API_CALL("verifymactoken(mac,reqtoken)", PluginAnnuaire, Api_VerifyMacToken);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_setURL)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString url = hRequest.GetArg("url");
	SetSettings("global/URL", url);

	return new ApiManager::ApiOk(QString("URL set to '%1'.").arg(url));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_getURL)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(QString("%1").arg(GetSettings("global/URL", "").toString()));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByMac)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByMac(hRequest.GetArg("mac").toLatin1());
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByName)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByName(hRequest.GetArg("name"));
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_VerifyMacToken)
{
	Q_UNUSED(account);
	Bunny * b = BunnyManager::GetBunny(hRequest.GetArg("mac").toLatin1());
	QString xml = "";
		
	if (hRequest.GetArg("reqtoken").toLatin1()==b->GetGlobalSetting("VApiToken","FAILED").toString() ) 
		xml += "<verify>true</verify>\n";
	else 
		xml += "<verify>false</verify>\n";
	
	return new ApiManager::ApiXml(xml);

}

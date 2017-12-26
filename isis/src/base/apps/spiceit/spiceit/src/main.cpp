/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <httpserver/httplistener.h>
#include <logging/filelogger.h>
#include "requesthandler.h"

using namespace stefanfrings;

/** Logger class */
FileLogger* logger;

/** Search the configuration file */
QString searchConfigFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QString fileName(appName+".ini");

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    searchList.append(binDir+"/../../etc"); // for development without shadow build
    searchList.append(binDir+"/../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(binDir+"/../../../../../"+appName+"/etc"); // for development with shadow build
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            // found
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
    return 0;
}

/**
  Entry point of the program.
  Quick and dirty, without cleaning up on exit (which is Ok for this simple program).
*/
int main(int argc, char *argv[])
{

    // Initialize the core application
    QCoreApplication app(argc, argv);
    app.setApplicationName("spiceit");
    app.setOrganizationName("USGS-Astrogeology");

    // Find the configuration file
    QString configFileName=searchConfigFile();

    // Configure logging into a file
    /*
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    logSettings->beginGroup("logging");
    logger=new FileLogger(logSettings,10000,&app);
    logger->installMsgHandler();
    */

    // Configure and start the TCP listener
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings,new RequestHandler(&app),&app);

    qWarning("Application has started");

    app.exec();

    qWarning("Application has stopped");
}

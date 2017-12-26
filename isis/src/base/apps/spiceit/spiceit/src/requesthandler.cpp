/**
  @file
  @author Stefan Frings
*/

#include <logging/filelogger.h>
#include "requesthandler.h"


/** Logger class */
extern FileLogger* logger;


RequestHandler::RequestHandler(QObject* parent)
    :HttpRequestHandler(parent)
{
    qDebug("RequestHandler: created");
}


RequestHandler::~RequestHandler()
{
    qDebug("RequestHandler: deleted");
}


void RequestHandler::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    qDebug("Conroller: path=%s",path.data());

    // Set a response header
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    // Return a simple HTML document
    response.write("<html><body>Hello World</body></html>",true);

    qDebug("RequestHandler: finished request");

    // Clear the log buffer
    if (logger)
    {
       logger->clear();
    }
}

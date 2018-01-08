#include "SpiceClient.h"

#include <iostream>
#include <sstream>
#include <QDataStream>
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include "Application.h"
#include "Constants.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"
#include "Table.h"

using namespace std;
using namespace Isis;

namespace Isis {
  /**
   * This initializes a SpiceClient. It forms the Json file to send and
   * starts in its own thread.
   *
   * @param url
   * @param port
   * @param cubeLabel
   * @param ckSmithed
   * @param ckRecon
   * @param ckNadir
   * @param ckPredicted
   * @param spkSmithed
   * @param spkRecon
   * @param spkPredicted
   * @param shape
   * @param startPad
   * @param endPad
   */
  SpiceClient::SpiceClient(QString url, int port, Pvl &cubeLabel,
      bool ckSmithed, bool ckRecon, bool ckNadir, bool ckPredicted,
      bool spkSmithed, bool spkRecon, bool spkPredicted,
      QString shape, double startPad, double endPad) : QThread() {

    p_jsonDocument = NULL;
    p_networkMgr = NULL;
    p_request = NULL;
    p_response = NULL;
    p_rawResponse = NULL;
    p_error = NULL;

    stringstream labelStream;
    labelStream << cubeLabel;
    QString labelText = QString::fromStdString(labelStream.str());

    QJsonObject properties {
      {"label", labelText},
      {"cksmithed", ckSmithed},
      {"ckrecon", ckRecon},
      {"cknadir", ckNadir},
      {"ckpredicted", ckPredicted},
      {"spksmithed", spkSmithed},
      {"spkrecon", spkRecon},
      {"spkpredicted", spkPredicted},
      {"shape", shape},
      {"startPad", startPad},
      {"endPad", endPad}
    };

    p_jsonDocument = new QJsonDocument(properties);

    p_request = new QNetworkRequest();

    QUrl qurl(url);

    qurl.setPort(port);
    p_request->setUrl(qurl);
    p_request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    moveToThread(this);
    start();
  }


  /**
   * This cleans up the spice client.
   *
   */
  SpiceClient::~SpiceClient() {
    delete p_jsonDocument;
    p_jsonDocument = NULL;

    delete p_error;
    p_error = NULL;

    delete p_networkMgr;
    p_networkMgr = NULL;

    delete p_request;
    p_request = NULL;

    delete p_response;
    p_response = NULL;

    delete p_rawResponse;
    p_rawResponse = NULL;
  }


  /**
   * This POSTS to the spice server
   */
  void SpiceClient::sendRequest() {
    p_networkMgr = new QNetworkAccessManager();

    connect(p_networkMgr, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));
    connect(p_networkMgr, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
            this, SLOT(authenticationRequired(QNetworkReply *, QAuthenticator *)));
    connect(p_networkMgr, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),
            this, SLOT(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(p_networkMgr, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));

    p_networkMgr->post(*p_request, p_jsonDocument->toJson());
    exec();
  }


  /**
   * This is called when the server responds.
   *
   * @param reply
   */
  void SpiceClient::replyFinished(QNetworkReply *reply) {
    QByteArray p_rawResponseByteArray = reply->readAll();

    p_response = new QJsonObject();

    try {

      QJsonDocument doc = QJsonDocument::fromJson(p_rawResponseByteArray);

      *p_response = doc.object();
      if (!p_response->value("Error").isUndefined()) {
        throw IException(IException::Unknown, p_response->value("Error").toString(), _FILEINFO_);
      }

      QFile finalOutput("output.txt");
      finalOutput.open(QIODevice::WriteOnly);
      finalOutput.write( doc.toJson() );
      finalOutput.close();

      // Make sure we can get the log out of it before continuing
      // applicationLog();
    }
    catch(IException &) {
      p_error = new QString();

      // Well, the XML is bad, maybe it's PVL
      try {
        Pvl pvlTest;
        stringstream s;
        s << *p_rawResponse;
        s >> pvlTest;

        PvlGroup &err = pvlTest.findGroup("Error", Pvl::Traverse);

        *p_error = "The Spice Server was unable to initialize the cube.";

        if (err.findKeyword("Message")[0] != "") {
          *p_error += "  The error reported was: ";
          *p_error += err.findKeyword("Message")[0];
        }
      }
      catch(IException &) {
        if (reply->error() != QNetworkReply::NoError) {
          *p_error = "An error occurred when talking to the server";

          switch (reply->error()) {
            case QNetworkReply::NoError:
              break;

            case QNetworkReply::ConnectionRefusedError:
              *p_error += ". The server refused the connection";
              break;

            case QNetworkReply::RemoteHostClosedError:
              *p_error += ". The server closed the connection";
              break;

            case QNetworkReply::HostNotFoundError:
              *p_error += ". The server was not found";
              break;

            case QNetworkReply::TimeoutError:
              *p_error += ". The connection timed out";
              break;

            case QNetworkReply::OperationCanceledError:
              *p_error += ". We aborted the network operation";
              break;

            case QNetworkReply::SslHandshakeFailedError:
              *p_error += ". Could not establish an encrypted connection";
              break;

            case QNetworkReply::TemporaryNetworkFailureError:
              *p_error += ". There was a temporary network failure";
              break;

            case QNetworkReply::NetworkSessionFailedError:
              *p_error += ". The connection was broken";
              break;

            case QNetworkReply::BackgroundRequestNotAllowedError:
              *p_error += ". The background request is not allowed";
              break;

            case QNetworkReply::TooManyRedirectsError:
              *p_error += ". The maximum limit of redirects was reached";
              break;

            case QNetworkReply::InsecureRedirectError:
              *p_error += ". A redirect from https to http occurred";
              break;

            case QNetworkReply::ProxyConnectionRefusedError:
              *p_error += ". The proxy server refused the connection";
              break;

            case QNetworkReply::ProxyConnectionClosedError:
              *p_error += ". The proxy server closed the connection";
              break;

            case QNetworkReply::ProxyNotFoundError:
              *p_error += ". The proxy server could not be found";
              break;

            case QNetworkReply::ProxyTimeoutError:
              *p_error += ". The connection to the proxy server timed out";
              break;

            case QNetworkReply::ProxyAuthenticationRequiredError:
              *p_error += ". The proxy server requires authentication";
              break;

            case QNetworkReply::ContentAccessDenied:
              *p_error += ". Access to the remove content was denied (401)";
              break;

            case QNetworkReply::ContentOperationNotPermittedError:
              *p_error += ". The operation requested on the server is not "
                          "permitted";
              break;

            case QNetworkReply::ContentNotFoundError:
              *p_error += ". The spice server script was not found (404)";
              break;

            case QNetworkReply::AuthenticationRequiredError:
              *p_error += ". The server requires authentication";
              break;

            case QNetworkReply::ContentReSendError:
              *p_error += ". The server requests for you to try again";
              break;

            case QNetworkReply::ContentConflictError:
              *p_error += ". There is a conflict with the current state of the resource";
              break;

            case QNetworkReply::ContentGoneError:
              *p_error += ". The requested resource is no longer available";
              break;

            case QNetworkReply::InternalServerError:
              *p_error += ". The server encountered an unexpected error";
              break;

            case QNetworkReply::OperationNotImplementedError:
              *p_error += ". The server does not support the functionality required to "
                          "the request";
              break;

            case QNetworkReply::ServiceUnavailableError:
              *p_error += ". The server is unable to handle the request at this time.";
              break;

            case QNetworkReply::ProtocolUnknownError:
              *p_error += ". The attempted network protocol is unknown";
              break;

            case QNetworkReply::ProtocolInvalidOperationError:
              *p_error += ". The network protocol did not support this "
                          "operation";
              break;

            case QNetworkReply::UnknownNetworkError:
              *p_error += ". An unknown network-related error occurred";
              break;

            case QNetworkReply::UnknownProxyError:
              *p_error += ". An unknown proxy-related error occurred";
              break;

            case QNetworkReply::UnknownContentError:
              *p_error += ". An unknown content-related error occurred";
              break;

            case QNetworkReply::ProtocolFailure:
              *p_error += ". A breakdown in the protocol was detected";
              break;

            case QNetworkReply::UnknownServerError:
              *p_error += ". An unknown error related to the server occurred.";
              break;
          }

        }
        else {
          // Well, we really don't know what this is.
          *p_error = "The server sent an unrecognized response";

          if (*p_rawResponse != "") {
            *p_error += " [";
            *p_error += *p_rawResponse;
            *p_error += "]";
          }
        }
      }
    }

    quit();
  }


  /**
   * Called if the server requires a authentication
   */
  void SpiceClient::authenticationRequired(QNetworkReply *, QAuthenticator *) {
    if(!p_response) {
      p_rawResponse = new QString();
      p_response = new QJsonObject();
    }

    *p_error = "Server expects authentication which is not currently ";
    *p_error += "supported";
    quit();
  }


  /**
   * Called if the server requires a authentication
   */
  void SpiceClient::proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *) {
    if(!p_response) {
      p_rawResponse = new QString();
      p_response = new QJsonObject();
    }

    *p_error = "Server expects authentication which is not currently ";
    *p_error += "supported";
    quit();
  }


  /**
   * @param reply
   * @param err
   */
  void SpiceClient::sslErrors(QNetworkReply *reply,
                              const QList<QSslError> & err) {
    if(!p_response) {
      p_rawResponse = new QString();
      p_response = new QJsonObject();
    }

    *p_error = "Server expects authentication which is not currently ";
    *p_error += "supported";

    quit();
  }


  /**
   * This returns the spiceinit'd PvlGroup from the server.
   *
   * @return PvlGroup
   */
  PvlGroup SpiceClient::kernelsGroup() {
    checkErrors();

    QString kernVal = p_response->value("Kernels Label").toVariant().toString();
    QString unencoded(QByteArray::fromHex(kernVal.toLatin1()).constData());

    stringstream pvlStream;
    pvlStream << unencoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.findGroup("Kernels", Pvl::Traverse);
  }


  /**
   * This returns the PvlGroup we should log to the console
   *
   * @return PvlGroup
   */
  PvlGroup SpiceClient::applicationLog() {
    checkErrors();

    QString value = p_response->value("Application Log").toString();
    QString decoded(QByteArray::fromHex(value.toUtf8().constData()));

    stringstream pvlStream;
    pvlStream << decoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.findGroup("Kernels", Pvl::Traverse);
  }

  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::pointingTable() {
    return readTable("Instrument Pointing", "InstrumentPointing");
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::positionTable() {
    return readTable("Instrument Position", "InstrumentPosition");
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::bodyRotationTable() {
    return readTable("Body Rotation", "BodyRotation");
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   */
  Table *SpiceClient::sunPositionTable() {
    return readTable("Sun Position", "SunPosition");
  }


  /**
   * This throws an exception if an error occurred
   */
  void SpiceClient::checkErrors() {
    if(p_error) {
      throw IException(IException::Unknown, *p_error, _FILEINFO_);
    }
  }


  /**
   * This yields the current thread until the server response is
   * received and initial (basic) processing is complete.
   */
  void SpiceClient::blockUntilComplete() {
    while(isRunning()) {
      yieldCurrentThread();
    }
  }


  PvlObject SpiceClient::naifKeywordsObject() {
    checkErrors();

    QString value = p_response->value("Kernels Label").toString();
    QString decoded(QByteArray::fromHex(value.toUtf8().constData()));

    stringstream pvlStream;
    pvlStream << decoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.findObject("NaifKeywords");
  }


  Table *SpiceClient::readTable(QString jsonName, QString tableName) {
    checkErrors();

    QString value = p_response->value(jsonName).toString();
    QByteArray decoded = QByteArray::fromHex(value.toUtf8().constData());

    QFile finalOutput(tableName + ".txt");
    finalOutput.open(QIODevice::WriteOnly);
    finalOutput.write(decoded);
    finalOutput.close();

    Table *table = new Table(tableName,tableName + ".txt");

    return table;
  }
};

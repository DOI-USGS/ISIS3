#include "SpiceClient.h"

#include <sstream>

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDomElement>

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
   * This initializes a SpiceClient. It forms the XML to send and
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
  SpiceClient::SpiceClient(IString url, int port, Pvl &cubeLabel,
      bool ckSmithed, bool ckRecon, bool ckNadir, bool ckPredicted,
      bool spkSmithed, bool spkRecon, bool spkPredicted,
      IString shape, double startPad, double endPad) : QThread() {
    p_xml = NULL;
    p_networkMgr = NULL;
    p_request = NULL;
    p_response = NULL;
    p_rawResponse = NULL;
    p_error = NULL;

    IString raw;
    p_xml = new IString();

    raw = "<input_label>\n";
    raw += "  <isis_version>\n";
    IString version = Application::Version();
    QByteArray isisVersionRaw(version.c_str());
    raw += IString(isisVersionRaw.toHex().constData()) + "\n";
    raw += "  </isis_version>\n";

    raw += "  <parameters>\n";
    raw += "    <cksmithed value='" + yesNo(ckSmithed) + "' />\n";
    raw += "    <ckrecon value='" + yesNo(ckRecon) + "' />\n";
    raw += "    <ckpredicted value='" + yesNo(ckPredicted) + "' />\n";
    raw += "    <cknadir value='" + yesNo(ckNadir) + "' />\n";
    raw += "    <spksmithed value='" + yesNo(spkSmithed) + "' />\n";
    raw += "    <spkrecon value='" + yesNo(spkRecon) + "' />\n";
    raw += "    <spkpredicted value='" + yesNo(spkPredicted) + "' />\n";
    raw += "    <shape value='" + shape + "' />\n";
    raw += "    <startpad time='" + IString(startPad) + "' />\n";
    raw += "    <endpad time='" + IString(endPad) + "' />\n";
    raw += "  </parameters>\n";

    raw += "  <label>\n";
    stringstream str;
    str << cubeLabel;
    raw += IString(QByteArray(str.str().c_str()).toHex().constData()) + "\n";

    raw += "  </label>\n";
    raw += "</input_label>";

    *p_xml = IString(QByteArray(raw.c_str()).toHex().constData());

    int contentLength = p_xml->length();
    IString contentLengthStr = IString((BigInt)contentLength);

    p_request = new QNetworkRequest();
    p_request->setUrl(QUrl(url));
    p_request->setRawHeader("User-Agent", "SpiceInit 1.0");
    p_request->setHeader(QNetworkRequest::ContentTypeHeader,
                         "application/x-www-form-urlencoded");
    //p_request->setRawHeader("Content-Length", contentLengthStr.c_str());
    //p_request->setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute,
    //    true);

    moveToThread(this);
    start();
  }


  /**
   * This cleans up the spice client.
   *
   */
  SpiceClient::~SpiceClient() {
    delete p_xml;
    p_xml = NULL;

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

    QByteArray data;
    QUrl params;
    params.addQueryItem("name", p_xml->c_str());
    data.append(params.encodedQuery());

    p_networkMgr->post(*p_request, data);
    exec();
  }


  /**
   * This is called when the server responds.
   *
   * @param reply
   */
  void SpiceClient::replyFinished(QNetworkReply *reply) {
    p_rawResponse = new IString(QString(reply->readAll()).toStdString());

    // Decode the response
    p_response = new IString();

    try {
      *p_response = IString(
          QByteArray::fromHex(QByteArray(p_rawResponse->c_str())).constData());

      // Make sure we can get the log out of it before continuing
      applicationLog();
    }
    catch(IException &) {
      p_error = new IString();

      // Well, the XML is bad, maybe it's PVL
      try {
        Pvl pvlTest;
        stringstream s;
        s << *p_rawResponse;
        s >> pvlTest;

        PvlGroup &err = pvlTest.FindGroup("Error", Pvl::Traverse);

        *p_error = "The Spice Server was unable to initialize the cube.";

        if (err.FindKeyword("Message")[0] != "") {
          *p_error += "  The error reported was: ";
          *p_error += err.FindKeyword("Message")[0];
        }
      }
      catch(IException &) {
        if (reply->error() != QNetworkReply::NoError) {
          *p_error = "An error occurred when talking to the server";

          switch (reply->error()) {
            case QNetworkReply::NoError:
              break;

            case QNetworkReply::TemporaryNetworkFailureError:
              *p_error += ". There was a temporary network failure";
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
      p_rawResponse = new IString();
      p_response = new IString();
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
      p_rawResponse = new IString();
      p_response = new IString();
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
      p_rawResponse = new IString();
      p_response = new IString();
    }

    *p_error = "Server expects authentication which is not currently ";
    *p_error += "supported";

    quit();
  }


  /**
   * This returns the root of the returned XML, throws an
   * appropriate error if the XML is wrong or missing.
   *
   * @return QDomElement
   */
  QDomElement SpiceClient::rootXMLElement() {
    if(!p_response || !p_rawResponse) {
      IString error = "No server response available";
      throw IException(IException::Io, error, _FILEINFO_);
    }

    QDomDocument document;
    QString errorMsg;
    int errorLine, errorCol;

    if(!p_response->empty() &&
        document.setContent(QString(p_response->c_str()),
                            &errorMsg, &errorLine, &errorCol)) {
      return document.firstChild().toElement();
    }
    else {
      IString msg = "Unexpected response from spice server [";
      msg += *p_rawResponse;
      msg += "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * This finds a tag (i.e. \<some_tag>) inside the current
   * element.
   *
   * @param currentElement
   * @param name
   *
   * @return QDomElement
   */
  QDomElement SpiceClient::findTag(QDomElement currentElement, IString name) {
    QString qtName = name.ToQt();
    for(QDomNode node = currentElement.firstChild();
        !node .isNull();
        node = node.nextSibling()) {
      QDomElement element = node.toElement();

      if(element.tagName() == qtName) {
        return element;
      }
    }

    IString msg = "Server response missing XML Tag [" + name + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }


  /**
   * This returns the spiceinit'd PvlGroup from the server.
   *
   * @return PvlGroup
   */
  PvlGroup SpiceClient::kernelsGroup() {
    checkErrors();

    QDomElement root = rootXMLElement();
    QDomElement kernelsLabel = findTag(root, "kernels_label");
    QString kernelsLabels = elementContents(kernelsLabel);

    IString unencoded(QByteArray::fromHex(kernelsLabels.toAscii()).constData());

    stringstream pvlStream;
    pvlStream << unencoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.FindGroup("Kernels", Pvl::Traverse);
  }


  /**
   * This returns the PvlGroup we should log to the console
   *
   * @return PvlGroup
   */
  PvlGroup SpiceClient::applicationLog() {
    checkErrors();

    QDomElement root = rootXMLElement();
    QDomElement logLabel = findTag(root, "application_log");
    QString logLabels = elementContents(logLabel);

    IString unencoded(QByteArray::fromHex(logLabels.toAscii()).constData());

    stringstream pvlStream;
    pvlStream << unencoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.FindGroup("Kernels", Pvl::Traverse);
  }


  /**
   * This returns the contents of the current element as a string.
   *   \<element>
   *     Contents
   *   \</element>
   *
   * @param element
   *
   * @return QString
   */
  QString SpiceClient::elementContents(QDomElement element) {
    return element.firstChild().toText().data();
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::pointingTable() {
    return readTable("instrument_pointing", "InstrumentPointing");
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::positionTable() {
    return readTable("instrument_position", "InstrumentPosition");
  }


  /**
   * This returns the table given by the server. The ownership of the table is
   *   given to the caller.
   *
   * @return Table*
   */
  Table *SpiceClient::bodyRotationTable() {
    return readTable("body_rotation", "BodyRotation");
  }


  /**
   * This returns the table given by the server.
   */
  Table *SpiceClient::sunPositionTable() {
    return readTable("sun_position", "SunPosition");
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

    QDomElement root = rootXMLElement();
    QDomElement kernelsLabel = findTag(root, "kernels_label");
    QString kernelsLabels = elementContents(kernelsLabel);

    IString unencoded(QByteArray::fromHex(kernelsLabels.toAscii()).constData());

    stringstream pvlStream;
    pvlStream << unencoded;

    Pvl labels;
    pvlStream >> labels;

    return labels.FindObject("NaifKeywords");
  }


  /**
   * Converts a boolean to "yes" or "no"
   *
   * @param boolVal
   *
   * @return IString
   */
  IString SpiceClient::yesNo(bool boolVal) {
    if(boolVal)
      return "yes";
    else
      return "no";
  }


  Table *SpiceClient::readTable(IString xmlName, IString tableName) {
    checkErrors();

    QDomElement root = rootXMLElement();
    QDomElement tablesTag = findTag(root, "tables");
    QDomElement pointingTag = findTag(tablesTag, xmlName);
    QString encodedString = elementContents(pointingTag);

    QByteArray encodedArray;
    for (int i = 0; i < encodedString.size(); i++) {
      encodedArray.append(encodedString.data()[i]);
    }

    QByteArray unencodedArray(QByteArray::fromHex(encodedArray));

    stringstream tableStream;
    tableStream.write(unencodedArray.data(), unencodedArray.size());

    Pvl lab;
    tableStream >> lab;

    Table *table = new Table(tableName);
    table->Read(lab, tableStream);

    return table;

  }
};

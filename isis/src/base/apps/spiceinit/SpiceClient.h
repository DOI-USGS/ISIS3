#ifndef SpiceClient_h
#define SpiceClient_h

#include <QThread>

class QAuthenticator;
class QDomElement;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QNetworkProxy;
class QSslError;

template <typename A> class QList;

namespace Isis {
  class iString;
  class Pvl;
  class PvlGroup;
  class PvlObject;
  class Table;

  /**
   * @author ????-??-?? Steven Lambright
   *
   * @internal
   */
  class SpiceClient : public QThread {
      Q_OBJECT

    public:
      SpiceClient(iString url, int port, Pvl &cubeLabel,
                  bool ckSmithed, bool ckRecon, bool ckPredicted, bool ckNadir,
                  bool spkSmithed, bool spkRecon, bool spkPredicted,
                  iString shape, double startPad, double endPad);
      virtual ~SpiceClient();

      void blockUntilComplete();

      PvlObject naifKeywordsObject();
      PvlGroup kernelsGroup();
      PvlGroup applicationLog();
      Table *pointingTable();
      Table *positionTable();
      Table *bodyRotationTable();
      Table *sunPositionTable();

    public slots:
      void sendRequest();

    private slots:
      void replyFinished(QNetworkReply *);
      void authenticationRequired(QNetworkReply *, QAuthenticator *);
      void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
      void sslErrors(QNetworkReply *, const QList<QSslError> &);

    private:
      static iString yesNo(bool boolVal);
      Table *readTable(iString xmlName, iString tableName);
      QDomElement rootXMLElement();
      QDomElement findTag(QDomElement currentElement, iString name);
      QString elementContents(QDomElement element);
      void checkErrors();

    private:
      iString *p_error;
      iString *p_xml; //!< XML Sent to server
      iString *p_rawResponse;  //!< Server raw response
      iString *p_response;  //!< Server decoded response
      QNetworkAccessManager *p_networkMgr; //!< Network manager does request
      QNetworkRequest *p_request; //!< Network request sent
  };

};


#endif

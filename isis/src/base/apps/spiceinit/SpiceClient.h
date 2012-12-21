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
class QString;

template <typename A> class QList;

namespace Isis {
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
      SpiceClient(QString url, int port, Pvl &cubeLabel,
                  bool ckSmithed, bool ckRecon, bool ckPredicted, bool ckNadir,
                  bool spkSmithed, bool spkRecon, bool spkPredicted,
                  QString shape, double startPad, double endPad);
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
      static QString yesNo(bool boolVal);
      Table *readTable(QString xmlName, QString tableName);
      QDomElement rootXMLElement();
      QDomElement findTag(QDomElement currentElement, QString name);
      QString elementContents(QDomElement element);
      void checkErrors();

    private:
      QString *p_error;
      QString *p_xml; //!< XML Sent to server
      QString *p_rawResponse;  //!< Server raw response
      QString *p_response;  //!< Server decoded response
      QNetworkAccessManager *p_networkMgr; //!< Network manager does request
      QNetworkRequest *p_request; //!< Network request sent
  };

};


#endif

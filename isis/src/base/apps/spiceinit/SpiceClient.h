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
  class IString;
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
      SpiceClient(IString url, int port, Pvl &cubeLabel,
                  bool ckSmithed, bool ckRecon, bool ckPredicted, bool ckNadir,
                  bool spkSmithed, bool spkRecon, bool spkPredicted,
                  IString shape, double startPad, double endPad);
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
      static IString yesNo(bool boolVal);
      Table *readTable(IString xmlName, IString tableName);
      QDomElement rootXMLElement();
      QDomElement findTag(QDomElement currentElement, IString name);
      QString elementContents(QDomElement element);
      void checkErrors();

    private:
      IString *p_error;
      IString *p_xml; //!< XML Sent to server
      IString *p_rawResponse;  //!< Server raw response
      IString *p_response;  //!< Server decoded response
      QNetworkAccessManager *p_networkMgr; //!< Network manager does request
      QNetworkRequest *p_request; //!< Network request sent
  };

};


#endif

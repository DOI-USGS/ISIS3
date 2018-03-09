#ifndef SpiceClient_h
#define SpiceClient_h

#include <QThread>

class QAuthenticator;
class QDomElement;
class QJsonDocument;
class QJsonValue;
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
   *    @history 2017-12-19 Summer Stapleton - Updated constructor and sendRequest() to handle Json
   *                            files rather than xml.
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
      QJsonValue tableToJson(QString file);
    public slots:
      void sendRequest();

    private slots:
      void replyFinished(QNetworkReply *);
      void authenticationRequired(QNetworkReply *, QAuthenticator *);
      void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
      void sslErrors(QNetworkReply *, const QList<QSslError> &);

    private:
      Table *readTable(QString xmlName, QString tableName);
      void checkErrors();

    private:
      QString *p_error;
      QJsonDocument *p_jsonDocument; //!< Json file to send to server
      QString *p_rawResponse;  //!< Server raw response
      QJsonObject *p_response;  //!< The QJsonObject constructed from the server response
      QNetworkAccessManager *p_networkMgr; //!< Network manager that makes request
      QNetworkRequest *p_request; //!< Network request sent
  };

};


#endif

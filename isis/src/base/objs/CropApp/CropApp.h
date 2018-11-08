#ifndef CropApp_h
#define CropApp_h
#include <QObject>
#include <QString>
#include <QThread>

namespace Isis{


class Buffer;
class Cube;
class LineManager;

class CropApp: public QObject {


    Q_OBJECT
    public:

    CropApp(QString &from, QString &to, int ssample = 1, int nsamples = -1, int sinc=1,
             int sline =1, int nlines=-1, int linc=1, bool propspice=true, Cube *cube=NULL);

    public slots:
    void start();
    void handleResults();

    signals:

    void operate(QString &from, QString &to, int ssample = 1,int nsamples = -1,int sinc=1,
                 int sline =1,int nlines=-1,int linc=1,bool propspice=true,Cube *cube=NULL);


    private:

    QThread cropThread;

    QString m_fromCube;
    QString m_toCube;
    int m_ssample;
    int m_nsamples;
    int m_sinc;
    int m_sline;
    int m_nlines;
    int m_linc;
    int m_sband;
    LineManager *m_in;
    Cube * m_cube;
    bool m_propspice;


    };

class cropper:public QObject{

Q_OBJECT
public:

  static int m_sline;
  static int m_linc;
  static int m_sband;
  static int m_ssample;
  static int m_sinc;
  static int m_osamples;
  static int m_olines;
  static int m_obands;
  static Cube *m_cube;
  static QString m_outputCubeName;
  static bool m_propspice;
  static LineManager *m_in;

  cropper(int ssample,int nsamples,
          int sinc, int sline,int nlines,int linc,bool propspice,QString to,Cube *cube);

   static void crop(Buffer &out);


public slots:
  void cropit(QString &from, QString &to, int ssample,int nsamples,
              int sinc, int sline,
              int nlines,int linc,bool propspice,Cube *cube);





signals:
  void resultReady(const QString &result);


};

}


#endif

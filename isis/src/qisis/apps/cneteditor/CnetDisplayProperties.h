#ifndef CnetDisplayProperties_H
#define CnetDisplayProperties_H


#include <QObject>


class QAtomicInt;
template< typename T > class QFutureWatcher;
template< typename A, typename B > class QMap;
class QReadWriteLock;
class QString;
class QTimer;


namespace Isis
{

  /**
   * @brief Handles how control networks should be displayed to the user
   *
   * This class handles how control networks should be displayed to the user.
   * For example, this class keeps track of whether cube serial numbers or file
   * names should be displayed to the user, and provides an interface to
   * translate between the two.
   *
   * This class is a singleton.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   */
  class CnetDisplayProperties : public QObject
  {
      Q_OBJECT
      
    public:
      // this is a singleton class!
      static CnetDisplayProperties * getInstance();
      virtual ~CnetDisplayProperties();
      
      bool currentlyComposing() const;
      
      QString getFileName(QString fileName) const;
      QString getImageName(QString cubeSerialNumber) const;
      QString getSerialNumber(QString imageId);
      bool getShowsFullPaths() const;
      
      void setCubeList(QString fileName);
      void setFileNameUsage(bool preferFileNames);
      void setShowsFullPaths(bool newState);
      
    
    private:
      CnetDisplayProperties();
    
      
    private: // not implemented
      CnetDisplayProperties(CnetDisplayProperties const &);
      CnetDisplayProperties & operator=(CnetDisplayProperties const &);
      
      
    signals:
      void composeProgressChanged(int);
      void composeProgressRangeChanged(int, int);
      void compositionFinished();
      
      
    private:
      QMap< QString, QString > composeSerialNumbers(QStringList fileNames);
      void nullify();
      
      
    private slots:
      void composeStatusUpdated();
      void serialNumbersComposed();
      
      
    private:
      bool useFileNames;
      QMap< QString, QString > * serialNumberToFileNameMap;
      
      QFutureWatcher< QMap< QString, QString > > * composeWatcher;
      QTimer * composeStatusPoller;
      
      QAtomicInt * composedCount;
      QAtomicInt * interruptFlag;
      bool curComposing;
      bool showFullPath;
      QReadWriteLock * readWriteLock;
    
      static CnetDisplayProperties * instance;
      
      
      
      // For explicit sharing of the comparison counter between multiple
      // copies of a LessThanFunctor object. This bypasses the need for a
      // static member in LessThanFunctor.
//       class LessThanFunctorData : public QSharedData
//       {
//         public:
//           LessThanFunctorData();
//           LessThanFunctorData(LessThanFunctorData const &);
//           ~LessThanFunctorData();
// 
//           int getCompareCount() const;
//           void incrementCompareCount();
//           
//           void setInterrupted(bool);
//           bool interrupted();
// 
// 
//         private:
//           QAtomicInt compareCount;
//           QAtomicInt interruptFlag;
//       };
  };
}

#endif

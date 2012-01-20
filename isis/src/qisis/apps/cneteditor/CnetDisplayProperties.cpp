#include "IsisDebug.h"

#include "CnetDisplayProperties.h"

#include <iostream>

#include <QAtomicInt>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QMap>
#include <QReadWriteLock>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QtConcurrentRun>

#include "Cube.h"
#include "iException.h"
#include "iString.h"
#include "SerialNumber.h"



#define prog_exception(x) throw Isis::iException::Message(      \
    Isis::iException::Programmer, x, _FILEINFO_)

#define user_exception(x) throw Isis::iException::Message(      \
    Isis::iException::User, x, _FILEINFO_)




using std::cerr;


namespace Isis
{
  CnetDisplayProperties * CnetDisplayProperties::instance = NULL;
  
  
  CnetDisplayProperties * CnetDisplayProperties::getInstance()
  {
    return instance ? instance : instance = new CnetDisplayProperties;
  }
  
  
  CnetDisplayProperties::CnetDisplayProperties()
  {
    nullify();
    
    useFileNames = true;
    curComposing = false;
    showFullPath = false;
    
    readWriteLock = new QReadWriteLock;
    
    serialNumberToFileNameMap = new QMap< QString, QString >;
    
    composedCount = new QAtomicInt;
    interruptFlag = new QAtomicInt;
    interruptFlag->fetchAndStoreRelaxed(0);
    
    composeStatusPoller = new QTimer;
    connect(composeStatusPoller, SIGNAL(timeout()),
              this, SLOT(composeStatusUpdated()));
    
    composeWatcher = new QFutureWatcher< QMap< QString, QString > >;
    connect(composeWatcher, SIGNAL(finished()),
            this, SLOT(serialNumbersComposed()));
  }
  
  
//   CnetDisplayProperties::CnetDisplayProperties(
//       CnetDisplayProperties const & other)
//   {
//     nullify();
//     
//     useFileNames = other.useFileNames;
//     serialNumberToFileNameMap = new QMap< QString, QString >(
//         *other.serialNumberToFileNameMap);
//     
//     composeStatusPoller = new QTimer;
//     
//     composeWatcher = new QFutureWatcher< QMap< QString, QString > >;
//     connect(composeWatcher, SIGNAL(finished()),
//             this, SLOT(serialNumbersComposed()));
//   }
  
  
  CnetDisplayProperties::~CnetDisplayProperties()
  {
    delete serialNumberToFileNameMap;
    serialNumberToFileNameMap = NULL;
    
    delete composeWatcher;
    composeWatcher = NULL;
    
    delete composedCount;
    composedCount = NULL;
    
    delete interruptFlag;
    interruptFlag = NULL;
  }
  
  
  bool CnetDisplayProperties::currentlyComposing() const
  {
    return curComposing;
  }
  
  
  QString CnetDisplayProperties::getFileName(QString fileName) const
  {
    return getShowsFullPaths() ? fileName : QFileInfo(fileName).fileName();
  }
  
  
  /**
   * @param cubeSerialNumber Cube serial number as a QString
   * 
   * @returns The file name associated with the given cube serial number, or
   *          the given cube serial number if a file name can not be found in
   *          the current cube list.
   * 
   * @see setCubeList()
   * 
   * This method is thread safe!
   */
  QString CnetDisplayProperties::getImageName(QString cubeSerialNumber) const
  {
    ASSERT(serialNumberToFileNameMap);
    
    QString imageName = cubeSerialNumber;
    
    readWriteLock->lockForRead();
    
    if (serialNumberToFileNameMap && useFileNames &&
        serialNumberToFileNameMap->contains(cubeSerialNumber))
    {
      QString value = serialNumberToFileNameMap->value(cubeSerialNumber);
//       cerr << "CnetDisplayProperties::getImageName called... value: " << qPrintable(value) << "\n";
      readWriteLock->unlock();
      
      if (value.toLower() != "unknown")
        imageName = getFileName(value);
    }
    else
    {
      readWriteLock->unlock();
    }
    
    return imageName;
  }
  
  
  QString CnetDisplayProperties::getSerialNumber(QString imageId)
  {
    ASSERT(serialNumberToFileNameMap);
    
    QString sn = imageId;
    
    if (serialNumberToFileNameMap && useFileNames)
    {
      readWriteLock->lockForRead();
      QMapIterator< QString, QString > i(*serialNumberToFileNameMap);
      
      bool found = false;
      while (!found && i.hasNext())
      {
        i.next();
        if (i.value() == imageId)
        {
          found = true;
          sn = i.key();
        }
      }
      
      readWriteLock->unlock();
    }
    
    return sn;
  }
  
  
  bool CnetDisplayProperties::getShowsFullPaths() const
  {
    return showFullPath;
  }


  void CnetDisplayProperties::setCubeList(QString fileName)
  {
    QFile imageListFile(fileName);

    if (!imageListFile.exists())
    {
      iString msg = "The file [";
      msg += (iString) fileName;
      msg += "does not exist.\n";
      prog_exception(msg);
    }

    if (!imageListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      iString msg = "The file [";
      msg += (iString) fileName;
      msg += "failed to open.\n";
      prog_exception(msg);
    }
    
    QStringList imageFileNames;
    QTextStream ts(&imageListFile);
    
    while (!ts.atEnd())
      imageFileNames << ts.readLine();
    
    imageListFile.close();
    
    curComposing = true;
    
    QFuture< QMap< QString, QString > > future = QtConcurrent::run(
        this, &CnetDisplayProperties::composeSerialNumbers, imageFileNames);
    
    composeWatcher->setFuture(future);
  }


  void CnetDisplayProperties::setFileNameUsage(bool preferFileNames)
  {
    useFileNames = preferFileNames;
  }
  
  
//   CnetDisplayProperties & CnetDisplayProperties::operator=(
//       CnetDisplayProperties const & other)
//   {
//     if (this != &other)
//     {
//       composeWatcher->waitForFinished();
//       useFileNames = other.useFileNames;
//     }
//     
//     return *this;
//   }


  void CnetDisplayProperties::setShowsFullPaths(bool newState)
  {
    showFullPath = newState;
  }

  
  QMap< QString, QString > CnetDisplayProperties::composeSerialNumbers(
      QStringList fileNames)
  {
    emit composeProgressRangeChanged(0, fileNames.size() - 1);
    composedCount->fetchAndStoreRelaxed(0);
    
    QMap< QString, QString > newMap;
    
    for (int i = 0; *interruptFlag == 0 && i < fileNames.size(); i++)
    {
//      cerr << "looping: " << i << "\n";
      iString fileName = fileNames[i];
      
      Cube cube;
      cube.open(fileName);
      
      newMap.insert(SerialNumber::Compose(fileName).c_str(), fileNames[i]);
      
      composedCount->fetchAndAddRelaxed(1);
    }
    
//     cerr << "just finished composing serial numbers (still in composing thread)...\n";
//     QMapIterator< QString, QString > i(newMap);
//     while (i.hasNext())
//     {
//       i.next();
//       cerr << qPrintable(i.key()) << ": " << qPrintable(i.value()) << "\n";
//     }
    
    return newMap;
  }


  void CnetDisplayProperties::nullify()
  {
    serialNumberToFileNameMap = NULL;
    composeWatcher = NULL;
    
    composedCount = NULL;
    interruptFlag = NULL;
    
    readWriteLock = NULL;
  }
  
  
  void CnetDisplayProperties::composeStatusUpdated()
  {
    emit composeProgressChanged(*composedCount);
  }


  void CnetDisplayProperties::serialNumbersComposed()
  {
    if (*interruptFlag)
    {
      interruptFlag->fetchAndStoreRelaxed(0);
    }
    else
    {
      readWriteLock->lockForWrite();
      *serialNumberToFileNameMap = composeWatcher->result();
      
//       cerr << "just finished composing serial numbers...\n";
//       QMapIterator< QString, QString > i(*serialNumberToFileNameMap);
//       while (i.hasNext())
//       {
//         i.next();
//         cerr << qPrintable(i.key()) << ": " << qPrintable(i.value()) << "\n";
//       }
      
      readWriteLock->unlock();
    }
    
    curComposing = false;
    composeStatusPoller->stop();
    emit composeProgressRangeChanged(0, 0);
    emit composeProgressChanged(0);
    emit compositionFinished();
  }
}

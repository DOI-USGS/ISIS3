#ifndef CnetDisplayProperties_H
#define CnetDisplayProperties_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>


class QAtomicInt;
template< typename T > class QFutureWatcher;
template< typename A, typename B > class QMap;
class QReadWriteLock;
class QString;
class QTimer;


namespace Isis {
  class ControlNet;

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
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   */
  class CnetDisplayProperties : public QObject {
      Q_OBJECT

    public:
      // this is a singleton class!
      static CnetDisplayProperties *getInstance();
      virtual ~CnetDisplayProperties();

      bool currentlyComposing() const;

      QList<QString> getCubeList(ControlNet *cnet) const;
      QString getFileName(QString fileName, bool forceFullPaths = false) const;
      QString getImageName(QString cubeSerialNumber,
          bool forceFullPaths = false) const;
      QString getSerialNumber(QString imageId);
      bool getShowsFullPaths() const;

      void setCubeList(QString fileName);
      void setFileNameUsage(bool preferFileNames);
      void setShowsFullPaths(bool newState);


    private:
      CnetDisplayProperties();


    private: // not implemented
      CnetDisplayProperties(CnetDisplayProperties const &);
      CnetDisplayProperties &operator=(CnetDisplayProperties const &);


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
      QMap< QString, QString > * m_serialNumberToFileNameMap;

      QFutureWatcher< QMap< QString, QString > > * m_composeWatcher;
      QTimer *m_composeStatusPoller;

      QAtomicInt *m_composedCount;
      QAtomicInt *m_interruptFlag;
      bool m_curComposing;
      bool m_showFullPath;
      QReadWriteLock *m_readWriteLock;

      static CnetDisplayProperties *m_instance;
  };
}

#endif

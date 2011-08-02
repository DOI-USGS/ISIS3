#ifndef ConcurrentControlNetReader_h
#define ConcurrentControlNetReader_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QObject>
#include <QString>

#include "ControlNetFile.h"


template<typename A> class QAtomicPointer;
template<typename A> class QFutureWatcher;


namespace Isis {
  class ControlNet;
  class ControlPoint;
  class Distance;


  /**
   * @brief This reads a control network in the background
   *
   * This class provides progress for reading a ControlNet.  Reading is done
   * in separate threads using QtConcurrent.  When reading large Pvl networks
   * the progress will hang at zero percent for a while.
   * 
   * @author Eric Hyer, Steven Lambright, Jai Rideout
   *
   * @internal
   *   @history 2011-07-27 Jai Rideout - The original thread count is now
   *                           restored after reading has finished.
   *   @history 2011-08-01 Steven Lambright - Fixed signals/slots not being
   *                           in correct thread.
   *     
   */
  class ConcurrentControlNetReader : public QObject {
      Q_OBJECT

    public:
      ConcurrentControlNetReader();
      ~ConcurrentControlNetReader();
      void read(QString filename);


    signals:
      void progressRangeChanged(int, int);
      void progressValueChanged(int);
      void networkReadFinished(ControlNet *);


    private:
      void nullify();


    private slots:
      void startBuildingNetwork();
      void networkBuilt();


    private:
      //! provides SIGNALS / SLOTS for ReadNetworkFunctor
      QFutureWatcher<LatestControlNetFile *> * m_readWatcher;
      
      //! provides SIGNALS / SLOTS for NetworkBuilder
      QFutureWatcher< QAtomicPointer<ControlNet> > * m_builderWatcher;
      
      //! This is the binary container when reading a network
      LatestControlNetFile *m_versionerFile;

      /** 
       * This is needed to store off the original QtConcurrent thread count so
       * that it can later be restored. This class uses a single thread to do
       * the reading, as no performance gains were realized with more threads.
       */
      int m_originalThreadCount;


    private:
      /**
       * @brief This functor reads the network file to the binary container
       *
       * This class is designed to be called with QtConcurrent::run()
       * 
       * @author Eric Hyer, Steven Lambright, Jai Rideout
       */
      class ReadNetworkFunctor : public std::unary_function<
          void, LatestControlNetFile *> {

        public:
          ReadNetworkFunctor(QString);
          ~ReadNetworkFunctor();
          LatestControlNetFile *operator()() const;


        private:
          //! needed by operator() to do call ControlNetVersioner::Read()
          QString m_networkFilename;
      };
      

      /**
       * @brief This functor builds the points in a network
       *
       * This class is designed to be called with QtConcurrent::mappedReduced()
       * 
       * @author Eric Hyer, Steven Lambright, Jai Rideout
       */
      class NetworkBuilder : public std::unary_function<
          const ControlPointFileEntryV0002 &, ControlPoint *> {

        public:
          NetworkBuilder(QString, QThread *);
          NetworkBuilder(NetworkBuilder const &);
          ~NetworkBuilder();
          ControlPoint * operator()(const ControlPointFileEntryV0002 &) const;
          NetworkBuilder & operator=(NetworkBuilder);

          static void addToNetwork(QAtomicPointer<ControlNet> &,
              ControlPoint * const &);


        private:
          void nullify();


        private:
          //! Needed for construction of ControlPoints
          Distance *m_majorRad;

          //! Needed for construction of ControlPoints
          Distance *m_minorRad;

          //! Needed for construction of ControlPoints
          Distance *m_polarRad;

          QThread *m_targetThread;
      };
  };
}

#endif

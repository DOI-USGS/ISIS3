#ifndef CubeDataThreadTester_h
#define CubeDataThreadTester_h

#include <QThread>

template<typename T> class QList;
template<typename A, typename B> class QPair;

namespace Isis {
  class Brick;
  class CubeDataThread;

  class CubeDataThreadTester : public QThread {
      Q_OBJECT

    public:
      CubeDataThreadTester(CubeDataThread *);
      virtual ~CubeDataThreadTester();

      //! Returns the number of tests done (testing methods count as several)
      int NumberOfTestsDone() {
        return p_numTestsDone;
      }

      //! Returns the cube data thread being tested
      CubeDataThread *DataThread() {
        return p_cubeDataThread;
      }

      void Connect();
      void ReadCubeTest(int);
      void ReadCubeTest2(int, int);
      void ReadCubeTest3(int);
      void WriteCubeTest(int);
      void WriteCubeTest2(int, int);
      void WriteCubeTest3(int);
      void WriteCubeTest3BreakDeadlock();
      void NotifyChangeTest(int);

    public slots:
      void ReadBrick(void *requester, int cubeId, const Isis::Brick *data);
      void ReadWriteBrick(void *requester, int cubeId, Isis::Brick *data);
      void BrickChanged(int cubeId, const Isis::Brick *data);

    signals:
      /**
       * Ask for a brick for reading.
       *
       * @param cubeId Cube identifier
       * @param startSample Brick starting sample
       * @param startLine Brick starting line
       * @param endSample Brick ending sample
       * @param endLine Brick ending line
       * @param band Brick band
       * @param caller A this pointer
       */
      void RequestReadCube(int cubeId, int startSample, int startLine,
                           int endSample, int endLine, int band, void *caller);


      /**
       * Ask for a brick for reading and writing.
       *
       * @param cubeId Cube identifier
       * @param startSample Brick starting sample
       * @param startLine Brick starting line
       * @param endSample Brick ending sample
       * @param endLine Brick ending line
       * @param band Brick band
       * @param caller A this pointer
       */
      void RequestReadWriteCube(int cubeId, int startSample, int startLine,
                                int endSample, int endLine, int band, void *caller);


      /**
       * Let the cube data thread know we're no longer working with a particular
       * brick.
       */
      void NotifyDoneWithData(int, const Isis::Brick *);

    private:
      //! This thread is centered completely around its event loop
      void run();

      //! The count of completed tests
      int p_numTestsDone;

      //! The data thread being tested
      CubeDataThread *p_cubeDataThread;

      //! True if this thread is started
      bool p_execStarted;

      //! True if we will notify done on the next brick received for R/W
      bool p_notifyDone;

      //! A list of bricks we haven't send the done signal for
      QVector< QPair<int, const Isis::Brick *> > * p_cachedDoneBricks;
  };

};


#endif

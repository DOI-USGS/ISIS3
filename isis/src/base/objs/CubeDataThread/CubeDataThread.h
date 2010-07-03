#ifndef CubeDataThread_h
#define CubeDataThread_h

#include <QObject>
#include <QThread>

namespace Isis {
  class Cube;
  class Filename;
  class Brick;
};

template<typename T> class QList;

template<typename A, typename B> class QPair;
template<typename A, typename B> class QMap;

class QReadWriteLock;
class QMutex;

namespace Isis {

  /**
   * @brief Encapsulation of Cube I/O with Change Notifications
   * 
   * The main purpose of this class is to create cube change notifications, 
   * ideal for GUI's with changing cubes. This class is designed to encapsulate
   * Cube I/O into a separate thread and serializes each cube I/O. This class
   * also speeds up I/O by reusing bricks instead of always reading from the
   * disk.
   *
   * This is not a full concurrency control/transaction handler. Consistent
   * states are not guaranteed, though a consistent state for any given brick
   * is, and results from reads do not guaranteed serial equivalence. Deadlocks
   * are possible, if two processes want to R/W to the same sections of data,
   * and it is up to the users of this class to avoid such conditions.
   *
   * @author 2010-01-15 Steven Lambright
   * 
   * @internal
   *   @history 2010-04-12 Eric Hyer - Added check for valid cube ID's for slots
   *                                   ReadCube and ReadWriteCube
   *
   *   @todo Add state recording/reverting functionality
   * 
   */
  class CubeDataThread : public QThread {
    Q_OBJECT

    public:
      CubeDataThread();
      virtual ~CubeDataThread();

      int AddCube(const Isis::Filename &fileName, 
                  bool mustOpenReadWrite = false);
                  
      int AddCube(Isis::Cube *cube);
                  
      void AddChangeListener();
      void RemoveChangeListener();

      int BricksInMemory();

    public slots:
      void ReadCube(int cubeId, int startSample, int startLine, 
                    int endSample, int endLine, int band, void *caller);
      void ReadWriteCube(int cubeId, int startSample, int startLine, 
                    int endSample, int endLine, int band, void *caller);

      void DoneWithData(int, const Isis::Brick *);

    signals:
      /**
       * This signal will be emitted when ReadCube has finished processing.
       *  
       * When done with the data, given you are the requester, you call the
       * DoneWithData slot (via a signal).
       * 
       * @param requester Pointer to the calling class (must ignore all 
       *   ReadReady signals where this != requester) 
       * @param cubeId Cube ID of the cube the data is from 
       * @param data The data in the cube
       */
      void ReadReady(void *requester, int cubeId, const Isis::Brick *data);

      /**
       * This signal will be emitted when ReadWriteCube has finished processing. 
       *  
       * When done with the data, given you are the requester, you call the
       * DoneWithData slot (via a signal).
       * 
       * @param requester Pointer to the calling class (must ignore all 
       *   ReadReady signals where this != requester) 
       * @param cubeId Cube ID of the cube the data is from 
       * @param data The data in the cube, also where you should write changes
       */
      void ReadWriteReady(void *requester, int cubeId, Isis::Brick *data);

      /** 
       * DO NOT CONNECT TO THIS SIGNAL WITHOUT CALLING AddChangeListener(). 
       *  
       * When a write occurs, and change listeners exist, this signal is emitted 
       *   with the new data.
       *  
       * When done with the data, given you are the requester, you call the
       *   DoneWithData slot (via a signal).
       * 
       * @param cubeId Cube ID of the change 
       * @param data Area written to the cube 
       */
      void BrickChanged(int cubeId, const Isis::Brick *data);

    private:
      int OverlapIndex(const Isis::Brick *initial, int cubeId, 
                       int instanceNum, bool &exact);

      void GetCubeData(int cubeId, int ss, int sl, int es, int el, int band,
                       void *caller, bool sharedLock);

      void AcquireLock(QReadWriteLock *lockObject, bool readLock);

      bool FreeBrick(int brickIndex);

      /**
       * This is a list of the opened cubes. Since opening cubes is allowed in
       * other threads (via AddCube(...)) and is accessed with many threads, all
       * operations on this map must be serialized (non-simultaneous). The 
       * p_managedCubesMutex enables this. 
       * 
       */
      QMap< int, QPair< bool, Isis::Cube * > > * p_managedCubes;

      //! This locks the member variable p_managedCubes
      QMutex * p_managedCubesMutex;

      /**
       * This is a list of bricks in memory and their locks. The following
       * assumptions are vital or at least important for understanding this data
       * structure:
       *   1) No two bricks have the exact same area.
       *   2) Deletions may only happen in the destructor or in FreeBrick(int)
       *   3) A brick with no locks on it is available for deletion later, but
       *        may not be deleted if p_currentLocksWaiting != 0 (something is
       *        attempting a lock somewhere else).
       *   4) Bricks may overlap, but locks only pertain to exact matches.
       *   5) If you want to make an exclusive (R/W) lock on a brick, you must
       *        first make all overlapping bricks available for deletion (and/or
       *        delete)
       *   6) If you wish to make a shared (R) lock on a brick, no overlapping
       *        bricks can be write locked.
       *   7) All operations happen in this thread in order to prevent checks
       *        happening for access executing simultaneously, at least more
       *        than AcquireLock() allows.
       *   8) New bricks must be appended to the end of this list.
       *   8) Searches for conflicts must start at the beginning of this list
       *        and proceed to the end.
       *   10) No NULL or invalid pointers may exist in this list. 
       */
      QList< QPair<QReadWriteLock *, Isis::Brick *> > * p_managedData;

      //! This is the associated cube ID with each brick
      QList< int > * p_managedDataSources;

      //! This is the number of shaded locks to put on a brick when changes made
      int p_numChangeListeners;

      //! This is the unique id counter for cubes 
      unsigned int p_currentId;

      /**
       * Number of locks being attempted that re-entered the event loop. As long 
       * as this isn't zero, no bricks should be removed from p_managedData.
       */
      unsigned int p_currentLocksWaiting;
  };

};


#endif

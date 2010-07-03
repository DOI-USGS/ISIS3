#include "CubeDataThread.h"

#include <QMap>
#include <QMutex>
#include <QReadWriteLock>
#include <QPair>
#include <QEventLoop>

#include "iString.h"
#include "Filename.h"
#include "Brick.h"
#include "Cube.h"

namespace Isis {

  /**
   * This constructs a CubeDataThread(). This will spawn a new thread and
   * move the ownership of this instance to itself (so slots are called
   * in this self-contained thread).
   *
   */
  CubeDataThread::CubeDataThread() {
    p_managedCubes = NULL;
    p_managedData = NULL;
    p_managedCubesMutex = NULL;
    p_managedDataSources = NULL;

    p_managedCubes = new QMap< int, QPair< bool, Isis::Cube * > >;
    p_managedData = new QList< QPair<QReadWriteLock *, Isis::Brick *> >;
    p_managedCubesMutex = new QMutex();
    p_managedDataSources = new QList< int >;

    p_numChangeListeners = 0;
    p_currentLocksWaiting = 0;
    p_currentId = 1; // start with ID == 1

    // Start this thread's event loop and make it so the slots are contained
    //   within this thread (this class automatically runs in its own thread)
    start();
    moveToThread(this);
  }


  /**
   * This class is a self-contained thread, so normally it would be bad to
   * simply delete it. However, this destructor will synchronize the shutdown
   * of the thread so you can safely delete an instance of this class without
   * using the deleteLater() method.
   *
   */
  CubeDataThread::~CubeDataThread() {
    // Shutdown the event loop(s)
    QThread::exit(0);

    while(!isFinished()) {
      QThread::yieldCurrentThread();
    }

    // Destroy the bricks still in memory
    if(p_managedData) {
      for(int i = p_managedData->size() - 1; i >= 0; i--) {
        delete(*p_managedData)[i].first;
        delete(*p_managedData)[i].second;
        p_managedData->removeAt(i);
      }
    }

    // Destroy the cubes still in memory
    if(p_managedCubes) {
      for(int i = p_managedCubes->size() - 1; i >= 0; i--) {
        if((p_managedCubes->end() - 1)->first)  // only delete if we own it!
          delete(p_managedCubes->end() - 1).value().second;
        p_managedCubes->erase(p_managedCubes->end() - 1);
      }
    }

    // Destroy the mutex that controls access to the cubes
    if(p_managedCubesMutex) {
      delete p_managedCubesMutex;
      p_managedCubesMutex = NULL;
    }

    // Destroy the data sources vector
    if(p_managedDataSources) {
      delete p_managedDataSources;
      p_managedDataSources = NULL;
    }
  }

  /**
   * This method is designed to be callable from any thread before data is
   * requested, though no known side effects exist if this is called during
   * other I/O operations (though I don't recommend it).
   *
   * If possible, the cube will be opened with R/W permissions, otherwise it
   * will be opened with read-only access
   *
   * @param fileName The cube to open
   * @param  mustOpenReadWrite If true and cube has read-only access then an
   *                           exception will be thrown
   *
   * @return int The cube ID necessary for retrieving information about this cube
   *         in the future.
   */
  int CubeDataThread::AddCube(const Isis::Filename &fileName,
                              bool mustOpenReadWrite) {
    Isis::Cube *newCube = new Isis::Cube();

    try {
      newCube->Open(fileName.Expanded(), "rw");
    }
    catch(Isis::iException &e) {
      if(!mustOpenReadWrite) {
        e.Clear();
        newCube->Open(fileName.Expanded(), "r");
      }
      else {
        throw;
      }
    }

    p_managedCubesMutex->lock();

    int newId = p_currentId;
    p_currentId ++;

    QPair< bool, Cube * > newEntry;
    newEntry.first = true; // we own this!
    newEntry.second = newCube;
    p_managedCubes->insert(newId, newEntry);

    p_managedCubesMutex->unlock();

    return newId;
  }


  /**
  * This method is designed to be callable from any thread before data is
  * requested, though no known side effects exist if this is called during
  * other I/O operations (though I don't recommend it).
  *
  * Ownership is not taken of this cube
  *
  * @param cube The cube to encapsulate
  *
  * @return int The cube ID necessary for retrieving information about this cube
  *         in the future.
  */
  int CubeDataThread::AddCube(Isis::Cube *cube) {
    p_managedCubesMutex->lock();

    int newId = p_currentId;
    p_currentId ++;

    QPair< bool, Cube * > newEntry;
    newEntry.first = false; // we don't own this!
    newEntry.second = cube;
    p_managedCubes->insert(newId, newEntry);

    p_managedCubesMutex->unlock();

    return newId;
  }


  /**
   * You must call this method after connecting to the BrickChanged signal,
   * otherwise you are not guaranteed a good Brick pointer.
   */
  void CubeDataThread::AddChangeListener() {
    p_numChangeListeners ++;
  }


  /**
   * You must call this method after disconnecting from the BrickChanged signal,
   * otherwise bricks cannot be freed from memory.
   */
  void CubeDataThread::RemoveChangeListener() {
    p_numChangeListeners --;
  }



  /**
   * This helper method reads in cube data and handles the locking of similar
   * bricks appropriately.
   *
   * @param cubeId Cube ID To Read From
   * @param ss Starting Sample Position
   * @param sl Starting Line Position
   * @param es Ending Sample Position
   * @param el Ending Line Position
   * @param band Band Number To Read From (multi-band bricks not supported at this
   *             time)
   * @param caller A pointer to the calling class, used to identify who requested
   *               the data when they receive either the ReadReady or the
   *               ReadWriteReady signal
   * @param sharedLock True if read-only, false if read-write
   */
  void CubeDataThread::GetCubeData(int cubeId, int ss, int sl,
                                   int es, int el, int band,
                                   void *caller, bool sharedLock) {
    Isis::Brick *requestedBrick = NULL;

    p_managedCubesMutex->lock();
    requestedBrick = new Isis::Brick(*p_managedCubes->value(cubeId).second,
                                     es - ss + 1,
                                     el - sl + 1, 1);
    requestedBrick->SetBasePosition(ss, sl, band);
    p_managedCubesMutex->unlock();

    // See if we already have this brick
    int instance = 0;
    int exactIndex = -1;
    bool exactMatch = false;
    int index = OverlapIndex(requestedBrick, cubeId, instance, exactMatch);

    while(index != -1) {
      if(sharedLock) {
        // make sure we can get read locks on exact overlaps
        //  We need to try to get the lock to verify partial overlaps not
        //   write locked and only keep read locks on exact matches.
        AcquireLock((*p_managedData)[index].first, true);

        if(!exactMatch) {
          (*p_managedData)[index].first->unlock();
        }
      }
      else {
        AcquireLock((*p_managedData)[index].first, false);

        // we arent actually writing to this, but now we know we can delete it
        (*p_managedData)[index].first->unlock();

        // destroy things that overlap but arent the same when asking to write
        if(!exactMatch && FreeBrick(index)) {
          instance --;
        }
      }

      if(exactMatch) {
        exactIndex = index;
      }

      instance++;
      index = OverlapIndex(requestedBrick, cubeId, instance, exactMatch);
    }

    if(exactIndex == -1) {
      p_managedCubesMutex->lock();

      p_managedCubes->value(cubeId).second->Read(*requestedBrick);

      QPair<QReadWriteLock *, Isis::Brick *> managedDataEntry;

      managedDataEntry.first = new QReadWriteLock();

      AcquireLock(managedDataEntry.first, sharedLock);

      managedDataEntry.second = requestedBrick;
      p_managedData->push_back(managedDataEntry);
      p_managedDataSources->push_back(cubeId);
      exactIndex = p_managedData->size() - 1;

      p_managedCubesMutex->unlock();
    }

    if(sharedLock) {
      emit ReadReady(caller, cubeId, (*p_managedData)[exactIndex].second);
    }
    else {
      emit ReadWriteReady(caller, cubeId, (*p_managedData)[exactIndex].second);
    }

  }


  /**
   * This method is exclusively used to acquire locks. This handles the problem
   * of being unable to receive signals that would free locks while waiting for
   * a lock to be made.
   *
   * @param lockObject Lock object we're trying to acquire a lock on
   * @param readLock True if we're trying for read lock, false for read/write
   */
  void CubeDataThread::AcquireLock(QReadWriteLock *lockObject, bool readLock) {
    if(readLock) {
      while(!lockObject->tryLockForRead()) {
        // while we can't get the lock, allow other processing to happen for
        //  brief periods of time
        //
        // Give time for things to happen in other threads
        QThread::yieldCurrentThread();

        p_currentLocksWaiting ++;
        QEventLoop eventLoop;
        eventLoop.processEvents();
        p_currentLocksWaiting --;
      }
    }
    else {
      while(!lockObject->tryLockForWrite()) {
        // while we can't get the lock, allow other processing to happen for
        //  brief periods of time
        //
        // Give time for things to happen in other threads
        QThread::yieldCurrentThread();

        p_currentLocksWaiting ++;
        QEventLoop eventLoop;
        eventLoop.processEvents();
        p_currentLocksWaiting --;
      }
    }
  }


  /**
   * This slot should be connected to and upon receiving a signal it will begin
   * the necessary cube I/O to get this data. When the data is available, a
   * ReadReady signal will be emitted with the parameter caller being equal to
   * the requester pointer in the signal. You should pass "this" for caller, and
   * you must ignore all ReadReady signals which do not have "requester == this"
   * -> otherwise your pointer is not guaranteed.
   *
   * @param cubeId Cube to read from
   * @param startSample Starting Sample Position
   * @param startLine Starting Line Position
   * @param endSample Ending Sample Position
   * @param endLine Ending Line Position
   * @param band Band Number To Read From (multi-band bricks not supported at this
   *             time)
   * @param caller A pointer to the calling class, used to identify who requested
   *               the data when they receive the ReadReady signal
   */
  void CubeDataThread::ReadCube(int cubeId, int startSample, int startLine,
                                int endSample, int endLine, int band,
                                void *caller) {

    if(!p_managedCubes->contains(cubeId)) {
      iString msg = "cube ID [";
      msg += iString(cubeId);
      msg += "] is not a valid cube ID";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    GetCubeData(cubeId, startSample, startLine, endSample, endLine, band,
                caller, true);
  }


  /**
   * This slot should be connected to and upon receiving a signal it will begin
   * the necessary cube I/O to get this data. When the data is available, a
   * ReadWriteReady signal will be emitted with the parameter caller being equal
   * to the requester pointer in the signal. You should pass "this" for caller,
   * and you must ignore all ReadReady signals which do not have "requester ==
   * this" -> otherwise your pointer is not guaranteed and you are corrupting the
   * process of the real requester.
   *
   * @param cubeId Cube to read from
   * @param startSample Starting Sample Position
   * @param startLine Starting Line Position
   * @param endSample Ending Sample Position
   * @param endLine Ending Line Position
   * @param band Band Number To Read From (multi-band bricks not supported at this
   *             time)
   * @param caller A pointer to the calling class, used to identify who requested
   *               the data when they receive the ReadWriteReady signal
   */
  void CubeDataThread::ReadWriteCube(int cubeId, int startSample, int startLine,
                                     int endSample, int endLine, int band,
                                     void *caller) {

    if(!p_managedCubes->contains(cubeId)) {
      iString msg = "cube ID [";
      msg += iString(cubeId);
      msg += "] is not a valid cube ID";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    GetCubeData(cubeId, startSample, startLine, endSample, endLine, band,
                caller, false);
  }


  /**
   * This is a searching method used to identify overlapping data already in
   * memory.
   *
   * @param overlapping Brick to check for overlaps with
   * @param cubeId Cube ID asssociated with this brick
   * @param instanceNum Which instance of overlap to return
   * @param exact This is set to false if the match found is not exactly
   *   the overlapping brick.
   *
   * @return int -1 for none found, otherwise the index into p_managedData and
   *   p_managedDataSources that an overlap was found at
   */
  int CubeDataThread::OverlapIndex(const Isis::Brick *overlapping,
                                   int cubeId,
                                   int instanceNum,
                                   bool &exact) {
    exact = false;

    // Start with extracting the range of the input (search) brick
    int startSample = overlapping->Sample(0);
    int endSample  = overlapping->Sample(overlapping->size() - 1);
    int startLine  = overlapping->Line(0);
    int endLine    = overlapping->Line(overlapping->size() - 1);
    int startBand  = overlapping->Band(0);
    int endBand    = overlapping->Band(overlapping->size() - 1);

    // Now let's search for overlaps
    for(int knownBrick = 0;
        knownBrick < p_managedData->size();
        knownBrick++) {
      int sourceCube = (*p_managedDataSources)[knownBrick];

      // Ignore other cubes; they can't overlap
      if(sourceCube != cubeId) continue;

      QPair<QReadWriteLock *, Isis::Brick *> &managedBrick =
        (*p_managedData)[knownBrick];

      Isis::Brick &brick = *managedBrick.second;

      // Get the range of this brick we've found in memory to see if any overlap
      //   exists
      int compareSampStart = brick.Sample(0);
      int compareSampEnd   = brick.Sample(brick.size() - 1);
      int compareLineStart = brick.Line(0);
      int compareLineEnd   = brick.Line(brick.size() - 1);
      int compareBandStart = brick.Band(0);
      int compareBandEnd   = brick.Band(brick.size() - 1);

      bool overlap = false;

      // sample start is inside our sample range
      if(compareSampStart >= startSample && compareSampStart <= endSample) {
        overlap = true;
      }

      // sample end is inside our sample range
      if(compareSampEnd >= startSample && compareSampEnd <= endSample) {
        overlap = true;
      }

      // line start is in our line range
      if(compareLineStart >= startLine && compareLineStart <= endLine) {
        overlap = true;
      }

      // line end is in our line range
      if(compareLineEnd >= startLine && compareLineEnd <= endLine) {
        overlap = true;
      }

      // band start is in our line range
      if(compareBandStart >= startBand && compareBandStart <= endBand) {
        overlap = true;
      }

      // band end is in our line range
      if(compareBandEnd >= startBand && compareBandEnd <= endBand) {
        overlap = true;
      }

      exact = false;
      if(compareSampStart == startSample &&
          compareSampEnd   == endSample   &&
          compareLineStart == startLine   &&
          compareLineEnd   == endLine     &&
          compareBandStart == startBand   &&
          compareBandEnd   == endBand) {
        exact = true;
      }

      // If we have overlap, and we're at the requested instance of overlap,
      //   return it.
      if(overlap) {
        instanceNum --;

        if(instanceNum < 0) return knownBrick;
      }
    }

    // None found at this instance
    return -1;
  }


  /**
   * When done processing with a brick (reading or writing) this slot needs to
   * be signalled to free locks and memory.
   *
   * @param cubeId Cube associated with the brick
   * @param brickDone Brick pointer given by ReadReady, ReadWriteReady or
   *                  BrickChanged. An equivalent brick is also acceptable
   *                  (exact same range, but not same pointer).
   */
  void CubeDataThread::DoneWithData(int cubeId, const Isis::Brick *brickDone) {
    int instance = 0;
    bool exactMatch = false;
    bool writeLock = false;

    int index = OverlapIndex(brickDone, cubeId, instance, exactMatch);

    while(index != -1) {
      // If this isn't the data they're finished with, we don't care about it
      if(!exactMatch) {
        instance++;
        index = OverlapIndex(brickDone, cubeId, instance, exactMatch);
        continue;
      }

      // Test if we had a write lock (tryLockForRead will fail). If we had a
      //   write lock make note of it.
      if(!(*p_managedData)[index].first->tryLockForRead()) {
        if(writeLock) {
          Isis::iString msg = "Overlapping data had write locks";
          throw Isis::iException::Message(Isis::iException::Programmer,
                                          msg,
                                          _FILEINFO_);
        }

        writeLock = true;
      }
      // A read lock was in place, undo the lock we just made
      else {
        if(writeLock) {
          Isis::iString msg = "Overlapping data had write locks";
          throw Isis::iException::Message(Isis::iException::Programmer,
                                          msg,
                                          _FILEINFO_);
        }

        // Unlock the lock we just made
        (*p_managedData)[index].first->unlock();
      }

      // If we had a write lock we need to write the data to the file and
      //   notify others of the change if we have listeners.
      if(writeLock) {
        p_managedCubesMutex->lock();
        Isis::Brick cpy(*brickDone);
        p_managedCubes->value(cubeId).second->Write(cpy);
        p_managedCubesMutex->unlock();

        // Unlock the existing lock
        (*p_managedData)[index].first->unlock();

        // No listeners? Remove this entry
        if(p_numChangeListeners == 0) {
          if(FreeBrick(index)) {
            // We've freed the one and only match, nobody wants to know about
            //   it, so we're done
            break;
          }
        }
        // We have listeners, lock the data the appropriate number of times and
        //   then emit the BrickChanged with a pointer
        else {
          // notify others of this change
          for(int i = 0; i < p_numChangeListeners; i++) {
            AcquireLock((*p_managedData)[index].first, true);
          }

          emit BrickChanged((*p_managedDataSources)[index],
                            (*p_managedData)[index].second);
        }
      }
      // if we had a read lock and no longer have any locks, remove data from
      //   list
      else {
        // We had the one and only (hopefully!) exact match, let's free it if
        //   we can get a write lock and be done.
        // Free original read lock
        (*p_managedData)[index].first->unlock();

        if((*p_managedData)[index].first->tryLockForWrite()) {
          (*p_managedData)[index].first->unlock();
          FreeBrick(index);
        }

        break;
      }

      instance++;
      index = OverlapIndex(brickDone, cubeId, instance, exactMatch);
    }
  }


  /**
   * This is used internally to delete bricks when possible.
   *
   * @param brickIndex Brick to request deletion
   *
   * @return bool True if deletion actually happened
   */
  bool CubeDataThread::FreeBrick(int brickIndex) {
    if(!(*p_managedData)[brickIndex].first->tryLockForWrite()) {
      std::cerr << "WARNING: UNSAFE FreeBrick CALL, LOCKS EXIST" << std::endl;
    }
    else {
      (*p_managedData)[brickIndex].first->unlock();
    }

    if(p_currentLocksWaiting == 0) {
      delete(*p_managedData)[brickIndex].first;
      delete(*p_managedData)[brickIndex].second;

      p_managedData->removeAt(brickIndex);
      p_managedDataSources->removeAt(brickIndex);

      // Try to free any leftover bricks too
      for(int i = 0; i < p_managedData->size(); i++) {
        if((*p_managedData)[i].first->tryLockForWrite()) {
          p_managedData->removeAt(i);
          p_managedDataSources->removeAt(i);
          i --;
        }
      }

      return true;
    }

    // no actual free was done
    return false;
  }


  /**
   * This is a helper method for both testing/debugging and general information
   * that provides the current number of bricks in memory.
   *
   * @return int Count of how many bricks reside in memory.
   */
  int CubeDataThread::BricksInMemory() {
    return p_managedData->size();
  }
};

#ifndef IsisDebug_h
#define IsisDebug_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifdef __GTHREADS
#error *****IsisDebug.h MUST be included before any system header files!*****
#endif

void startMonitoringMemory();
void stopMonitoringMemory();
void SegmentationFault(int);
void Abort(int);
void InterruptSignal(int);

#ifdef CWDEBUG
/**
 * These includes may seem out of order but are necessary to be in this order
 * for our debugging library libcwd
 */
#include <libcwd/sys.h>
#include <libcwd/debug.h>
#include <execinfo.h>
#include <dlfcn.h>

#include <fstream>
#include <QMutex>

/**
 * The next two includes are bad, but are necessary without complicating code to
 * an extreme. These shouldn't cause an issue because they are not included in
 * release mode, or ever on systems without libcwd. QString is necessary for
 * string conversions and Constants.h for the definition of BigInt.
 */
#include "QString.h"
#include "Constants.h"

/**
 * This asserts the pointer is delete-able
 */
#define ASSERT_PTR(x)                                                       \
          if (libcwd::test_delete(x))                                       \
          {                                                                 \
            std::cerr << ">> " << __FILE__ << ":" << __LINE__ <<            \
            " error: ASSERT POINTER " << #x << " FAILED\n";                 \
          }

class MyMutex : public QMutex {
  public:
    bool trylock() {
      return tryLock();
    }
};

/**
 * @author ????-??-?? Steven Lambright
 *
 * @internal
 */
class StackTrace {
  public:
    /**
     * This returns the current stack trace. This must be
     * done avoiding extra includes and inside this header
     * file to accomplish stack traces with only the application
     * in debug mode and not adding extra includes.
     *
     * Caller takes ownership of the return value.
     *
     * @return char* String containing stack trace
     */
    static void GetStackTrace(std::vector<std::string> *stackTraceResult) {
      stackTraceResult->clear();
      const int MAX_STACK_DEPTH = 1024;

      void *stackTrace[MAX_STACK_DEPTH];
      int stackSize = backtrace(stackTrace, MAX_STACK_DEPTH);

      for(int stackEl = 2; stackEl < stackSize; stackEl ++) {
        std::string currElement;
        void *addr = stackTrace[stackEl];

        libcwd::location_ct addrInfo(addr);
        std::string demangled_name;
        libcwd::demangle_symbol(addrInfo.mangled_function_name(), demangled_name);

        currElement = ">> ";

        if(addrInfo.is_known()) {
          currElement +=
            std::string(addrInfo.file()) +
            std::string(":") +
            QString((Isis::BigInt)addrInfo.line()) +
            std::string(" --- ") +
            demangled_name;
        }
        else {
          currElement += "?????:0 --- " + demangled_name;
        }

        stackTraceResult->push_back(currElement);
      }
    }
};

#else
#define ASSERT_PTR(x)


/**
 * @author ????-??-?? Steven Lambright
 *
 * @internal
 */
class StackTrace {
  public:
    static void GetStackTrace(const void *) {}
};
#endif

#ifdef DEBUG
#include <iostream>
#define ASSERT(x)                                                         \
          if (!(x))                                                         \
          {                                                                 \
            std::cerr << ">> " << __FILE__ << ":" << __LINE__ <<            \
            " error: ASSERT " << #x << " FAILED\n";                         \
          }
#else
#define ASSERT(x)
#endif

#endif

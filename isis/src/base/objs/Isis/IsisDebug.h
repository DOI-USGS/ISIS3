#ifndef IsisDebug_h
#define IsisDebug_h

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
   * release mode, or ever on systems without libcwd. iString is necessary for 
   * string conversions and Constants.h for the definition of BigInt. 
   */
  #include "iString.h"
  #include "Constants.h"

  #define ASSERT(x)                                                         \
          if (!(x))                                                         \
          {                                                                 \
            std::cerr << ">> " << __FILE__ << ":" << __LINE__ <<            \
            " error: ASSERT " << #x << " FAILED\n";                         \
          }

  /**
   * This asserts the pointer is delete-able
   */
  #define ASSERT_PTR(x)                                                     \
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
    
      for (int stackEl = 2; stackEl < stackSize; stackEl ++) {
        std::string currElement;
        void *addr = stackTrace[stackEl];
    
        libcwd::location_ct addrInfo(addr);
        std::string demangled_name;
        libcwd::demangle_symbol(addrInfo.mangled_function_name(), demangled_name);
    
        currElement = ">> ";
    
        if (addrInfo.is_known()) {
          currElement += 
          std::string(addrInfo.file()) +
          std::string(":") +
          Isis::iString((Isis::BigInt)addrInfo.line()) +
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
  #define ASSERT(x)
  #define ASSERT_PTR(x)

  class StackTrace {
    public:
      static void GetStackTrace(const void *) {}
  };
#endif
#endif

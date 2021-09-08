#ifndef NaifContext_h
#define NaifContext_h

/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2021/08/21 20:02:37 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <stack>
#include <boost/shared_ptr.hpp>

namespace Isis {
  class NaifSnapshot;
  
  /**
   * @brief Manages the main lifecycle of f2c'd NAIF state.
   *
   * @internal
   */
  class NaifContext {
    friend class NaifSnapshot;
  
    public:
      static void createForThread();
      static void destroyForThread();
      
      static NaifContext* get() { return m_self; }

      boost::shared_ptr<NaifSnapshot> top() { return m_stack.top(); }
      
      void push(boost::shared_ptr<NaifSnapshot>& snapshot);
      void push_copy(boost::shared_ptr<NaifSnapshot>& snapshot);
      
      boost::shared_ptr<NaifSnapshot> pop();
      
    private:
      NaifContext();
      ~NaifContext();
      
      static thread_local NaifContext* m_self;
      
      std::stack<boost::shared_ptr<NaifSnapshot>> m_stack;
  };
  
#define NAIF_GETSET(type, name) \
  inline type name() { return m_isis.name; } \
  inline void set_##name(type v) { m_isis.name = v; }
  
  /**
   * @brief Takes a copy of the current NAIF state.
   *
   * Can pass this to threads for them to load copies of the state.
   *
   * @internal
   */
  class NaifSnapshot {
    friend class NaifContext;

    private:
      /**
       * @brief Private ISIS state that must be attached to NAIF state.
       *
       * For example: Whether certain kernels have been loaded.
       *
       * @internal
       */
      struct IsisState {
        IsisState() 
          : naifStatusInitialized(false), iTimeInitialized(false), targetPckLoaded(false)
          , amicaTimingLoaded(false), hayabusaTimingLoaded(false), mdisTimingLoaded(false)
          , mocWagoLoaded(false), hiJitCubeLoaded(false), hiCalTimingLoaded(false) {}
      
        bool naifStatusInitialized;
        bool iTimeInitialized;
        bool targetPckLoaded;
        bool amicaTimingLoaded;
        bool hayabusaTimingLoaded;
        bool mdisTimingLoaded;
        bool mocWagoLoaded;
        bool hiJitCubeLoaded;
        bool hiCalTimingLoaded;
      };
      
    public:
      NaifSnapshot();
      NaifSnapshot(boost::shared_ptr<void> naif, IsisState& isis);
      NaifSnapshot(const NaifSnapshot& ctx);
      
      IsisState&              isisState() { return m_isis; }
      boost::shared_ptr<void> naifState() { return m_naif; }
      
      NAIF_GETSET(bool,       naifStatusInitialized);
      NAIF_GETSET(bool,       iTimeInitialized);
      NAIF_GETSET(bool,       targetPckLoaded);
      NAIF_GETSET(bool,       amicaTimingLoaded);
      NAIF_GETSET(bool,       hayabusaTimingLoaded);
      NAIF_GETSET(bool,       mdisTimingLoaded);
      NAIF_GETSET(bool,       mocWagoLoaded);
      NAIF_GETSET(bool,       hiJitCubeLoaded);
      NAIF_GETSET(bool,       hiCalTimingLoaded);
      
    private:
      boost::shared_ptr<void> m_naif;
      IsisState               m_isis;
  };
  
  /**
   * Pushes a copy of the input state onto the CSPICE state stack.
   *
   * Pops when out of scope.
   *
   * This class is not thread safe. Ensure only 1 thread has set
   * the state as activate.
   *
   * @internal
   */
  class PushNaifSnapshot {
    public:
      PushNaifSnapshot(boost::shared_ptr<NaifSnapshot> snapshot);
      ~PushNaifSnapshot();
  };
  
  /**
   * Sets the given snapshot as current.
   *
   * Pops when out of scope.
   *
   * Duplicating snapshots is thread safe (as long as the source state isn't active!).
   *
   * @internal
   */
  class PushNaifSnapshotCopy {
    public:
      PushNaifSnapshotCopy(boost::shared_ptr<NaifSnapshot> snapshot);
      ~PushNaifSnapshotCopy();
  };
};

#endif

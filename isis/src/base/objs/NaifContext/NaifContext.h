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
  
#define NAIF_GETSET(type, name)                   \
private:                                          \
  type name;                                      \
public:                                           \
  inline type get_##name() const { return name; } \
  inline void set_##name(type v) { name = v; }
  
  /**
   * @brief Manages the main lifecycle of f2c'd NAIF state.
   *
   * @internal
   */
  class NaifContext {
    // Little wrapper object so we can use the NaifContext
    // default copy constructor. 
    class CSpiceState {
      public:
        CSpiceState();
        CSpiceState(const CSpiceState &src);

        void* operator()() const { return m_state.get(); }
        
      private:
        boost::shared_ptr<void> m_state;
    };
  
    public:
      NaifContext();

      // A shared state for programs that haven't been converted to private state;
      static NaifContext * ctx();
      static void        * naif();

      static NaifContext * UseDefaultIfNull(NaifContext *naif) { return naif ? naif : ctx(); }

      NAIF_GETSET(bool, naifStatusInitialized);
      NAIF_GETSET(bool, iTimeInitialized);
      NAIF_GETSET(bool, targetPckLoaded);
      NAIF_GETSET(bool, amicaTimingLoaded);
      NAIF_GETSET(bool, hayabusaTimingLoaded);
      NAIF_GETSET(bool, mdisTimingLoaded);
      NAIF_GETSET(bool, mocWagoLoaded);
      NAIF_GETSET(bool, hiJitCubeLoaded);
      NAIF_GETSET(bool, hiCalTimingLoaded);

      // Supply this to the NAIF kernels.
      void* operator()() const { return m_cspice(); }
      void* get() const { return m_cspice(); }
      
    private:
      CSpiceState m_cspice;
  };

  // Should we ever need to use shared pointers.
  typedef NaifContext* NaifContextPtr;
};

#endif

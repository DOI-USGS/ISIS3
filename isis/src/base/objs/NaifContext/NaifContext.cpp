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
#include "NaifContext.h"

#include "cspice_state.h"

#include <boost/make_shared.hpp>

namespace Isis {

  static NaifContext static_global_state;

  NaifContext::CSpiceState::CSpiceState() : m_state(cspice_alloc(), &cspice_free) {}
  NaifContext::CSpiceState::CSpiceState(const CSpiceState &src) : m_state(cspice_copy(src.m_state.get()), &cspice_free) {}

  NaifContext::NaifContext()
    : m_cspice()
    , naifStatusInitialized(false), iTimeInitialized(false), targetPckLoaded(false)
    , amicaTimingLoaded(false), hayabusaTimingLoaded(false), mdisTimingLoaded(false)
    , mocWagoLoaded(false), hiJitCubeLoaded(false), hiCalTimingLoaded(false)
  {
  }

  NaifContext *NaifContext::ctx() { return &static_global_state; }
  void        *NaifContext::ctx() { return static_global_state(); }
  
}

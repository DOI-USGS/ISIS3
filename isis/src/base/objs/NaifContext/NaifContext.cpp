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

  thread_local NaifContext* NaifContext::m_self = nullptr;

  void NaifContext::createForThread() {
    if (m_self)
      throw std::runtime_error("NaifContext already created for thread!");
      
    m_self = new NaifContext();
  }
  
  void NaifContext::destroyForThread() {
    if (!m_self)
      throw std::runtime_error("NaifContext doesn't exist for thread!");
      
    delete m_self;
    m_self = nullptr;
  }

  NaifContext::NaifContext() {
    cspice_init();

    boost::shared_ptr<void> initial_cspice(cspice_save(), &cspice_free);
    NaifSnapshot::IsisState initial_isis;
    boost::shared_ptr<NaifSnapshot> initial_state = boost::make_shared<NaifSnapshot>(initial_cspice, initial_isis);
    push(initial_state);
  }

  NaifContext::~NaifContext() {
    pop();
    cspice_shutdown();
  }
  
  void NaifContext::push(boost::shared_ptr<NaifSnapshot>& snapshot) {
    m_stack.push(snapshot);
    cspice_push(snapshot->naifState().get());
  }
  
  void NaifContext::push_copy(boost::shared_ptr<NaifSnapshot>& snapshot) {
    boost::shared_ptr<NaifSnapshot> copy = boost::make_shared<NaifSnapshot>(*snapshot);
    m_stack.push(copy);
    cspice_push(copy->naifState().get());  
  }
      
  boost::shared_ptr<NaifSnapshot> NaifContext::pop() {
    cspice_pop();
    auto r = m_stack.top();
    m_stack.pop();

    return r;
  }


  NaifSnapshot::NaifSnapshot() 
    : m_naif(cspice_save(), &cspice_free)
    , m_isis(NaifContext::get()->top()->m_isis) 
    {}
  NaifSnapshot::NaifSnapshot(boost::shared_ptr<void> naif, IsisState& isis)
    : m_naif(naif)
    , m_isis(isis)
    {}
  NaifSnapshot::NaifSnapshot(const NaifSnapshot &src) 
    : m_naif(cspice_copy(src.m_naif.get()), &cspice_free)
    , m_isis(src.m_isis)
    {}


  PushNaifSnapshot::PushNaifSnapshot(boost::shared_ptr<NaifSnapshot> snapshot) {
    NaifContext::get()->push(snapshot);
  }

  PushNaifSnapshot::~PushNaifSnapshot() {
    NaifContext::get()->pop();
  }


  PushNaifSnapshotCopy::PushNaifSnapshotCopy(boost::shared_ptr<NaifSnapshot> snapshot) {
    NaifContext::get()->push_copy(snapshot);
  }

  PushNaifSnapshotCopy::~PushNaifSnapshotCopy() {
    NaifContext::get()->pop();
  }
}

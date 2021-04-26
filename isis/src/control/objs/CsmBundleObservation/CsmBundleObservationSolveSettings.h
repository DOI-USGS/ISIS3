#ifndef CsmBundleObservationSolveSettings_h
#define CsmBundleObservationSolveSettings_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <csm.h>
#include <QSharedPointer.h>

namespace Isis {

  /**
   * @brief Class to hold solve settings for CSM observations
   *
   * @ingroup ControlNetworks
   *
   * @author 2021-04-19 Jesse Mapel
   *
   */
  class CsmBundleObservationSolveSettings {

    public:
      CsmBundleObservationSolveSettings(csm::param::Set solveSet);
      ~CsmBundleObservationSolveSettings();

      csm::param::Set solveSet();

    private:
      csm::param::Set m_solveSet; //! The set of parameters to solve for

  };

  typedef QSharedPointer<CsmBundleObservationSolveSettings> CsmBundleObservationSolveSettingsQsp;

}

#endif // CsmBundleObservationSolveSettings_h
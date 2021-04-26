#ifndef CsmBundleObservation_h
#define CsmBundleObservation_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QStringList>
#include <QVector>

#include "BundleImage.h"
#include "CsmBundleObservationSolveSettings.h"
#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "LinearAlgebra.h"
#include "AbstractBundleObservation.h"

namespace Isis {

  /**
   * @brief Class for bundle observations
   *
   * This class is used for creating a bundle observation. Contained BundleImages are stored as
   * shared pointers, so they will be automatically deleted when all shared pointers are deleted.
   *
   * @ingroup ControlNetworks
   *
   * @author 2021-04-19 Jesse Mapel
   *
   */
  class CsmBundleObservation : public AbstractBundleObservation {

    public:
      // default constructor
      CsmBundleObservation();

      // constructor
      CsmBundleObservation(BundleImageQsp image, QString observationNumber, QString instrumentId,
                        BundleTargetBodyQsp bundleTargetBody);

      // copy constructor
      CsmBundleObservation(const CsmBundleObservation &src);

      // destructor
      ~CsmBundleObservation();

      // equals operator
      CsmBundleObservation &operator=(const CsmBundleObservation &src);

      // copy method
      void copy(const CsmBundleObservation &src);

      bool setSolveSettings(CsmBundleObservationSolveSettingsQsp solveSettings);

      int numberParameters();

      const BundleObservationSolveSettingsQsp solveSettings();

      bool applyParameterCorrections(LinearAlgebra::Vector corrections);

      void bundleOutputString(std::ostream &fpOut,bool errorPropagation);
      QString bundleOutputCSV(bool errorPropagation);

      QString formatBundleOutputString(bool errorPropagation, bool imageCSV=false);

   private:
      bool initParameterWeights();

    private:
      CsmBundleObservationSolveSettingsQsp m_solveSettings; //!< Solve settings for this observation.
      std::vector<int> m_paramIndices; //!< The indices of the parameters the observation is solving for
  };
}

#endif // CsmBundleObservation_h

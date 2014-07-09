#ifndef BundleObservation_h
#define BundleObservation_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QStringList>
#include <QVector>

#include <boost/numeric/ublas/vector.hpp>

#include "BundleImage.h"
#include "BundleObservationSolveSettings.h"
#include "SpiceRotation.h"
#include "SpicePosition.h"

namespace Isis {

  class BundleObservation : public QVector< BundleImage* > {

    public:
      // default constructor
      BundleObservation();

      // constructor
      BundleObservation(BundleImage* image, QString observationNumber, QString instrumentId);

      // destructor
      ~BundleObservation();

    // copy constructor
    BundleObservation(const BundleObservation& src);

    // equals operator
    BundleObservation& operator=(const BundleObservation &src);

    // copy method
    void copy(const BundleObservation& src);

    bool setSolveSettings(BundleObservationSolveSettings* solveSettings);

    void setIndex(int n);
    int index();

    int numberPositionParameters();
    int numberPointingParameters();
    int numberParameters();

    QString instrumentId();

    SpiceRotation* spiceRotation();
    SpicePosition* spicePosition();

    boost::numeric::ublas::vector< double >& parameterWeights();
    boost::numeric::ublas::vector< double >& parameterCorrections();
    boost::numeric::ublas::vector< double >& parameterSolution();
    boost::numeric::ublas::vector< double >& aprioriSigmas();
    boost::numeric::ublas::vector< double >& adjustedSigmas();

    const BundleObservationSolveSettings* solveSettings() { return m_solveSettings;}

//    QStringList serialNumbers();

    bool applyParameterCorrections(boost::numeric::ublas::vector<double> corrections);
    bool initializeExteriorOrientation();

    QString formatBundleOutputString(bool errorPropagation);

    private:
    bool initParameterWeights();

    private:
      QString m_observationNumber; //!< this is typically equivalent to serial number
                                   //!< except in the case of "observation mode" (e.g.
                                   //!< Lunar Orbiter) where for each image in the
                                   //!< observation the observation # is the serial #
                                   //!< augmented with an additional integer

      int m_Index;

      QStringList m_serialNumbers; //!< list of all cube serial numbers in observation

      QString m_instrumentId;      //!< spacecraft instrument id

      BundleObservationSolveSettings* m_solveSettings; //!< solve settings for this observation

      SpiceRotation* m_instrumentRotation;   //!< Instrument spice rotation (in primary image)
      SpicePosition* m_instrumentPosition;   //!< Instrument spice position (in primary image)

      boost::numeric::ublas::vector< double > m_weights;            //!< parameter weights
      boost::numeric::ublas::vector< double > m_corrections;        //!< cumulative parameter correction vector
      boost::numeric::ublas::vector< double > m_solution;           //!< parameter solution vector
      boost::numeric::ublas::vector< double > m_aprioriSigmas;      //!< a posteriori (adjusted) parameter sigmas
      boost::numeric::ublas::vector< double > m_adjustedSigmas;     //!< a posteriori (adjusted) parameter sigmas
  };
}

#endif // BundleObservation_h

#include "BundleLidarControlPoint.h"

#include <QDebug>

#include "ControlMeasure.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "SpecialPixel.h"

namespace Isis {


  /**
   * Constructs a BundleLidarControlPoint object from a LidarControlPoint. Only the
   * non-ignored measures are added to the BundleLidarControlPoint.
   *  
   * @param controlPoint Pointer to a ControlPoint that will be used to 
   *                     construct this BundleLidarControlPoint.
   */
  BundleLidarControlPoint::BundleLidarControlPoint(LidarControlPoint *point)
    : BundleControlPoint((ControlPoint*)  point) {

    // additional lidar specific initializations
  }


  /**
   * Copy constructor. Constructs a BundleLidarControlPoint object from an existing
   * BundleLidarControlPoint.
   *  
   * @param src The BundleLidarControlPoint to be copied.
   */
//  BundleLidarControlPoint::BundleLidarControlPoint(const BundleLidarControlPoint &src) {
//    : BundleControlPoint(((ControlPoint) src) {
//    copy(src);
//  }


  /**
   * Destructor for BundleLidarControlPoint.
   */
  BundleLidarControlPoint::~BundleLidarControlPoint() {
  }


  /**
   * Copies given BundleLidarControlPoint to this BundleLidarControlPoint.
   *  
   * @param src The BundleLidarControlPoint to be copied.
   */
  void BundleLidarControlPoint::copy(const BundleLidarControlPoint &src) {

    BundleControlPoint::copy((BundleControlPoint) src);
/*
    // sanity check
    clear();

    m_controlPoint = src.m_controlPoint;

    int numMeasures = src.size();

    for (int i = 0; i < numMeasures; i++)
      append(BundleMeasureQsp( new BundleMeasure(*(src.at(i))) ));

    m_corrections = src.m_corrections;
    m_aprioriSigmas = src.m_aprioriSigmas;
    m_adjustedSigmas = src.m_adjustedSigmas;
    m_weights = src.m_weights;
    m_nicVector = src.m_nicVector;
*/
  }
}

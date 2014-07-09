#include "BundleControlPointVector.h"

#include "BundleControlPoint.h"
#include "BundleObservationVector.h"
#include "Camera.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"

#include "IException.h"


namespace Isis {

  /**
   * constructor
   */
  BundleControlPointVector::BundleControlPointVector() {
  }


  /**
   * destructor
   */
  BundleControlPointVector::~BundleControlPointVector() {
    qDeleteAll(*this);
    clear();
  }


  /**
   * add new BundleControlPoint method
   */
  BundleControlPoint* BundleControlPointVector::addControlPoint(ControlPoint* point) {

    BundleControlPoint* bundleControlPoint = new BundleControlPoint(point);

    append(bundleControlPoint);

    return bundleControlPoint;
  } 

}



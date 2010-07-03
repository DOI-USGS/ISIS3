#include <cmath>
#include "HapkePhotoModel.h"
#include "iException.h"

using namespace std;

namespace Isis {
  HapkePhotoModel::HapkePhotoModel (Pvl &pvl) : PhotoModel(pvl) {
    p_photoHh = 0.0;
    p_photoB0 = 0.0;
    p_photoTheta = 0.0;
    p_photoWh = 0.5;
    p_photoThetaold = -999.0;
    
    PvlGroup &algorithm = pvl.FindObject("PhotometricModel").FindGroup("Algorithm",Pvl::Traverse);

    if (algorithm.HasKeyword("Wh")) {
      SetPhotoWh(algorithm["Wh"]);
    }

    if (algorithm.HasKeyword("Hh")) {
      SetPhotoHh(algorithm["Hh"]);
    }

    if (algorithm.HasKeyword("B0")) {
      SetPhotoB0(algorithm["B0"]);
    }

    p_photoB0save = p_photoB0;

    if (algorithm.HasKeyword("Theta")) {
      SetPhotoTheta(algorithm["Theta"]);
    }
  }


  /**
    * Set the Hapke single scattering albedo component.
    * This parameter is limited to values that are >0 and
    * <=1.
    *
    * @param wh  Hapke single scattering albedo component, default is 0.5
    */
   void HapkePhotoModel::SetPhotoWh (const double wh) {
     if (wh <= 0.0 || wh > 1.0) {
       string msg = "Invalid value of Hapke wh [" +
           iString(wh) + "]";
       throw iException::Message(iException::User,msg,_FILEINFO_);
     }
     p_photoWh = wh;
   }


 /**
   * Set the Hapke opposition surge component. This is one of 
   * two opposition surge components needed for the Hapke model. 
   * This parameter is limited to values that are >=0.
   *
   * @param hh  Hapke opposition surge component, default is 0.0
   */
  void HapkePhotoModel::SetPhotoHh (const double hh) {
    if (hh < 0.0) {
      string msg = "Invalid value of Hapke hh [" +
          iString(hh) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoHh = hh;
  }


 /**
   * Set the Hapke opposition surge component. This is one of
   * two opposition surge components needed for the Hapke model.
   * This parameter is limited to values that are >=0.
   *
   * @param b0  Hapke opposition surge component, default is 0.0
   */
  void HapkePhotoModel::SetPhotoB0 (const double b0) {
    if (b0 < 0.0) {
      string msg = "Invalid value of Hapke b0 [" +
          iString(b0) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoB0 = b0;
  }


 /**
   * Set the Hapke macroscopic roughness component. This parameter
   * is limited to values that are >=0 and <=90.
   *
   * @param theta  Hapke macroscopic roughness component, default is 0.0
   */
  void HapkePhotoModel::SetPhotoTheta (const double theta) {
    if (theta < 0.0 || theta > 90.0) {
      string msg = "Invalid value of Hapke theta [" +
          iString(theta) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoTheta = theta;
  }


  void HapkePhotoModel::SetStandardConditions(bool standard) {
    PhotoModel::SetStandardConditions(standard);

    if (standard) {
      p_photoB0save = p_photoB0;
      p_photoB0 = 0.0;
    } 
    else {
      p_photoB0 = p_photoB0save;
    }
  }
}

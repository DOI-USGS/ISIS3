#include "Pvl.h"
#include "NormModel.h"
#include "PhotoModel.h"
#include "AtmosModel.h"
#include "iException.h"

namespace Isis {
  /**
   * Create a NormModel object.  Because this is a pure virtual class you can
   * not create a NormModel class directly.  Instead, see the NormModelFactory 
   * class.
   * 
   * @param pvl  A pvl object containing a valid NormModel specification
   * 
   * @see normalizationModels.doc
   */
  NormModel::NormModel (Pvl &pvl, PhotoModel &pmodel) {
    p_normAlgorithmName = "Unknown";
    p_normPM = &pmodel;
    p_normAM = NULL;
    p_normWavelength = 1.0;
  }

  /**
   * Create a NormModel object.  Because this is a pure virtual class you can
   * not create a NormModel class directly.  Instead, see the NormModelFactory 
   * class.
   * 
   * @param pvl  A pvl object containing a valid NormModel specification
   * 
   * @see normalizationModels.doc
   */
  NormModel::NormModel (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel) {
    p_normAlgorithmName = "Unknown";
    p_normPM = &pmodel;
    p_normAM = &amodel;
    p_normWavelength = 1.0;
  }

  /**
   * Set the wavelength parameter. This value is obtained
   * from the BandBin Center keyword of the image. This must
   * be set by the application.
   */
  void NormModel::SetNormWavelength(double wavelength) {
    p_normWavelength = wavelength;
  }

  /**
   * Calculate the normalization albedo using photometric angle information
   * 
   * @param pha  input phase angle
   * @param inc  input incidence angle
   * @param ema  input emission angle
   * @param dn   input albedo value
   *          
   */
  void NormModel::CalcNrmAlbedo(double pha, double inc, double ema, double dn,
      double &albedo, double &mult, double &base) {

    // Check validity of photometric angles
    //if (pha > 180.0 || inc > 90.0 || ema > 90.0 || pha < 0.0 ||
    //    inc < 0.0 || ema < 0.0) {
    //  std::string msg = "Invalid photometric angles";
    //  throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    //}

    // Perform normalization
    NormModelAlgorithm(pha,inc,ema,dn,albedo,mult,base);
  }

  /**
   * Calculate the normalization albedo using photometric angle information
   * 
   * @param pha  input phase angle
   * @param inc  input incidence angle for ellipsoid
   * @param ema  input emission angle for ellipsoid
   * @param deminc  input incidence angle for dem
   * @param demema  input emission angle for dem
   * @param dn  input albedo value
   *          
   */
  void NormModel::CalcNrmAlbedo(double pha, double inc, double ema,
      double deminc, double demema, double dn, double &albedo,
      double &mult, double &base) {

    // Check validity of photometric angles
    //if (pha > 180.0 || inc > 90.0 || ema > 90.0 || pha < 0.0 ||
    //    inc < 0.0 || ema < 0.0) {
    //  std::string msg = "Invalid photometric angles";
    //  throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    //}

    // Perform normalization
    NormModelAlgorithm(pha,inc,ema,deminc,demema,dn,albedo,mult,base);
  }
}

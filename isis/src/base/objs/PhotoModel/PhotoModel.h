#ifndef PhotoModel_h
#define PhotoModel_h
/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2008/11/05 23:38:02 $
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

#include <string>
#include <vector>
#include "NumericalApproximation.h"
#include "Pvl.h"

namespace Isis {
  /**
   * @brief
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
   *          code
   *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
   *  @history 2007-07-31 Steven Lambright - Moved children methods out of this
   *                      class and into the children classes
   *  @history 2008-03-07 Janet Barrett - Moved variables and related
   *                      methods that pertain to Hapke specific parameters
   *                      to this class from the HapkeHen class. Also added
   *                      the code to set standard conditions.
   *  @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *  @history 2008-07-09 Steven Lambright - Fixed unit test
   *  @history 2008-10-17 Steven Lambright - Moved Hapke-specific methods out of
   *                      this class and into children classes.
   *  @history 2008-11-05 Jeannie Walldren - Moved PhtAcos() from
   *                      NumericalMethods class.
   */
  class PhotoModel {
    public:
      PhotoModel(Pvl &pvl);
      virtual ~PhotoModel() {};

      //! Return algorithm name found in Pvl file from constructor
      inline QString AlgorithmName() const {
        return p_photoAlgorithmName;
      };

      virtual void SetStandardConditions(bool standard);
      //! Returns true if standard conditions are used, i.e., if SetStandardConditions(true) has been called.  This is initialized to false in the constructor.
      bool StandardConditions() const {
        return p_standardConditions;
      }

      // Obtain topographic derivative
      double PhtTopder(double phase, double incidence, double emission);

      // Obtain arccosine
      static double PhtAcos(double cosang);

      // Calculate the surface brightness
      double CalcSurfAlbedo(double pha, double inc, double ema);

      virtual void SetPhotoL(const double l) {};

      //! Return photometric L value
      inline double PhotoL() const {
        return p_photoL;
      };

      virtual void SetPhotoK(const double k) {};

      //! Return photometric K value
      inline double PhotoK() const {
        return p_photoK;
      };

      virtual void SetPhotoHg1(const double hg1) {};

      //! Return photometric Hg1 value
      inline double PhotoHg1() const {
        return p_photoHg1;
      };

      virtual void SetPhotoHg2(const double hg2) {};

      //! Return photometric Hg2 value
      inline double PhotoHg2() const {
        return p_photoHg2;
      };

      virtual void SetPhotoBh(const double bh) {};

      //! Return photometric Bh value
      inline double PhotoBh() const {
        return p_photoBh;
      };

      virtual void SetPhotoCh(const double ch) {};

      //! Return photometric Ch value
      inline double PhotoCh() const {
        return p_photoCh;
      };

      virtual void SetPhotoWh(const double wh) {};

      //! Return photometric Wh value
      inline double PhotoWh() const {
        return p_photoWh;
      };

      virtual void SetPhotoHh(const double hh) {};

      //! Return photometric Hh value
      inline double PhotoHh() const {
        return p_photoHh;
      };

      virtual void SetPhotoB0(const double b0) {};

      //! Return photometric B0 value
      inline double PhotoB0() const {
        return p_photoB0;
      };

      virtual void SetPhotoTheta(const double theta) {};

      //! Return photometric Theta value
      inline double PhotoTheta() const {
        return p_photoTheta;
      };

      virtual void SetOldTheta(double theta) {};

      virtual void SetPhoto0B0Standard(const std::string &b0standard) {};

      //! Return photometric B0 standardization value
      inline std::string Photo0B0Standard() const {
        return p_photo0B0Standard;
      }

      //! Hapke's approximation to Chandra's H function
      inline double Hfunc(double u, double gamma) {
        return (1.0 + 2.0 * u) / (1.0 + 2.0 * u * gamma);
      }
 
      virtual void SetPhotoPhaseList(const std::string phasestrlist) {};
      virtual void SetPhotoKList(const std::string kstrlist) {};
      virtual void SetPhotoLList(const std::string kstrlist) {};
      virtual void SetPhotoPhaseCurveList(const std::string phasecurvestrlist) {};

      //! Return photometric phase angle list
      inline std::vector<double> PhotoPhaseList() const {
        return p_photoPhaseList;
      };

      //! Return photometric k value list
      inline std::vector<double> PhotoKList() const {
        return p_photoKList;
      };

      //! Return photometric l value list
      inline std::vector<double> PhotoLList() const {
        return p_photoLList;
      };

      //! Return photometric phase curve value list
      inline std::vector<double> PhotoPhaseCurveList() const {
        return p_photoPhaseCurveList;
      };

    protected:
      virtual double PhotoModelAlgorithm(double phase,
                                         double incidence, double emission) = 0;
      double p_photoL;
      double p_photoK;
      double p_photoHg1;
      double p_photoHg2;
      double p_photoBh;
      double p_photoCh;
      double p_photoCott;
      double p_photoCot2t;
      double p_photoTant;
      double p_photoSr;
      double p_photoOsr;
      QString p_algName;
      std::string p_photo0B0Standard;
      double p_photoWh;
      double p_photoHh;
      double p_photoB0;
      double p_photoB0save;
      double p_photoTheta;
      double p_photoThetaold;

      std::vector<double> p_photoPhaseList;
      std::vector<double> p_photoKList;
      std::vector<double> p_photoLList;
      std::vector<double> p_photoPhaseCurveList;
      int p_photoPhaseAngleCount;
      NumericalApproximation p_photoKSpline;
      NumericalApproximation p_photoLSpline;
      NumericalApproximation p_photoBSpline;

    private:
      //! Unique name of the photometric model
      QString p_photoAlgorithmName;
      //! Indicates whether standard conditions are used
      bool p_standardConditions;
  };
};

#endif

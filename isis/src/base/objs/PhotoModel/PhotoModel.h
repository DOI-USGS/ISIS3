#ifndef PhotoModel_h
#define PhotoModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>

#include <QString>
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
      }

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

      virtual void SetPhotoL(const double l) {
        p_photoL = l;
      }

      //! Return photometric L value
      inline double PhotoL() const {
        return p_photoL;
      }

      virtual void SetPhotoK(const double k) {
        p_photoK = k;
      }

      //! Return photometric K value
      inline double PhotoK() const {
        return p_photoK;
      }

      virtual void SetPhotoHg1(const double hg1) {
        p_photoHg1 = hg1;
      }

      //! Return photometric Hg1 value
      inline double PhotoHg1() const {
        return p_photoHg1;
      }

      virtual void SetPhotoHg2(const double hg2) {
        p_photoHg2 = hg2;
      }

      //! Return photometric Hg2 value
      inline double PhotoHg2() const {
        return p_photoHg2;
      }

      virtual void SetPhotoBh(const double bh) {
        p_photoBh = bh;
      }

      //! Return photometric Bh value
      inline double PhotoBh() const {
        return p_photoBh;
      }

      virtual void SetPhotoCh(const double ch) {
        p_photoCh = ch;
      }

      //! Return photometric Ch value
      inline double PhotoCh() const {
        return p_photoCh;
      }

      virtual void SetPhotoWh(const double wh) {
        p_photoWh = wh;
      }

      //! Return photometric Wh value
      inline double PhotoWh() const {
        return p_photoWh;
      }

      virtual void SetPhotoHh(const double hh) {
        p_photoHh = hh;
      }

      //! Return photometric Hh value
      inline double PhotoHh() const {
        return p_photoHh;
      }

      virtual void SetPhotoB0(const double b0) {
        p_photoB0 = b0;
      }

      //! Return photometric B0 value
      inline double PhotoB0() const {
        return p_photoB0;
      }

      virtual void SetPhotoTheta(const double theta) {
        p_photoTheta = theta;
      }

      //! Return photometric Theta value
      inline double PhotoTheta() const {
        return p_photoTheta;
      }

      // virtual void SetOldTheta(double theta) = 0;

      virtual void SetPhoto0B0Standard(const QString &b0standard) {
        p_photo0B0Standard = b0standard;
      }

      //! Return photometric B0 standardization value
      inline QString Photo0B0Standard() const {
        return p_photo0B0Standard;
      }

      //! Hapke's approximation to Chandra's H function
      inline double Hfunc(double u, double gamma) {
        return (1.0 + 2.0 * u) / (1.0 + 2.0 * u * gamma);
      }

      virtual void SetPhotoPhaseList(const QString) {}
      virtual void SetPhotoKList(const QString) {}
      virtual void SetPhotoLList(const QString) {}
      virtual void SetPhotoPhaseCurveList(const QString) {}

      //! Return photometric phase angle list
      inline std::vector<double> PhotoPhaseList() const {
        return p_photoPhaseList;
      }

      //! Return photometric k value list
      inline std::vector<double> PhotoKList() const {
        return p_photoKList;
      }

      //! Return photometric l value list
      inline std::vector<double> PhotoLList() const {
        return p_photoLList;
      }

      //! Return photometric phase curve value list
      inline std::vector<double> PhotoPhaseCurveList() const {
        return p_photoPhaseCurveList;
      }

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
      QString p_photo0B0Standard;
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

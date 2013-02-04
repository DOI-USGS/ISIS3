#ifndef Hapke_h
#define Hapke_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/11/05 23:38:50 $
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
#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Hapke-Henyey-Greenstein photometric model.
   *  Derive model albedo using complete Hapke model with
   *  Henyey-Greenstein single-particle phase function
   *  whose coefficients are hg1 and hg2, plus single
   *  scattering albedo wh, opposition surge parameters
   *  hh and b0, and macroscopic roughness theta.
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1989-08-02 Unknown
   *
   * @internal
   *   @history 2008-01-14 Janet Barret - Imported into Isis3 from Isis2.
   *   @history 2008-03-07 Janet Barrett - Moved variables and related
   *                           methods that pertain to Hapke specific parameters
   *                           to the PhotoModel class.
   *   @history 2008-06-18 Stuart Sides - Fixed doc error
   *   @history 2008-10-17 Steven Lambright - Changed inheritance, moved
   *                           HapkeHen specific methods into this class from
   *                           the base class.
   *   @history 2008-11-05 Jeannie Walldren - Added documentation
   *                           from Isis2 files
   *   @history 2012-10-15 Janet Barrett - Shortened the ZEROB0STANDARD 
   *                           parameter name to ZEROB0ST. This was done so that
   *                           the GUI interface does not have to be resized every
   *                           time it is opened. The shortened name helps to keep
   *                           the parameters within the default GUI size when it is
   *                           first opened. References #453. Fixes #1288.
   */
  class Hapke : public PhotoModel {
    public:
      Hapke(Pvl &pvl);
      virtual ~Hapke() {};

      void SetPhotoHg1(const double hg1);
      //! Return photometric Hg1 value
/*      inline double PhotoHg1() const {
        return p_photoHg1;
      };*/

      void SetPhotoHg2(const double hg2);
      //! Return photometric Hg2 value
/*      inline double PhotoHg2() const {
        return p_photoHg2;
      };*/

      void SetPhotoBh(const double bh);
      //! Return photometric Bh value
/*      inline double PhotoBh() const {
        return p_photoBh;
      };*/

      void SetPhotoCh(const double ch);
      //! Return photometric Ch value
/*      inline double PhotoCh() const {
        return p_photoCh;
      };*/

      void SetPhotoWh(const double wh);
      //! Return photometric Wh value
/*      inline double PhotoWh() const {
        return p_photoWh;
      };*/

      void SetPhotoHh(const double hh);
      //! Return photometric Hh value
/*      inline double PhotoHh() const {
        return p_photoHh;
      };*/

      void SetPhotoB0(const double b0);
      //! Return photometric B0 value
/*      inline double PhotoB0() const {
        return p_photoB0;
      };*/

      void SetPhotoTheta(const double theta);
      //! Return photometric Theta value
/*      inline double PhotoTheta() const {
        return p_photoTheta;
      };*/

      void SetOldTheta(double theta) {
        p_photoThetaold = theta;
      }

      void SetPhoto0B0Standard(const std::string &b0standard);
      //! Return photometric B0 standardization value
/*      inline std::string Photo0B0Standard() const {
        return p_photo0B0Standard;
      }*/

      //! Hapke's approximation to Chandra's H function
/*      inline double Hfunc(double u, double gamma) {
        return (1.0 + 2.0 * u) / (1.0 + 2.0 * u * gamma);
      }*/

      void SetStandardConditions(bool standard);

      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    protected:
      
    private:
/*      double p_photoHg1;
      double p_photoHg2;
      double p_photoBh;
      double p_photoCh;
      double p_photoCott;
      double p_photoCot2t;
      double p_photoTant;
      double p_photoSr;
      double p_photoOsr;
      IString p_algName;
      std::string p_photo0B0Standard;
      double p_photoWh;
      double p_photoHh;
      double p_photoB0;
      double p_photoB0save;
      double p_photoTheta;
      double p_photoThetaold;*/

  };
};

#endif

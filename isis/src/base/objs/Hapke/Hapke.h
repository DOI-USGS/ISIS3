#ifndef Hapke_h
#define Hapke_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2008-01-14 Janet Barret - Imported into ISIS from Isis2.
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
   *   @history 2016-08-28 Kelvin Rodriguez - SetPhoto0B0Standard now uses QString.
   *                           Part of porting to OS X 10.11
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


      void SetPhoto0B0Standard(const QString &b0standard);

      void SetStandardConditions(bool standard);

      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    protected:
    private:
  };
};

#endif

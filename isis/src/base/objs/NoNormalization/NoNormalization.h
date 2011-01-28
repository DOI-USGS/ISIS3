#if !defined(NoNormalization_h)
#define NoNormalization_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/18 18:10:58 $
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

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief NoNormalization - perform simple correction without normalization (a*dn +b)
   *
   * @author 2008-03-17 Janet Barrett
   *
   * @internal
   *   @history 2008-03-17 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *   @history 2010-05-18 Janet Barrett - Modified class so that it does nothing to
   *                       the incoming dn value. The outgoing albedo value will be
   *                       the same as the incoming dn value.
   *   @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                       from the ellipsoid or the DEM
   *   @history 2011-01-28 Janet Barrett - Fixed NormModelAlgorithm so that it applies 
   *                       the photometric correction to the incoming dn value
   *
   */
  class NoNormalization : public NormModel {
    public:
      NoNormalization(Pvl &pvl, PhotoModel &pmodel);
      virtual ~NoNormalization() {};

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);
  };
};

#endif

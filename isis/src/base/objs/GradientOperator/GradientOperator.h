#ifndef GradientOperator_h
#define GradientOperator_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/06/10 23:40:52 $
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

#include "InterestOperator.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief Gradient interest operator
   *
   * This class is used to construct a gradient interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * @see InterestOperator
   *
   * @author 2006-02-11 Jacob Danton
   *
   * @internal
   *   @history 2010-06-10 Sharmila Prasad  Updated to accomadate CnetValidMeasure base class
   *   @history 2010-11-10 Sharmila Prasad - Updated unittest to accomodate changes in the DefFile
   */
  class GradientOperator : public InterestOperator {
    public:
      GradientOperator(Pvl &pPvl) : InterestOperator(pPvl) {};
      virtual ~GradientOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif

#if !defined(StandardDeviationOperator_h)
#define StandardDeviationOperator_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/06/10 23:46:37 $
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
   * @brief Standard deviation interest operator
   *
   * This class is used to construct a standard deviation interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * @see InterestOperator
   *
   * @author 2006-02-11 Jacob Danton
   *
   * @internal
   *   @history 2006-02-11 Jacob Danton - Original Version
   *   @history 2007-08-01 Steven Koechle - Fixed error where
   *            Interest was compairing the uninitialized value of
   *            pixels[n] to see if it was a special pixel.
   *   @history 2007-08-02 Steven Koechle - Removed
	  *            CompareInterests virtual method.
	  *   @history 2010-06-10 Sharmila Prasad - Changes to accomodate CnetValidMeasure base class
   */
  class StandardDeviationOperator : public InterestOperator {
    public:
      StandardDeviationOperator(Pvl &pPvl) : InterestOperator(pPvl) {
        p_worstInterest = 0.0;
      };
      virtual ~StandardDeviationOperator() {};

    protected:
      virtual double Interest(Chip &chip);
  };
};

#endif

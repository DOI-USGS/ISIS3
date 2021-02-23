#ifndef MoravecOperator_h
#define MoravecOperator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "InterestOperator.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief Moravec Interest Operator
   *
   * This class is used to construct a Moravec interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   *
   * This is more commonly refered to as Moravec Corner Detection.
   * The algorithm was taken from the following website:
   * http://www.cim.mcgill.ca/~dparks/CornerDetector/mainMoravec.htm
   *
   * Moravec Interest Operator works by creating a N x N size
   * boxcar around the "point of interest." Then it walks another
   * boxcar of the same size around the center point, computing
   * the interest of that comparison by the equation: the sum of
   * pow(An-Bn,2) from 1 to the number of pixels in the boxcar,
   * where A1 and B1 are the top left pixel. By walking the boxcar
   * around the edges it will create 8 comparisons. It will then
   * take the smallest value out of the 8 comparisons and that
   * will be the interest value of the "point of interest."
   *
   * @see InterestOperator
   *
   * @author 2007-08-02 Steven Koechle
   *
   * @internal
   *   @history 2007-08-02 Steven Koechle - Original Version
   *   @history 2008-08-16 Steven Koechle - Fixed Documentation
   *   @history 2010-11-10 Sharmila Prasad - Updated unittest for changes in the deffile
   */
  class MoravecOperator : public InterestOperator {
    public:
      /**
       * This constructor creates a Moravec Interest Operator
       *
       * @param pvl Pvl to create MoravecOperator from
       */
      MoravecOperator(Pvl &pPvl) : InterestOperator(pPvl) {};
      /** This is the virtual destructor for MoravecOperator */
      virtual ~MoravecOperator() {};

    protected:
      virtual double Interest(Chip &chip);
      virtual int Padding();
  };
};

#endif

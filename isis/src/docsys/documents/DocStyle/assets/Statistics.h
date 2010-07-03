#ifndef Statistics_h
#define Statistics_h
/**
 * @file
 * $Date: 2006/10/31 23:18:12 $
 * $Revision: 1.1.1.1 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */


#include "SpecialPixel.h"

namespace Isis {
    /**
    * @brief This class is used to accumulate statistics on double arrays.
    *
    * This class is used to accumulate statistics on double arrays. In
    * particular, it is highly useful for obtaining statistics on cube data.
    * Parameters which can be computed are 1) @b average, 2) @b standard
    * @b deviation, 3) @b variance, 4) @b minimum, 5) @b maximum and 6)
    * @b various @b counts of valid and/or special pixels.
    *
    * The following example shows a simple set up and usage of the Statistics
    * class to calculate the average of a set of values:
    *
    * @code
    *   Statistics myStats ;
    *   double myData [] = { 1.0, 3.0, 2.4, 7.5 } ;
    *
    *   myStats.AddData (myData, 4) ;
    *   double myAverage = myStats.Average () ;
    *   cout << "The average of the data is " << myAverage << endl ;
    * @endcode
    *
    * For an example of how the Statistics object is used in %Isis, see the
    * Histogram object (inherits from Statistics) and the stats application,
    * stats.cpp (uses the Statistics child class Histogram).
    *
    * @ingroup Statistics
    *
    * @author Jeff Anderson - 2002-05-06
    *
    * @whatsnew 2002-05-06 The current sum and squared sum of the data can be retrieved
    *                      with the Sum and SumSquare methods.
    *
    * @internal
    * @history 2002-05-08 Jeff Anderson - Added Chebyshev and Best minimum/maximum methods.
    * @history 2004-05-11 Jeff Anderson - Moved Reset, AddData and RemoveData methods into public space.
    * @history 2004-06-28 Jeff Anderson - Added Sum and SumSquare methods.
    *
    * @todo 2005-02-07 Deborah Lee Soltesz - add example using cube data to the class documentation
    *
    */
  class Statistics {
    private:
      double p_sum;           //!< Sum accumulator.
      double p_sumsum;        //!< Sum-squared accumulator.
      double p_minimum;       //!< Minimum double value encountered.
      double p_maximum;       //!< Maximum double value encountered.
      double p_totalPixels;   //!< Count of total pixels processed.
      double p_validPixels;   //!< Count of valid pixels (non-special) processed.
      double p_nullPixels;    //!< Count of null pixels processed.
      double p_lrsPixels;     //!< Count of low instrument saturation pixels processed.
      double p_lisPixels;     //!< Count of low representation saturation pixels processed.
      double p_hrsPixels;     //!< Count of high instrument saturation pixels processed.
      double p_hisPixels;     //!< Count of high instrument representation pixels processed.
      bool   p_removedData;   /**< Indicates the RemoveData method was called which implies
                                   p_minimum and p_maximum are invalid. */

    public:
      Statistics ();
      ~Statistics ();

      void Reset ();
      void AddData (const double *data, const unsigned int count);
      void RemoveData (const double *data, const unsigned int count);

      double Average () const;
      double StandardDeviation () const;
      double Variance () const;

      double Minimum () const;
      double Maximum () const;
      double ChebyshevMinimum (const double percent=99.5) const;
      double ChebyshevMaximum (const double percent=99.5) const;
      double BestMinimum (const double percent=99.5) const;
      double BestMaximum (const double percent=99.5) const;

      double TotalPixels () const;
      double ValidPixels () const;
      double NullPixels () const;
      double LisPixels () const;
      double LrsPixels () const;
      double HisPixels () const;
      double HrsPixels () const;


      /**
       * Returns the sum of all the data
       *
       * @return (double) Sum of the data
       */
      double Sum() const { return p_sum; };

      /**
       * Returns the sum of all the squared data
       *
       * @return (double) Sum of the squared data
       */
      double SumSquare () const { return p_sumsum; };
  };
} // end namespace isis




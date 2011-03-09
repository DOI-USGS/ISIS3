#ifndef ControlMeasureLogData_h
#define ControlMeasureLogData_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/09/01 17:53:05 $
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

namespace Isis {
  class iString;
  class PBControlNetLogData_Point_Measure_DataEntry;
  class PvlKeyword;

  /**
   * @brief Statistical and similar ControlMeasure associated information
   *
   * This class represents information that is related to, or associated with,
   * a control measure but is not part of the measure itself.
   *
   * @author 2010-12-22 Steven Lambright
   *
   * @see ControlMeasure
   *
   * @internal
   *   @history 2010-12-22 Steven Lambright - Original version
   *   @history 2011-03-08 Eric Hyer - MaximumNumericLogDataType now makes sense
   */
  class ControlMeasureLogData {
    public:
      /**
       * Please do not change existing values in this list except the size -
       *   it will break backwards compadibility.
       *
       * This is the list of log data for control measures. To add a new
       *   element, put it in the list (anywhere), assign it a value one greater
       *   than the current maximum, and increase the maximum's value.
       *   Then add a case to ControlMeasureLogData::DataTypeToName.
       *   Once you've done that, you're done! :)
       */
      enum NumericLogDataType {
        /**
         * This is a placeholder for unset values.
         */
        InvalidNumericLogDataType = 0,

        /**
         * Eccentricity is pointreg information for sub-pixel registered
         * measures based on the algorithm chosen. This does not apply to
         * adaptive algorithms.
         *
         * When computing a registration to sub-pixel accuracy, the eccentricity
         * of the surface model can be compared to a tolerance to determine if
         * the registration succeeded or not.
         *
         * Eccentricity generally ranges between 0 and 1. A value of 0
         * represents a perfect circle, while a value of 1 represents a
         * parabola. Anything in between is an ellipse, while anything greater
         * than 1 can be considered a hyperbola.
         *
         * Eccentricity cannot always be computed, due to the nature of its
         * calculation, which takes square roots.
         * If the pointreg algorithm would take the square root of a negative
         * number, then it assumes the eccentricity to be 0 (a perfect circle)
         * under the belief that false positives are more acceptable than false
         * negatives.
         *
         * Eccentricity can be reported in any instance where sub-pixel accuracy
         * is computed (when the keyword "SubpixelAccuracy" in group "Algorithm"
         * is set to true in pointreg).
         */
        Eccentricity              = 1,

        /**
         * GoodnessOfFit is pointreg information for reference measures.
         *
         * This measures how well the computed fit area matches the pattern
         * area.
         */
        GoodnessOfFit             = 2,

        /**
         * Control measures store z-scores in pairs. A pair contains the
         * z-scores of the minimum and maximum pixels in the pattern chip
         * generated for the given measure during point registration. Each
         * z-score indicates how many standard deviations the given pixel value
         * is above or below the mean DN.
         */
        MinimumPixelZScore        = 3,

        /**
         * @see MinimumPixelZScore
         */
        MaximumPixelZScore        = 4,

        /**
         * Pointreg information indicating amount in pixels measure has shifted
         * from apriori.
         */
        PixelShift                = 5,
      };
      /**
       * This value must be > the largest enumerated value in this type or
       * convertions to and from Pvl will not work.
       */
      static const int MaximumNumericLogDataType = 6;
      

      ControlMeasureLogData();
      ControlMeasureLogData(NumericLogDataType);
      ControlMeasureLogData(PvlKeyword);
      ControlMeasureLogData(NumericLogDataType, double value);
      ControlMeasureLogData(
          const PBControlNetLogData_Point_Measure_DataEntry &);
      ControlMeasureLogData(const ControlMeasureLogData & other);
      ~ControlMeasureLogData();

      void SetNumericalValue(double);
      void SetDataType(NumericLogDataType);

      double GetNumericalValue() const;
      NumericLogDataType GetDataType() const;
      bool IsValid() const;
      PvlKeyword ToKeyword() const;

      PBControlNetLogData_Point_Measure_DataEntry ToProtocolBuffer() const;

      iString DataTypeToName(NumericLogDataType) const;

    private:
      void Init();

      //! Which kind of value are we storing
      NumericLogDataType p_dataType;
      //! The actual value of the data
      double p_numericalValue;
  };
}

#endif

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

class QVariant;

namespace Isis {
  class iString;
  class ControlNetLogDataProtoV0001_Point_Measure_DataEntry;
  class ControlPointFileEntryV0002_Measure_MeasureLogData;
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
   *   @history 2011-04-04 Steven Lambright - Added error checking to the
   *                         conversion to protocol buffer
   *   @history 2011-04-11 Steven Lambright - Added GetValue method
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
         * Deprecated keyword kept for backwards compatability with older
         * Control Networks.  DO NOT USE!!
         */
        Obsolete_Eccentricity              = 1,

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
         * Deprecated keyword kept for backwards compatability with older
         * Control Networks.  DO NOT USE!!
         */
        PixelShift                = 5,

        /**
         * Deprecated keyword kept for backwards compatability with older
         * Control Networks.  DO NOT USE!!
         */
        WholePixelCorrelation       = 6,

        /**
         * Deprecated keyword kept for backwards compatability with older
         * Control Networks.  DO NOT USE!!
         */
        SubPixelCorrelation       = 7,

        /**
         * Deprecated keyword kept for backwards compatability with older
         * Control Networks.  DO NOT USE!!
         */
        Obsolete_AverageResidual           = 8
      };
      /**
       * This value must be > the largest enumerated value in this type or
       * convertions to and from Pvl will not work.
       */
      static const int MaximumNumericLogDataType = 9;


      ControlMeasureLogData();
      ControlMeasureLogData(NumericLogDataType);
      ControlMeasureLogData(PvlKeyword);
      ControlMeasureLogData(NumericLogDataType, double value);
      ControlMeasureLogData(
          const ControlNetLogDataProtoV0001_Point_Measure_DataEntry &);
      ControlMeasureLogData(
          const ControlPointFileEntryV0002_Measure_MeasureLogData &);
      ControlMeasureLogData(const ControlMeasureLogData & other);
      ~ControlMeasureLogData();

      void SetNumericalValue(double);
      void SetDataType(NumericLogDataType);

      double GetNumericalValue() const;
      NumericLogDataType GetDataType() const;
      QVariant GetValue() const;
      bool IsValid() const;
      PvlKeyword ToKeyword() const;

      ControlPointFileEntryV0002_Measure_MeasureLogData
          ToProtocolBuffer() const;

      NumericLogDataType NameToDataType(iString name) const;
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

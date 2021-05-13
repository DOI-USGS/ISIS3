#ifndef ControlMeasureLogData_h
#define ControlMeasureLogData_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

class QString;
class QVariant;

namespace Isis {
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
   *   @history 2017-12-21 Adam Goins - Removed protobuf references.
   *   @history 2018-01-04 Adam Goins - Added variable names to method declarations.
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
      ControlMeasureLogData(NumericLogDataType dataType);
      ControlMeasureLogData(PvlKeyword keywordRep);
      ControlMeasureLogData(NumericLogDataType dataType, double value);
      ControlMeasureLogData(const ControlMeasureLogData & other);
      ~ControlMeasureLogData();

      void SetNumericalValue(double value);
      void SetDataType(NumericLogDataType newDataType);

      double GetNumericalValue() const;
      NumericLogDataType GetDataType() const;
      QVariant GetValue() const;
      bool IsValid() const;
      PvlKeyword ToKeyword() const;

      NumericLogDataType NameToDataType(QString name) const;
      QString DataTypeToName(NumericLogDataType type) const;

    private:
      void Init();

      //! Which kind of value are we storing
      NumericLogDataType p_dataType;
      //! The actual value of the data
      double p_numericalValue;
  };
}

#endif

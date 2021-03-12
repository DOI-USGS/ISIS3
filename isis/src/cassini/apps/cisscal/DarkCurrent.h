#ifndef DARKCURRENT_H
#define DARKCURRENT_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>
#include <QMap>
#include <vector>
#include "FileName.h"

namespace Isis {
  class CissLabels;
  /**
   * @brief Compute Cassini ISS dark current subtraction
   *
   * This class was created in order to perform necessary
   * calculations for computing the two-dimensional dark current
   * array to be subtracted from Cassini ISS images during
   * calibration using the <B>Isis cisscal</B> application.
   *
   * @ingroup Cassini
   * @author 2008-11-05 Jeannie Walldren
   * @internal
   *  @history 2008-11-05 Jeannie Walldren - Original Version
   *  @history 2009-01-26 Jeannie Walldren - Changed declarations of 2
   *           dimensional std::vectors
   *  @history 2009-05-27 Jeannie Walldren - Added p_flightSoftware variable.
   *                         Updated ComputeLineTime() code with algorithm from
   *                         the new version of linetime.pro in idl cisscal 3.6.
   *                         Fixed instrument data rate value in the
   *                         constructor.
   *  @history 2010-07-19 Jeannie Walldren - Fixed formatting.
   *  @history 2016-08-28 Kelvin Rodriguez - Removed usused private member variables to
   *           eliminate unused member variables warnings in clang. Part of porting to OS X 10.11.
   *  @history 2017-08-22 Cole Neubauer - Updated DarkCurrent for latest Cisscal upgrade
   */
  class DarkCurrent {
    public:
      // implied open file
      DarkCurrent(CissLabels &cissLab);
      ~DarkCurrent() {};  //!< Empty Destructor

      std::vector <std::vector <double> > ComputeDarkDN();
      //! Retrieves the name of the bias distortion table
      FileName BiasDistortionTable() {
        return p_bdpath;
      };
      //! Retrieves the name of the dark parameters file
      FileName DarkParameterFile()  {
        return p_dparamfile;
      };
      /**
       * @brief Compute IDL LinearInterpolation
       *
       * This class was created in order to mimic IDL's Linear Interpol method
       *
       * @author 2017-08-24 Cole Neubauer
       *
       * @internal
       *  @history 2017-08-22 Cole Neubauer - Original Version
       */
      class IDLLinearInterpolation : public QMap<double,double> {
        public:
          double evaluate( const double input) const;
      };
    private:
      double ComputeLineTime(int lline);
      void   FindDarkFiles();
      void   ComputeTimeArrays();
      std::vector <std::vector <double> > MakeDarkArray();
      std::vector <std::vector <double> > MakeManyLineDark(Brick &darkBrick);


      int p_lines;            //!< Number of lines in the image.
      int p_samples;          //!< Number of samples in the image.
      FileName p_bdpath;      //!< Bias distortion table for the image.  Only exists for narrow camera images.
      FileName p_dparamfile;  //!< Dark parameters file for the image.
      FileName p_hotpixfile;  //!< Erroneously bright hotpixels to encorporate into Dark Parameters.

      //LABEL VARIABLES
      int p_btsm;                   //!< Value dependent upon <b>PvlKeyword</b> DelayedReadoutFlag. Valid values are: "No"=0, "Yes"=1, "Unknown"=-1.  Called "botsim" or "btsm" in IDL code.
      double p_compRatio;           //!< Value of <b>PvlKeyword</b> CompressionRatio from the labels of the image.  Called "ratio" in IDL code.
      QString p_compType;            //!< Value of <b>PvlKeyword</b> CompressionType from the labels of the image.  Called "comp" in IDL code.
      QString p_dataConvType;        //!< Value of <b>PvlKeyword</b> DataConversionType from the labels of the image.  Called "conv" in IDL code.
      double p_expDur;              //!< Value of <b>PvlKeyword</b> ExposureDuration from the labels of the image.  Called "exposure" or "time" in IDL code.
      QString p_flightSoftware;     //!< Value of <b>PvlKeyword</b> FlightSoftwareVersion from the labels of the image.  Called "fsw" in IDL code.
      int p_gainMode;               //!< Value of <b>PvlKeyword</b> GainModeId from the labels of the image.
      bool p_narrow;                //!< Indicates whether the image is from a narrow-angle camera
      int p_readoutIndex;           //!< Value of <b>PvlKeyword</b> InstrumentDataRate from the labels of the image.  Called "rdind" or "roindex" in IDL code.
      int p_readoutOrder;           //!< Value of <b>PvlKeyword</b> ReadoutOrder from the labels of the image. Valid values are: NAC first = 0, WAC first = 1.  Called "roo" in IDL code.
      QString p_sum;                //!< Summing mode, as found in the labels of the image.  This integer is created as an QString so that it may be added to a QString.  Called "sum" in IDL code.
      int p_telemetryRate;          //!< Telemetry rate of the image in packets per second.  This is dependent on the range of the instrument data rate.  Called "cdsr" in IDL code.
      QString p_imageTime;          //!<Actual Time Stamp On Photo.
      std::vector <std::vector <double> > p_startTime;    //!< Array of start times for each pixel of the image.
      std::vector <std::vector <double> > p_endTime;      //!< Array of end times for each pixel of the image.
      std::vector <std::vector <double> > p_duration;     //!< Array of durations for each pixel of the image.
  };
};
#endif

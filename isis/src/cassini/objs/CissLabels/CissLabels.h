#ifndef CISSLABELS_H
#define CISSLABELS_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>

#include <QString>

namespace Isis {
  class Pvl;
  /**
   * @brief Read values from Cassini ISS labels
   *
   * This class is designed to be used with images imported from
   * Cassini ISS using @b ciss2isis.  It contains accessor methods
   * for the values of Keywords in the Instrument Group of the
   * cube's labels.
   *
   * @ingroup Cassini-Huygens
   * @author 2008-11-05 Jeannie Walldren
   * @internal
   *  @history 2008-11-05 Jeannie Walldren - Original Version
   *  @history 2008-11-07 Jeannie Walldren - Fixed documentation.
   *  @history 2011-05-03 Jeannie Walldren - Fixed documentation.
   *  @history 2017-08-22 Cole Neubauer - Added labels for latest Cisscal upgrade
   *  @history 2019-08-14 Kaitlyn Lee - Added ShutterStateId.
   */

  class CissLabels {
    public:
      CissLabels(Pvl &lab);
      CissLabels(const QString &file);
      //! Destroys the CissLabels object
      ~CissLabels() {};


      /**
       * @brief Returns whether InstrumentId from the Instrument
       *   group is "ISSNA".
       *
       *   Indicates whether the camera used a narrow-angle lens
       *   ("ISSNA"). False implies it used a wide-angle lens
       *   ("ISSWA").
       *
       *   @returns @b bool True if instrument ISSNA
       */
      inline bool            NarrowAngle()           const {
        return p_cissNA;
      };


      /**
       * @brief Returns whether InstrumentId from the Instrument
       *   group is "ISSWA".
       *
       *   Indicates whether the camera used a wide-angle lens
       *   ("ISSWA"). False implies it used a narrow-angle lens
       *   ("ISSNA").
       *
       *   @returns @b bool True if instrument ISSWA
       */
      inline bool            WideAngle()           const {
        return !p_cissNA;
      };


      /**
       * @brief Returns BiasStripMean from the Intstrument group.
       *
       *  Finds the mean of the overclocked pixels.  If the image has
       *  DataConversionType of "Lossy", the bias strip mean is not
       *  valid unless the Flight Software Version is 1.4. Valid
       *  values include all real numbers.
       *
       *  @returns @b double BiasStripMean
       */
      inline double          BiasStripMean()         const {
        return p_biasStripMean;
      };


      /**
       * @brief Returns CompressionRatio from the Instrument group.
       *
       *  Finds the ratio of the expected image size to the actual
       *  size.  Valid values include any real number or
       *  "NotCompressed". This method returns an
       *  @b QString so that values other than
       *  "NotCompressed" may be converted to @b double.
       *
       *  @returns @b QString CompressionRatio
       */
      inline QString         CompressionRatio()      const {
        return p_compressionRatio;
      };


      /**
       * @brief Returns CompressionType from the Instrument group.
       *
       *  Finds the method of data compression used for the image.
       *   Valid values include "NotCompressed",
       *  "Lossless" (a.k.a Rice), or "Lossy" (a.k.a. Discrete Cosine
       *  Transform).
       *
       *   @returns @b QString CompressionType
       */
      inline QString         CompressionType()       const {
        return p_compressionType;
      };


      /**
       *  @brief Returns DataConversionType from the Instrument group.
       *
       *  Finds the method used to convert the image from 12 to 8
       *  bits.  Valid values include "12Bit" (no conversion),
       *  "Table" (converted using look-up table), or "8LSB" (kept the
       *  8 least significant bits only).
       *
       *  @returns @b QString DataConversionType
       */
      inline QString         DataConversionType()    const {
        return p_dataConversionType;
      };


      /**
       * @brief Returns DelayedReadoutFlag from the Instrument group.
       *
       *  Indicates whether the image waited while the ther camera was
       *  performing a readout.  Valid values include "Yes", "No", or
       *  "Unknown".
       *
       *  @returns @b QString DelayedReadoutFlag
       */
      inline QString         DelayedReadoutFlag()    const {
        return p_delayedReadoutFlag;
      };


      /**
       * @brief Returns ExposureDuration from the Instrument group.
       *
       *  Finds the exposure duration in milliseconds for the image.
       *  There are 62 valid values between 0 and 1200000, or -999.0
       *  (if data is unavailable).
       *
       *  @returns @b double ExposureDuration
       */
      inline double          ExposureDuration()      const {
        return p_exposureDuration;
      };


      /**
       * @brief Returns a two-element array of the optical filters
       *  found in the BandBin group.
       *
       *  Takes FilterName string from the BandBin group and splits
       *  the combination into a vector of filter names.\n
       *  Valid combinations include:
       *  <UL>
       *   <LI>For narrow-angle cameras:
       *   <UL>
       *     <LI> Filter 1: "CL1", "RED", "BL1", "UV2", "UV1", "IRP0",
       *     "P120", "P60", "P0", "HAL", "IR4", "IR2"
       *     <LI> Filter 2: "CL2", "GRN", "UV3", "BL2", "MT2", "CB2",
       *     "MT3", "CB3", "MT1", "CB1", "IR3", "IR1"
       *   </UL>
       *   <LI>For wide-angle cameras:
       *   <UL>
       *     <LI> Filter 1: "CL1", "IR3", "IR4", "IR5", "CB3", "MT3",
       *     "CB2", "MT2", "IR2"
       *     <LI> Filter 2: "CL2", "RED", "GRN", "BL1", "VIO", "HAL",
       *     "IRP90", "IRP0", "IR1"
       *   </UL>
       * </UL>
       *
       *  @returns @b vector @b \<QString\> The optical filter names.
       */
      std::vector <QString>       FilterName()              const {
        return p_filter;
      };


      /**
       * @brief Returns a two-element array of indices associated with
       *  optical filter names.
       *
       *  Takes FilterName string from the BandBin group and assigns
       *  an index for each filter. Valid values are between 0 and 35.
       *  This method is not called for wide-angle cameras.
       *
       *  @returns @b vector @b \<int\> The filter indices.
       */
      std::vector <int>           FilterIndex()             const {
        return p_filterIndex;
      };


      /**
       * @brief Returns FlightSoftwareVersion from the Instrument
       *  group.
       *
       *  Retrieves the flight software version used for this image.
       *  Valid values include 1.2, 1.3, 1.4, or "Unknown". This
       *  method returns an @b QString so that any value other
       *  than "Unknown" may be converted to @b double.
       *
       *  @returns @b QString FlightSoftwareVersion
       */
      inline QString         FlightSoftwareVersion() const {
        return p_flightSoftwareVersion;
      };


      /**
       * @brief Returns the first element of OpticsTemperature from
       *  the Instrument group.
       *
       *  Retrieves the first value of the two-element array
       *  containing front and rear optics temperatures.  Valid values
       *  are greater than -999.0 degrees Celcius.
       *
       *  @returns @b double OpticsTemperature[0]
       */
      inline double          FrontOpticsTemp()       const {
        return p_frontOpticsTemp;
      };


      /**
       * @brief Returns GainModeId from the Instrument group.
       *
       *  Finds the electronics gain setting in electrons per DN.
       *  Valid values include 12, 29, 95, or 215. These values correspond
       *  to GainState 3, 2, 1, and 0, respectively.
       *
       *  @returns @b int GainModeId
       *  @see GainState()
       */
      inline int             GainModeId()            const {
        return p_gainModeId;
      };


      /**
       * @brief Returns GainState from the Instrument group.
       *
       *  Finds the gain state, which is dependent on the Gain Mode ID.
       *  Valid values include 0, 1, 2, or 3.  These values correspond
       *  to GainModeId 215, 95, 29, and 12, respectively.
       *
       *  @returns @b int GainState
       *  @see GainModeId()
       */
      inline int             GainState()            const {
        return p_gainState;
      };


      /**
       * @brief Returns ImageNumber from the Archive group.
       *
       *  Finds the number of seconds on the clock at shutter close.
       *  Valid values include real numbers.
       *
       *  @returns @b double ImageNumber
       */
      inline double          ImageNumber()             const {
        return p_imageNumber;
      };


      /**
       * @brief Returns InstrumentDataRate from the Instrument group.
       *
       *  Finds the rate at which data was transferred out, in
       *  kilobits per second.  Valid values include 60.9, 121.9,
       *  182.8, 243.7, 304.6, 365.6, or -999.0 (if data is
       *  unavailable).
       *
       *  @returns @b double InstrumentDataRate
       */
      inline double          InstrumentDataRate()    const {
        return p_instrumentDataRate;
      };


      /**
       * @brief Returns InstrumentId from the Instrument group.
       *
       *  Finds the type of camera used.  Valid values include "ISSNA"
       *  (also called "NAC" or "narrow-angle") or "ISSWA" (also
       *  called "WAC" or "wide-angle").
       *
       *  @returns @b QString InstrumentId
       */
      inline QString         InstrumentId()          const {
        return p_instrumentId;
      };


      /**
       * @brief Returns the lower case form of InstrumentModeId from
       *  the Instrument group.
       *
       *  Finds the summation mode used for this image. All images
       *  have 1 band and an equal number of lines and samples.  Valid
       *  values include "full" (1024x1024), "sum2" (512x512), or
       *  "sum4" (256x256). These values correspond to SummingMode 1,
       *  2, and 4, respectively.
       *
       *  @returns @b double lower-cased InstrumentModeId
       *  @see SummingMode()
       */
      inline QString         InstrumentModeId()      {
        return p_instrumentModeId.toLower();
      };


      /**
       * @brief Returns ReadoutCycleIndex from the Instrument group.
       *
       *  Finds the index associated with the image in the Readout
       *  Cycle table.  Valid values are "Unknown" or in the range
       *  0-15. This method returns an @b QString so that any
       *  value not equal to "Unknown" may be converted to
       *  @b int.
       *
       *  @returns @b QString ReadoutCycleIndex
       */
      inline QString         ReadoutCycleIndex()     const {
        return p_readoutCycleIndex;
      };


      /**
       * @brief Returns ReadoutOrder from the Instrument group.
       *
       *  Finds the integer value representing the readout order of
       *  the image. Valid values are
       *  <UL>
       *    <LI>0 : indicates narrow-angle was read out first
       *    <LI>1 : indicates wide-angle was read out first
       * </UL>
       *
       *  @returns @b int ReadoutOrder
       */
      inline int             ReadoutOrder()     const {
        return p_readoutOrder;
      };


      /**
       * @brief Returns ShutterModeId from the Instrument group.
       *
       *  Indicates whether the exposure was part of a joint
       *  observation with the other ISS camera. Valid values include
       *  "BothSim", "NacOnly", "WacOnly",
       *  or "Unknown".
       *
       *  @returns @b QString ShutterModeId
       */
      inline QString         ShutterModeId()         const {
        return p_shutterModeId;
      };


      /**
       * @brief Returns ShutterStateId from the Instrument group.
       *
       *  Indicates whether the shutter was enabled during image
       *  exposure. Valid values include "Disabled" and "Enabled".
       *
       *  @returns @b QString ShutterStateId
       */
      inline QString         ShutterStateId()         const {
        return p_shutterStateId;
      };


      /**
       * @brief Returns SummingMode from the Instrument group.
       *
       *  Finds the summation mode, which is dependent on the
       *  Instrument Mode ID. Valid values include 1, 2, or 4. These
       *  values correspond to InstrumentModeId "Full", "Sum2", and
       *  "Sum4", respectively.
       *
       *  @returns @b int SummingMode
       *  @see InstrumentModeId()
       */
      inline int             SummingMode()           const {
        return p_summingMode;
      };


      /**
       * @brief Returns whether AntiBloomingFlag from the Instrument
       *  group is "On".
       *
       *  Indicates whether anti-blooming was used for the image.
       *  False implies the anti-blooming flag is "Off" or "Unknown".
       *
       *  @returns @b bool True if AntiBloomingStateFlag "On"
       */
      inline bool            AntibloomingOn()        const {
        return p_antiblooming;
      };


      /**
       * @brief Returns the TargetName label as a QString.
       *
       *  Returns the TargetName from the labels as a QString.
       *  The returned value is the lower case name of subject of the image.
       *
       *  @returns @b QString TargetName
       */
       inline QString          TargetName()                const {
         return p_targetName.toLower();
       };


       /**
        * @brief Returns the Time the photo was taken as a QString.
        *
        *  Returns the ImageTime from the labels as a QString.
        *  The returned value is the lower case name of subject of the image.
        *
        *  @returns @b QString TargetName
        */
        inline QString          ImageTime()                const {
          return p_imageTime.toLower();
        };

    private:
      void Init(Pvl &lab);
      void ReadLabels(Pvl &lab);
      void ComputeImgProperties();


      //! Value of the PDS keyword AntiBloomingFlag in the cube's labels
      QString p_ABflag;
      //! Indicates whether anti-blooming state flag on
      bool p_antiblooming;
      //! Value of the PDS keyword BiasStripMean in the cube's labels
      double p_biasStripMean;
      //! Indicates whether camera is narrow-angle
      bool p_cissNA;
      //! Value of the PDS keyword CompressionRatio in the cube's labels
      QString p_compressionRatio;
      //! Value of the PDS keyword CompressionType in the cube's labels
      QString p_compressionType;
      //! Value of the PDS keyword DataConversionType in the cube's labels
      QString p_dataConversionType;
      //! Value of the PDS keyword DelayedReadoutFlag in the cube's labels
      QString p_delayedReadoutFlag;
      //! Value of the PDS keyword ExposureDuration in the cube's labels
      double p_exposureDuration;
      //! Two-element array of optical filters used for this image
      std::vector <QString> p_filter;
      //! Two-element array of filter indices corresponding to optical filters
      std::vector <int> p_filterIndex;
      //! Value of the PDS keyword FlightSoftwareVersion in the cube's labels
      QString p_flightSoftwareVersion;
      //! Value of the PDS keyword OpticsTemperature[0] in the cube's labels
      double p_frontOpticsTemp;
      //! Value of the PDS keyword GainModeId in the cube's labels
      int p_gainModeId;
      //! Value of the PDS keyword GainState in the cube's labels
      int p_gainState;
      //! Value of the PDS keyword ImageNumber in the cube's labels
      double p_imageNumber;
      //! Value of the PDS keyword ImageTime in the cube's labels
      double p_instrumentDataRate;
      //! Value of the PDS keyword InstrumentId in the cube's labels
      QString p_instrumentId;
      //! Value of the PDS keyword InstrumentModeId in the cube's labels
      QString p_instrumentModeId;
      //! Value of the PDS keyword ReadoutCycleIndex in the cube's labels
      QString p_readoutCycleIndex;
      //! Value of the PDS keyword ReadoutOrder in the cube's labels
      int p_readoutOrder;
      //! Value of the PDS keyword ShutterModeId in the cube's labels
      QString p_shutterModeId;
      //! Value of the PDS keyword ShutterState in the cube's labels
      QString p_shutterStateId;
      //! Value of the PDS keyword SummingMode in the cube's labels
      int p_summingMode;
      //! Value of the PDS keyword TargetName in the cube's labels
      QString p_targetName;
      //! Value of the PDS Keyword ImageTime in the cube's labels
      QString p_imageTime;
  };
};

#endif

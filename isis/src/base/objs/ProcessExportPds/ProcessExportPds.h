#ifndef ProcessExportPds_h
#define ProcessExportPds_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProcessExport.h"
#include <vector>

class QString;

namespace Isis {
  class PvlFormatPds;
  class Table;

  /**
   * @brief Process class for exporting cubes to PDS standards
   *
   * This class extends the ProcessExport class to allow the user
   * to export cubes to PDS format.
   *
   * Tables from the cube may also be exported. These exported PDS tables may be
   * attached or detached. This should correspond to whether the labels of the
   * exported PDS file are attached or detached. NOTE: If attached, the labels
   * of the table should not be altered in the export program unless
   * functionality is added to deal with the new start byte values for the
   * tables. 
   *  
   * @ingroup HighLevelCubeIO
   *
   * @author 2006-09-05 Stuart Sides
   *
   * @internal
   *   @history 2006-09-05 Stuart Sides - Original version
   *   @history 2006-12-14 Stuart Sides - Modified keword units to be PDS
   *                           compliant
   *   @history 2008-05-20 Steven Lambright - Fixed documentation
   *   @history 2008-08-07 Christopher Austin - Added fixed label export
   *                           capability
   *   @history 2008-10-02 Christopher Austin - Fixed LabelSize() and
   *                           OutputLabel() in accordace to the pds end of line
   *                           sequence requirement
   *   @history 2008-12-17 Steven Lambright - Added calculations for OFFSET and
   *                           SCALEFACTOR keywords
   *   @history 2009-05-31 Kris Becker - Included the number of bands in the
   *                           computation of the number of FILE_RECORDS for
   *                           fixed PDS type products.  It assumed only 1 band.
   *   @history 2010-02-24 Janet Barrett - Added code to support JPEG2000.
   *   @history 2010-07-21 Sharmila Prasad - Fixed error while converting
   *                           resolution from Meters to Kilometers
   *   @history 2012-04-06 Kris Becker - Correct label padding whereby spaces
   *                           are used as the fill character instead of '\0'.
   *                           This makes it compliant with PDS specifications.
   *   @history 2012-11-21 Jeannie Backer - Added functionality to allow export
   *                           of Isis3 Table objects to binary PDS tables. The
   *                           PDS tables may be attached or detached. If
   *                           attached, the labels of the table should not be
   *                           altered in the export program unless
   *                           functionality is added to deal with the new start
   *                           byte values. References #678.
   *   @history 2014-06-06 Kristin Berry - Added default units to assume if there
   *                           are no units on certain values in the input Isis cube.
   *                           Unlabeled radii are assumed to be in meters; map scales
   *                           to be in meters/pixel, and map resolutions to be in
   *                           pixels/degree.
   *   @history 2017-05-17 Makayla Shepherd & Ian Humphrey - Added updateChecksumInLabel() to
   *                           convert the placeholder value to the actual generated checksum
   *                           value. Modified StreamImageRoot() and FixedImageRoot() to create
   *                           CHECKSUM placeholder in the labels if we are generating a checksum.
   *                           Fixes #1013.
   *   @history 2017-10-18 Makayla Shepard - Removed code associated with completely empty unused
   *                           file pdsExportAllMapping.trn. References #5202. 
   */

  class ProcessExportPds : public Isis::ProcessExport {
    public:
      /**
       * File type to be exported
       * @see http://pds.nasa.gov/documents/sr/AppendixA.pdf
       */
      enum PdsFileType   {
        Image,        /**< Two dimensional array of line/sample values. These
                           files generallly have the extension *.img or *.imq**/
        Qube,         /**< Multi-dimensional array (1-3 dimensional) whose axes
                           may be interpreted as line/sample/band.  These files
                           generally have the extension *.qub**/
        SpectralQube, /**< Three dimensional objects with two spatial dimensions
                           and one spectral dimension. These files generally
                           have the extension *.qub**/
        JP2Image      /**< Image coding system JPEG 2000 formatted image. These
                           files generally have the extension *.jp2 **/
      };

      /**
       * Resolution units per pixel of the exported PDS file
       */
      enum PdsResolution {
        Meter,    //!< Meters per pixel
        Kilometer //!< Kilometers per pixel
      };

      /**
       * Record format type of exported PDS file.
       *
       * @see http://pds.nasa.gov/documents/sr/Chapter15.pdf
       */
      enum PdsExportType {
        Stream, //!< Stream Records. This type is generally used for ASCII files.
        Fixed   /**< Fixed length records. PDS recommends that FIXED_LENGTH
                     records are used whenever possible.**/
      };

      ProcessExportPds();
      ~ProcessExportPds();

      // Work with PDS labels for all data set types
      void StandardAllMapping(Pvl &mainPvl);

      // Work with PDS labels for data sets of type IMAGE
      void StreamImageRoot(Pvl &mainPvl);
      void FixedImageRoot(Pvl &mainPvl);
      void StreamJP2ImageRoot(Pvl &mainPvl);
      void FixedJP2ImageRoot(Pvl &mainPvl);
      void StandardImageImage(Pvl &mainPvl);
      void StandardJP2Image(Pvl &mainPvl);

      void SetPdsResolution(PdsResolution resolutionUnits);
      void SetExportType(PdsExportType recordFormat);

      virtual Pvl &StandardPdsLabel(ProcessExportPds::PdsFileType type);

      void OutputLabel(std::ofstream &pdsFileStream);
      void updateChecksumInLabel(std::ofstream &pdsFileStream);
      void OutputDetachedLabel();

      void ExportTable(Isis::Table isisTable, QString detachedPdsTableFileName="");

      // include this using declaration to indicate that ProcessExportPds
      // objects that call a StartProcess() method that has not been overridden
      // here should use the corresponding base class definitions
      using ProcessExport::StartProcess;
      void StartProcess(std::ofstream &fout);

      // Accessors
      bool Detached();
      bool Attached();

      // Mutators
      void SetDetached(QString detachedLabelFile);
      void SetAttached();
      void ForceBands(bool force);
      void ForceBandName(bool force);
      void ForceCenterFilterWavelength(bool force);
      void ForceBandwidth(bool force);
      void ForceBandStorageType(bool force);
      void ForceOffset(bool force);
      void ForceScalingFactor(bool force);
      void ForceSampleBits(bool force);
      void ForceSampleBitMask(bool force);
      void ForceSampleType(bool force);
      void ForceCoreNull(bool force);
      void ForceCoreLrs(bool force);
      void ForceCoreLis(bool force);
      void ForceCoreHrs(bool force);
      void ForceCoreHis(bool force);

    protected:
      int LineBytes();
      int LabelSize();
      virtual void CreateImageLabel();
      void CreateQubeLabel();
      void CreateSpectralQubeLabel();

      QString ProjectionName(Pvl &inputLabel);

      PvlFormatPds *m_formatter;  /**< Used to determine how to format the
                                       keyword values in the PDS file.*/
      Pvl *m_label;               //!< Exported PDS label.
      PdsExportType m_exportType; //!< Stream or Fixed

    private:
      PdsResolution m_exportResolution; //!< Meters or kilometers.
      bool m_forceBands;             /**< Indicates whether to keep the
                                          BANDS keyword in the PDS labels.*/
      bool m_forceBandName;          /**< Indicates whether to keep the
                                          BAND_NAME keyword in the PDS labels.*/
      bool m_forceCenterFilterWavelength; /**< Indicates whether to keep the
                                          CENTER_FILTER_WAVELENGTH keyword in
                                          the PDS labels.*/
      bool m_forceBandwidth;         /**< Indicates whether to keep the
                                          BANDWIDTH keyword in the PDS labels.*/
      bool m_forceBandStorageType;   /**< Indicates whether to add the
                                          BAND_STORAGE_TYPE keyword in the PDS
                                          labels.*/
      bool m_forceOffset;            /**< Indicates whether to add the
                                          OFFSET keyword in the PDS labels.*/
      bool m_forceScalingFactor;     /**< Indicates whether to add the
                                          SCALING_FACTOR keyword in the PDS
                                          labels.*/
      bool m_forceSampleBits;        /**< Indicates whether to add the
                                          SAMPLE_BITS keyword in the PDS labels.*/
      bool m_forceSampleBitMask;     /**< Indicates whether to add the
                                          SAMPLE_BIT_MASK keyword in the PDS
                                          labels.*/
      bool m_forceSampleType;        /**< Indicates whether to add the
                                          SAMPLE_TYPE keyword in the PDS
                                          labels.*/
      bool m_forceCoreNull;          /**< Indicates whether to add the
                                          CORE_NULL keyword in the PDS labels.*/
      bool m_forceCoreLrs;           /**< Indicates whether to add the
                                          CORE_LOW_REPR_SATURATION keyword in
                                          the PDS labels.*/
      bool m_forceCoreLis;           /**< Indicates whether to add the
                                          CORE_LOW_INSTR_SATURATION keyword in
                                           the PDS labels.*/
      bool m_forceCoreHrs;           /**< Indicates whether to add the
                                          CORE_HIGH_REPR_SATURATION keyword in
                                           the PDS labels.*/
      bool m_forceCoreHis;           /**< Indicates whether to add the
                                          CORE_HIGH_INSTR_SATURATION keyword in
                                          the PDS labels.*/
      bool m_detachedLabel;          /**< Indicates whether the PDS file
                                          will be detached.*/
      QString m_detachedPdsLabelFile;/**< The name of the detached PDS label
                                          file.*/
      PdsFileType m_pdsFileType;     /**< Image, Qube, Spectral Qube, or
                                          JP2 Image*/

      std::vector<int> m_tableStartRecord;/**< Record number where the added
                                           table data begins. The order of the
                                           tables represented in this vector
                                           corresponds to the order of the table
                                           data in m_tableBuffers**/
      std::vector<int> m_tableRecords; /**< Number of records in each added
                                           table. The order of the tables
                                           represented in this vector
                                           corresponds to the order of the table
                                           data in m_tableBuffers. **/
      std::vector<char *> m_tableBuffers; /**< Vector containing the binary
                                           table data for each of the added
                                           tables. The order of the tables
                                           represented in this vector
                                           corresponds to the order of the table
                                           data in m_tableRecords. **/
  };
}

#endif

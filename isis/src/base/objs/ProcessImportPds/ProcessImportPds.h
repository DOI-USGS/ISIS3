#ifndef ProcessImportPds_h
#define ProcessImportPds_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessImport.h"

#include <vector>

#include "Pvl.h"
#include "PvlGroup.h"
#include <QString>

namespace Isis {
  class PvlToPvlTranslationManager;
  class Table;
  /**
   * @brief Convert PDS archive files to Isis format
   *
   * This class is used to import PDS archive files into Isis format. The class
   * can handle PDS images, qubes and spectral cubes. It can also convert
   * projection information if it exists.
   *
   * Here is an example of how to use ProcessImportPds
   * @code
   *   ImportPds p;
   *   Pvl plab;
   *   p.SetPdsFile("imagefile.img", "", plab);
   *   p.SetOutputCube("TO");
   *   p.StartProcess();
   *   Pvl proj;
   *   p.TranslatePdsProjection (proj);
   *   p.AddLabel (proj);
   *   p.EndProcess();
   * @endcode
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2002-08-06 Tracie Sucharski
   *
   * @internal
   *  @history 2003-01-24 Tracie Sucharski - Fixed bug in processing 8bit data.
   *                          In the Swap method needed to return unsigned char,
   *                          not char.
   *  @history 2003-02-13 Stuart Sides - Added a unit test.
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                          isis.astrogeology...
   *  @history 2003-05-30 Stuart Sides - Fixed compiler error after -O1 flag was
   *                          added to g++
   *  @history 2003-09-10 Tracie Sucharski - Complete redesign to handle
   *                          different raw file formats.
   *  @history 2003-10-09 Stuart Sides - Added capabilities for reading PDS
   *                          files
   *  @history 2003-10-10 Stuart Sides - Added capabilities to get the
   *                          translation files from the user preferences BASE
   *                          directory.
   *  @history 2003-10-10 Stuart Sides - Fixed bug for PDS files. When the image
   *                          name was explicitly given the open statement was
   *                          attempting to open the label file.
   *  @history 2003-10-16 Stuart Sides - Added a section for debuging all the
   *                          parameters which can be set before processing
   *                          starts.
   *  @history 2003-10-16 Stuart Sides - Added a second parameter to the Pvl
   *                          constructor in SetVicarFile. This allows the vicar
   *                          label to be read into a Pvl without modifying the
   *                          repeated "PROPERTY" keyword.
   *  @history 2003-10-23 Stuart Sides - Added new member functions
   *                          "AddImportLabel()" and "AddImportLabel(Pvl).
   *                          AddImportLabel() uses the original label file to
   *                          create a Pvl and write it to the output cube label.
   *                          AddImportLabel(Pvl) uses the Pvl argument to write
   *                          the import label to the output cube.
   *  @history 2004-01-21 Jeff Anderson - Modified SetOutputCube method to
   *                          propagate the input pixel type, base, and
   *                          multipliers to the output cube. The old method
   *                          always generated real pixel values in the output.
   *  @history 2004-01-22 Jeff Anderson - Modified the SetVicarFile method to
   *                          return the vicar labels internalized in a PVL
   *                          object.
   *  @history 2004-02-04 Jeff Anderson - Modified SetPdsFile method to allow for
   *                          SPECTRAL_QUBE objects to handle Themis data.
   *  @history 2004-02-10 Stuart Sides - Separated PDS capabilities from Import.
   *  @history 2005-02-09 Elizabeth Ribelin - Modified file to support Doxygen
   *                          documentation
   *  @history 2006-06-13 Brendan George - Modified to add OriginalLabel blob
   *                          automatically, and added a function to allow the
   *                          user to prevent this.
   *  @history 2006-07-12 Stuart Sides - Modified the translation of projection
   *                          keywords such as CenterLongitude, so the would not
   *                          throw and error if the keywods was not a number.
   *                          This was necessary because map a planet creates
   *                          PDS labels with "N/A" in several keywords.
   *  @history 2006-10-26 Stuart Sides - Added unsigned 16 bit ability
   *  @history 2006-11-16 Brendan George - Changed instances of "Foreign" to
   *                          "Input" and "Native" to "Output"
   *  @history 2007-01-24 Stuart Sides - Added ability to identify the
   *                          difference between PDS and ISIS2 files, and
   *                          process them differently.
   *  @history 2007-02-08 Brendan George - Fixed TranslateIsis2Instrument to
   *                          remove the appended z in the StartTime keyword,
   *                          if present.
   *  @history 2007-04-09 Tracie Sucharski - Added GetProjectionOffsetMults
   *                          method which will find the correct multiplication
   *                          factors for the projection offsets depending on
   *                          the keyword and pattern in
   *                          pdsProjectionLineSampToXY.def. Added IsIsis2
   *                          method. Made changes to projection translation
   *                          tables for additional values for Longitude
   *                          direction, latitude type and if the min or max
   *                          longitude values is greater than 180, change
   *                          longitude domain to 360.
   *  @history 2007-05-31 Steven Koechle -Moddified to assume all signed bytes
   *                          input pixels are actually unsigned.
   *  @history 2007-06-04 Jeff Anderson - Modified to deal with projection
   *                          conversion generically
   *  @history 2007-08-07 Steven Lambright - Modified to support translating
   *                          some PDS labels for pds2isis and default longitude
   *                          domain is now 360.
   *  @history 2007-08-29 Steven Koechle - Modified to use new SetSpecialValues
   *                          method from ProcessImport
   *  @history 2007-10-16 Steven Koechle - Modified TranslatePdsProjection() to
   *                          not add the min & max lat long keywords if they
   *                          have null values.
   *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   *  @history 2008-06-13 Steven Lambright - Updated algorithm to decide domain
   *                          and calculate correct longitudes in
   *                          ExtractPdsProjection
   *  @history 2008-06-13 Noah Hild - Added support for the FILE object
   *  @history 2008-06-13 Steven Lambright - Updated ExtractPdsProjection to
   *                          change the latitude type to planetocentric if both
   *                          the planet radii are the same
   *  @history 2008-08-08 Steven Lambright - Fixed bug where the longitude
   *                          keywords could be incorrecty interpretted (minumum
   *                          longitude and maximum longitude are swapped).
   *  @history 2009-07-16 Steven Lambright - Fixed bug where the longitude is
   *                          misordered
   *  @history 2009-12-15 Steven Lambright - Hard-coded translation table is now
   *                          a valid Pvl
   *  @history 2009-12-18 Janet Barrett - Added capability to process JPEG2000
   *                          files with a detached PDS label
   *  @history 2010-08-23 Steven Lambright - Non-numeric string values for
   *                          PDS projection rotations are now supported.
   *  @history 2010-08-27 Steven Lambright - Longitude domain correction added
   *                          for center and pole longitude keywords in PDS
   *                          projections
   *  @history 2010-11-17 Eric Hyer - Inside ProcessPdsImageLabel method
   *                          Absolute paths are now supported for the ^IMAGE
   *                          Keyword
   *  @history 2010-12-09 Sharmila Prasad - Set default offset to be 1 for
   *                          detatched label and  offset not set
   *  @history 2011-02-11 Mackenzie Boyd - Added methods ProcessDataFilePointer,
   *                          ProcessPixelBitandType, and ProcessSpecialPixels,
   *                          removed duplicate code in ProcessImage and
   *                          ProcessQube labels. Fixed functionality regarding
   *                          ^QUBE pointer having no offest.
   *  @history 2011-04-27 Mackenzie Boyd - Changed ProcessQubeLabels to set BIP
   *                          for BANDSAMPLELINE axes instead of LINEBANDSAMPLE
   *                          and added exception for unknown storage order.
   *  @history 2012-05-03 Tracie Sucharski - Added a try/catch in SetPdsFile
   *                          when attempting to read PDS label
   *  @history 2012-11-21 Jeannie Backer - Added methods and member variables to
   *                          import binary PDS tables found in the PDS file.
   *                          Added a default destructor. References #700.
   *  @history 2013-03-12 Steven Lambright and Tracie Sucharski - Added ProcessPdsRdnLabel() to
   *                          handle Chandrayaan M3 RDN files.  Added a file type to handle
   *                          Chandrayaan Loc and Obs files on the same import as the Rdn files.
   *                          Also added support for 64 bit input data.
   *                          Note:  There may be loss of precision since the output type is 32-bit.
   *                          Return reference to imported table.  Needed so that M3 table data
   *                          can be flipped to match image data depending on yaw and orbit limb
   *                          direction.
   *  @history 2014-02-11 Janet Barrett - Created new version of SetPdsFile method
   *                          so that calling applications can intercept the PDS
   *                          label before it gets loaded by this class. This is
   *                          needed so that missing keywords can be added to the
   *                          PDS label by the application before it gets loaded.
   *                          This also required moving some of the code from the
   *                          SetPdsFile method to a new method, ProcessLabel. Fixes
   *                          #2036.
   *  @history 2014-03-19 Kristin Berry - Added a Finalize method
   *                          identical to the old EndProcess
   *                          method so that
   *                          ProcessImportPds::Finalize is
   *                          called, rather than
   *                          Process::Finalize on
   *                          ProcessImportPds objects. Marked
   *                          EndProcess as deprecated.
   *  @history 2015-01-19 Sasha Brownsberger - Made destructor virtual. References #2215.
   *  @history 2015-03-10 Tyler Wilson Added to unit test to test opening Galileo NIMS cube files.
   *                          References #2368.
   *  @history 2017-01-03 Jesse Mapel - Added support for importing combined spectrum
   *                          images such as from the Hyabusa NIRS. Fixes #4573.
   *  @history 2017-05-29 Kristin Berry - Update to the DataTrailer handling code so that its size
   *                          (DataTrailerBytes) is not inappropriately re-set if we have specified
   *                          it previously. References #3888.
   *
   *  @todo 2005-02-09 Finish documentation-lots of holes with variable
   *                          definitions in .h file and .cpp methods, and  insert
   *                          implementation example
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp: changed ReportError method to
   *                          truncate paths before data directory. Allows test to pass when not
   *                          using the default data area. Fixes #4738.
   *   @history 2017-06-26 Summer Stapleton - Added functions to identify and report changes to
   *                          the default projection offsets and multipliers. Fixes #4887.
   *   @history 2017-12-20 Summer Stapleton - Modified error message in
   *                          ProcessImportPds::ProcessLabel() to be more discriptive. Fixes #4883.
   *   @history 2018-01-19 Christopher Combs - Changed ProcessDataFilePointer call to reflect 
   *                          changes made to voy2isis. Fixes #4345, #4421.
   *
   */
  class ProcessImportPds : public ProcessImport {

    public:
      enum PdsFileType {
        Image = 1,
        Qube = 2,
        SpectralQube = 4,
        L0 = 8,
        Rdn = 16,
        Loc = 32,
        Obs = 64,
        CombinedSpectrum = 128,
        All = Image | Qube | SpectralQube | L0 | Rdn | Loc | Obs | CombinedSpectrum
      };
      ProcessImportPds();
      virtual ~ProcessImportPds();
      bool GetProjectionOffsetChange();
      PvlGroup GetProjectionOffsetGroup();
      void SetPdsFile(const QString &pdsLabelFile, const QString &pdsDataFile,
                      Pvl &pdsLabel, PdsFileType allowedTypes = All);
      void SetPdsFile(const Pvl &pdsLabelPvl, const QString &pdsDataFile,
                      PdsFileType allowedTypes = All);
      void ProcessLabel(const QString &pdsDataFile, PdsFileType allowedTypes);

      void TranslatePdsProjection(Pvl &lab);
      void TranslateIsis2Labels(Pvl &lab);
      void TranslatePdsLabels(Pvl &lab);

      bool IsIsis2();

      void OmitOriginalLabel();

      Table &ImportTable(QString pdsTableName);
      // since we are overriding StartProcess(), we must specify for other
      // overloaded calls to StartProcess(), the ProcessImport class method
      // definitions should be used.
      using ProcessImport::StartProcess;
      void StartProcess();
      void StartProcess(void funct(Isis::Buffer &out));
      void EndProcess();
      void Finalize();
    private:


      enum Source {
        NOSOURCE,
        PDS,
        ISIS2
      };

      enum EncodingType {
        NONE,
        JP2
      };

      void ProcessDataFilePointer(PvlToPvlTranslationManager & pdsXlater,
                                  const bool & calcOffsetOnly);
      void ProcessPixelBitandType(PvlToPvlTranslationManager & pdsXlater);
      void ProcessSpecialPixels(PvlToPvlTranslationManager & pdsXlater, const bool & isQube);

      void ProcessPdsImageLabel(const QString &pdsDataFile);
      void ProcessPdsQubeLabel(const QString &pdsDataFile, const QString &transFile);
      void ProcessPdsM3Label(const QString &pdsDataFile, PdsFileType fileType);
      void ProcessPdsCombinedSpectrumLabel(const QString &pdsDataFile);

      void ExtractPdsProjection(PvlToPvlTranslationManager &pdsXlater);
      void GetProjectionOffsetMults(double &xoff, double &yoff,
                                    double &xmult, double &ymult);

      void IdentifySource(Pvl &lab);

      void TranslateIsis2BandBin(Pvl &lab);
      void TranslateIsis2Instrument(Pvl &lab);
      void TranslatePdsBandBin(Pvl &lab);
      void TranslatePdsArchive(Pvl &lab);

      Pvl p_pdsLabel;            //!< Internalized PDS label
      QString p_labelFile;   //!< The filename where the PDS label came from

      QString p_transDir;        //!< Base data directory

      // Encoding info
      EncodingType p_encodingType;    /**< The encoding type of the image data.
                                           The only encoding type currently
                                            supported is JP2 (JPEG2000).*/
      QString p_jp2File;          /**< The name of the file containing the
                                           encoded JP2 data.*/

      // Projection info
      QString p_projection;       /**< The name of the projection found in
                                             the PDS projection labels*/

      bool p_projectionOffsetChange;    /**< Whether the projection offsets were updated upon
                                            loading*/

      PvlGroup p_projectionOffsetGroup;      /**< Log information for projection offsets*/

      QString p_targetName;       //!<
      double p_equatorialRadius;      /**< Equatorial radius found in the PDS
                                           projection labels*/
      double p_polarRadius;           /**< The polar radius found in the PDS
                                           projection labels*/
      QString p_longitudeDirection; /**< Longitude direction found in the
                                             PDS projection labels*/
      int p_longitudeDomain;            /**< Longitude domain found in the PDS
                                             projection labels*/
      QString p_latitudeType;       /**< The latitude type found in the PDS
                                             projection labels*/
      double p_minimumLatitude;         /**< Minimum latitude found in the PDS
                                             projection labels*/
      double p_maximumLatitude;         /**< Maximum latitude found in the PDS
                                             projection labels*/
      double p_minimumLongitude;        /**< Minimum longitude found in the PDS
                                             projection labels*/
      double p_maximumLongitude;        /**< Maximum longitude found in the PDS
                                             projection labels*/
      double p_pixelResolution;         /**< Pixel resolution found in the PDS
                                             projection labels*/
      double p_scaleFactor;             /**< The scale factor found in the PDS
                                             projection labels*/
      double p_rotation;                /**< The rotation found in the PDS
                                             labels*/
      double p_sampleProjectionOffset;   //!<
      double p_lineProjectionOffset;     //!<
      double p_upperLeftX;               //!<
      double p_upperLeftY;               //!<

      bool p_keepOriginalLabel;       /**< determines whether or not to keep the
                                           OriginalLabel blob.*/
      std::vector<Table> p_tables; /**< Vector of Isis Table objects that
                                        were imported from PDS and need to be
                                        added to the imported cube file. */

      Source p_source;
  };
};

#endif

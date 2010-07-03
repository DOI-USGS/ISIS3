#ifndef ProcessImportPds_h
#define ProcessImportPds_h
/**
 * @file
 * $Revision: 1.24 $
 * $Date: 2010/02/22 02:26:11 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ProcessImport.h"

namespace Isis {
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
  *                                         In the Swap method needed to return
  *                                         unsigned char, not char.
  *  @history 2003-02-13 Stuart Sides - Added a unit test.
  *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
  *                                     isis.astrogeology...
  *  @history 2003-05-30 Stuart Sides - Fixed compiler error after -O1 flag was
  *                                     added to g++
  *  @history 2003-09-10 Tracie Sucharski - Complete redesign to handle
  *                                         different raw file formats.
  *  @history 2003-10-09 Stuart Sides - Added capabilities for reading PDS files
  *  @history 2003-10-10 Stuart Sides - Added capabilities to get the
  *                                     translation files from the user
  *                                     preferences BASE directory.
  *  @history 2003-10-10 Stuart Sides - Fixed bug for PDS files. When the image
  *                                     name was explicitly given the open
  *                                     statement was attempting to open the
  *                                     label file.
  *  @history 2003-10-16 Stuart Sides - Added a section for debuging all the
  *                                     parameters which can be set before
  *                                     processing starts.
  *  @history 2003-10-16 Stuart Sides - Added a second parameter to the Pvl
  *                                     constructor in SetVicarFile. This allows
  *                                     the vicar label to be read into a Pvl
  *                                     without modifying the repeated
  *                                     "PROPERTY" keyword.
  *  @history 2003-10-23 Stuart Sides - Added new member functions
  *                                    "AddImportLabel()"and"AddImportLabel(Pvl).
  *                                     AddImportLabel() uses the original label
  *                                     file to create a Pvl and write it to the
  *                                     output cube label. AddImportLabel(Pvl)
  *                                     uses the Pvl argument to write the
  *                                     import label to the output cube.
  *  @history 2004-01-21 Jeff Anderson - Modified SetOutputCube method to
  *                                      propagate the input pixel type, base,
  *                                      and multipliers to the output cube. The
  *                                      old method always generated real pixel
  *                                      values in the output.
  *  @history 2004-01-22 Jeff Anderson - Modified the SetVicarFile method to
  *                                      return the vicar labels internalized
  *                                      in a PVL object.
  *  @history 2004-02-04 Jeff Anderson - Modified SetPdsFile method to allow for
  *                                      SPECTRAL_QUBE objects to handle Themis
  *                                      data.
  *  @history 2004-02-10 Stuart Sides - Separated PDS capabilities from Import.
  *  @history 2005-02-09 Elizabeth Ribelin - Modified file to support Doxygen
  *                                          documentation
  *  @history 2006-06-13 Brendan George - Modified to add OriginalLabel blob
  *                                       automatically, and added a function to
  *                                       allow the user to prevent this.
  *  @history 2006-07-12 Stuart Sides - Modified the translation of projection
  *                                     keywords such as CenterLongitude, so
  *                                     the would not throw and error if the
  *                                     keywods was not a number. This was
  *                                     necessary because map a planet creates
  *                                     PDS labels with "N/A" in several
  *                                     keywords.
  *
  *  @history 2006-10-26 Stuart Sides - Added unsigned 16 bit ability
  *
  *  @history 2006-11-16 Brendan George - Changed instances of "Foreign" to "Input"
  *                                       and "Native" to "Output"
  *
  *  @history 2007-01-24 Stuart Sides - Added ability to identify the difference
  *                                     between PDS and ISIS2 files, and process
  *                                     them differently.
  *
  *  @history 2007-02-08 Brendan George - Fixed TranslateIsis2Instrument to
  *                                       remove the appended z in the StartTime
  *                                       keyword, if present.
  *
  *  @history 2007-04-09 Tracie Sucharski - Added GetProjectionOffsetMults
  *                            method which will find the correct multiplication
  *                            factors for the projection offsets depending
  *                            on the keyword and pattern in
  *                            pdsProjectionLineSampToXY.def.
  *                            Added IsIsis2 method.
  *                            Made changes to projection translation tables for
  *                            additional values for Longitude direction,
  *                            latitude type and if the min or max longitude
  *                            values is greater than 180, change longitude
  *                            domain to 360.
  *
  *  @history 2007-05-31 Steven Koechle -Moddified to assume all signed bytes
  *                           input pixels are actually unsigned.
  *
  *  @history 2007-06-04 Jeff Anderson - Modified to deal with projection
  *                           conversion generically
  * 
  *  @history 2007-08-07 Steven Lambright - Modified to support translating some
  *                         PDS labels for pds2isis and default longitude domain is
  *                         now 360.
  * 
  *  @history 2007-08-29 Steven Koechle - Modified to use new SetSpecialValues method
  *                          from ProcessImport
  * 
  *  @history 2007-10-16 Steven Koechle - Modified TranslatePdsProjection() to
  *                          not add the min & max lat long keywords if they have
  *                          null values.
  *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo    
  *  @history 2008-06-13 Steven Lambright - Updated algorithm to decide domain and 
  *                          calculate correct longitudes in ExtractPdsProjection
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
  *  @todo 2005-02-09 Finish documentation-lots of holes with variable
  *                   definitions in .h file and .cpp methods, and  insert
  *                   implementation example
  */
  class ProcessImportPds : public ProcessImport {

    public:
      ProcessImportPds();
      void SetPdsFile (const std::string &pdsLabelFile, const std::string &pdsDataFile,
                       Isis::Pvl &pdsLabel);

      void TranslatePdsProjection (Isis::Pvl &lab);
      void TranslateIsis2Labels (Isis::Pvl &lab);
      void TranslatePdsLabels (Isis::Pvl &lab);

      bool IsIsis2();

      void OmitOriginalLabel();

      void EndProcess();

    private:

      enum Source { NOSOURCE, PDS, ISIS2 };
      enum EncodingType { NONE, JP2 };

      Isis::Pvl p_pdsLabel;      //!<Internalized PDS label
      std::string p_labelFile;   //!<The filename where the PDS label came from

      Isis::iString p_transDir;  //!Base data directory

      // Encoding info
      EncodingType p_encodingType;       /**<The encoding type of the image data.
                                            The only encoding type currently
                                            supported is JP2 (JPEG2000).*/
      std::string p_jp2File;             /**<The name of the file containing the
                                            encoded JP2 data.*/

      // Projection info
      std::string p_projection;          /**<The name of the projection found in
                                             the PDS projection labels*/
      std::string p_targetName;          //!<
      double p_equatorialRadius;         /**<Equatorial radius found in the PDS
                                             projection labels*/
      double p_polarRadius;              /**<The polar radius found in the PDS
                                             projection labels*/
      std::string p_longitudeDirection;  /**<Longitude direction found in the
                                             PDS projection labels*/
      int p_longitudeDomain;             /**<Longitude domain found in the PDS
                                             projection labels*/
      std::string p_latitudeType;        /**<The latitude type found in the PDS
                                             projection labels*/
      double p_minimumLatitude;          /**<Minimum latitude found in the PDS
                                             projection labels*/
      double p_maximumLatitude;          /**<Maximum latitude found in the PDS
                                             projection labels*/
      double p_minimumLongitude;         /**<Minimum longitude found in the PDS
                                             projection labels*/
      double p_maximumLongitude;         /**<Maximum longitude found in the PDS
                                             projection labels*/
      double p_pixelResolution;          /**<Pixel resolution found in the PDS
                                             projection labels*/
      double p_scaleFactor;              /**<The scale factor found in the PDS
                                             projection labels*/
      double p_rotation;                 /**<The rotation found in the PDS
                                            labels*/
      double p_sampleProjectionOffset;   //!<
      double p_lineProjectionOffset;     //!<
      double p_upperLeftX;               //!<
      double p_upperLeftY;               //!<

      bool p_keepOriginalLabel;           /**<determines whether or not to keep the
                                             OriginalLabel blob.*/

      Source p_source;

      void ProcessPdsImageLabel (const std::string &pdsDataFile);
      void ProcessPdsQubeLabel (const std::string &pdsDataFile, const std::string &transFile);

      void ExtractPdsProjection(Isis::PvlTranslationManager &pdsXlater);
      void GetProjectionOffsetMults (double &xoff, double &yoff,
                                     double &xmult,double &ymult);

      void IdentifySource (Isis::Pvl &lab);

      void TranslateIsis2BandBin (Isis::Pvl &lab);
      void TranslateIsis2Instrument (Isis::Pvl &lab);
      void TranslatePdsBandBin (Isis::Pvl &lab);
      void TranslatePdsArchive (Isis::Pvl &lab);

  };
};

#endif

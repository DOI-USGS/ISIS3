/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/06/21 18:39:22 $
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
#include "Preference.h"

#include <QDebug>

#include "Application.h"
#include "IException.h"
#include "IString.h"
#include "Portal.h"
#include "ProcessMosaic.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "SpecialPixel.h"
#include "Table.h"

using namespace std;

namespace Isis {
  /**
   * This is the name of the table in the Cube which will contain the image file names.
   */
  const char *ProcessMosaic::TRACKING_TABLE_NAME = "InputImages";

  /**
   * ProcessMosaic Contructor
   *
   * Initialize the class members to default
   */
  ProcessMosaic::ProcessMosaic() {
    // Set the BandBin Match
    SetBandBinMatch(true);

    // Initialize the structure Track Info
    m_trackingEnabled    = false;
    m_createOutputMosaic   = false;
    m_bandPriorityBandNumber  = 0;
    m_bandPriorityKeyName  = "";
    m_bandPriorityKeyValue = "";
    m_bandPriorityUseMaxValue = false;

    // Initialize the Special Pixel Flags
    m_placeHighSatPixels = false;
    m_placeLowSatPixels = false;
    m_placeNullPixels = false;

    // Default Priority OnTop
    m_imageOverlay = PlaceImagesOnTop;

    m_enforceMatchDEM = false;

    // Initialize the data members
    m_iss = -1;
    m_isl = -1;
    m_isb = -1;
    m_ins = -1;
    m_inl = -1;
    m_inb = -1;
    m_oss = -1;
    m_osl = -1;
    m_osb = -1;
    m_onb = -1;
  }


  //!  Destroys the Mosaic object. It will close all opened cubes.
  ProcessMosaic::~ProcessMosaic() {
  }


  /**
  * This method invokes the process by mosaic operation over a single input cube
  * and single output cube. Unlike other Isis process objects, no application .
  * function will be called. The processing is handled entirely within the
  * mosaic object. The input cube must be pixel aligned with the output cube
  * before mosaiking. If the input cube does not overlay any of the output cube,
  * no processing takes place. There are 3 priorities input, mosaic and band. Has the
  * ability to track the origin of the mosaic if the flag is set. Some conditions apply
  * like tracking turned off for multiband input with input or mosaic priority.
  *
  * @param os The sample position of input cube starting sample relative to
  *           the output cube. The cordinate is in output cube space and may
  *           be any integer value negative or positive.
  *
  * @param ol The line position of input cube starting line relative to the
  *           output cube. The cordinate is in output cube space and may be
  *           any integer value negative or positive.
  *
  * @param ob The band position of input cube starting band relative to the
  *           output cube. The cordinate is in output cube space and must be
  *           a legal band number within the output cube.
  *
  * @throws IException::Message
  *
  * @author Sharmila Prasad (8/25/2009)
  */
  void ProcessMosaic::StartProcess(const int &os, const int &ol, const int &ob) {
    // Error checks ... there must be one input and one output
    if ((OutputCubes.size() != 1) || (InputCubes.size() != 1)) {
      QString m = "You must specify exactly one input and one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    bool bTrackExists = false;
    if (!m_createOutputMosaic) {
      bTrackExists = GetTrackStatus();
    }

    int ins = m_ins;
    int inl = m_inl;
    int inb = m_inb;
    int iss = m_iss;
    int isl = m_isl;
    int isb = m_isb;

    if (ins == -1)
      ins = (int)InputCubes[0]->sampleCount();

    if (inl == -1)
      inl = (int)InputCubes[0]->lineCount();

    if (inb == -1)
      inb = (int)InputCubes[0]->bandCount();

    // Adjust the input sub-area if it overlaps any edge of the output cube
    m_oss = os;
    m_osl = ol;
    m_osb = ob;

    // Left edge
    if (m_oss < 1) {
      iss = iss - m_oss + 1;
      ins = ins + m_oss - 1;
      m_oss = 1;
    }
    // Top edge
    if (m_osl < 1) {
      isl = isl - m_osl + 1;
      inl = inl + m_osl - 1;
      m_osl = 1;
    }
    // Right edge
    if ((m_oss + ins - 1) > OutputCubes[0]->sampleCount()) {
      ins = OutputCubes[0]->sampleCount() - m_oss + 1;
    }
    // Bottom edge
    if ((m_osl + inl - 1) > OutputCubes[0]->lineCount()) {
      inl = OutputCubes[0]->lineCount() - m_osl + 1;
    }

    PvlGroup imgPosition("ImageLocation");
    imgPosition += PvlKeyword("File", InputCubes[0]->fileName());
    imgPosition += PvlKeyword("StartSample", toString(m_oss));
    imgPosition += PvlKeyword("StartLine", toString(m_osl));
    m_imagePositions += imgPosition;
          
    // Tests for completly off the mosaic
    if ((ins < 1) || (inl < 1)) {
      QString m = "The input cube does not overlap the mosaic";
      throw IException(IException::User, m, _FILEINFO_);
    }

    // Band Adjustments
    if (m_osb < 1) {
      isb = isb - m_osb + 1;
      inb = inb + m_osb - 1;
      m_osb = 1;
    }

    p_progress->SetMaximumSteps(
        (int)InputCubes[0]->lineCount() * (int)InputCubes[0]->bandCount());
    p_progress->CheckStatus();

    // Tracking is done for:
    // (1) Band priority,
    // (2) Ontop and Beneath priority with number of bands equal to 1,
    // (3) Ontop priority with all the special pixel flags set to true
    if (m_trackingEnabled) {
      if (!(m_imageOverlay == UseBandPlacementCriteria ||
          ((m_imageOverlay == PlaceImagesOnTop || m_imageOverlay == PlaceImagesBeneath) &&
           // tracking band was already created for Tracking=true
           (OutputCubes[0]->bandCount()-1) == 1) ||
          (m_imageOverlay == PlaceImagesOnTop && m_placeHighSatPixels && m_placeLowSatPixels &&
           m_placeNullPixels)) ){
        QString m = "Tracking cannot be True for multi-band Mosaic with ontop or beneath priority";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }

    // *******************************************************************************

    Pvl *inLab  = InputCubes[0]->label();
    // Create / Match DEM Shape Model if bMatchDEM Flag is enabled
    if (m_enforceMatchDEM){
      MatchDEMShapeModel();
    }

    // Check to make sure the bandbins match if necessary
    if (m_enforceBandBinMatch) {
      Pvl *outLab = OutputCubes[0]->label();

      if (inLab->findObject("IsisCube").hasGroup("BandBin")) {
        // Check to make sure the output cube has a bandbin group & make sure it
        // matches the input cube bandbin group
        if (!m_createOutputMosaic && outLab->findObject("IsisCube").hasGroup("BandBin")) {
          MatchBandBinGroup(isb, inb);
        }
        // Otherwise copy the input cube bandbin to the output file
        else {
          AddBandBinGroup(isb);
        }
      }
      // BandBin group is not found
      else {
        QString m = "Match BandBin cannot be True when the Image does not have the BandBin group";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }
    // Match BandBin set to false and CREATE and TRACKING is true
    else {
      if (m_createOutputMosaic) {
        if (inLab->findObject("IsisCube").hasGroup("BandBin")) {
          AddBandBinGroup(isb);
        }
        else {
          AddDefaultBandBinGroup();
        }
      }
    }

    // Even if the track flag is off, if the track table exists continue tracking
    if (bTrackExists) {
      m_trackingEnabled = true;
    }

    int iOriginBand = 0, iChanged = 0;

    // Do this before SetMosaicOrigin as we don't want to set the filename
    // in the table unless the band info is valid
    int bandPriorityInputBandNumber = -1;
    int bandPriorityOutputBandNumber = -1;
    if (m_imageOverlay == UseBandPlacementCriteria ) {
      bandPriorityInputBandNumber = GetBandIndex(true);
      bandPriorityOutputBandNumber = GetBandIndex(false);
    }

    // Image name into the table & Get the index for this input file
    int iIndex = GetIndexOffsetByPixelType();

    // Set the Mosaic Origin is Tracking is enabled
    if (m_trackingEnabled) {
      SetMosaicOrigin(iIndex);
    }
    else if (m_imageOverlay == AverageImageWithMosaic && m_createOutputMosaic) {
      ResetCountBands();
    }

    m_onb = OutputCubes[0]->bandCount();

    if (m_trackingEnabled) {
      //Get the last band set aside for "Origin" 1 based
      iOriginBand = OutputCubes[0]->bandCount();
      m_onb--;
      iChanged = 0;

      // For mosaic creation, the input is copied onto mosaic by default
      if (m_imageOverlay == UseBandPlacementCriteria && !m_createOutputMosaic) {
        BandComparison(iss, isl, isb, ins, inl, inb,
                       bandPriorityInputBandNumber, bandPriorityOutputBandNumber, iIndex);
      }
    }
    else if (m_imageOverlay == AverageImageWithMosaic) {
      m_onb /= 2;
    }

    // Process Band Priority with no tracking
    if (m_imageOverlay == UseBandPlacementCriteria && !m_trackingEnabled ) {
      BandPriorityWithNoTracking(iss, isl, isb, ins, inl, inb, bandPriorityInputBandNumber,
                                 bandPriorityOutputBandNumber);
    }
    else {
      // Create portal buffers for the input and output files
      Portal iPortal(ins, 1, InputCubes[0]->pixelType());
      Portal oPortal(ins, 1, OutputCubes[0]->pixelType());
      Portal origPortal(ins, 1, OutputCubes[0]->pixelType());

      for (int ib = isb, ob = m_osb; ib < (isb + inb) && ob <= m_onb; ib++, ob++) {
        for (int il = isl, ol = m_osl; il < isl + inl; il++, ol++) {
          // Set the position of the portals in the input and output cubes
          iPortal.SetPosition(iss, il, ib);
          InputCubes[0]->read(iPortal);

          oPortal.SetPosition(m_oss, ol, ob);
          OutputCubes[0]->read(oPortal);

          if (m_trackingEnabled) {
            origPortal.SetPosition(m_oss, ol, iOriginBand);
            OutputCubes[0]->read(origPortal);
          }
          else if (m_imageOverlay == AverageImageWithMosaic) {
            origPortal.SetPosition(m_oss, ol, (ob+m_onb));
            OutputCubes[0]->read(origPortal);
          }

          bool bChanged = false;
          // Move the input data to the output
          for (int pixel = 0; pixel < oPortal.size(); pixel++) {
            // Creating Mosaic, copy the input onto mosaic
            // regardless of the priority
            if (m_createOutputMosaic) {
              oPortal[pixel] = iPortal[pixel];
              if (m_trackingEnabled) {
                origPortal[pixel] = iIndex;
                bChanged = true;
              }
              else if (m_imageOverlay == AverageImageWithMosaic) {
                if (IsValidPixel(iPortal[pixel])) {
                  origPortal[pixel]=1;
                  bChanged = true;
                }
              }
              iChanged++;
            }
            // Band Priority
            else if (m_trackingEnabled && m_imageOverlay == UseBandPlacementCriteria) {
              int iPixelOrigin = qRound(origPortal[pixel]);
              if (iPixelOrigin == iIndex) {
                oPortal[pixel] = iPortal[pixel];
                iChanged++;
                bChanged = true;
              }
            }
            // OnTop/Input Priority
            else if (m_imageOverlay == PlaceImagesOnTop) {
              if (IsNullPixel(oPortal[pixel])  ||
                 IsValidPixel(iPortal[pixel]) ||
                 (m_placeHighSatPixels && IsHighPixel(iPortal[pixel])) ||
                 (m_placeLowSatPixels  && IsLowPixel(iPortal[pixel]))  ||
                 (m_placeNullPixels    && IsNullPixel(iPortal[pixel]))) {
                oPortal[pixel] = iPortal[pixel];
                if (m_trackingEnabled) {
                  origPortal[pixel] = iIndex;
                  bChanged = true;
                }
                iChanged++;
              }
            }
            // AverageImageWithMosaic priority
            else if (m_imageOverlay == AverageImageWithMosaic) {
              bChanged |= ProcessAveragePriority(pixel, iPortal, oPortal, origPortal);
            }
            // Beneath/Mosaic Priority
            else if (m_imageOverlay == PlaceImagesBeneath) {
              if (IsNullPixel(oPortal[pixel])) {
                oPortal[pixel] = iPortal[pixel];
                // Set the origin if number of input bands equal to 1
                // and if the track flag was set
                if (m_trackingEnabled) {
                  origPortal[pixel] = iIndex;
                  bChanged = true;
                }
                iChanged++;
              }
            }
          } // End sample loop
          if (bChanged) {
            if (m_trackingEnabled || m_imageOverlay == AverageImageWithMosaic) {
            OutputCubes[0]->write(origPortal);
            }
          }
          OutputCubes[0]->write(oPortal);
          p_progress->CheckStatus();
        } // End line loop
      }   // End band loop
    }
  } // End StartProcess

  
  /**
   * Accessor for the placed images and their locations.
   *
   * @return The list of placed images
   */
  PvlObject ProcessMosaic::imagePositions() {
    return m_imagePositions;
  }

  
  /**
   * Opens an input cube specified by the user. This method is overloaded and
   * adds the requirement that only one input cube can be specified.
   *
   * @return Cube*
   *
   * @param parameter User parameter to obtain file to open. Typically, the value
   *                  is "FROM". For example, the user can specify on the command
   *                  line FROM=myfile.cub and this method will attempt to open
   *                  the cube "myfile.cub" if the parameter was set to "FROM".
   *
   * @param ss The starting sample within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param sl The starting line within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param sb The starting band within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param ns The number of samples from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of samples in the cube
   *
   * @param nl The number of lines from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of lines in the cube
   *
   * @param nb The number of bands from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of bands in the cube
   *
   * @throws IException::Message
   */
  Cube *ProcessMosaic::SetInputCube(const QString &parameter,
                                    const int ss, const int sl, const int sb,
                                    const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
    if (InputCubes.size() > 0) {
      QString m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    m_iss = ss;
    m_isl = sl;
    m_isb = sb;
    m_ins = ns;
    m_inl = nl;
    m_inb = nb;

    Cube *cInCube = Process::SetInputCube(parameter);

    //get the output label
    Pvl *cInPvl = InputCubes[0]->label();
    if (cInPvl->findGroup("Dimensions", Pvl::Traverse).hasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->findGroup("Dimensions", Pvl::Traverse).findKeyword("Bands");
      QString sStr(cBandKey[0]);
      if (toInt(sStr) < nb) {
        QString m = "The parameter number of input bands exceeds the actual number of bands in the "
                   "input cube";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }
    return cInCube;
  }


  /**
   * Opens an input cube specified by the user. This method is overloaded and
   * adds the requirement that only one input cube can be specified.
   *
   * @return Cube*
   *
   * @param fname
   *
   * @param att
   *
   * @param ss The starting sample within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param sl The starting line within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param sb The starting band within the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to 1
   *
   * @param ns The number of samples from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of samples in the cube
   *
   * @param nl The number of lines from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of lines in the cube
   *
   * @param nb The number of bands from the input cube. This allowd the
   *           application to choose a sub-area from the input cube to be place
   *           into the mosaic. Defaults to number of bands in the cube
   *
   * @throws IException
   */
  Cube *ProcessMosaic::SetInputCube(const QString &fname,
                                          CubeAttributeInput &att,
                                          const int ss, const int sl, const int sb,
                                          const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
    if (InputCubes.size() > 0) {
      QString m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    m_iss = ss;
    m_isl = sl;
    m_isb = sb;
    m_ins = ns;
    m_inl = nl;
    m_inb = nb;

    Cube *cInCube = Process::SetInputCube(fname, att);

    //check if the number of bands specified is not greater than the actual number of bands in the input
    Pvl *cInPvl = InputCubes[0]->label();
    if (cInPvl->findGroup("Dimensions", Pvl::Traverse).hasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->findGroup("Dimensions", Pvl::Traverse).findKeyword("Bands");
      QString sStr(cBandKey[0]);
      if (toInt(sStr) < nb) {
        QString m = "The parameter number of input bands exceeds the actual number of bands in the input cube";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }
    return cInCube;
  }


  /**
   * Opens an output cube specified by the user. This method is overloaded and
   * adds the requirement that only one output cube can be specified. The output
   * cube must exist before calling SetOutputCube.
   *
   * @return Cube*
   *
   * @param psParameter User parameter to obtain file to open. Typically, the value
   *                    is "TO". For example, the user can specify on the command
   *                    line TO=mosaic.cub and this method will attempt to open the
   *                    cube "mosaic.cub" if the parameter was set to "TO". .
   *
   * @throws IException::Message
   */
  Cube *ProcessMosaic::SetOutputCube(const QString &psParameter) {

    // Make sure there is only one output cube
    if (OutputCubes.size() > 0) {
      QString m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Attempt to open a cube ... get the filename from the user parameter
    // (e.g., "TO") and the cube size from an input cube
    Cube *cube = new Cube;
    try {
      QString fname = Application::GetUserInterface().GetFileName(psParameter);
      cube->open(fname, "rw");
    }
    catch (IException &) {
      delete cube;
      throw;
    }

    if (m_createOutputMosaic) {
      Pvl *outLab = cube->label();
      if (outLab->findObject("IsisCube").hasGroup("BandBin")) {
        outLab->findObject("IsisCube").deleteGroup("BandBin");
      }
    }

    // Everything is fine so save the cube on the stack
    AddOutputCube(cube);
    return cube;
  }


  /** 
   * 
   */ 
  void ProcessMosaic::SetBandBinMatch(bool enforceBandBinMatch) {
    m_enforceBandBinMatch = enforceBandBinMatch;
  }


  /**
   * This method creates a table if not already created to hold
   * the image file names if the track flag is true. If table
   * exists, checks if the image already exists and if it does
   * not, then adds the new image file name. If the field size is
   * smaller than the new image name, then it resizes all the
   * records to new file size. When the table is newly created,it
   * resets the origin band to default based on pixel type.
   *
   * @param index - the input file index
   *
   * @returns none
   *
   * @throws an exception if the number of images exceeds the
   *            pixel size.
   *
   * @throws iException::Message
   *
   * @author Sharmila Prasad (8/28/2009)
   */
  void ProcessMosaic::SetMosaicOrigin(int &index) {
    // Get only the file name
    QString sInputFile = FileName(InputCubes[0]->fileName()).name();
    QString sTableName = TRACKING_TABLE_NAME;

    // Get the serial number
    QString sSerialNumber = SerialNumber::Compose(*(InputCubes[0]));
    int iFileNameLen  = sInputFile.length();
    int iSerialNumLen = sSerialNumber.length();
    int iFieldLength = iSerialNumLen;
    if (iFileNameLen > iSerialNumLen) {
      iFieldLength = iFileNameLen;
    }

    // Get output file name
    QString sOutputFile = FileName(OutputCubes[0]->fileName()).name();

    Pvl *cPvlOut = OutputCubes[0]->label();

    // Create a table record with the new image file name and serial number info
    TableRecord cFileRecord;

    // Populate with File Name
    TableField cFileField("FileName", TableField::Text, iFieldLength);
    cFileField = sInputFile;
    cFileRecord += cFileField;

    // Populate with Serial Number
    TableField cSNField("SerialNumber", TableField::Text, iFieldLength);
    cSNField = sSerialNumber;
    cFileRecord += cSNField;

    int iNumObjs = cPvlOut->objects();
    PvlObject cPvlObj;

    // Check if the Table exists
    if (cPvlOut->hasObject("Table")) {
      for (int i = 0; i < iNumObjs; i++) {
        cPvlObj = cPvlOut->object(i);
        if (cPvlObj.hasKeyword("Name", Pvl::Traverse)) {
          PvlKeyword cNameKey = cPvlObj.findKeyword("Name", Pvl::Traverse);
          if (cNameKey[0] == sTableName) {
            PvlKeyword cFieldKey = cPvlObj.findGroup("Field").findKeyword("Size");

            //set the tracker flag to true as the tracking table exists
            m_trackingEnabled = true;

            // Create a new blank table
            Table cFileTable(sTableName);

            // Read and make a copy of the existing tracking table
            Table cFileTable_Copy = Table(sTableName);
            OutputCubes[0]->read(cFileTable_Copy);

            // Records count
            int iRecs = cFileTable_Copy.Records();

            // Check if the image index can be accomadated in the pixel size
            bool bFull = false;
            switch (sizeof(OutputCubes[0]->pixelType())) {
              case 1:
                // Index is 1 based as 0=Null invalid value
                if (iRecs >= (VALID_MAX1 - 1))
                  bFull = true;
                break;
              case 2:
                // Signed 16bits with some special pixels
                if (iRecs > (VALID_MAX2 - VALID_MIN2 + 1))
                  bFull = true;
                break;

              case 4:
                // Max float mantissa
                if (iRecs > (FLOAT_STORE_INT_PRECISELY_MAX_VALUE -
                             FLOAT_STORE_INT_PRECISELY_MIN_VALUE + 1))
                  bFull = true;
                break;
            }

            if (bFull) {
              QString msg = "The number of images in the Mosaic exceeds the pixel size";
              throw IException(IException::Programmer, msg, _FILEINFO_);
            }

            for (int i = 0; i < iRecs; i++) {
              // Get the file name and trim out the characters filled due to resizing
              QString sTableFile = QString(QString(cFileTable_Copy[i][0]).toAscii().data());
              int found = sTableFile.lastIndexOf(".cub");
              if (found != -1) {
                // clear the packing characters - get only the file name
                sTableFile.remove(found + 4);
              }

              if (sTableFile == sInputFile) {
                index += i;
                return;
              }

              // To initialize the new table, on the first file name comparison, check the size of
              // the existing table record with the size of the new record being added
              if (!i) {
                if (toInt(QString(cFieldKey[0])) < iFieldLength) {
                  TableRecord cFileRecordUpdate;
                  TableField cFileFieldUpdate("FileName", TableField::Text, iFieldLength);
                  cFileFieldUpdate = (QString)cFileTable_Copy[i][0];
                  cFileRecordUpdate += cFileFieldUpdate;

                  // Populate with Serial Number
                  TableField cSNFieldUpdate("SerialNumber", TableField::Text, iFieldLength);
                  cSNFieldUpdate = (QString)cFileTable_Copy[i][1];
                  cFileRecordUpdate += cSNFieldUpdate;
                  // add new record and set the size for all the other records
                  cFileTable = Table(sTableName, cFileRecordUpdate);
                }
                else {
                  cFileTable = Table(sTableName, cFileTable_Copy[i]);
                }
              }

              // Add the existing records into the new table
              cFileTable += cFileTable_Copy[i]; // what if record size was resized above??? new record does not match table record size
            }
            // Get the current image file index
            index += iRecs;

            // Add the current input image record to the new table
            cFileTable +=  cFileRecord;

            // Copy the new table to the output Mosaic
            OutputCubes[0]->write(cFileTable);
            break;   //break while loop
          }
        }
      }//end for loop
    }

    //creating new table if track flag is true
    if (m_createOutputMosaic && m_trackingEnabled) {
      Table cFileTable(sTableName, cFileRecord);
      cFileTable += cFileRecord;
      OutputCubes[0]->write(cFileTable);
      //reset the origin band based on pixel type
      ResetOriginBand();
    }
  }


  /**
   * Set the keyword/value to use for comparing when using band priority.
   */
  void ProcessMosaic::SetBandKeyword(QString bandPriorityKeyName, QString bandPriorityKeyValue) {
    m_bandPriorityKeyName = bandPriorityKeyName;
    m_bandPriorityKeyValue = bandPriorityKeyValue;
  }


  /**
   * Set the band to use for priorities when using band priority.
   */
  void ProcessMosaic::SetBandNumber(int bandPriorityBandNumber) {
    m_bandPriorityBandNumber = bandPriorityBandNumber;
  }


  /**
   * Set whether to take the image with the max or min value when
   *   using band priority.
   */
  void ProcessMosaic::SetBandUseMaxValue(bool useMax) {
    m_bandPriorityUseMaxValue = useMax;
  }


  /**
   * Flag to indicate that the mosaic is being newly created
   * Indication that the new label specific to the mosaic needs to be created.
   *
   * @author Sharmila Prasad (1/19/2011)
   *
   * @param createOutputMosaic - Set Create Flag True/False
   */
  void ProcessMosaic::SetCreateFlag(bool createOutputMosaic) {
    m_createOutputMosaic = createOutputMosaic;
  }


  /**
   * When true, high saturation (HRS, HIS) will be considered valid data for the purposes of
   *   placing pixels in the output mosaic.
   */
  void ProcessMosaic::SetHighSaturationFlag(bool placeHighSatPixels) {
    m_placeHighSatPixels = placeHighSatPixels;
  }


  void ProcessMosaic::SetImageOverlay(ImageOverlay placement) {
    m_imageOverlay = placement;
  }


  /**
   * When true, low saturation (LRS, LIS) will be considered valid data for the purposes of
   *   placing pixels in the output mosaic.
   */
  void ProcessMosaic::SetLowSaturationFlag(bool placeLowSatPixels) {
    m_placeLowSatPixels = placeLowSatPixels;
  }

  /**
   * Set the DEM match flag. 
   * @param matchDEM If true, the match is enforced between Input & Mosaic 
   */ 
  void ProcessMosaic::SetMatchDEM(bool matchDEM) {
    m_enforceMatchDEM = matchDEM;
  }


  /**
   * When true, Null pixels will be considered valid data for the purposes of
   *   placing pixels in the output mosaic.
   */
  void ProcessMosaic::SetNullFlag(bool placeNullPixels) {
    m_placeNullPixels = placeNullPixels;
  }


  void ProcessMosaic::SetTrackFlag(bool trackingEnabled) {
    m_trackingEnabled = trackingEnabled;
  }


  /**
   * @see SetHighSaturationFlag()
   */
  bool ProcessMosaic::GetHighSaturationFlag() const {
    return m_placeHighSatPixels;
  }


  /** 
   *  
   */ 
  ProcessMosaic::ImageOverlay ProcessMosaic::GetImageOverlay() const {
    return m_imageOverlay;
  }


  /**
   * @see SetLowSaturationFlag()
   */
  bool ProcessMosaic::GetLowSaturationFlag() const {
    return m_placeLowSatPixels;
  }


  /**
   * @see SetNullFlag()
   */
  bool ProcessMosaic::GetNullFlag() const {
    return m_placeNullPixels;
  }


  /**
   * @see SetTrackFlag()
   */
  bool ProcessMosaic::GetTrackFlag() const {
    return m_trackingEnabled;
  }


  /**
   * This is the line where the image was placed into the output mosaic.
   */
  int ProcessMosaic::GetInputStartLineInMosaic() const {
    return m_osl;
  }


  /**
   * This is the sample where the image was placed into the output mosaic.
   */
  int ProcessMosaic::GetInputStartSampleInMosaic() const {
    return m_oss;
  }


  /**
   * This is the band where the image was placed into the output mosaic.
   */
  int ProcessMosaic::GetInputStartBandInMosaic() const {
    return m_osb;
  }


  /**
   * Convert an ImageOverlay to a QString. This is used to translate between
   *   mapmos, handmos, and automos' interfaces into an ImageOverlay.
   */
  QString ProcessMosaic::OverlayToString(ImageOverlay imageOverlay) {
    QString result;

    switch (imageOverlay) {
      case PlaceImagesOnTop:
        result = "OnTop";
        break;

      case PlaceImagesBeneath:
        result = "Beneath";
        break;

      case UseBandPlacementCriteria:
        result = "Band";
        break;

      case AverageImageWithMosaic:
        result = "Average";
        break;

      case NumImageOverlayOptions:
        break;
    }

    if (result == "") {
      throw IException(IException::Unknown,
                       "Cannot convert overlay [" + toString((int)imageOverlay) + "] to a string",
                       _FILEINFO_);
    }

    return result;
  }


  /**
   * Convert a QString to an ImageOverlay (case-insensitive). This is used to translate between
   *   mapmos, handmos, and automos' interfaces into an ImageOverlay.
   */
  ProcessMosaic::ImageOverlay ProcessMosaic::StringToOverlay(QString imageOverlayString) {
    QString imageOverlayStringUpper = imageOverlayString.toUpper();
    for (int i = 0; i < NumImageOverlayOptions; i++) {
      if (OverlayToString((ImageOverlay)i).toUpper() == imageOverlayStringUpper)
        return (ImageOverlay)i;
    }

    throw IException(IException::Unknown,
                     "The text [" + imageOverlayString + "] does not correspond to any known "
                     "image overlay modes (mosaic priorities)",
                     _FILEINFO_);
  }


  /**
   * Match the Shape Model for input and mosaic. If creating the mosaic,
   * copy the input ShapeModel from the input label.
   * Store only the file name of the Shape Model
   *
   * @author Sharmila Prasad (1/24/2011)
   */
  void ProcessMosaic::MatchDEMShapeModel() {
    Pvl* inLabel  = InputCubes[0]->label();
    Pvl* outLabel = OutputCubes[0]->label();

    if (outLabel->findObject("IsisCube").hasGroup("Mosaic")) {
      PvlGroup outMosaicGrp = outLabel->findObject("IsisCube").findGroup("Mosaic");
      if (outMosaicGrp.hasKeyword("ShapeModel")) {
        if (inLabel->findObject("IsisCube").hasGroup("Kernels")) {
          PvlGroup inMosaicGrp = inLabel->findObject("IsisCube").findGroup("Kernels");
          if (outMosaicGrp.hasKeyword("ShapeModel") && inMosaicGrp.hasKeyword("ShapeModel")) {
            PvlKeyword outShapeModelKey = outMosaicGrp.findKeyword("ShapeModel");
            QString sShapeModel = inMosaicGrp.findKeyword("ShapeModel")[0];
            int found = sShapeModel.lastIndexOf("/");
            if (found != -1) {
              sShapeModel.remove(0, found + 1);
            }
            if (sShapeModel == outShapeModelKey[0]) {
              return;
            }
          }
        }
        QString sErrMsg = "Input and Mosaic DEM Shape Model do not match";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }
    else {
      if (m_createOutputMosaic) {
        if (inLabel->findObject("IsisCube").hasGroup("Kernels")) {
          QString sShapeModel =
              inLabel->findObject("IsisCube").findGroup("Kernels").findKeyword("ShapeModel")[0];
          int found = sShapeModel.lastIndexOf("/");
          if (found != -1){
            sShapeModel.remove(0, found+1);
          }
          PvlObject & outIsisCubeObj = outLabel->findObject("IsisCube");
          PvlGroup mosaicGrp("Mosaic");
          PvlKeyword shapeModelKey("ShapeModel");
          shapeModelKey += sShapeModel;
          mosaicGrp += shapeModelKey;
          outIsisCubeObj += mosaicGrp;
        }
      }
    }
  }


  /**
   * Reset all the count bands to default at the time of mosaic creation
   *
   * @author Sharmila Prasad (1/13/2011)
   */
  void ProcessMosaic::ResetCountBands()
  {
    int iBand   = OutputCubes[0]->bandCount();
    int iLines  = OutputCubes[0]->lineCount();
    int iSample = OutputCubes[0]->sampleCount();

    Portal origPortal(iSample, 1, OutputCubes[0]->pixelType());
    int iStartCountBand = iBand/2 + 1;

    for (int band=iStartCountBand; band<=iBand; band++) {
      for (int i = 1; i <= iLines; i++) {
        origPortal.SetPosition(1, i, band);  //sample, line, band position
        OutputCubes[0]->read(origPortal);
        for (int iPixel = 0; iPixel < origPortal.size(); iPixel++) {
          origPortal[iPixel] = 0;
        }
        OutputCubes[0]->write(origPortal);
      }
    }
  }


  /**
   * Calculate DN value for a pixel for AverageImageWithMosaic priority and set the
   * Count band portal
   *
   * @author Sharmila Prasad (1/13/2011)
   *
   * @param piPixel     - Pixel index
   * @param piPortal    - Input Portal
   * @param poPortal    - Output Portal
   * @param porigPortal - Count Portal
   *
   * @return bool
   */
  bool ProcessMosaic::ProcessAveragePriority(int piPixel, Portal& piPortal, Portal& poPortal,
                                             Portal& porigPortal)
  {
    bool bChanged=false;
    if (IsValidPixel(piPortal[piPixel]) && IsValidPixel(poPortal[piPixel])) {
      int iCount = (int)porigPortal[piPixel];
      double dNewDN = (poPortal[piPixel] * iCount + piPortal[piPixel]) / (iCount + 1);
      poPortal[piPixel] = dNewDN;
      porigPortal[piPixel] =iCount +1;
      bChanged = true;
    }
    // Input-Valid, Mosaic-Special
    else if (IsValidPixel(piPortal[piPixel])) {
      poPortal[piPixel] = piPortal[piPixel];
      porigPortal[piPixel] = 1;
      bChanged = true;
    }
    // Input-Special, Flags-True
    else if (IsSpecial(piPortal[piPixel])) {
      if ((m_placeHighSatPixels && IsHighPixel(piPortal[piPixel])) ||
         (m_placeLowSatPixels  && IsLowPixel (piPortal[piPixel]))  ||
         (m_placeNullPixels    && IsNullPixel(piPortal[piPixel]))) {
        poPortal[piPixel]    = piPortal[piPixel];
        porigPortal[piPixel] = 0;
        bChanged = true;
      }
    }
    return bChanged;
  }


  /**
   *  This method matches the input BandBin group to the mosaic BandBin Group
   *  and allows band to be replaced in mosaic if it is NA (not assigned).
   *  It expects the bands to be contiguous
   *
   *  returns None
   *
   *  @throws IException::Message
   *
   *  @author Sharmila Prasad (9/25/2009)
   */
  void ProcessMosaic::MatchBandBinGroup(int origIsb, int &inb) {
    Pvl *inLab  = InputCubes[0]->label();
    Pvl *outLab = OutputCubes[0]->label();

    PvlGroup &inBin  = inLab->findGroup("BandBin", Pvl::Traverse);
    PvlGroup &outBin = outLab->findGroup("BandBin", Pvl::Traverse);
    if (inBin.keywords() != outBin.keywords()) {
      QString msg = "Pvl Group [BandBin] does not match between the input and output cubes";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    //pvl - zero based
    int isb = (origIsb - 1);
    int osb = (m_osb - 1);
    int iOutBandsHalf = OutputCubes[0]->bandCount()/2;

    for (int i = 0; i < outBin.keywords(); i++) {
      PvlKeyword &outKey = outBin[i];
      QString sOutName = outKey.name();
      if (inBin.hasKeyword(sOutName)) {
        PvlKeyword &inKey = inBin[sOutName];
        for (int j = osb, k = isb; j < outKey.size() && k < inKey.size(); j++, k++) {
          if (outKey[j] == "NA") {
            outKey[j] = inKey[k];
            if (m_imageOverlay == AverageImageWithMosaic) {
              if (sOutName.contains("Filter") ||
                  sOutName.contains("Name")) {
                outKey[j+iOutBandsHalf] = inKey[k] + "_Count";
              }
              else {
                outKey[j+iOutBandsHalf] = "Avg_Count";
              }
            }
          }
          else if (outKey[j] != inKey[k]) {
            QString msg = "Pvl Group [BandBin] in Key[" + outKey.name() + "] In value" + inKey[k] +
                         "and Out value=" + outKey[j] + " do not match";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
      }
      else {
        QString msg = "Pvl Group [BandBin] In Keyword[" + inBin[i].name() + "] and Out Keyword[" +
                     outBin[i].name() + "] does not match";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    int inputRange = InputCubes[0]->bandCount() - isb;
    int outputRange = OutputCubes[0]->bandCount() - osb;
    inb = min(inputRange, outputRange);
  }


  /**
   *  This method adds the BandBin group to the mosaic corresponding
   *  to the actual bands in the mosaic
   *
   *  returns None
   *
   *  @author Sharmila Prasad (9/24/2009)
   *
   *  @history 2011-01-12 Sharmila Prasad Added logic for Count Bands for
   *  AverageImageWithMosaic Priority
   */
  void ProcessMosaic::AddBandBinGroup(int origIsb) {
    Pvl *inLab  = InputCubes[0]->label();
    Pvl *outLab = OutputCubes[0]->label();

    int iOutBands = OutputCubes[0]->bandCount();

    if (m_trackingEnabled) {
      iOutBands -= 1;     // leave tracking band
    }
    else if (m_imageOverlay == AverageImageWithMosaic) {
      iOutBands /= 2;
    }

    int isb = origIsb - 1; // array zero based
    int osb = m_osb - 1;

    PvlGroup &cInBin  = inLab->findGroup("BandBin", Pvl::Traverse);
    PvlGroup cOutBin("BandBin");

    int iInBands = InputCubes[0]->bandCount();

    for (int i = 0; i < cInBin.keywords(); i++) {
      PvlKeyword &cInKey = cInBin[i];
      int iInKeySize = cInKey.size();
      PvlKeyword cOutKey(cInKey.name());

      for (int b = 0; b < osb; b++) {
        cOutKey += "NA";
      }
      for (int b = osb; b < iOutBands; b++) {
        if (isb < iInKeySize) {
          cOutKey += cInKey[isb++];
        }
        else {
          cOutKey += "NA";
        }
      }

      // Add the "TRACKING" band to the Keyword if the flag is set and also if the number of
      // input cube bands is same as the the keysize of the keyword in the BandBin group.
      if (m_trackingEnabled && iInBands == iInKeySize) {
        cOutKey += "TRACKING"; // for the origin band
      }

      // Tag the Count Bands if priority is AverageImageWithMosaic.
      else if (m_imageOverlay == AverageImageWithMosaic) {
        int iTotalOutBands = OutputCubes[0]->bandCount();
        isb = origIsb - 1; // reset the input starting band
        int iOutStartBand = iOutBands + osb;
        QString sKeyName = cInKey.name();
        bool bFilterKey = false;
        if (sKeyName.contains("Filter")   ||
            sKeyName.contains("Original") ||
            sKeyName.contains("Name")) {
          bFilterKey = true;
        }
        for (int ob=iOutBands; ob<iTotalOutBands; ob++) {
          if (isb < iInKeySize && ob >= iOutStartBand) {
            if (bFilterKey) {
              cOutKey += cInKey[isb++] + "_Count";
            }
            else {
              cOutKey += 0;
              isb++;
            }
          }
          else {
            cOutKey += 0;
          }
        }
      }

      // Check for units and make sure output keyword units value is set to input
      // keyword units value
      if (cOutKey.unit() != cInKey.unit()) {
        cOutKey.setUnits((QString)(cInKey.unit()));
      }

      cOutBin += cOutKey;
      isb = origIsb - 1;        // reinitialize the input starting band
    }
    outLab->findObject("IsisCube").addGroup(cOutBin);
  }


  /**
   * AddDefaultBandBinGroup
   *
   * This method adds a default BandBin group on Mosaic creation
   * if the MatchBandBin Group is set to false and Tracking to set
   * to true
   *
   * Return void
   */
  void ProcessMosaic::AddDefaultBandBinGroup() {
    Pvl *outLab = OutputCubes[0]->label();

    PvlGroup cOutBin("BandBin");

    int iOutBands = OutputCubes[0]->bandCount();
    int iOutBandsTotal = iOutBands;

    if (m_trackingEnabled) {
      iOutBands--; // Leave tracking band
    }
    else if (m_imageOverlay == AverageImageWithMosaic) {
      iOutBands /= 2;
    }

    PvlKeyword cOutKey("FilterName");

    for (int i=0; i<iOutBands; i++) {
      cOutKey += "NA";
    }

    if (m_imageOverlay == AverageImageWithMosaic) {
      for (int i=iOutBands; i<iOutBandsTotal; i++) {
        cOutKey += "NA_Count";
      }
    }

    if (m_trackingEnabled) {
      cOutKey += "TRACKING";
    }

    cOutBin += cOutKey;

    outLab->findObject("IsisCube").addGroup(cOutBin);
  }


  /** 
   *  Get the Band Index in an image of type (input/output)
   */ 
  int ProcessMosaic::GetBandIndex(bool inputFile) {
    bool bFound = false;
    int iBandIndex = 0;

    Pvl cPvlLabel;

    if (inputFile)
      cPvlLabel = *(InputCubes[0]->label());
    else
      cPvlLabel = *(OutputCubes[0]->label());

    //if non-zero integer, must be original band #, 1 based
    if (m_bandPriorityBandNumber) {
      PvlKeyword cKeyOrigBand;
      if (cPvlLabel.findGroup("BandBin", Pvl::Traverse).hasKeyword("OriginalBand")) {
        cKeyOrigBand = cPvlLabel.findGroup("BandBin", Pvl::Traverse).findKeyword("OriginalBand");
      }
      int iSize = cKeyOrigBand.size();
      QString buff = toString(m_bandPriorityBandNumber);
      for (int i = 0; i < iSize; i++) {
        if (buff == cKeyOrigBand[i]) {
          iBandIndex = i + 1; //1 based get band index
          bFound = true;
          break;
        }
      }
    }
    //key name
    else {
      PvlKeyword cKeyName;
      if (cPvlLabel.findGroup("BandBin", Pvl::Traverse).hasKeyword(m_bandPriorityKeyName)) {
        cKeyName = cPvlLabel.findGroup("BandBin", Pvl::Traverse).findKeyword(m_bandPriorityKeyName);
      }
      int iSize = cKeyName.size();
      for (int i = 0; i < iSize; i++) {
        if (m_bandPriorityKeyValue.toUpper() == cKeyName[i].toUpper()) {
          iBandIndex = i + 1; //1 based get key value index
          bFound = true;
          break;
        }
      }
    }
    if (!bFound) {
      QString msg = "Invalid Band / Key Name, Value ";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    return iBandIndex;
  }


  /**
   * This method compares the specified band of the input and
   * output using the criteria (lesser or greater) to assign the
   * pixel origin(input fileindex) to the origin band. In the StartProcess(),
   * input pixel is assigned to the output if the origin pixel equals the current
   * input file index
   *
   * @index     - FileName Index for the origin band (default +
   *                zero based index)
   *
   * @throws IException::Message
   *
   * @author Sharmila Prasad (9/04/2009)
   */
  void ProcessMosaic::BandComparison(int iss, int isl, int isb, int ins, int inl, int inb,
      int bandPriorityInputBandNumber, int bandPriorityOutputBandNumber, int index) {
    //
    // Create portal buffers for the input and output files
    Portal cIportal(ins, 1, InputCubes[0]->pixelType());
    Portal cOportal(ins, 1, OutputCubes[0]->pixelType());
    Portal origPortal(ins, 1, OutputCubes[0]->pixelType());

    //Get the last band set aside for "Origin"
    int iOriginBand = OutputCubes[0]->bandCount();

    for (int iIL = isl, iOL = m_osl; iIL < isl + inl; iIL++, iOL++) {
      // Set the position of the portals in the input and output cubes
      cIportal.SetPosition(iss, iIL, bandPriorityInputBandNumber);
      InputCubes[0]->read(cIportal);

      cOportal.SetPosition(m_oss, iOL, bandPriorityOutputBandNumber);
      OutputCubes[0]->read(cOportal);

      origPortal.SetPosition(m_oss, iOL, iOriginBand);
      OutputCubes[0]->read(origPortal);

      // Move the input data to the output
      for (int iPixel = 0; iPixel < cOportal.size(); iPixel++) {
        if (IsNullPixel(origPortal[iPixel]) ||
            (m_placeHighSatPixels && IsHighPixel(cIportal[iPixel])) ||
            (m_placeLowSatPixels  && IsLowPixel(cIportal[iPixel])) ||
            (m_placeNullPixels    && IsNullPixel(cIportal[iPixel]))) {
          origPortal[iPixel] = index;
        }
        else {
          if (IsValidPixel(cIportal[iPixel])) {
            if (IsSpecial(cOportal[iPixel]) ||
                (m_bandPriorityUseMaxValue == false  && cIportal[iPixel] < cOportal[iPixel]) ||
                (m_bandPriorityUseMaxValue == true && cIportal[iPixel] > cOportal[iPixel])) {
              origPortal[iPixel] = index;
            }
          }
        }
      }
      OutputCubes[0]->write(origPortal);
    }
  }


  /**
   * Mosaicking for Band Priority with no Tracking
   *
   * @author Sharmila Prasad (1/4/2012)
   */
  void ProcessMosaic::BandPriorityWithNoTracking(
      int iss, int isl, int isb, int ins, int inl, int inb,
      int bandPriorityInputBandNumber, int bandPriorityOutputBandNumber) {
    // Create portal buffers for the input and output files pointing to the
    // specified band for comparison
    Portal iComparePortal(ins, 1, InputCubes[0]->pixelType());
    Portal oComparePortal(ins, 1, OutputCubes[0]->pixelType());

    Portal resultsPortal (ins, 1, OutputCubes[0]->pixelType());

    // Create portal buffers for the input and output files
    Portal iPortal(ins, 1, InputCubes[0]->pixelType());
    Portal oPortal(ins, 1, OutputCubes[0]->pixelType());

    for (int inLine = isl, outLine = m_osl; inLine < isl + inl; inLine++, outLine++) {
      // Set the position of the portals in the input and output cubes
      iComparePortal.SetPosition(iss, inLine, bandPriorityInputBandNumber);
      InputCubes[0]->read(iComparePortal);

      oComparePortal.SetPosition(m_oss, outLine, bandPriorityOutputBandNumber);
      OutputCubes[0]->read(oComparePortal);

      Portal iPortal(ins, 1, InputCubes[0]->pixelType());
      Portal oPortal(ins, 1, OutputCubes[0]->pixelType());

      bool inCopy = false;

      // Move the input data to the output
      for (int iPixel = 0; iPixel < ins; iPixel++) {
        resultsPortal[iPixel] = false;
        if (m_createOutputMosaic) {
          resultsPortal[iPixel] = true;
          inCopy = true;
        }
        else if ((m_placeHighSatPixels && IsHighPixel(iComparePortal[iPixel])) ||
                (m_placeLowSatPixels  && IsLowPixel (iComparePortal[iPixel])) ||
                (m_placeNullPixels    && IsNullPixel(iComparePortal[iPixel]))) {
          resultsPortal[iPixel] = true;
          inCopy = true;
        }
        else {
          if (IsValidPixel(iComparePortal[iPixel])) {
            if (IsSpecial(oComparePortal[iPixel]) ||
                (m_bandPriorityUseMaxValue == false  && iComparePortal[iPixel] <
                 oComparePortal[iPixel]) ||
                (m_bandPriorityUseMaxValue == true && iComparePortal[iPixel] >
                 oComparePortal[iPixel])) {
              resultsPortal[iPixel] = true;
              inCopy = true;
            }
          }
        }
      }

      if (inCopy) {
        for (int ib = isb, ob = m_osb; ib < (isb + inb) && ob <= m_onb; ib++, ob++) {
          // Set the position of the portals in the input and output cubes
          iPortal.SetPosition(iss, inLine, ib);
          InputCubes[0]->read(iPortal);

          oPortal.SetPosition(m_oss, outLine, ob);
          OutputCubes[0]->read(oPortal);

          for (int iPixel = 0; iPixel < ins; iPixel++) {
            if (resultsPortal[iPixel]) {
              oPortal[iPixel] = iPortal[iPixel];
            }
          }
          OutputCubes[0]->write(oPortal);
        }
      }
    }
  }


  /**
   * This method returns the start value depending on the pixel
   * type 8,16,32 bit.
   *
   * @returns the start/offset value
   *
   * @throws iException::Message
   *
   * @author Sharmila Prasad (8/28/2009)
   */
  int ProcessMosaic::GetIndexOffsetByPixelType() {
    int iOffset = 0;

    switch (SizeOf(OutputCubes[0]->pixelType())) {
      case 1:
        iOffset = VALID_MIN1;
        break;

      case 2:
        iOffset = VALID_MIN2;
        break;

      case 4:
        iOffset = FLOAT_STORE_INT_PRECISELY_MIN_VALUE;
        break;
    }
    return iOffset;
  }


  /**
   * This method  returns the defaults(unassigned origin value)
   * depending on the pixel type.
   *
   * @No parameters
   *
   * @returns default value
   *
   * @throws IException::Message
   *
   * @author Sharmila Prasad (9/10/2009)
   */
  int ProcessMosaic::GetOriginDefaultByPixelType() {
    int iDefault;

    switch (SizeOf(OutputCubes[0]->pixelType())) {
      case 1:
        iDefault = NULL1;
        break;

      case 2:
        iDefault = NULL2;
        break;

      case 4:
        iDefault = INULL4;
        break;

      default:
        QString msg = "ProcessMosaic::GetOriginDefaultByPixelType - Invalid Pixel Type";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return iDefault;
  }


  /**
   * This method sets the origin band to defaults(unassigned value)
   * depending on the pixel type.
   *
   * @No parameters and no return value
   *
   * @author Sharmila Prasad (8/28/2009)
   */
  void ProcessMosaic::ResetOriginBand() {
    int iBand   = OutputCubes[0]->bandCount();
    int iLines  = OutputCubes[0]->lineCount();
    int iSample = OutputCubes[0]->sampleCount();

    int iDefault = GetOriginDefaultByPixelType();

    Portal origPortal(iSample, 1, OutputCubes[0]->pixelType());

    for (int i = 1; i <= iLines; i++) {
      origPortal.SetPosition(1, i, iBand);  //sample, line, band position
      OutputCubes[0]->read(origPortal);
      for (int iPixel = 0; iPixel < origPortal.size(); iPixel++) {
        origPortal[iPixel] = (float)(iDefault);
      }
      OutputCubes[0]->write(origPortal);
    }
    //Test();
  }


  /**
   * This method searchs the mosaic label for a table with name
   * "InputFile". If found return true else false. Checks for the
   * existence of the origin table
   *
   * @returns the table index of the current image
   *
   * @author Sharmila Prasad (9/22/2009)
   */
  bool ProcessMosaic::GetTrackStatus() {
    //get the output label
    Pvl *cPvlOut = OutputCubes[0]->label();

    bool bTableExists = false;
    int iNumObjs = cPvlOut->objects();
    PvlObject cPvlObj;

    //Check if table already exists
    if (cPvlOut->hasObject("Table")) {
      for (int i = 0; i < iNumObjs; i++) {
        cPvlObj = cPvlOut->object(i);
        if (cPvlObj.hasKeyword("Name", Pvl::Traverse)) {
          PvlKeyword cNameKey = cPvlObj.findKeyword("Name", Pvl::Traverse);
          if (cNameKey[0] == TRACKING_TABLE_NAME) {
            bTableExists = true;
          }
        }
      }
    }

    return bTableExists;
  }

}

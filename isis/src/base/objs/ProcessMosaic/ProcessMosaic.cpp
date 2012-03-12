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

#include "ProcessMosaic.h"
#include "SpecialPixel.h"
#include "IException.h"
#include "Application.h"
#include "Pvl.h"
#include "Table.h"
#include "SerialNumber.h"
#include "iString.h"

using namespace std;

namespace Isis {

  /**
   * ProcessMosaic Contructor
   *
   * Initialize the class members to default
   */
  ProcessMosaic::ProcessMosaic() {

    // Set the BandBin Match
    SetBandBinMatch(true);

    // Initialize the structure Track Info
    m_mosaicOptions.bTrack    = false;
    m_mosaicOptions.bCreate   = false;
    m_mosaicOptions.iBandNum  = 0;
    m_mosaicOptions.sKeyName  = "";
    m_mosaicOptions.sKeyValue = "";
    m_mosaicOptions.eCriteria = Lesser;
    m_mosaicOptions.iInBand   = 0;
    m_mosaicOptions.iOutBand  = 0;

    // Initialize the Special Pixel Flags
    m_highSat = false;
    m_lowSat = false;
    m_null = false;

    // Default Priority OnTop
    m_priority = input;

    m_matchDEM = false;

    // Initialize the data members
    m_oss = -1;
    m_osl = -1;
    m_osb = -1;
  };

  /**
   * ProcessMosaic Destructor
   *
   */
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
  * @throws Isis::IException::Message
  *
  * @author Sharmila Prasad (8/25/2009)
  */
  void ProcessMosaic::StartProcess(const int &os, const int &ol, const int &ob) {

    // Error checks ... there must be one input and one output
    if((OutputCubes.size() != 1) || (InputCubes.size() != 1)) {
      string m = "You must specify exactly one input and one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    bool bTrackExists = false;
    if(!m_mosaicOptions.bCreate) {
      bTrackExists = GetTrackStatus();
    }

    // **
    // Set up the input sub cube (This must be a legal sub-area of the input cube)
    // *

    if(m_ins == 0) m_ins = (int)InputCubes[0]->getSampleCount();
    if(m_inl == 0) m_inl = (int)InputCubes[0]->getLineCount();
    if(m_inb == 0) m_inb = (int)InputCubes[0]->getBandCount();

    // Adjust the input sub-area if it overlaps any edge of the output cube
    m_oss = os;
    m_osl = ol;
    m_osb = ob;

    // Left edge
    if(m_oss < 1) {
      m_iss = m_iss - m_oss + 1;
      m_ins = m_ins + m_oss - 1;
      m_oss = 1;
    }
    // Top edge
    if(m_osl < 1) {
      m_isl = m_isl - m_osl + 1;
      m_inl = m_inl + m_osl - 1;
      m_osl = 1;
    }
    // Right edge
    if((m_oss + m_ins - 1) > OutputCubes[0]->getSampleCount()) {
      m_ins = OutputCubes[0]->getSampleCount() - m_oss + 1;
    }
    // Bottom edge
    if((m_osl + m_inl - 1) > OutputCubes[0]->getLineCount()) {
      m_inl = OutputCubes[0]->getLineCount() - m_osl + 1;
    }

    // Tests for completly off the mosaic
    if((m_ins < 1) || (m_inl < 1)) {
      string m = "The input cube does not overlap the mosaic";
      throw IException(IException::User, m, _FILEINFO_);
    }

    // Band Adjustments
    if(m_osb < 1) {
      m_isb = m_isb - m_osb + 1;
      m_inb = m_inb + m_osb - 1;
      m_osb = 1;
    }

    p_progress->SetMaximumSteps((int)InputCubes[0]->getLineCount() * (int)InputCubes[0]->getBandCount());
    p_progress->CheckStatus();

    // Tracking is done for:
    // (1) Band priority,
    // (2) Ontop and Beneath priority with number of bands equal to 1,
    // (3) Ontop priority with all the special pixel flags set to true
    if(m_mosaicOptions.bTrack) {
      if(!(m_priority == band  ||
          ((m_priority == input || m_priority == mosaic) &&
           (OutputCubes[0]->getBandCount()-1) == 1) ||   // tracking band was already created for Tracking=true
          (m_priority == input && m_highSat && m_lowSat && m_null)) ){
        string m = "Tracking cannot be True for multi-band Mosaic with ontop or beneath priority";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }

    // *******************************************************************************

    Pvl *inLab  = InputCubes[0]->getLabel();
    // Create / Match DEM Shape Model if bMatchDEM Flag is enabled
    if (m_matchDEM){
      MatchDEMShapeModel();
    }

    // Check to make sure the bandbins match if necessary
    if(m_bandBinMatch) {
      Pvl *outLab = OutputCubes[0]->getLabel();

      if(inLab->FindObject("IsisCube").HasGroup("BandBin")) {
        // Check to make sure the output cube has a bandbin group & make sure it
        // matches the input cube bandbin group
        if(!m_mosaicOptions.bCreate && outLab->FindObject("IsisCube").HasGroup("BandBin")) {
          m_inb = 0;

          MatchBandBinGroup();
        }
        // Otherwise copy the input cube bandbin to the output file
        else {
          AddBandBinGroup();
        }
      }
      // BandBin group is not found
      else {
        string m = "Match BandBin cannot be True when the Image does not have the BandBin group";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
    }
    // Match BandBin set to false and CREATE and TRACKING is true
    else {
      if(m_mosaicOptions.bCreate) {
        if (inLab->FindObject("IsisCube").HasGroup("BandBin")) {
          AddBandBinGroup();
        }
        else {
          AddDefaultBandBinGroup();
        }
      }
    }

    // Even if the track flag is off, if the track table exists continue tracking
    if(bTrackExists) {
      m_mosaicOptions.bTrack = true;
    }

    int iOriginBand = 0, iChanged = 0;

    // Do this before SetMosaicOrigin as we don't want to set the filename
    // in the table unless the band info is valid
    if(m_priority == band) {
      // Find the band to be compared
      FileType eFileType = FileType(0);
      GetBandIndex(eFileType);
      eFileType = FileType(1);
      GetBandIndex(eFileType);
    }

    // Image name into the table & Get the index for this input file
    //int iInNumBands = m_inb - m_isb + 1; // actual number of inpur bands being transfered to mosaic
    int iIndex = GetIndexOffsetByPixelType();

    // Set the Mosaic Origin is Tracking is enabled
    if(m_mosaicOptions.bTrack) {
      SetMosaicOrigin(iIndex);
    }
    else if(m_priority == average && m_mosaicOptions.bCreate) {
      ResetCountBands();
    }

    m_onb = OutputCubes[0]->getBandCount();

    if(m_mosaicOptions.bTrack) {
      iOriginBand = OutputCubes[0]->getBandCount(); //Get the last band set aside for "Origin" 1 based
      m_onb--;
      iChanged = 0;

      // For mosaic creation, the input is copied onto mosaic by default
      if(m_priority == band && !m_mosaicOptions.bCreate) {
        BandComparison(iIndex);
      }
    }
    else if(m_priority == average) {
      m_onb /= 2;
    }

    // Process Band Priority with no tracking
    if (m_priority == band && !m_mosaicOptions.bTrack ) {
      BandPriorityWithNoTracking();
    }
    else {
      // Create portal buffers for the input and output files
      Isis::Portal iPortal   (m_ins, 1, InputCubes[0]->getPixelType());
      Isis::Portal oPortal   (m_ins, 1, OutputCubes[0]->getPixelType());
      Isis::Portal origPortal(m_ins, 1, OutputCubes[0]->getPixelType());

      for(int ib = m_isb, ob = m_osb; ib < (m_isb + m_inb) && ob <= m_onb; ib++, ob++) {
        for(int il = m_isl, ol = m_osl; il < m_isl + m_inl; il++, ol++) {
          // Set the position of the portals in the input and output cubes
          iPortal.SetPosition(m_iss, il, ib);
          InputCubes[0]->read(iPortal);

          oPortal.SetPosition(m_oss, ol, ob);
          OutputCubes[0]->read(oPortal);

          if(m_mosaicOptions.bTrack) {
            origPortal.SetPosition(m_oss, ol, iOriginBand);
            OutputCubes[0]->read(origPortal);
          }
          else if(m_priority == average) {
            origPortal.SetPosition(m_oss, ol, (ob+m_onb));
            OutputCubes[0]->read(origPortal);
          }

          bool bChanged = false;
          // Move the input data to the output
          for(int pixel = 0; pixel < oPortal.size(); pixel++) {
            // Creating Mosaic, copy the input onto mosaic
            // regardless of the priority
            if(m_mosaicOptions.bCreate) {
              oPortal[pixel] = iPortal[pixel];
              if(m_mosaicOptions.bTrack) {
                origPortal[pixel] = iIndex;
                bChanged = true;
              }
              else if(m_priority == average) {
                if(Isis::IsValidPixel(iPortal[pixel])) {
                  origPortal[pixel]=1;
                  bChanged = true;
                }
              }
              iChanged++;
            }
            // Band Priority
            else if(m_mosaicOptions.bTrack && m_priority == band) {
              int iPixelOrigin = (int) origPortal[pixel];
              if(iPixelOrigin == iIndex) {
                oPortal[pixel] = iPortal[pixel];
                iChanged++;
                bChanged = true;
              }
            }
            // OnTop/Input Priority
            else if(m_priority == input) {
              if(Isis::IsNullPixel(oPortal[pixel])  ||
                 Isis::IsValidPixel(iPortal[pixel]) ||
                 (m_highSat && Isis::IsHighPixel(iPortal[pixel])) ||
                 (m_lowSat  && Isis::IsLowPixel(iPortal[pixel]))  ||
                 (m_null    && Isis::IsNullPixel(iPortal[pixel]))) {
                oPortal[pixel] = iPortal[pixel];
                if(m_mosaicOptions.bTrack) {
                  origPortal[pixel] = iIndex;
                  bChanged = true;
                }
                iChanged++;
              }
            }
            // Average priority
            else if(m_priority == average) {
              bChanged |= ProcessAveragePriority(pixel, iPortal, oPortal, origPortal);
            }
            // Beneath/Mosaic Priority
            else if(m_priority == mosaic) {
              if(Isis::IsNullPixel(oPortal[pixel])) {
                oPortal[pixel] = iPortal[pixel];
                // Set the origin if number of input bands equal to 1
                // and if the track flag was set
                if(m_mosaicOptions.bTrack) {
                  origPortal[pixel] = iIndex;
                  bChanged = true;
                }
                iChanged++;
              }
            }
          } // End sample loop
          if(bChanged) {
            if(m_mosaicOptions.bTrack || m_priority == average) {
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
   * Match the Shape Model for input and mosaic. If creating the mosaic,
   * copy the input ShapeModel from the input label.
   * Store only the file name of the Shape Model
   *
   * @author Sharmila Prasad (1/24/2011)
   */
  void ProcessMosaic::MatchDEMShapeModel(void)
  {
    Pvl* inLabel  = InputCubes[0]->getLabel();
    Pvl* outLabel = OutputCubes[0]->getLabel();

    if(outLabel->FindObject("IsisCube").HasGroup("Mosaic")) {
      PvlGroup outMosaicGrp = outLabel->FindObject("IsisCube").FindGroup("Mosaic");
      if(outMosaicGrp.HasKeyword("ShapeModel")) {
        if(inLabel->FindObject("IsisCube").HasGroup("Kernels")) {
          PvlGroup inMosaicGrp = inLabel->FindObject("IsisCube").FindGroup("Kernels");
          if(outMosaicGrp.HasKeyword("ShapeModel") && inMosaicGrp.HasKeyword("ShapeModel")) {
            PvlKeyword outShapeModelKey = outMosaicGrp.FindKeyword("ShapeModel");
            string sShapeModel = inMosaicGrp.FindKeyword("ShapeModel")[0];
            //cout <<"Orig in=" << sShapeModel << endl;
            size_t found = sShapeModel.rfind ("/");
            if(found != string::npos) {
              sShapeModel.replace (0, found+1, "\0");
            }
            if(sShapeModel == outShapeModelKey[0]) {
              return;
            }
          }
        }
        std::string sErrMsg = "Input and Mosaic DEM Shape Model do not match";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }
    else {
      if(m_mosaicOptions.bCreate) {
        if(inLabel->FindObject("IsisCube").HasGroup("Kernels")) {
          string sShapeModel = inLabel->FindObject("IsisCube").FindGroup("Kernels").FindKeyword("ShapeModel")[0];
          size_t found = sShapeModel.rfind ("/");
          if(found != string::npos){
            sShapeModel.replace (0, found+1, "\0");
          }
          PvlObject & outIsisCubeObj = outLabel->FindObject("IsisCube");
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
  void ProcessMosaic::ResetCountBands(void)
  {
    int iBand   = OutputCubes[0]->getBandCount();
    int iLines  = OutputCubes[0]->getLineCount();
    int iSample = OutputCubes[0]->getSampleCount();

    Isis::Portal origPortal(iSample, 1, OutputCubes[0]->getPixelType());
    int iStartCountBand = iBand/2 + 1;

    for(int band=iStartCountBand; band<=iBand; band++) {
      for(int i = 1; i <= iLines; i++) {
        origPortal.SetPosition(1, i, band);  //sample, line, band position
        OutputCubes[0]->read(origPortal);
        for(int iPixel = 0; iPixel < origPortal.size(); iPixel++) {
          origPortal[iPixel] = 0;
        }
        OutputCubes[0]->write(origPortal);
      }
    }
  }
  /**
   * Calculate DN value for a pixel for average priority and set the
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
    if(Isis::IsValidPixel(piPortal[piPixel]) && Isis::IsValidPixel(poPortal[piPixel])) {
      int iCount = (int)porigPortal[piPixel];
      double dNewDN = (poPortal[piPixel] * iCount + piPortal[piPixel]) / (iCount + 1);
      poPortal[piPixel] = dNewDN;
      porigPortal[piPixel] =iCount +1;
      bChanged = true;
    }
    // Input-Valid, Mosaic-Special
    else if(Isis::IsValidPixel(piPortal[piPixel])) {
      poPortal[piPixel] = piPortal[piPixel];
      porigPortal[piPixel] = 1;
      bChanged = true;
    }
    // Input-Special, Flags-True
    else if(Isis::IsSpecial(piPortal[piPixel])) {
      if((m_highSat && Isis::IsHighPixel(piPortal[piPixel])) ||
         (m_lowSat  && Isis::IsLowPixel (piPortal[piPixel]))  ||
         (m_null    && Isis::IsNullPixel(piPortal[piPixel]))) {
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
  *  @throws Isis::IException::Message
  *
  *  @author Sharmila Prasad (9/25/2009)
  */
  void ProcessMosaic::MatchBandBinGroup(void) {
    Pvl *inLab  = InputCubes[0]->getLabel();
    Pvl *outLab = OutputCubes[0]->getLabel();

    PvlGroup &inBin  = inLab->FindGroup("BandBin", Pvl::Traverse);
    PvlGroup &outBin = outLab->FindGroup("BandBin", Pvl::Traverse);
    if(inBin.Keywords() != outBin.Keywords()) {
      string msg = "Pvl Group [BandBin] does not match between the input and output cubes";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    //pvl - zero based
    int isb = (m_isb - 1), osb = (m_osb - 1);
    int iOutBandsHalf = OutputCubes[0]->getBandCount()/2;

    for(int i = 0; i < outBin.Keywords(); i++) {
      PvlKeyword &outKey = outBin[i];
      string sOutName = outKey.Name();
      if(inBin.HasKeyword(sOutName)) {
        PvlKeyword &inKey = inBin[sOutName];
        for(int j = osb, k = isb; j < outKey.Size() && k < inKey.Size(); j++, k++) {
//           m_inb++;
          if(outKey[j] == "NA") {
            outKey[j] = inKey[k];
            if(m_priority == average) {
              if(sOutName.find("Filter") != string::npos ||
                 sOutName.find("Name") != string::npos) {
                outKey[j+iOutBandsHalf] = inKey[k] + "_Count";
              }
              else {
                outKey[j+iOutBandsHalf] = "Avg_Count";
              }
            }
          }
          else if(outKey[j] != inKey[k]) {
            string msg = "Pvl Group [BandBin] in Key[" + outKey.Name() + "] In value" + inKey[k] +
                         "and Out value=" + outKey[j] + " do not match";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
      }
      else {
        string msg = "Pvl Group [BandBin] In Keyword[" + inBin[i].Name() + "] and Out Keyword[" + outBin[i].Name() +
                     "] does not match";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
//     m_inb /= outBin.Keywords();

    int inputRange = InputCubes[0]->getBandCount() - isb;
    int outputRange = OutputCubes[0]->getBandCount() - osb;
    m_inb = min(inputRange, outputRange);
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
   *  Average Priority
   */
  void ProcessMosaic::AddBandBinGroup(void) {
    Pvl *inLab  = InputCubes[0]->getLabel();
    Pvl *outLab = OutputCubes[0]->getLabel();

    int iOutBands = OutputCubes[0]->getBandCount();

    if(m_mosaicOptions.bTrack) {
      iOutBands -= 1;     // leave tracking band
    }
    else if(m_priority == average) {
      iOutBands /= 2;
    }

    int isb = m_isb - 1; // array zero based
    int osb = m_osb - 1;

    PvlGroup &cInBin  = inLab->FindGroup("BandBin", Pvl::Traverse);
    PvlGroup cOutBin("BandBin");

    int iInBands = InputCubes[0]->getBandCount();

    for(int i = 0; i < cInBin.Keywords(); i++) {
      PvlKeyword &cInKey = cInBin[i];
      int iInKeySize = cInKey.Size();
      PvlKeyword cOutKey(cInKey.Name());

      for(int b = 0; b < osb; b++) {
        cOutKey += "NA";
      }
      for(int b = osb; b < iOutBands; b++) {
        if(isb < iInKeySize) {
          cOutKey += cInKey[isb++];
        }
        else {
          cOutKey += "NA";
        }
      }

      // Add the "TRACKING" band to the Keyword if the flag is set and also if the number of
      // input cube bands is same as the the keysize of the keyword in the BandBin group.
      if(m_mosaicOptions.bTrack && iInBands == iInKeySize) {
        cOutKey += "TRACKING"; // for the origin band
      }

      // Tag the Count Bands if priority is Average.
      else if(m_priority == average) {
        int iTotalOutBands = OutputCubes[0]->getBandCount();
        isb = m_isb - 1; // reset the input starting band
        int iOutStartBand = iOutBands + osb;
        string sKeyName = cInKey.Name();
        bool bFilterKey = false;
        if(sKeyName.find("Filter") != string::npos ||
           sKeyName.find("Original") != string::npos   ||
           sKeyName.find("Name") != string::npos) {
          bFilterKey = true;
        }
        for(int ob=iOutBands; ob<iTotalOutBands; ob++) {
          if(isb < iInKeySize && ob >= iOutStartBand) {
            if(bFilterKey) {
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
      if(cOutKey.Unit() != cInKey.Unit()) {
        cOutKey.SetUnits((iString)(cInKey.Unit()));
      }

      cOutBin += cOutKey;
      isb = m_isb - 1;        // reinitialize the input starting band
    }
    outLab->FindObject("IsisCube").AddGroup(cOutBin);
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
  void ProcessMosaic::AddDefaultBandBinGroup(void) {
    Pvl *outLab = OutputCubes[0]->getLabel();

    PvlGroup cOutBin("BandBin");

    int iOutBands = OutputCubes[0]->getBandCount();
    int iOutBandsTotal = iOutBands;

    if(m_mosaicOptions.bTrack) {
      iOutBands--; // Leave tracking band
    }
    else if(m_priority == average) {
      iOutBands /= 2;
    }

    PvlKeyword cOutKey("FilterName");

    for(int i=0; i<iOutBands; i++) {
      cOutKey += "NA";
    }

    if(m_priority == average) {
      for(int i=iOutBands; i<iOutBandsTotal; i++) {
        cOutKey += "NA_Count";
      }
    }

    if(m_mosaicOptions.bTrack) {
      cOutKey += "TRACKING";
    }

    cOutBin += cOutKey;

    outLab->FindObject("IsisCube").AddGroup(cOutBin);
  }


  /**
   * Given filetype(input or output) returns the band index in that file for the
   * band info stored in class member trackinfo for band priority
   *
   * @peFileType- Filetype input or output
   *
   * @returns Band Index
   *
   * @throws Isis::iException::Message
   *
   * @author Sharmila Prasad (9/04/2009)
   */
  int ProcessMosaic::GetBandIndex(const FileType &peFileType) {
    bool bFound = false;
    int iBandIndex = 0;

    Pvl cPvlLabel;

    if(peFileType == inFile)
      cPvlLabel = *(InputCubes[0]->getLabel());
    else
      cPvlLabel = *(OutputCubes[0]->getLabel());

    //if non-zero integer, must be original band #, 1 based
    if(m_mosaicOptions.iBandNum) {
      PvlKeyword cKeyOrigBand;
      if(cPvlLabel.FindGroup("BandBin", Pvl::Traverse).HasKeyword("OriginalBand")) {
        cKeyOrigBand = cPvlLabel.FindGroup("BandBin", Pvl::Traverse).FindKeyword("OriginalBand");
      }
      int iSize = cKeyOrigBand.Size();
      char buff[64];
      sprintf(buff, "%d", m_mosaicOptions.iBandNum);
      for(int i = 0; i < iSize; i++) {
        if(std::string(buff) == cKeyOrigBand[i]) {
          iBandIndex = i + 1; //1 based get band index
          bFound = true;
          break;
        }
      }
    }
    //key name
    else {
      PvlKeyword cKeyName;
      if(cPvlLabel.FindGroup("BandBin", Pvl::Traverse).HasKeyword(m_mosaicOptions.sKeyName)) {
        cKeyName = cPvlLabel.FindGroup("BandBin", Pvl::Traverse).FindKeyword(m_mosaicOptions.sKeyName);
      }
      int iSize = cKeyName.Size();
      for(int i = 0; i < iSize; i++) {
        if(Isis::iString::Equal(m_mosaicOptions.sKeyValue.c_str(), cKeyName[i].c_str())) {
          iBandIndex = i + 1; //1 based get key value index
          bFound = true;
          break;
        }
      }
    }
    if(!bFound) {
      string msg = "Invalid Band / Key Name, Value ";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(peFileType == inFile)
      m_mosaicOptions.iInBand = iBandIndex;
    else
      m_mosaicOptions.iOutBand = iBandIndex;

    return  iBandIndex;
  }

  /**
   * This method compares the specified band of the input and
   * output using the criteria (lesser or greater) to assign the
   * pixel origin(input fileindex) to the origin band. In the StartProcess(),
   * input pixel is assigned to the output if the origin pixel equals the current
   * input file index
   *
   * @index     - Filename Index for the origin band (default +
   *                zero based index)
   *
   * @throws Isis::IException::Message
   *
   * @author Sharmila Prasad (9/04/2009)
   */
  void ProcessMosaic::BandComparison(int index) {
    //
    // Create portal buffers for the input and output files
    Isis::Portal cIportal(m_ins, 1, InputCubes[0]->getPixelType());
    Isis::Portal cOportal(m_ins, 1, OutputCubes[0]->getPixelType());
    Isis::Portal origPortal(m_ins, 1, OutputCubes[0]->getPixelType());

    //Get the last band set aside for "Origin"
    int iOriginBand = OutputCubes[0]->getBandCount();

    for(int iIL = m_isl, iOL = m_osl; iIL < m_isl + m_inl; iIL++, iOL++) {
      // Set the position of the portals in the input and output cubes
      cIportal.SetPosition(m_iss, iIL, m_mosaicOptions.iInBand);
      InputCubes[0]->read(cIportal);

      cOportal.SetPosition(m_oss, iOL, m_mosaicOptions.iOutBand);
      OutputCubes[0]->read(cOportal);

      origPortal.SetPosition(m_oss, iOL, iOriginBand);
      OutputCubes[0]->read(origPortal);

      // Move the input data to the output
      for(int iPixel = 0; iPixel < cOportal.size(); iPixel++) {
        if(Isis::IsNullPixel(origPortal[iPixel]) ||
            (m_highSat && Isis::IsHighPixel(cIportal[iPixel])) ||
            (m_lowSat  && Isis::IsLowPixel(cIportal[iPixel])) ||
            (m_null    && Isis::IsNullPixel(cIportal[iPixel]))) {
          origPortal[iPixel] = index;
        }
        else {
          if(Isis::IsValidPixel(cIportal[iPixel])) {
            if(Isis::IsSpecial(cOportal[iPixel]) ||
                (m_mosaicOptions.eCriteria == Lesser  && cIportal[iPixel] < cOportal[iPixel]) ||
                (m_mosaicOptions.eCriteria == Greater && cIportal[iPixel] > cOportal[iPixel])) {
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
  void ProcessMosaic::BandPriorityWithNoTracking(void) {
    // Create portal buffers for the input and output files pointing to the
    // specified band for comparison
    Isis::Portal iComparePortal(m_ins, 1, InputCubes[0]->getPixelType());
    Isis::Portal oComparePortal(m_ins, 1, OutputCubes[0]->getPixelType());

    Isis::Portal resultsPortal (m_ins, 1, OutputCubes[0]->getPixelType());

    // Create portal buffers for the input and output files
    Isis::Portal iPortal(m_ins, 1, InputCubes[0]->getPixelType());
    Isis::Portal oPortal(m_ins, 1, OutputCubes[0]->getPixelType());

    for(int inLine = m_isl, outLine = m_osl; inLine < m_isl + m_inl; inLine++, outLine++) {
      // Set the position of the portals in the input and output cubes
      iComparePortal.SetPosition(m_iss, inLine, m_mosaicOptions.iInBand);
      InputCubes[0]->read(iComparePortal);

      oComparePortal.SetPosition(m_oss, outLine, m_mosaicOptions.iOutBand);
      OutputCubes[0]->read(oComparePortal);

      Isis::Portal iPortal(m_ins, 1, InputCubes[0]->getPixelType());
      Isis::Portal oPortal(m_ins, 1, OutputCubes[0]->getPixelType());

      bool inCopy = false;

      // Move the input data to the output
      for(int iPixel = 0; iPixel < m_ins; iPixel++) {
        resultsPortal[iPixel] = false;
        if(m_mosaicOptions.bCreate) {
          resultsPortal[iPixel] = true;
          inCopy = true;
        }
        else if((m_highSat && Isis::IsHighPixel(iComparePortal[iPixel])) ||
            (m_lowSat  && Isis::IsLowPixel (iComparePortal[iPixel]))  ||
            (m_null    && Isis::IsNullPixel(iComparePortal[iPixel]))) {
          resultsPortal[iPixel] = true;
          inCopy = true;
        }
        else {
          if(Isis::IsValidPixel(iComparePortal[iPixel])) {
            if(Isis::IsSpecial(oComparePortal[iPixel]) ||
                (m_mosaicOptions.eCriteria == Lesser  && iComparePortal[iPixel] < oComparePortal[iPixel]) ||
                (m_mosaicOptions.eCriteria == Greater && iComparePortal[iPixel] > oComparePortal[iPixel])) {
              resultsPortal[iPixel] = true;
              inCopy = true;
            }
          }
        }
      }

      if(inCopy) {
        for(int ib = m_isb, ob = m_osb; ib < (m_isb + m_inb) && ob <= m_onb; ib++, ob++) {
          // Set the position of the portals in the input and output cubes
          iPortal.SetPosition(m_iss, inLine, ib);
          InputCubes[0]->read(iPortal);

          oPortal.SetPosition(m_oss, outLine, ob);
          OutputCubes[0]->read(oPortal);

          for(int iPixel = 0; iPixel < m_ins; iPixel++) {
            if(resultsPortal[iPixel]) {
              oPortal[iPixel] = iPortal[iPixel];
            }
          }
          OutputCubes[0]->write(oPortal);
        }
      }
    }
  }

  /**
   * Debugging
   *
   * @author Sharmila Prasad (9/2/2009)
   */
  void ProcessMosaic::Test(void) {
    int iBand    = OutputCubes[0]->getBandCount();
    int iLines   = OutputCubes[0]->getLineCount();
    int iSamples = OutputCubes[0]->getSampleCount();
    int iFileIndex, iFileCount;

    Isis::Portal origPortal(iSamples, 1, OutputCubes[0]->getPixelType());
    if(m_priority == average) {
      for(int line=1; line<=iLines; line++) {
        origPortal.SetPosition(1, line, iBand/2+1);  //sample, line, band position
        OutputCubes[0]->read(origPortal);
        for(int iPixel=0; iPixel<origPortal.size(); iPixel++) {
          iFileCount = (int)origPortal[iPixel];
        }
        if(line >= 2) {
          break;
        }
      }
    }
    else {
      int  iOffset = GetOriginDefaultByPixelType();

      for(int line=1; line<=iLines; line++) {
        origPortal.SetPosition(1, line, iBand);  //sample, line, band position
        OutputCubes[0]->read(origPortal);
        for(int iPixel=0; iPixel<origPortal.size(); iPixel++) {
          iFileIndex = (int)origPortal[iPixel] - iOffset;
        }
        if(line >= 2) {
          break;
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
   * @throws Isis::iException::Message
   *
   * @author Sharmila Prasad (8/28/2009)
   */
  int ProcessMosaic::GetIndexOffsetByPixelType(void) {
    int iOffset = 0;

    switch(SizeOf(OutputCubes[0]->getPixelType())) {
      case 1:
        iOffset = VALID_MIN1;
        break;

      case 2:
        iOffset = VALID_MIN2;
        break;

      case 4:
        iOffset = FLOAT_MIN;
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
   * @throws Isis::IException::Message
   *
   * @author Sharmila Prasad (9/10/2009)
   */
  int ProcessMosaic::GetOriginDefaultByPixelType(void) {
    int iDefault;

    switch(SizeOf(OutputCubes[0]->getPixelType())) {
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
        string msg = "ProcessMosaic::GetOriginDefaultByPixelType - Invalid Pixel Type";
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
  void ProcessMosaic::ResetOriginBand(void) {
    int iBand   = OutputCubes[0]->getBandCount();
    int iLines  = OutputCubes[0]->getLineCount();
    int iSample = OutputCubes[0]->getSampleCount();

    int iDefault = GetOriginDefaultByPixelType();

    Isis::Portal origPortal(iSample, 1, OutputCubes[0]->getPixelType());

    for(int i = 1; i <= iLines; i++) {
      origPortal.SetPosition(1, i, iBand);  //sample, line, band position
      OutputCubes[0]->read(origPortal);
      for(int iPixel = 0; iPixel < origPortal.size(); iPixel++) {
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
  bool ProcessMosaic::GetTrackStatus(void) {
    //get the output label
    Pvl *cPvlOut = OutputCubes[0]->getLabel();

    bool bTableExists = false;
    int iNumObjs = cPvlOut->Objects();
    PvlObject cPvlObj;

    //Check if table already exists
    if(cPvlOut->HasObject("Table")) {
      for(int i = 0; i < iNumObjs; i++) {
        cPvlObj = cPvlOut->Object(i);
        if(cPvlObj.HasKeyword("Name", Pvl::Traverse)) {
          PvlKeyword cNameKey = cPvlObj.FindKeyword("Name", Pvl::Traverse);
          if(cNameKey[0] == SRC_IMAGE_TBL) {
            bTableExists = true;
          }
        }
      }
    }

    return bTableExists;
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
   * @throws Isis::iException::Message
   *
   * @author Sharmila Prasad (8/28/2009)
   */
  void ProcessMosaic::SetMosaicOrigin(int &index) {
    // Get only the file name
    std::string sInputFile = Filename(InputCubes[0]->getFilename()).Name();
    std::string sTableName = SRC_IMAGE_TBL;

    // Get the serial number
    std::string sSerialNumber = SerialNumber::Compose(*(InputCubes[0]));
    int iFileNameLen  = sInputFile.length();
    int iSerialNumLen = sSerialNumber.length();
    int iFieldLength = iSerialNumLen;
    if(iFileNameLen > iSerialNumLen) {
      iFieldLength = iFileNameLen;
    }

    // Get output file name
    std::string sOutputFile = Filename(OutputCubes[0]->getFilename()).Name();

    Pvl *cPvlOut = OutputCubes[0]->getLabel();

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

    int iNumObjs = cPvlOut->Objects();
    PvlObject cPvlObj;

    // Check if the Table exists
    if(cPvlOut->HasObject("Table")) {
      for(int i = 0; i < iNumObjs; i++) {
        cPvlObj = cPvlOut->Object(i);
        if(cPvlObj.HasKeyword("Name", Pvl::Traverse)) {
          PvlKeyword cNameKey = cPvlObj.FindKeyword("Name", Pvl::Traverse);
          if(cNameKey[0] == sTableName) {
            PvlKeyword cFieldKey = cPvlObj.FindGroup("Field").FindKeyword("Size");

            //set the tracker flag to true as the tracking table exists
            m_mosaicOptions.bTrack = true;

            // Create a new blank table
            Table cFileTable(sTableName);

            // Read and make a copy of the existing tracking table
            Table cFileTable_Copy = Table(sTableName);
            OutputCubes[0]->read(cFileTable_Copy);

            // Records count
            int iRecs = cFileTable_Copy.Records();

            // Check if the image index can be accomadated in the pixel size
            bool bFull = false;
            switch(sizeof(OutputCubes[0]->getPixelType())) {
              case 1:
                if(iRecs >= (VALID_MAX1 - 1))                // Index is 1 based as 0=Null invalid value
                  bFull = true;
                break;
              case 2:
                if(iRecs > (VALID_MAX2 - VALID_MIN2 + 1))    // Signed 16bits with some special pixels
                  bFull = true;
                break;

              case 4:
                if(iRecs > (FLOAT_MAX - FLOAT_MIN + 1))      // Max float mantissa
                  bFull = true;
                break;
            }

            if(bFull) {
              string msg = "The number of images in the Mosaic exceeds the pixel size";
              throw IException(IException::Programmer, msg, _FILEINFO_);
            }

            for(int i = 0; i < iRecs; i++) {
              // Get the file name and trim out the characters filled due to resizing
              string sTableFile = string(cFileTable_Copy[i][0]);
              size_t found = sTableFile.rfind(".cub");
              if(found != string::npos) {
                sTableFile.erase(found + 4); // clear the packing characters - get only the file name
              }

              // ostm << "File=" << string(cFileTable_Copy[i][0]).c_str() << "   Length=" << string(cFileTable_Copy[i][0]).length() << "  Compare=" << str.compare(sInputFile) << "\n";
              if(sTableFile.compare(sInputFile) == 0) {
                index += i;
                return;
              }

              // To initialise the new table, on the first file name comparison,
              // check the size of the existing table record with the size of the new record being added
              if(!i) {
                if(iString(cFieldKey[0]).ToInteger() < iFieldLength) {
                  TableRecord cFileRecordUpdate;
                  TableField cFileFieldUpdate("FileName", TableField::Text, iFieldLength);
                  cFileFieldUpdate = (std::string)cFileTable_Copy[i][0];
                  cFileRecordUpdate += cFileFieldUpdate;

                  // Populate with Serial Number
                  TableField cSNFieldUpdate("SerialNumber", TableField::Text, iFieldLength);
                  cSNFieldUpdate = (std::string)cFileTable_Copy[i][1];
                  cFileRecordUpdate += cSNFieldUpdate;
                  cFileTable = Table(sTableName, cFileRecordUpdate);  // add new record and set the size for all the other records
                }
                else {
                  cFileTable = Table(sTableName, cFileTable_Copy[i]);
                }
              }

              // Add the existing records into the new table
              cFileTable += cFileTable_Copy[i];
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
    if(m_mosaicOptions.bCreate && m_mosaicOptions.bTrack) {
      Table cFileTable(sTableName, cFileRecord);
      cFileTable += cFileRecord;
      OutputCubes[0]->write(cFileTable);
      //reset the origin band based on pixel type
      ResetOriginBand();
    }
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
   * @throws Isis::IException::Message
   */
  Isis::Cube *ProcessMosaic::SetInputCube(const std::string &parameter,
                                          const int ss, const int sl, const int sb,
                                          const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
    if(InputCubes.size() > 0) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    m_iss = ss;
    m_isl = sl;
    m_isb = sb;
    m_ins = ns;
    m_inl = nl;
    m_inb = nb;

    Isis::Cube *cInCube = Isis::Process::SetInputCube(parameter);

    //get the output label
    Pvl *cInPvl = InputCubes[0]->getLabel();
    if(cInPvl->FindGroup("Dimensions", Pvl::Traverse).HasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->FindGroup("Dimensions", Pvl::Traverse).FindKeyword("Bands");
      iString sStr(cBandKey[0]);
      if(sStr.ToInteger() < nb) {
        string m = "The parameter number of input bands exceeds the actual number of bands in the input cube";
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
   * @throws Isis::IException
   */
  Isis::Cube *ProcessMosaic::SetInputCube(const std::string &fname,
                                          Isis::CubeAttributeInput &att,
                                          const int ss, const int sl, const int sb,
                                          const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
    if(InputCubes.size() > 0) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    m_iss = ss;
    m_isl = sl;
    m_isb = sb;
    m_ins = ns;
    m_inl = nl;
    m_inb = nb;

    Isis::Cube *cInCube = Isis::Process::SetInputCube(fname, att);

    //check if the number of bands specified is not greater than the actual number of bands in the input
    Pvl *cInPvl = InputCubes[0]->getLabel();
    if(cInPvl->FindGroup("Dimensions", Pvl::Traverse).HasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->FindGroup("Dimensions", Pvl::Traverse).FindKeyword("Bands");
      iString sStr(cBandKey[0]);
      if(sStr.ToInteger() < nb) {
        string m = "The parameter number of input bands exceeds the actual number of bands in the input cube";
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
   * @throws Isis::IException::Message
   */
  Isis::Cube *ProcessMosaic::SetOutputCube(const std::string &psParameter) {

    // Make sure there is only one output cube
    if(OutputCubes.size() > 0) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Attempt to open a cube ... get the filename from the user parameter
    // (e.g., "TO") and the cube size from an input cube
    Isis::Cube *cube = new Isis::Cube;
    try {
      string fname = Application::GetUserInterface().GetFilename(psParameter);
      cube->open(fname, "rw");
    }
    catch(IException &) {
      delete cube;
      throw;
    }

    if(m_mosaicOptions.bCreate) {
      Pvl *outLab = cube->getLabel();
      if(outLab->FindObject("IsisCube").HasGroup("BandBin")) {
        outLab->FindObject("IsisCube").DeleteGroup("BandBin");
      }
    }

    // Everything is fine so save the cube on the stack
    OutputCubes.push_back(cube);
    return cube;
  }

} // end namespace isis

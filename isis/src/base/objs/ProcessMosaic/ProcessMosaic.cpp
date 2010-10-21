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

#include "Portal.h"
#include "ProcessMosaic.h"
#include "SpecialPixel.h"
#include "iException.h"
#include "Application.h"
#include "Pvl.h"
#include "Table.h"
#include "SerialNumber.h"
#include "iString.h"

using namespace std;

//#define _DEBUG_

namespace Isis {

#ifdef _DEBUG_
  //debugging
  fstream ostm;

  void StartDebug() {
    ostm.open("Debug.log", std::ios::out | std::ios::app);
    ostm << "\n*************************\n";
  }

  void CloseDebug() {
    ostm << "\n*************************\n";
    ostm.close();
  }
#endif

  /**
   * ProcessMosaic Contructor
   *
   * Initialize the class members to default
   */
  ProcessMosaic::ProcessMosaic() {

    // Set the BandBin Match
    SetBandBinMatch(true);

    // Initialize the structure Track Info
    mtTrackInfo.bTrack    = false;
    mtTrackInfo.bCreate   = false;
    mtTrackInfo.iBandNum  = 0;
    mtTrackInfo.sKeyName  = "";
    mtTrackInfo.sKeyValue = "";
    mtTrackInfo.eCriteria = Lesser;
    mtTrackInfo.iInBand   = 0;
    mtTrackInfo.iOutBand  = 0;

    // Initialize the Special Pixel Flags
    mbHighSat = false;
    mbLowSat = false;
    mbNull = false;

    // Default Priority OnTop
    mePriority = input;

    // Initialize the data members
    miOss = -1;
    miOsl = -1;
    miOsb = -1;

#ifdef _DEBUG_
    StartDebug();
#endif
  };

  /**
   * ProcessMosaic Destructor
   *
   */
  ProcessMosaic::~ProcessMosaic() {
#ifdef _DEBUG_
    CloseDebug();
#endif
  }


  /**
   * This method invokes the process by mosaic operation over a single input cube
   * and single output cube. Unlike other Isis process objects, no application .
   * function will be called. The processing is handled entirely within the
   * mosaic object. The input cube must be pixel aligned with the output cube
   * before mosaiking. If the input cube does not overlay any of the output cube,
   * no processing takes place and no errors are thrown.
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
   * @param priority This parameter determines which cube takes priority
   *                 when both the input and output cubes contain non null
   *                 data. There ara two possible values (input and mosaic).
   *                 When this parameter is set to input all non null pixels
   *                 will be transfered to the mosaic. Null pixels will not
   *                 be transfered. When this parameter is set to mosaic
   *                 input pixel values will only be transfered to the mosaic
   *                 when the mosaic contains a null value.
   *
   * @throws Isis::iException::Message
   */
  void ProcessMosaic::StartProcess(const int &os, const int &ol,
                                   const int &ob, const MosaicPriority &priority) {

    int outSample = os;
    int outLine = ol;
    int outFile = ob;

    // Error checks ... there must be one input and one output
    if((OutputCubes.size() != 1) || (InputCubes.size() != 1)) {
      string m = "You must specify exactly one input and one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    // Check to make sure the bandbins match if necessary
    if(mbBandbinMatch) {
      Pvl *inLab = InputCubes[0]->Label();
      Pvl *outLab = OutputCubes[0]->Label();

      if(inLab->FindObject("IsisCube").HasGroup("BandBin")) {
        PvlGroup &inBin = inLab->FindGroup("BandBin", Pvl::Traverse);
        // Check to make sure the output cube has a bandbin group & make sure it
        // matches the input cube bandbin group
        if(outLab->FindObject("IsisCube").HasGroup("BandBin")) {
          PvlGroup &outBin = outLab->FindGroup("BandBin", Pvl::Traverse);
          if(inBin.Keywords() != outBin.Keywords()) {
            string msg = "Pvl Group [BandBin] does not match between the input and output cubes";
            throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
          }
          for(int i = 0; i < outBin.Keywords(); i++) {
            PvlKeyword &outKey = outBin[i];
            if(inBin.HasKeyword(outKey.Name())) {
              PvlKeyword &inKey = inBin[outKey.Name()];
              for(int v = 0; v < outKey.Size(); v++) {
                if(outKey[v] != inKey[v]) {
                  string msg = "Pvl Group [BandBin] Keyword[" + outBin[i].Name() +
                               "] does not match between the input and output cubes";
                  throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
                }
              }
            }
            else {
              string msg = "Pvl Group [BandBin] Keyword[" + outBin[i].Name() +
                           "] does not match between the input and output cubes";
              throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
            }
          }
        }
        // Otherwise copy the input cube bandbin to the output file
        else {
          outLab->FindObject("IsisCube").AddGroup(inBin);
        }
      }
    }

    if((outFile < 1) || ((outFile + p_inb - 1) > OutputCubes[0]->Bands())) {
      string m = "All bands from the input cube must fit within the output mosaic";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    // Set up the input sub cube (This must be a legal sub-area of the input cube)
    int ins = p_ins;
    int inl = p_inl;
    int inb = p_inb;

    if(ins == 0) ins = (int)InputCubes[0]->Samples();
    if(inl == 0) inl = (int)InputCubes[0]->Lines();
    if(inb == 0) inb = (int)InputCubes[0]->Bands();

    // Adjust the input sub-area if it overlaps any edge of the output cube

    int iss = p_iss;
    int isl = p_isl;

    // Left edge
    if(outSample < 1) {
      iss = iss - outSample + 1;
      ins = ins + outSample - 1;
      outSample = 1;
    }
    // Top edge
    if(outLine < 1) {
      isl = isl - outLine + 1;
      inl = inl + outLine - 1;
      outLine = 1;
    }
    // Right edge
    if((outSample + ins - 1) > OutputCubes[0]->Samples()) {
      ins = OutputCubes[0]->Samples() - outSample + 1;
    }
    // Bottom edge
    if((outLine + inl - 1) > OutputCubes[0]->Lines()) {
      inl = OutputCubes[0]->Lines() - outLine + 1;
    }

    // Tests for completly off the mosaic
    if((ins < 1) || (inl < 1)) {
      string m = "The input cube does not overlap the mosaic";
      throw Isis::iException::Message(Isis::iException::User, m, _FILEINFO_);
    }

    // Create portal buffers for the input and output files
    Isis::Portal iportal(ins, 1, InputCubes[0]->PixelType());
    Isis::Portal oportal(ins, 1, OutputCubes[0]->PixelType());

    // Start the progress meter
    p_progress->SetMaximumSteps(inl * inb);
    p_progress->CheckStatus();

    for(int ib = p_isb, ob = outFile; ib < p_isb + inb; ib++, ob++) {
      for(int il = isl, ol = outLine; il < isl + inl; il++, ol++) {
        // Set the position of the portals in the input and output cubes
        iportal.SetPosition(iss, il, ib);
        InputCubes[0]->Read(iportal);

        oportal.SetPosition(outSample, ol, ob);
        OutputCubes[0]->Read(oportal);

        // Move the input data to the output
        for(int pixel = 0; pixel < oportal.size(); pixel++) {
          if(priority == input) {
            if(!Isis::IsNullPixel(iportal[pixel])) oportal[pixel] = iportal[pixel];
          }
          else if(priority == mosaic) {
            if(Isis::IsNullPixel(oportal[pixel])) oportal[pixel] = iportal[pixel];
          }
        } // End sample loop

        OutputCubes[0]->Write(oportal);
        p_progress->CheckStatus();
      } // End line loop
    } // End band loop
  } // End StartProcess

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
  * @throws Isis::iException::Message
  *
  * @author sprasad (8/25/2009)
  */
  void ProcessMosaic::StartProcess(const int &os, const int &ol, const int &ob) {
    int outSample = os;
    int outLine   = ol;
    int outFile   = ob;


#ifdef _DEBUG_
    ostm << "\n*********** ProcessMosaic::StartProcess **********\n";
    ostm << "** Parameters **\n";
    ostm << "priority=" << mePriority << "  outsample=" << os << "  outline=" << ol << "  outBand=" << ob << "\n\n";
#endif

    // Error checks ... there must be one input and one output
    if((OutputCubes.size() != 1) || (InputCubes.size() != 1)) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string m = "You must specify exactly one input and one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    bool bTrackExists = false;
    if(!mtTrackInfo.bCreate) {
      bTrackExists = GetTrackStatus();
      if(!bTrackExists && mePriority == band) {
#ifdef _DEBUG_
        CloseDebug();
#endif
        string m = "Band cannot be a priority if Track Origin is not set";
        throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
      }
    }

    // **
    // Set up the input sub cube (This must be a legal sub-area of the input cube)
    // *
    int ins = p_ins;
    int inl = p_inl;
    int inb = p_inb;

    if(ins == 0) ins = (int)InputCubes[0]->Samples();
    if(inl == 0) inl = (int)InputCubes[0]->Lines();
    if(inb == 0) inb = (int)InputCubes[0]->Bands();

    // Adjust the input sub-area if it overlaps any edge of the output cube
    int iss = p_iss;
    int isl = p_isl;
    int isb = p_isb;

    miOss = outSample;
    miOsl = outLine;
    miOsb = outFile;

    // Left edge
    if(outSample < 1) {
      miOss = outSample - 1;
      iss = iss - outSample + 1;
      ins = ins + outSample - 1;
      outSample = 1;
    }
    // Top edge
    if(outLine < 1) {
      miOsl = outLine - 1;
      isl = isl - outLine + 1;
      inl = inl + outLine - 1;
      outLine = 1;
    }
    // Right edge
    if((outSample + ins - 1) > OutputCubes[0]->Samples()) {
      ins = OutputCubes[0]->Samples() - outSample + 1;
    }
    // Bottom edge
    if((outLine + inl - 1) > OutputCubes[0]->Lines()) {
      inl = OutputCubes[0]->Lines() - outLine + 1;
    }

    // Tests for completly off the mosaic
    if((ins < 1) || (inl < 1)) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string m = "The input cube does not overlap the mosaic";
      throw Isis::iException::Message(Isis::iException::User, m, _FILEINFO_);
    }

    // Band Adjustments
    if(outFile < 1) {
      miOsb = outFile - 1;
      isb = isb - outFile + 1;
      inb = inb + outFile - 1;
      outFile = 1;
    }

    p_progress->SetMaximumSteps((int)InputCubes[0]->Lines() * (int)InputCubes[0]->Bands());
    p_progress->CheckStatus();

#ifdef _DEBUG_
    ostm << "\n*** Input Stats ***  PixelType=" << InputCubes[0]->PixelType()  << "\nBands Start=" << p_isb << "     Number=" << inb << "\n";
    ostm << "Samples Start="  << iss << "   Number=" << ins << "\n";
    ostm << "Lines Start="    << isl << "   Number="   << inl << "\n";


    ostm << "*** Output Stats ***\n" << "Bands Start=" << outFile << "   Number=" << OutputCubes[0]->Bands() << "\n";
    ostm << "Start Samples= " << outSample << " Lines= " << outLine   << "\n";

#endif
    // *******************************************************************************

    // Check to make sure the bandbins match if necessary
    if(mbBandbinMatch) {
      Pvl *inLab  = InputCubes[0]->Label();
      Pvl *outLab = OutputCubes[0]->Label();

#ifdef _DEBUG_
      ostm << "BandBinMatch Flag is set to true\n";
#endif
      if(inLab->FindObject("IsisCube").HasGroup("BandBin")) {
        // Check to make sure the output cube has a bandbin group & make sure it
        // matches the input cube bandbin group
        if(!mtTrackInfo.bCreate && outLab->FindObject("IsisCube").HasGroup("BandBin")) {
          inb = 0;
#ifdef _DEBUG_
          ostm << "\n*** Matching Mosaic label ***\n";
          ostm << "isb=" << isb << "   outFile=" << outFile << "   inb=" << inb << "\n";
#endif
          MatchBandBinGroup(isb, outFile, inb);
        }
        // Otherwise copy the input cube bandbin to the output file
        else {
#ifdef _DEBUG_
          ostm << "\n*** Creating Mosaic label ***\n";
#endif
          AddBandBinGroup(isb, outFile);
        }
      }
      // BandBin group is not found
      else {
        string m = "Match BandBin cannot be True when the image does not have the BandBin group";
        throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
      }
    }
    // Match BandBin set to false and CREATE and TRACKING is true
    else {
      if(mtTrackInfo.bCreate){
        AddBandBinGroup(isb, outFile);
      }
    }

    // Even if the track flag is off, if the track table exists continue tracking
    if(bTrackExists) {
      mtTrackInfo.bTrack = true;
    }

    int iOriginBand = 0, iChanged = 0;

    // Do this before SetMosaicOrigin as we don't want to set the filename
    // in the table unless the band info is valid
    if(mePriority == band) {
      // Find the band to be compared
      FileType eFileType = FileType(0);
      GetBandIndex(eFileType);
      eFileType = FileType(1);
      GetBandIndex(eFileType);
    }

    // Image name into the table & Get the index for this input file
    int iInNumBands = inb - isb + 1; // actual number of inpur bands being transfered to mosaic
    int iIndex = GetIndexOffsetByPixelType();

    // Tracking is done for:
    // (1) Band priority,
    // (2) Ontop and Beneath priority with number of bands equal to 1,
    // (3) Ontop priority with all the special pixel flags set to true
    if(mtTrackInfo.bTrack) {
      if(mePriority == band  ||
          ((mePriority == input || mePriority == mosaic) && iInNumBands == 1) ||
          (mePriority == input && mbHighSat && mbLowSat && mbNull)) {
        SetMosaicOrigin(iIndex);
      }
      else {
        mtTrackInfo.bTrack = false;
      }
    }

#ifdef _DEBUG_
    ostm << "Track=" << mtTrackInfo.bTrack << "   Index=" << iIndex << "  Input Bands=" << iInNumBands << "\n";
#endif

    int iOutNumBands = OutputCubes[0]->Bands();

    if(mtTrackInfo.bTrack) {
      iOriginBand = OutputCubes[0]->Bands(); //Get the last band set aside for "Origin" 1 based
      iOutNumBands--;
      iChanged = 0;
#ifdef _DEBUG_
      ostm << "iIndex=" << iIndex << "  iOriginBand=" << iOriginBand << "   Priority=" << mePriority << "\n*********************\n";
#endif

      // For mosaic creation, the input is copied onto mosaic by default
      if(mePriority == band && !mtTrackInfo.bCreate) {
        BandComparison(iIndex, ins, inl, iss, isl, outSample, outLine);
      }
    }

    // Create portal buffers for the input and output files
    Isis::Portal iportal(ins, 1, InputCubes[0]->PixelType());
    Isis::Portal oportal(ins, 1, OutputCubes[0]->PixelType());
    Isis::Portal cOrgPortal(ins, 1, OutputCubes[0]->PixelType());

#ifdef _DEBUG_
    ostm << "\n*** Input Stats ***\n" << "Bands Start=" << p_isb << "   Number=" << inb << "  Actual#=" << iInNumBands << "\n";
    ostm << "Samples Start="   << iss << "   Number="  << ins   << "\n";
    ostm << "Lines Start="     << isl << "   Number="  << inl   << "\n";

    ostm << "*** Output Stats ***\n" << "Bands Start=" << outFile << "   Number=" << OutputCubes[0]->Bands() << "\n";
    ostm << "Start Samples= " << outSample << " Lines= " << outLine   << "\n";
#endif

    for(int ib = isb, ob = outFile; ib < (isb + inb) && ob <= iOutNumBands; ib++, ob++) {
#ifdef _DEBUG_
      //ostm <<"\nBand Input=" << ib << "    Output=" << ob << "   priority=" << mePriority << "\n";
#endif
      for(int il = isl, ol = outLine; il < isl + inl; il++, ol++) {
#ifdef _DEBUG_
        //ostm <<"Line Input=" << il << "    Output=" << ol << "   priority=" << mePriority << "\n";
#endif

        // Set the position of the portals in the input and output cubes
        iportal.SetPosition(iss, il, ib);
        InputCubes[0]->Read(iportal);

        oportal.SetPosition(outSample, ol, ob);
        OutputCubes[0]->Read(oportal);

        if(mtTrackInfo.bTrack) {
          cOrgPortal.SetPosition(outSample, ol, iOriginBand);
          OutputCubes[0]->Read(cOrgPortal);
        }

        bool bChanged = false;
        // Move the input data to the output
        for(int pixel = 0; pixel < oportal.size(); pixel++) {
          // Creating Mosaic, copy the input onto mosaic
          // regardless of the priority
          if(mtTrackInfo.bCreate) {
            oportal[pixel] = iportal[pixel];
            if(mtTrackInfo.bTrack) {
              cOrgPortal[pixel] = iIndex;
              bChanged = true;
            }
            iChanged++;
          }
          // Band Priority
          else if(mtTrackInfo.bTrack && mePriority == band) {
            int iPixelOrigin = GetPixelOrigin(ol, outSample, pixel);
            if(iPixelOrigin == iIndex) {
              oportal[pixel] = iportal[pixel];
              iChanged++;
              bChanged = true;
            }
          }
          // OnTop/Input Priority
          else if(mePriority == input) {
            if(Isis::IsNullPixel(oportal[pixel])  ||
                Isis::IsValidPixel(iportal[pixel]) ||
                mbHighSat && Isis::IsHighPixel(iportal[pixel]) ||
                mbLowSat  && Isis::IsLowPixel(iportal[pixel])  ||
                mbNull    && Isis::IsNullPixel(iportal[pixel])) {
              oportal[pixel] = iportal[pixel];
              if(mtTrackInfo.bTrack) {
                cOrgPortal[pixel] = iIndex;
                bChanged = true;
              }
              iChanged++;
            }
          }
          // Beneath/Mosaic Priority
          else if(mePriority == mosaic) {
            if(Isis::IsNullPixel(oportal[pixel])) {
              oportal[pixel] = iportal[pixel];
              // Set the origin if number of input bands equal to 1 and if the track flag was set
              if(mtTrackInfo.bTrack) {
                cOrgPortal[pixel] = iIndex;
                bChanged = true;
              }
              iChanged++;
            }
          }
        } // End sample loop

        if(mtTrackInfo.bTrack && bChanged) {
          OutputCubes[0]->Write(cOrgPortal);
        }
        OutputCubes[0]->Write(oportal);
        p_progress->CheckStatus();
      } // End line loop
    }   // End band loop

    if(mtTrackInfo.bTrack) {
#ifdef _DEBUG_
      ostm << "\n****** Changed=" << iChanged << "  *************\n";
      //Test();
#endif
    }
  } // End StartProcess


  /**
  *  This method matches the input BandBin group to the mosaic BandBin Group
  *  and allows band to be replaced in mosaic if it is NA (not assigned).
  *  It expects the bands to be contiguous
  *
  *  @piIsb - Input starting Band#
  *  @piOsb - Output starting Band#
  *  @piInb - Actual number of bands matching the mosaic
  *
  *  returns None
  *
  *  @throws Isis::iException::Message
  *
  *  @author sprasad (9/25/2009)
  */
  void ProcessMosaic::MatchBandBinGroup(const int piIsb, const int piOsb, int &piInb) {
    Pvl *inLab  = InputCubes[0]->Label();
    Pvl *outLab = OutputCubes[0]->Label();

    PvlGroup &inBin  = inLab->FindGroup("BandBin", Pvl::Traverse);
    PvlGroup &outBin = outLab->FindGroup("BandBin", Pvl::Traverse);
    if(inBin.Keywords() != outBin.Keywords()) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string msg = "Pvl Group [BandBin] does not match between the input and output cubes";
      throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
    }

    //zero based
    int iIsb = (piIsb - 1), iOsb = (piOsb - 1);

    for(int i = 0; i < outBin.Keywords(); i++) {
      PvlKeyword &outKey = outBin[i];
      if(inBin.HasKeyword(outKey.Name())) {
        PvlKeyword &inKey = inBin[outKey.Name()];
        for(int j = iOsb, k = iIsb; j < outKey.Size() && k < inKey.Size(); j++, k++) {
          piInb++;
          if(outKey[j] == "NA") {
            outKey[j] = inKey[k];
          }
          else if(outKey[j] != inKey[k]) {
#ifdef _DEBUG_
            CloseDebug();
#endif
            string msg = "Pvl Group [BandBin] in Key[" + outKey.Name() + "] In value" + inKey[k] +
                         "and Out value=" + outKey[j] + " do not match";
            throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
          }
        }
      }
      else {
#ifdef _DEBUG_
        CloseDebug();
#endif
        string msg = "Pvl Group [BandBin] In Keyword[" + inBin[i].Name() + "] and Out Keyword[" + outBin[i].Name() +
                     "] does not match";
        throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    piInb /= outBin.Keywords();
  }

  /**
   *  This method adds the BandBin group to the mosaic corresponding
   *  to the actual bands in the mosaic
   *
   *  @piIsb   - Input starting Band#
   *  @piOsb   - Output starting Band#
   *
   *  returns None
   *
   *  @author sprasad (9/24/2009)
   */
  void ProcessMosaic::AddBandBinGroup(int piIsb, int piOsb) {
    Pvl *inLab  = InputCubes[0]->Label();
    Pvl *outLab = OutputCubes[0]->Label();

    int iOutBands = OutputCubes[0]->Bands();

    if(mtTrackInfo.bTrack) {
      iOutBands -= 1;     // leave tracking band
    }

    int iIsb = piIsb - 1; // array zero based
    int iOsb = piOsb - 1;

#ifdef _DEBUG_
    ostm << "\n***** AddBandBinGroup *****\nStart Bands Input=" << piIsb << "   Output=" << piOsb << "\n\n";
#endif
    PvlGroup &cInBin  = inLab->FindGroup("BandBin", Pvl::Traverse);
    PvlGroup cOutBin("BandBin");

    int iInBands = InputCubes[0]->Bands();

    for(int i = 0; i < cInBin.Keywords(); i++) {
      PvlKeyword &cInKey = cInBin[i];
      int iInKeySize = cInKey.Size();
      PvlKeyword cOutKey(cInKey.Name());
      for(int b = 0; b < iOsb; b++) {
        cOutKey += "NA";
      }
      for(int b = iOsb; b < iOutBands; b++) {
        if(iIsb < cInKey.Size()) {
          cOutKey += cInKey[iIsb++];
        }
        else {
          cOutKey += "NA";
        }
      }

      // Add the "TRACKING" band to the Keyword if the flag is set and also if the number of
      // input cube bands is same as the the keysize of the keyword in the BandBin group.
      if(mtTrackInfo.bTrack && iInBands == iInKeySize) {
        cOutKey += "TRACKING"; // for the origin band
      }

      // Check for units and make sure output keyword units value is set to input
      // keyword units value
      if(cOutKey.Unit() != cInKey.Unit()) {
        cOutKey.SetUnits((iString)(cInKey.Unit()));
      }

      cOutBin += cOutKey;
      iIsb = piIsb - 1;        // reinitialize the input starting band
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
    Pvl *outLab = OutputCubes[0]->Label();

    PvlGroup cOutBin("BandBin");

    int iOutBands = OutputCubes[0]->Bands();
    iOutBands--; // Leave tracking band

    PvlKeyword cOutKey("FilterName");

    for(int i = 0; i < iOutBands; i++) {
      cOutKey += "NA";
    }

    cOutKey += "TRACKING";
    cOutBin += cOutKey;

    outLab->FindObject("IsisCube").AddGroup(cOutBin);
  }
  /**
   *  This method gets the origin of a pixel in the mosaic from
   *  the origin band given the line, sample and starting mosaic
   *  sample numbers
   *
   *  @piLineNum   - Input Line #
   *  @piSS        - Starting Mosaic Sample #
   *  @piSampleNum - Pixel index (Sample #)
   *
   *  returns the pixel origin
   *
   *  @author sprasad (8/28/2009)
   */
  int ProcessMosaic::GetPixelOrigin(int piLineNum, int piSS, int piSampleNum) {
    Isis::Portal cOrgPortal((piSampleNum + 1), 1, OutputCubes[0]->PixelType());

    //Get the last band set aside for "Origin"
    int iOriginBand = OutputCubes[0]->Bands();

    // 1 based
    cOrgPortal.SetPosition(piSS, piLineNum, iOriginBand);
    OutputCubes[0]->Read(cOrgPortal);

    return (int) cOrgPortal[piSampleNum];

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
   * @author sprasad (9/04/2009)
   */
  int ProcessMosaic::GetBandIndex(const FileType &peFileType) {
    bool bFound = false;
    int iBandIndex = 0;

    Pvl cPvlLabel;

    if(peFileType == inFile)
      cPvlLabel = *(InputCubes[0]->Label());
    else
      cPvlLabel = *(OutputCubes[0]->Label());

#ifdef _DEBUG_
    ostm << "**GetBandIndex**\n";
#endif
    //if non-zero integer, must be original band #, 1 based
    if(mtTrackInfo.iBandNum) {
      PvlKeyword cKeyOrigBand;
      if(cPvlLabel.FindGroup("BandBin", Pvl::Traverse).HasKeyword("OriginalBand")) {
        cKeyOrigBand = cPvlLabel.FindGroup("BandBin", Pvl::Traverse).FindKeyword("OriginalBand");
      }
      int iSize = cKeyOrigBand.Size();
#ifdef _DEBUG_
      ostm << "Num of Original Bands=" << iSize << "\n";
#endif
      char buff[64];
      sprintf(buff, "%d", mtTrackInfo.iBandNum);
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
#ifdef _DEBUG_
      ostm << "Key Name=" << mtTrackInfo.sKeyName << "   Value=" << mtTrackInfo.sKeyValue << "\n";
#endif
      if(cPvlLabel.FindGroup("BandBin", Pvl::Traverse).HasKeyword(mtTrackInfo.sKeyName)) {
        cKeyName = cPvlLabel.FindGroup("BandBin", Pvl::Traverse).FindKeyword(mtTrackInfo.sKeyName);
      }
      int iSize = cKeyName.Size();
      for(int i = 0; i < iSize; i++) {
#ifdef _DEBUG_
        ostm << "Band=" << cKeyName[i].c_str() << "\n";
#endif
        if(Isis::iString::Equal(mtTrackInfo.sKeyValue.c_str(), cKeyName[i].c_str())) {
          iBandIndex = i + 1; //1 based get key value index
          bFound = true;
          break;
        }
      }
    }
    if(!bFound) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string msg = "Invalid Band / Key Name, Value ";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
#ifdef _DEBUG_
      CloseDebug();
#endif
    }

#ifdef _DEBUG_
    ostm << "iBandNum=" << iBandIndex << "\n";
#endif
    if(peFileType == inFile)
      mtTrackInfo.iInBand = iBandIndex;
    else
      mtTrackInfo.iOutBand = iBandIndex;

    return  iBandIndex;
  }

  /**
   * This method compares the specified band of the input and
   * output using the criteria (lesser or greater) to assign the
   * pixel origin(input fileindex) to the origin band. In the StartProcess(),
   * input pixel is assigned to the output if the origin pixel equals the current
   * input file index
   *
   * @piIndex     - Filename Index for the origin band (default +
   *                zero based index)
   * @piIns       - Number of input samples
   * @piInl       - Number of input lines
   * @piIss       - Starting input sample
   * @piIsl       - Starting input line
   * @piOss       - Starting output sample
   * @piOsl       - Starting output line
   *
   * @throws Isis::iException::Message
   *
   * @author sprasad (9/04/2009)
   */
  void ProcessMosaic::BandComparison(int piIndex, int piIns, int piInl, int piIss, int piIsl, int piOss, int piOsl) {
    //
    // Create portal buffers for the input and output files
    Isis::Portal cIportal(piIns, 1, InputCubes[0]->PixelType());
    Isis::Portal cOportal(piIns, 1, OutputCubes[0]->PixelType());
    Isis::Portal cOrgPortal(piIns, 1, OutputCubes[0]->PixelType());

    //Get the last band set aside for "Origin"
    int iOriginBand = OutputCubes[0]->Bands();

#ifdef _DEBUG_
    ostm << "*** Band Comparison ***\npeBCriteria=" << mtTrackInfo.eCriteria << "  iOriginBand=" << iOriginBand << "  piIndex=" << piIndex << "   piIns=" << piIns ;
    ostm << "   piInl=" << piInl << "  piIss=" << piIss << "  piIsl=" << piIsl << "    piOss=" << piOss << "    piOsl=" << piOsl << "   Num lines=" << piInl << "\n";
    ostm << " ** Special Pixels ** HS=" << mbHighSat << "  LS=" << mbLowSat << "  Null=" << mbNull << "\n";
#endif

    for(int iIL = piIsl, iOL = piOsl; iIL < piIsl + piInl; iIL++, iOL++) {
      // Set the position of the portals in the input and output cubes
      cIportal.SetPosition(piIss, iIL, mtTrackInfo.iInBand);
      InputCubes[0]->Read(cIportal);

      cOportal.SetPosition(piOss, iOL, mtTrackInfo.iOutBand);
      OutputCubes[0]->Read(cOportal);

      cOrgPortal.SetPosition(piOss, iOL, iOriginBand);
      OutputCubes[0]->Read(cOrgPortal);

      // Move the input data to the output
      for(int iPixel = 0; iPixel < cOportal.size(); iPixel++) {
#ifdef _DEBUG_
        //ostm << endl << iIL << ".In="<< (int)cIportal[iPixel] <<"  Out="<< (int)cOportal[iPixel] << "  IsSpecial(In)=" << Isis::IsSpecial(cIportal[iPixel]) << "  Compare=" << (cIportal[iPixel] < cOportal[iPixel]);
#endif

        if(Isis::IsNullPixel(cOrgPortal[iPixel]) ||
            mbHighSat && Isis::IsHighPixel(cIportal[iPixel]) ||
            mbLowSat  && Isis::IsLowPixel(cIportal[iPixel]) ||
            mbNull    && Isis::IsNullPixel(cIportal[iPixel])) {
          cOrgPortal[iPixel] = piIndex;
#ifdef _DEBUG_
          //ostm << "   SP >>   Origin=" << (int)cOrgPortal[iPixel];
#endif
        }
        else {
          if(Isis::IsValidPixel(cIportal[iPixel])) {
            if(Isis::IsSpecial(cOportal[iPixel]) ||
                (mtTrackInfo.eCriteria == Lesser  && cIportal[iPixel] < cOportal[iPixel]) ||
                (mtTrackInfo.eCriteria == Greater && cIportal[iPixel] > cOportal[iPixel])) {
              cOrgPortal[iPixel] = piIndex;
#ifdef _DEBUG_
              //ostm << "   CRITERIA >>   Origin=" << (int)cOrgPortal[iPixel];
#endif
            }
          }
        }
      }
      OutputCubes[0]->Write(cOrgPortal);
    }
  }

  /**
   * Debugging
   *
   * @author sprasad (9/2/2009)
   */
  void ProcessMosaic::Test(void) {
    int iBand    = OutputCubes[0]->Bands();
    int iLines   = OutputCubes[0]->Lines();
    int iSamples = OutputCubes[0]->Samples();
    int iFileIndex;

    Isis::Portal cOrgPortal(iSamples, 1, OutputCubes[0]->PixelType());
    int  iOffset = GetOriginDefaultByPixelType();

#ifdef _DEBUG_
    ostm << "\nTesting\nPixel Type=" << SizeOf(OutputCubes[0]->PixelType()) << "    Offset=" << iOffset << "    OriginBand=" << iBand << "\n";
#endif

    for(int line = 1; line <= iLines; line++) {
      cOrgPortal.SetPosition(1, line, iBand);  //sample, line, band position
      OutputCubes[0]->Read(cOrgPortal);
      for(int iPixel = 0; iPixel < cOrgPortal.size(); iPixel++) {
        iFileIndex = (int)cOrgPortal[iPixel] - iOffset;
#ifdef _DEBUG_
        ostm << "Line=" << line  << "  Value=" << (int)cOrgPortal[iPixel] << "  FileIndex=" << iFileIndex << "\n";
#endif
      }
      if(line >= 2) {
        break;
      }
    }
#ifdef _DEBUG_
    ostm << "************************************************\n";
#endif
  }


  /**
   * This method returns the start value depending on the pixel
   * type 8,16,32 bit.
   *
   * @returns the start/offset value
   *
   * @throws Isis::iException::Message
   *
   * @author sprasad (8/28/2009)
   */
  int ProcessMosaic::GetIndexOffsetByPixelType(void) {
    int iOffset = 0;

    switch(SizeOf(OutputCubes[0]->PixelType())) {
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
   * @throws Isis::iException::Message
   *
   * @author sprasad (9/10/2009)
   */
  int ProcessMosaic::GetOriginDefaultByPixelType(void) {
    int iDefault;

    switch(SizeOf(OutputCubes[0]->PixelType())) {
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
#ifdef _DEBUG_
        CloseDebug();
#endif
        string msg = "ProcessMosaic::GetOriginDefaultByPixelType - Invalid Pixel Type";
        throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    return iDefault;
  }

  /**
   * This method sets the origin band to defaults(unassigned value)
   * depending on the pixel type.
   *
   * @No parameters and no return value
   *
   * @author sprasad (8/28/2009)
   */
  void ProcessMosaic::ResetOriginBand(void) {
    int iBand   = OutputCubes[0]->Bands();
    int iLines  = OutputCubes[0]->Lines();
    int iSample = OutputCubes[0]->Samples();

    int iDefault = GetOriginDefaultByPixelType();

    Isis::Portal cOrgPortal(iSample, 1, OutputCubes[0]->PixelType());

#ifdef _DEBUG_
    ostm << "\n*****ResetOriginBand****\nDefault=" << iDefault << "  Band=" << iBand << "  Pixels size=" << OutputCubes[0]->PixelType() << "  Portal Size=" << cOrgPortal.size() << "\n";
    int iPixelIndex = 0;
#endif

    for(int i = 1; i <= iLines; i++) {
      cOrgPortal.SetPosition(1, i, iBand);  //sample, line, band position
      OutputCubes[0]->Read(cOrgPortal);
      for(int iPixel = 0; iPixel < cOrgPortal.size(); iPixel++) {
        cOrgPortal[iPixel] = (float)(iDefault);
#ifdef _DEBUG_
        if(iPixel < 10) {
          //ostm << iPixelIndex++ <<"."<<(int)cOrgPortal[iPixel] << "\n";
        }
#endif
      }
      OutputCubes[0]->Write(cOrgPortal);
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
   * @author sprasad (9/22/2009)
   */
  bool ProcessMosaic::GetTrackStatus(void) {
    //get the output label
    Pvl *cPvlOut = OutputCubes[0]->Label();

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
#ifdef _DEBUG_
    ostm << "GetTrackStatus, Track Status=" << bTableExists << endl;
#endif
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
   * @param piIndex - the input file index
   *
   * @returns none
   *
   * @throws an exception if the number of images exceeds the
   *            pixel size.
   *
   * @throws Isis::iException::Message
   *
   * @author sprasad (8/28/2009)
   */
  void ProcessMosaic::SetMosaicOrigin(int &piIndex) {
    // Get only the file name
    std::string sInputFile = Filename(InputCubes[0]->Filename()).Name();
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
    std::string sOutputFile = Filename(OutputCubes[0]->Filename()).Name();

    Pvl *cPvlOut = OutputCubes[0]->Label();

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

#ifdef _DEBUG_
    ostm << "\n***SetMosaicOrigin***\nFile Name=" << sInputFile << "  Length=" << sInputFile.length() << "  Track=" << mtTrackInfo.bTrack << "\n";
    //ostm << "Record Size="<< cFileRecord.RecordSize() << "\n";
#endif

    int iNumObjs = cPvlOut->Objects();
    PvlObject cPvlObj;

    // Check if the Table exists
    if(cPvlOut->HasObject("Table")) {
#ifdef _DEBUG_
      ostm << "Table object Exists\n";
#endif
      for(int i = 0; i < iNumObjs; i++) {
        cPvlObj = cPvlOut->Object(i);
        if(cPvlObj.HasKeyword("Name", Pvl::Traverse)) {
          PvlKeyword cNameKey = cPvlObj.FindKeyword("Name", Pvl::Traverse);
#ifdef _DEBUG_
          ostm << "key name=" <<  cNameKey[0] << "   Tablename=" << sTableName << "\n";
#endif
          if(cNameKey[0] == sTableName) {
            PvlKeyword cFieldKey = cPvlObj.FindGroup("Field").FindKeyword("Size");
#ifdef _DEBUG_
            ostm << "Found Table Name = " << cNameKey[0] << "\n";
#endif

            //set the tracker flag to true as the tracking table exists
            mtTrackInfo.bTrack = true;

            // Create a new blank table
            Table cFileTable(sTableName);

            // Read and make a copy of the existing tracking table
            Table cFileTable_Copy = Table(sTableName);
            OutputCubes[0]->Read(cFileTable_Copy);

            // Records count
            int iRecs = cFileTable_Copy.Records();

#ifdef _DEBUG_
            ostm << "\nRecords =" << iRecs << "\n";
#endif

            // Check if the image index can be accomadated in the pixel size
            bool bFull = false;
            switch(sizeof(OutputCubes[0]->PixelType())) {
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
#ifdef _DEBUG_
              CloseDebug();
#endif
              string msg = "The number of images in the Mosaic exceeds the pixel size";
              throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
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
#ifdef _DEBUG_
                ostm << "Image file Exists\n";
#endif
                piIndex += i;
                return;
              }

              // To initialise the new table, on the first file name comparison,
              // check the size of the existing table record with the size of the new record being added
              if(!i) {
                if(iString(cFieldKey[0]).ToInteger() < iFieldLength) {
                  TableRecord cFileRecordUpdate;
                  TableField cFileFieldUpdate("FileName", TableField::Text, iFieldLength);
                  cFileFieldUpdate = cFileTable_Copy[i][0];
                  cFileRecordUpdate += cFileFieldUpdate;

                  // Populate with Serial Number
                  TableField cSNFieldUpdate("SerialNumber", TableField::Text, iFieldLength);
                  cSNFieldUpdate = cFileTable_Copy[i][1];
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
            piIndex += iRecs;

            // Add the current input image record to the new table
            cFileTable +=  cFileRecord;

            // Copy the new table to the output Mosaic
            OutputCubes[0]->Write(cFileTable);
            break;   //break while loop
          }
        }
      }//end for loop
    }

    //creating new table if track flag is true
    if(mtTrackInfo.bCreate && mtTrackInfo.bTrack) {
#ifdef _DEBUG_
      ostm << "Creating Table " << SRC_IMAGE_TBL << " \n";
#endif
      Table cFileTable(sTableName, cFileRecord);
      cFileTable += cFileRecord;
      OutputCubes[0]->Write(cFileTable);
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
   * @throws Isis::iException::Message
   */
  Isis::Cube *ProcessMosaic::SetInputCube(const std::string &parameter,
                                          const int ss, const int sl, const int sb,
                                          const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
#ifdef _DEBUG_
    ostm << "InputCubes.size()=" << InputCubes.size() << "\n";
#endif
    if(InputCubes.size() > 0) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    p_iss = ss;
    p_isl = sl;
    p_isb = sb;
    p_ins = ns;
    p_inl = nl;
    p_inb = nb;

    Isis::Cube *cInCube = Isis::Process::SetInputCube(parameter);

    //get the output label
    Pvl *cInPvl = InputCubes[0]->Label();
    if(cInPvl->FindGroup("Dimensions", Pvl::Traverse).HasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->FindGroup("Dimensions", Pvl::Traverse).FindKeyword("Bands");
      iString sStr(cBandKey[0]);
      if(sStr.ToInteger() < nb) {
#ifdef _DEBUG_
        CloseDebug();
#endif
        string m = "The parameter number of input bands exceeds the actual number of bands in the input cube";
        throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
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
   * @throws Isis::iException::Message
   */
  Isis::Cube *ProcessMosaic::SetInputCube(const std::string &fname,
                                          Isis::CubeAttributeInput &att,
                                          const int ss, const int sl, const int sb,
                                          const int ns, const int nl, const int nb) {

    // Make sure only one input is active at a time
    if(InputCubes.size() > 0) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    p_iss = ss;
    p_isl = sl;
    p_isb = sb;
    p_ins = ns;
    p_inl = nl;
    p_inb = nb;

    Isis::Cube *cInCube = Isis::Process::SetInputCube(fname, att);

    //check if the number of bands specified is not greater than the actual number of bands in the input
    Pvl *cInPvl = InputCubes[0]->Label();
    if(cInPvl->FindGroup("Dimensions", Pvl::Traverse).HasKeyword("Bands")) {
      PvlKeyword &cBandKey = cInPvl->FindGroup("Dimensions", Pvl::Traverse).FindKeyword("Bands");
      iString sStr(cBandKey[0]);
      if(sStr.ToInteger() < nb) {
#ifdef _DEBUG_
        CloseDebug();
#endif
        string m = "The parameter number of input bands exceeds the actual number of bands in the input cube";
        throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
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
   * @throws Isis::iException::Message
   */
  Isis::Cube *ProcessMosaic::SetOutputCube(const std::string &psParameter) {

    // Make sure there is only one output cube
#ifdef _DEBUG_
    ostm << "OutputCubes.size()=" << OutputCubes.size() << "\n";
#endif
    if(OutputCubes.size() > 0) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      string m = "You must specify exactly one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer, m, _FILEINFO_);
    }

    // Attempt to open a cube ... get the filename from the user parameter
    // (e.g., "TO") and the cube size from an input cube
    Isis::Cube *cube = new Isis::Cube;
    try {
      string fname = Application::GetUserInterface().GetFilename(psParameter);
      cube->Open(fname, "rw");
    }
    catch(Isis::iException &e) {
#ifdef _DEBUG_
      CloseDebug();
#endif
      delete cube;
      throw;
    }

    if(mtTrackInfo.bCreate) {
      Pvl *outLab = cube->Label();
      if(outLab->FindObject("IsisCube").HasGroup("BandBin")) {
        outLab->FindObject("IsisCube").DeleteGroup("BandBin");
      }
    }

    // Everything is fine so save the cube on the stack
    OutputCubes.push_back(cube);
    return cube;
  }

} // end namespace isis

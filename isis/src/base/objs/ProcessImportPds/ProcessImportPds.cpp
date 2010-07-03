/**
 * @file
 * $Revision: 1.35 $
 * $Date: 2010/02/22 02:26:15 $
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

#include <iostream>
#include <string>
#include <sstream>

#include "Preference.h"

#include "iException.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "PvlTokenizer.h"
#include "PixelType.h"
#include "SpecialPixel.h"
#include "iString.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "OriginalLabel.h"
#include "Projection.h"

using namespace std;
namespace Isis {

  /**
  * Constructor.
  */
  ProcessImportPds::ProcessImportPds(){
    p_keepOriginalLabel = true;
    p_encodingType = NONE;
    p_jp2File.clear();

    // Set up a translater for PDS file of type IMAGE
    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    p_transDir = (string) dataDir["Base"];
  }

 /**
  * Set the input label file, data file and initialize a Pvl with the PDS labels.
  *
  * @param pdsLabelFile The name of the PDS label file.This must be the file
  *                     where the label is. It can be an attached or detached
  *                     label.
  *
  * @param pdsDataFile The name of the PDS data file where the actual image/cube
  *                    data is stored. This parameter can be an empty string, in
  *                    which case the label information will be searched to find
  *                    the data file name or the data will be assumed to be
  *                    after the label information.
  *
  * @param pdsLabel  The label from the input PDS/Isis2 file
  *
  * @throws Isis::iException::Message
  */
  void ProcessImportPds::SetPdsFile (const std::string &pdsLabelFile,
                                     const std::string &pdsDataFile,
                                     Isis::Pvl &pdsLabel) {

    // Internalize the PDS label in the PVL that was passed in
    pdsLabel.Read (pdsLabelFile);

    // Save the label and file for future use
    p_pdsLabel = pdsLabel;
    p_labelFile = pdsLabelFile;

    // Create a temporary Isis::PvlTranslationManager so we can find out what
    // type of PDS file this is (i.e., Qube or Image or SpectralQube)
    stringstream trnsStrm;
    trnsStrm << "Group = PdsTypeImage" << endl;
    trnsStrm << "  InputPosition = ROOT" << endl;
    trnsStrm << "  InputPosition = FILE" << endl;
    trnsStrm << "  InputPosition = UNCOMPRESSED_FILE" << endl;
    trnsStrm << "  InputKey = ^IMAGE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsTypeQube" << endl;
    trnsStrm << "  InputKey = ^QUBE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsTypeSpectralQube" << endl;
    trnsStrm << "  InputKey = ^SPECTRAL_QUBE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsEncodingType" << endl;
    trnsStrm << "  InputPosition = COMPRESSED_FILE" << endl;
    trnsStrm << "  InputKey = ENCODING_TYPE" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsCompressedFile" << endl;
    trnsStrm << "  InputPosition = COMPRESSED_FILE" << endl;
    trnsStrm << "  InputKey = FILE_NAME" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "END";

    Isis::PvlTranslationManager pdsXlater (p_pdsLabel, trnsStrm);

    // Check to see if we are dealing with a JPEG2000 file
    Isis::iString str;
    if (pdsXlater.InputHasKeyword("PdsEncodingType")) {
      str = pdsXlater.Translate("PdsEncodingType");
      if (str == "JP2") {
        p_encodingType = JP2;
        str = pdsXlater.Translate("PdsCompressedFile");
        if (pdsDataFile.empty()) {
          Isis::Filename lfile (p_labelFile);
          Isis::Filename ifile (lfile.Path() + "/" + str);
          if (ifile.Exists()) {
            p_jp2File = ifile.Expanded();
          }
          else {
            string tmp = ifile.Expanded();
            str.DownCase();
            ifile = lfile.Path() + "/" + str;
            if (ifile.Exists()) {
              p_jp2File = ifile.Expanded();
            }
            else {
              string msg = "Unable to find input file [" + tmp + "] or [" +
                            ifile.Expanded() + "]";
              throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
            }
          }
        }
      }
      else {
        string msg = "Unsupported encoding type in [" + p_labelFile + "]";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }
    }

    // Call the correct label processing
    if (pdsXlater.InputHasKeyword("PdsTypeImage")) {
      ProcessPdsImageLabel (pdsDataFile);
    }
    else if (pdsXlater.InputHasKeyword("PdsTypeQube")) {
      ProcessPdsQubeLabel (pdsDataFile, "pdsQube.trn");
    }
    else if (pdsXlater.InputHasKeyword("PdsTypeSpectralQube")) {
      ProcessPdsQubeLabel (pdsDataFile, "pdsSpectralQube.trn");
    }
    else {
      string msg = "Unknown label type in [" + p_labelFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    // Find out if this is a PDS file or an ISIS2 file
    IdentifySource(p_pdsLabel);

    return;
  }

 /**
  * Process the PDS label of type IMAGE.
  *
  * @param pdsDataFile The name of the PDS data file where the actual image/cube
  *                    data is stored. This parameter can be an empty string, in
  *                    which case the label information will be searched to find
  *                    the data file name or the data will be assumed to be
  *                    after the label information.
  *
  * @throws Isis::iException::Message
  */
  void ProcessImportPds::ProcessPdsImageLabel (const std::string &pdsDataFile) {
    Isis::Filename transFile (p_transDir + "/translations/pdsImage.trn"); 
    Isis::PvlTranslationManager pdsXlater (p_pdsLabel, transFile.Expanded());

    Isis::iString str;

    str = pdsXlater.Translate("CoreLinePrefixBytes");
    SetDataPrefixBytes(str.ToInteger());

    str = pdsXlater.Translate("CoreLineSuffixBytes");
    SetDataSuffixBytes(str.ToInteger());

    str = pdsXlater.Translate ("CoreBitsPerPixel");
    int bitsPerPixel = str.ToInteger();
    str = pdsXlater.Translate ("CorePixelType");
    if ((str == "Real") && (bitsPerPixel == 32)) {SetPixelType (Isis::Real);}
    else if ((str == "Integer") && (bitsPerPixel == 8)) {SetPixelType (Isis::UnsignedByte);}
    else if ((str == "Integer") && (bitsPerPixel == 16)) {SetPixelType (Isis::SignedWord);}
    else if ((str == "Integer") && (bitsPerPixel == 32)) {SetPixelType (Isis::SignedInteger);}
    else if ((str == "Natural") && (bitsPerPixel == 8)) {SetPixelType (Isis::UnsignedByte);}
    else if ((str == "Natural") && (bitsPerPixel == 16)) {SetPixelType (Isis::UnsignedWord);}
    else if ((str == "Natural") && (bitsPerPixel == 16)) {SetPixelType (Isis::SignedWord);}
    else if ((str == "Natural") && (bitsPerPixel == 32)) {SetPixelType (Isis::UnsignedInteger);}
    else {
      string msg = "Invalid PixelType and BitsPerPixel combination [" + str +
                   ", " + Isis::iString(bitsPerPixel) + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    str = pdsXlater.Translate ("CoreByteOrder");
    SetByteOrder (Isis::ByteOrderEnumeration(str));

    str = pdsXlater.Translate ("CoreSamples");
    int ns = str.ToInteger();
    str = pdsXlater.Translate ("CoreLines");
    int nl = str.ToInteger();
    str = pdsXlater.Translate ("CoreBands");
    int nb = str.ToInteger();
    SetDimensions (ns, nl, nb);

    // Set any special pixel values
    double pdsNull = Isis::NULL8;
    if (pdsXlater.InputHasKeyword("CoreNull")) {
      str = pdsXlater.Translate("CoreNull");
      if (str != "NULL") {
        pdsNull = str.ToDouble();
      }
    }
    else if (pdsXlater.InputHasKeyword("CoreNull2")) {
      str = pdsXlater.Translate("CoreNull2");
      if (str != "NULL") {
        pdsNull = str.ToDouble();
      }
    }

    double pdsLrs = Isis::Lrs;
    if (pdsXlater.InputHasKeyword("CoreLrs")) {
      str = pdsXlater.Translate("CoreLrs");
      if (str != "NULL") {
        pdsLrs = str.ToDouble();
      }
    }
    else if (pdsXlater.InputHasKeyword("CoreLrs2")) {
      str = pdsXlater.Translate("CoreLrs2");
      if (str != "NULL") {
        pdsLrs = str.ToDouble();
      }
    }

    double pdsLis = Isis::Lis;
    if (pdsXlater.InputHasKeyword("CoreLis")) {
      str = pdsXlater.Translate("CoreLis");
      if (str != "NULL") {
        pdsLis = str.ToDouble();
      }
    }
    else if (pdsXlater.InputHasKeyword("CoreLis2")) {
      str = pdsXlater.Translate("CoreLis2");
      if (str != "NULL") {
        pdsLis = str.ToDouble();
      }
    }

    double pdsHrs = Isis::Hrs;
    if (pdsXlater.InputHasKeyword("CoreHrs")) {
      str = pdsXlater.Translate("CoreHrs");
      if (str != "NULL") {
        pdsHrs = str.ToDouble();
      }
    }
    else if (pdsXlater.InputHasKeyword("CoreHrs2")) {
      str = pdsXlater.Translate("CoreHrs2");
      if (str != "NULL") {
        pdsHrs = str.ToDouble();
      }
    }

    double pdsHis = Isis::His;
    if (pdsXlater.InputHasKeyword("CoreHis")) {
      str = pdsXlater.Translate("CoreHis");
      if (str != "NULL") {
        pdsHis = str.ToDouble();
      }
    }
    else if (pdsXlater.InputHasKeyword("CoreHis2")) {
      str = pdsXlater.Translate("CoreHis2");
      if (str != "NULL") {
        pdsHis = str.ToDouble();
      }
    }

    SetSpecialValues (pdsNull, pdsLrs, pdsLis, pdsHrs, pdsHis);

    //-----------------------------------------------------------------
    // Find the data filename it may be the same as the label file
    // OR the label file may contain a pointer to the data
    //-----------------------------------------------------------------

    // Use the name supplied by the application if it is there
    if (pdsDataFile.length() > 0) {
      SetInputFile (pdsDataFile);
    }
    // If the data is in JPEG 2000 format, then use the name of the file
    // from the label
    else if (p_jp2File.length() > 0) {
      SetInputFile (p_jp2File);
    }
    // Use the "^IMAGE or ^QUBE" label to get the filename for the image data
    // Get the path portion from user entered label file spec
    else if (pdsXlater.InputKeyword("DataFilePointer").Size() == 2) {
      Isis::iString dataFile;
      dataFile = pdsXlater.Translate ("DataFilePointer", 0);
      Isis::Filename lfile (p_labelFile);
      Isis::Filename ifile (lfile.Path() + "/" + dataFile);
      if (ifile.Exists()) {
        SetInputFile (ifile.Expanded());
      }
      else {
        string tmp = ifile.Expanded();
        dataFile.DownCase();
        ifile = lfile.Path() + "/" + dataFile;
        if (ifile.Exists()) {
          SetInputFile (ifile.Expanded());
        }
        else {
          string msg = "Unable to find input file [" + tmp + "] or [" +
                       ifile.Expanded() + "]";
          throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
        }
      }
    }
    // The ^IMAGE keyword contains either the filename or the offset
    // within this file
    else if (pdsXlater.InputKeyword("DataFilePointer").Size() == 1) {
      // Try converting the single value to an integer. If that works
      // then the iamge data must be in the same file as the labels
      try {
        str = pdsXlater.Translate("DataFilePointer");
        str.ToInteger();
        SetInputFile (p_labelFile);
      }
      // If it can't be converted to an integer then it must be
      // the name of the file that contains the image data
      catch (Isis::iException &e) {
        Isis::iString dataFile;
        dataFile = pdsXlater.Translate ("DataFilePointer", 0);
        Isis::Filename lfile (p_labelFile);
        Isis::Filename ifile (lfile.Path() + "/" + dataFile);
        if (ifile.Exists()) {
          SetInputFile (ifile.Expanded());
        }
        else {
          string tmp = ifile.Expanded();
          dataFile.DownCase();
          ifile = lfile.Path() + "/" + dataFile;
          if (ifile.Exists()) {
            SetInputFile (ifile.Expanded());
          }
          else {
            string msg = "Unable to find input file [" + tmp + "] or [" +
                         ifile.Expanded() + "]";
            throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
          }
        }
      }
    }

    // Could not find a filename for the image data
    else {
      string msg = "No data filename available on command line or in [" +
                   p_labelFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    //----------------------------------------------------------------
    // Calculate the file header size
    //----------------------------------------------------------------
    
    Isis::iString units;
    if (pdsXlater.InputKeyword ("DataStart").Size() == 1) {
      str = pdsXlater.Translate ("DataStart", 0);
      units = pdsXlater.InputKeyword ("DataStart").Unit();
      try {
        str.ToInteger ();
      }
      catch (Isis::iException &e) {
        str = "1";
        units = "BYTES";
      }
    }
    else if (pdsXlater.InputKeyword("DataStart").Size() == 2) {
      str = pdsXlater.Translate ("DataStart", 1);
      units = pdsXlater.InputKeyword ("DataStart").Unit(1);
    }
    else {
      string msg = "Invalid PDS labels in file [" + p_labelFile +
                   "]. Label value [" +
                   pdsXlater.InputKeywordName("DataStart") + "]" ;
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    units.Trim(" \t\r\v\n");
    // Set the number of file header bytes using a byte count
    // byte counts are 1 based
    if ((units == "BYTES") || (units == "B")) {
      SetFileHeaderBytes(str.ToInteger() - 1);
    }
    // Set the number of file header bytes using a record number
    else {
      Isis::iString recSize = pdsXlater.Translate("DataFileRecordBytes");
      SetFileHeaderBytes ((str.ToInteger() - 1) * recSize.ToInteger());
    }

    //------------------------------------------------------------
    // Find the image data base and multiplier
    //------------------------------------------------------------
    str = pdsXlater.Translate ("CoreBase");
    SetBase (str.ToDouble());
    str = pdsXlater.Translate ("CoreMultiplier");
    SetMultiplier (str.ToDouble());

    // Find the organization of the image data
    str =pdsXlater.Translate("CoreOrganization");

    if (p_encodingType == JP2) {
      SetOrganization (ProcessImport::JP2);
    }
    else if (str == "BSQ") {
      SetOrganization (ProcessImport::BSQ);
    }
    else if (str == "BIP") {
      SetOrganization (ProcessImport::BIP);
    }
    else if (str == "BIL") {
      SetOrganization (ProcessImport::BIL);
    }
  }


 /**
  * Process the PDS label of type QUBE or SPECTRALQUBE.
  *
  * @param pdsDataFile The name of the PDS data file where the actual image/cube
  *                    data is stored. This parameter can be an empty string, in
  *                    which case the label information will be searched to find
  *                    the data file name or the data will be assumed to be
  *                    after the label information.
  *
  * @param transFile
  *
  * @throws Isis::iException::Message
  */
  void ProcessImportPds::ProcessPdsQubeLabel (const std::string &pdsDataFile,
                                              const std::string &transFile) {

    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    Isis::iString transDir = (string) dataDir["Base"];
    Isis::Filename tFile (transDir + "/translations/" + transFile);

    Isis::PvlTranslationManager pdsXlater (p_pdsLabel, tFile.Expanded());

    Isis::iString str;

    // Find the organization of the image data
    // Save off which axis the samples, lines and bands are on
    int linePos = 0;
    int samplePos = 0;
    int bandPos = 0;
    int val = pdsXlater.InputKeyword("CoreOrganization").Size();
    Isis::iString tmp = "";
    for (int i=0; i<val; i++) {
      str = pdsXlater.Translate("CoreOrganization", i);
      tmp += str;
      if (str == "SAMPLE") {
        samplePos = i;
      }
      else if (str == "LINE") {
        linePos = i;
      }
      else if (str == "BAND") {
        bandPos = i;
      }
      else {
        string message = "Unknown file axis name [" + str + "]";
        throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
      }
    }

    if (p_encodingType == JP2) {
      SetOrganization (ProcessImport::JP2);
    }
    else if (tmp == "SAMPLELINEBAND") {
      SetOrganization (ProcessImport::BSQ);
    }
    else if (tmp == "LINEBANDSAMPLE") {
      SetOrganization (ProcessImport::BIP);
    }
    else if (tmp == "SAMPLEBANDLINE") {
      SetOrganization (ProcessImport::BIL);
    }

    // Set the number of byte preceding the second dimension (left side plane)
    // There are no capabilities in a PDS QUBE for this
    SetDataPrefixBytes (0);

    // Set the number of bytes following the second dimension (right side plane)
    str = pdsXlater.Translate("SuffixItemSize");
    int suffix = str.ToInteger();
    str = pdsXlater.Translate("AxisSuffixCount", 0);
    suffix *= str.ToInteger();
    SetDataSuffixBytes (suffix);

    str = pdsXlater.Translate("SuffixItemSize");
    int trailer = str.ToInteger();
    str = pdsXlater.Translate("AxisSuffixCount", 1);
    trailer *= str.ToInteger();
    str = pdsXlater.Translate ("CoreSamples", samplePos);
    trailer *= str.ToInteger();
    trailer += suffix;
    SetDataTrailerBytes (trailer);

    str = pdsXlater.Translate ("CoreBitsPerPixel");
    int bitsPerPixel = str.ToInteger();
    str = pdsXlater.Translate ("CorePixelType");
    if ((str == "Real") && (bitsPerPixel == 32)) {SetPixelType (Isis::Real);}
    else if ((str == "Integer") && (bitsPerPixel == 8)) {SetPixelType (Isis::UnsignedByte);}
    else if ((str == "Integer") && (bitsPerPixel == 16)) {SetPixelType (Isis::SignedWord);}
    else if ((str == "Integer") && (bitsPerPixel == 32)) {SetPixelType (Isis::SignedInteger);}
    else if ((str == "Natural") && (bitsPerPixel == 8)) {SetPixelType (Isis::UnsignedByte);}
    else if ((str == "Natural") && (bitsPerPixel == 16)) {SetPixelType (Isis::UnsignedWord);}
    else if ((str == "Natural") && (bitsPerPixel == 16)) {SetPixelType (Isis::SignedWord);}
    else if ((str == "Natural") && (bitsPerPixel == 32)) {SetPixelType (Isis::UnsignedInteger);}
    else {
      string msg = "Invalid PixelType and BitsPerPixel combination [" + str +
                   ", " + Isis::iString(bitsPerPixel) + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    // Set the byte order
    str = pdsXlater.Translate ("CoreByteOrder");
    SetByteOrder (Isis::ByteOrderEnumeration (str));

    // Set the number of samples, lines and bands
    str = pdsXlater.Translate ("CoreSamples", samplePos);
    int ns = str.ToInteger();
    str = pdsXlater.Translate ("CoreLines", linePos);
    int nl = str.ToInteger();
    str = pdsXlater.Translate ("CoreBands", bandPos);
    int nb = str.ToInteger();
    SetDimensions (ns, nl, nb);

    // Set any special pixel values
    double pdsNull = Isis::NULL8;
    if (pdsXlater.InputHasKeyword("CoreNull")) {
      str = pdsXlater.Translate("CoreNull");
      if (str != "NULL") {
        pdsNull = str.ToDouble();
      }
    }
    double pdsLrs = Isis::Lrs;
    if (pdsXlater.InputHasKeyword("CoreLrs")) {
      str = pdsXlater.Translate("CoreLrs");
      if (str != "NULL") {
        pdsLrs = str.ToDouble();
      }
    }
    double pdsLis = Isis::Lis;
    if (pdsXlater.InputHasKeyword("CoreLis")) {
      str = pdsXlater.Translate("CoreLis");
      if (str != "NULL") {
        pdsLis = str.ToDouble();
      }
    }
    double pdsHrs = Isis::Hrs;
    if (pdsXlater.InputHasKeyword("CoreHrs")) {
      str = pdsXlater.Translate("CoreHrs");
      if (str != "NULL") {
        pdsHrs = str.ToDouble();
      }
    }
    double pdsHis = Isis::Hrs;
    if (pdsXlater.InputHasKeyword("CoreHis")) {
      str = pdsXlater.Translate("CoreHis");
      if (str != "NULL") {
        pdsHis = str.ToDouble();
      }
    }
    SetSpecialValues (pdsNull, pdsLrs, pdsLis, pdsHrs, pdsHis);

    //---------------------------------------------------------------
    // Find the data filename, it may be the same as the label file
    // Or the label file may contain a pointer to the data
    //---------------------------------------------------------------

    // Use the name supplied by the application if it is there
    if (pdsDataFile.length() > 0) {
      SetInputFile (pdsDataFile);
    }

    // If the data is in JPEG 2000 format, then use the name of the file
    // from the label
    else if (p_jp2File.length() > 0) {
      SetInputFile (p_jp2File);
    }

    // Get the filename for the image data
    // Get the path portion from user entered label file spec
    else if (pdsXlater.InputKeyword("DataFilePointer").Size() == 2) {
      Isis::iString dataFile;
      dataFile = pdsXlater.Translate ("dataFilePointer", 0);
      Isis::Filename lfile (p_labelFile);
      Isis::Filename ifile (lfile.Path() + "/" + dataFile);
      if (ifile.Exists()) {
        SetInputFile (ifile.Expanded());
      }
      else {
        string tmp = ifile.Expanded();
        dataFile.DownCase();
        ifile = lfile.Path() + "/" + dataFile;
        if (ifile.Exists()) {
          SetInputFile (ifile.Expanded());
        }
        else {
          string msg = "Unable to find input file [" + tmp + "] or [" +
                       ifile.Expanded() + "]";
          throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
        }
      }
    }

    // Use the same name as the label file to get the filename for the image data
    else if (pdsXlater.InputKeyword("DataFilePointer").Size() == 1) {
      SetInputFile (p_labelFile);
    }

    // Could not find a filename for the image data
    else {
      string msg = "No data filename available on command line or in [" +
                   p_labelFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    //----------------------------------------------------------------
    // Calculate the file header size
    //----------------------------------------------------------------
    Isis::iString units;
    if (pdsXlater.InputKeyword ("DataFilePointer").Size() == 1) {
      str = pdsXlater.Translate ("DataFilePointer", 0);
      units = pdsXlater.InputKeyword ("DataFilePointer").Unit();
    }
    else if (pdsXlater.InputKeyword("DataFilePointer").Size() == 2) {
      str = pdsXlater.Translate ("DataFilePointer", 1);
      units = pdsXlater.InputKeyword ("DataFilePointer").Unit(1);
    }
    else {
      string msg = "Invalid PDS labels in file [" + p_labelFile +
                   "]. Label value [" +
                   pdsXlater.InputKeywordName("DataFilePointer") + "]" ;
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    units.Trim(" \t\r\v\n");
    // Set the number of file header bytes using a byte count
    // Byte positions are 1 based
    if (units == "BYTES") {
      SetFileHeaderBytes(str.ToInteger() - 1);
    }
    // Set the number of file header bytes using a record number
    else {
      Isis::iString recSize = pdsXlater.Translate("DataFileRecordBytes");
      SetFileHeaderBytes ((str.ToInteger() - 1) * recSize.ToInteger());
    }

    //------------------------------------------------------------
    // Find the image data base and multiplier
    //------------------------------------------------------------
    // First see if there are base and multiplier in the band bin group
    if ((pdsXlater.InputHasKeyword("BandBase")) &&
        (pdsXlater.InputHasKeyword("BandMultiplier"))) {
      vector<double> bases;
      vector<double> mults;
      for (int i=0; i<pdsXlater.InputKeyword("BandBase").Size(); i++) {
        str = pdsXlater.Translate("BandBase", i);
        bases.push_back(str.ToDouble());
        str = pdsXlater.Translate("BandMultiplier", i);
        mults.push_back(str.ToDouble());
      }
      SetBase(bases);
      SetMultiplier(mults);
    }
    else {
      str = pdsXlater.Translate ("CoreBase");
      SetBase (str.ToDouble());
      str = pdsXlater.Translate ("CoreMultiplier");
      SetMultiplier (str.ToDouble());
    }
  }

 /**
  * Fills the passed in label with the projection information from the PDS label
  * file. The application must write add the projection parameters to the output
  * cube if desired.
  *
  * @param lab The label where the projection parameters will be placed. 
  *  
  * @history 2008-06-06 Tracie Sucharski - Added new InputGroup for 
  *                              PdsProjectionTypeImage to handle Magellan.
  *  
  */
  void ProcessImportPds::TranslatePdsProjection (Isis::Pvl &lab) {

    // Create a temporary Isis::PvlTranslationManager so we can find out what
    // type of projection labels exist
    stringstream trnsStrm;
    trnsStrm << "Group = PdsProjectionTypeImage" << endl;
    trnsStrm << "  InputPosition = IMAGE_MAP_PROJECTION" << endl;
    trnsStrm << "  InputPosition = IMAGE_MAP_PROJECTION_CATALOG" << endl;
    trnsStrm << "  InputKey = MAP_PROJECTION_TYPE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsProjectionTypeQube" << endl;
    trnsStrm << "  InputPosition = (QUBE,IMAGE_MAP_PROJECTION)" << endl;
    trnsStrm << "  InputKey = MAP_PROJECTION_TYPE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PdsProjectionTypeSpectralQube" << endl;
    trnsStrm << "  InputPosition = (SPECTRAL_QUBE,IMAGE_MAP_PROJECTION)" << endl;
    trnsStrm << "  InputKey = MAP_PROJECTION_TYPE" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "END";

    Isis::PvlTranslationManager projType (p_pdsLabel, trnsStrm);

    // Set up the correct projection translation table for this label
    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    Isis::iString transDir = (string) dataDir["Base"];

    Isis::Filename transFile;
    if (projType.InputHasKeyword("PdsProjectionTypeImage")) {
      transFile = transDir + "/" + "translations/pdsImageProjection.trn";
    }
    else if (projType.InputHasKeyword("PdsProjectionTypeQube")) {
      transFile = transDir + "/" + "translations/pdsQubeProjection.trn";
    }
    else if (projType.InputHasKeyword("PdsProjectionTypeSpectralQube")) {
      transFile = transDir + "/" + "translations/pdsSpectralQubeProjection.trn";
    }
    else {
      return;
    }

    Isis::PvlTranslationManager pdsXlater (p_pdsLabel, transFile.Expanded());

    ExtractPdsProjection(pdsXlater);

    Isis::PvlGroup mapGroup("Mapping");
    mapGroup += Isis::PvlKeyword("ProjectionName", p_projection);
    mapGroup += Isis::PvlKeyword("TargetName", p_targetName);
    mapGroup += Isis::PvlKeyword("EquatorialRadius", p_equatorialRadius, "meters");
    mapGroup += Isis::PvlKeyword("PolarRadius", p_polarRadius, "meters");
    mapGroup += Isis::PvlKeyword("LongitudeDirection", p_longitudeDirection);
    mapGroup += Isis::PvlKeyword("LongitudeDomain", p_longitudeDomain);
    mapGroup += Isis::PvlKeyword("LatitudeType", p_latitudeType);
    if (p_minimumLatitude != Isis::NULL8) {
      mapGroup += Isis::PvlKeyword("MinimumLatitude", p_minimumLatitude);
    }
    if (p_maximumLatitude != Isis::NULL8) {
      mapGroup += Isis::PvlKeyword("MaximumLatitude", p_maximumLatitude);
    }
    if (p_minimumLongitude != Isis::NULL8) {
      mapGroup += Isis::PvlKeyword("MinimumLongitude", p_minimumLongitude);
    }
    if (p_maximumLongitude != Isis::NULL8) {
      mapGroup += Isis::PvlKeyword("MaximumLongitude", p_maximumLongitude);
    }

    // if both longitudes exist, verify they are ordered correctly
    if (p_minimumLongitude != Isis::NULL8 && p_maximumLongitude != Isis::NULL8) {
      if(p_maximumLongitude <= p_minimumLongitude) {
        if(p_longitudeDomain == 180) {
          mapGroup["MinimumLongitude"] = -180;
          mapGroup["MaximumLongitude"] =  180;
        }
        else {
          mapGroup["MinimumLongitude"] = 0;
          mapGroup["MaximumLongitude"] = 360;
        }
      }
    }

    mapGroup += Isis::PvlKeyword("PixelResolution", p_pixelResolution, "meters/pixel");
    mapGroup += Isis::PvlKeyword("Scale", p_scaleFactor, "pixels/degree");
    mapGroup += Isis::PvlKeyword("UpperLeftCornerX", p_upperLeftX, "meters");
    mapGroup += Isis::PvlKeyword("UpperLeftCornerY", p_upperLeftY, "meters");
    if (p_rotation != 0.0) {
      mapGroup += Isis::PvlKeyword("Rotation", p_rotation);
    }

    // To handle new projections without the need to modify source code
    // we will construct a filename from the projection.  The filename will
    // contain the projection specific translations from PDS to ISIS for each
    // projection

    string projSpecificFilename = "$base/translations/pdsImport";
    projSpecificFilename += p_projection + ".trn";
    Isis::PvlTranslationManager specificXlater (p_pdsLabel, projSpecificFilename);

    lab.AddGroup(mapGroup);
    specificXlater.Auto(lab);

    OutputCubes[0]->PutGroup(lab.FindGroup("Mapping"));
  }

 /**
  * Extract all possible PDS projection parameters from the PDS label
  *
  * @param pdsXlater
  *
  * @throws Isis::iException::Message
  *
  * @history 2007-04-12 Tracie Sucharski - Modified the projection translation
  *                            tables to include additional versions of
  *                            Longitude direction, latitude type and if
  *                            the min or max longitude values is greater than
  *                            180, change longitude domain to 360.
  *                            Read projection mults/offsets from def file
  *                            so lat/lon values are correct.
  * @history 2007-07-12 Stuart Sides- Modified to handle units
  *                            of meters on the pixel
  *                            resolution
  *  
  * @history 2008-06-06 Tracie Sucharski, Added LineProjectionOffset2 for 
  *                             Magellan images.
  * @history 2008-06-09 Tracie Sucharski, Added MinimumLongitude2 and 
  *                             MaximumLongitude2 for Magellan images.
  *  
  */
  void ProcessImportPds::ExtractPdsProjection(Isis::PvlTranslationManager &pdsXlater) {

    Isis::iString str;

    if (pdsXlater.InputHasKeyword("ProjectionName")) {
      p_projection = pdsXlater.Translate("ProjectionName");
    }
    else {
      string message = "No projection name in labels";
      throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
    }

    if (pdsXlater.InputHasKeyword("TargetName")) {
      p_targetName = pdsXlater.Translate("TargetName");
    }
    else {
      string message = "No target name in labels";
      throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
    }

    if (pdsXlater.InputHasKeyword("EquatorialRadius")) {
      str = pdsXlater.Translate ("EquatorialRadius");
      p_equatorialRadius = str.ToDouble() * 1000.0;
    }
    else {
      string message = "No equatorial radius name in labels";
      throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
    }

    if (pdsXlater.InputHasKeyword("PolarRadius")) {
      str = pdsXlater.Translate ("PolarRadius");
      p_polarRadius = str.ToDouble() * 1000.0;
    }
    else {
      string message = "No polar radius in labels";
      throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
    }

    if (pdsXlater.InputHasKeyword("LongitudeDirection")) {
      p_longitudeDirection = pdsXlater.Translate ("LongitudeDirection");
    }
    else {
      p_longitudeDirection = pdsXlater.Translate ("LongitudeDirection2");
    }

    if(p_polarRadius == p_equatorialRadius) {
      p_latitudeType = "Planetocentric";
    }
    else if (pdsXlater.InputHasKeyword("LatitudeType2")) {
      p_latitudeType = pdsXlater.Translate ("LatitudeType2");
    }
    else {
      p_latitudeType = pdsXlater.Translate ("LatitudeType");
    }

    if (pdsXlater.InputHasKeyword("MinimumLatitude")) {
      str = pdsXlater.Translate ("MinimumLatitude");
      try {
        p_minimumLatitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_minimumLatitude = Isis::NULL8;
      }
    }
    else {
      p_minimumLatitude = Isis::NULL8;
    }

    if (pdsXlater.InputHasKeyword("MaximumLatitude")) {
      str = pdsXlater.Translate ("MaximumLatitude");
      try {
        p_maximumLatitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_maximumLatitude = Isis::NULL8;
      }
    }
    else {
      p_maximumLatitude = Isis::NULL8;
    }

    // This variable represents if the longitudes were read in as
    //   positive west
    bool positiveWest = false;
    if (pdsXlater.InputHasKeyword("MinimumLongitude")) {
      str = pdsXlater.Translate ("MinimumLongitude");
      try {
        positiveWest = true;
        p_minimumLongitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_minimumLongitude = Isis::NULL8;
      }
    }
    else if (pdsXlater.InputHasKeyword("MinimumLongitude2")) {
      str = pdsXlater.Translate ("MinimumLongitude2");
      try {
        p_minimumLongitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_minimumLongitude = Isis::NULL8;
      }
    }
    else {
      p_minimumLongitude = Isis::NULL8;
    }

    if (pdsXlater.InputHasKeyword("MaximumLongitude")) {
      str = pdsXlater.Translate ("MaximumLongitude");
      try {
        positiveWest = true;
        p_maximumLongitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_maximumLongitude = Isis::NULL8;
      }
    }
    else if (pdsXlater.InputHasKeyword("MaximumLongitude2")) {
      str = pdsXlater.Translate ("MaximumLongitude2");
      try {
        p_maximumLongitude = str.ToDouble();
      }
      catch (Isis::iException &e) {
        e.Clear();
        p_maximumLongitude = Isis::NULL8;
      }
    }
    else {
      p_maximumLongitude = Isis::NULL8;
    }

    str = pdsXlater.Translate ("LongitudeDomain");
    p_longitudeDomain = str.ToInteger();

    /**
     * The input file does not have a longitude domain. 
     * We need to figure it out!
     * 
     * The current process is two-step. First, we use the
     * longitude direction to swap into what should be the
     * proper order of min,max longitude. Then, if the values
     * are still misordered, we have a 180 domain projection.
     * Try converting the minimum to 180 domain, which hopefully
     * results in ordering the min,max properly. Only do this to
     * the minimum because if they are out of order, then it must 
     * be something like 330-30 which needs to be -30-30.
     * 
     * pdsImageProjection.trn assumes EasternMost is the MINIMUM, 
     * which is PositiveWest. For a PositiveEast image this 
     * swap should occur. On a PositiveWest image this swap should not
     * occur.
     */
    if(positiveWest && (p_longitudeDirection.compare("PositiveEast") == 0)) {
      double tmp = p_minimumLongitude;
      p_minimumLongitude = p_maximumLongitude;
      p_maximumLongitude = tmp;
    }

    if(p_minimumLongitude > p_maximumLongitude) {
      // Force the change to 180
      p_longitudeDomain = 180;
      p_minimumLongitude = Isis::Projection::To180Domain(p_minimumLongitude);
    }

     //  If either the minimumLongitude or maximumLongitude are < 0, change
     //  longitude Domain to 180.
    if (p_minimumLongitude < 0 || p_maximumLongitude < 0) {
      p_longitudeDomain = 180;
    }

    str = pdsXlater.Translate ("PixelResolution");
    p_pixelResolution = str.ToDouble();
    str = pdsXlater.InputKeyword ("PixelResolution").Unit();
    str.UpCase();
    // Assume KM/PIXEL if the unit doesn't exist or is not METERS/PIXEL
    if ((str != "METERS/PIXEL") && (str != "M/PIXEL")) {
      p_pixelResolution *= 1000.0;
    }

    str = pdsXlater.Translate ("Scale");
    p_scaleFactor = str.ToDouble();

    str = pdsXlater.Translate("Rotation");
    p_rotation = str.ToDouble();

    //  Look for projection offsets/mults to convert between line/samp and x/y
    double xoff,yoff,xmult,ymult;
    GetProjectionOffsetMults(xoff,yoff,xmult,ymult);

    if (pdsXlater.InputHasKeyword("LineProjectionOffset")) {
      str = pdsXlater.Translate ("LineProjectionOffset");
    }
    else {
      str = pdsXlater.Translate ("LineProjectionOffset2");
    }
    p_lineProjectionOffset = str.ToDouble();
    p_upperLeftY = ymult * (p_lineProjectionOffset + yoff) * p_pixelResolution;

    if (pdsXlater.InputHasKeyword("SampleProjectionOffset")) {
      str = pdsXlater.Translate ("SampleProjectionOffset");
    }
    else {
      str = pdsXlater.Translate ("SampleProjectionOffset2");
    }
    p_sampleProjectionOffset = str.ToDouble();
    p_upperLeftX = xmult * (p_sampleProjectionOffset + xoff) * p_pixelResolution;


  }

  /**
   * End the processing sequence and cleans up by closing cubes,
   * freeing memory, etc. Adds the OriginalLabel data to the end
   * of the cube file, unless OmitOriginalLabel() has been called.
   */
  void ProcessImportPds::EndProcess(){
    if (p_keepOriginalLabel) {
      OriginalLabel ol(p_pdsLabel);
      for (unsigned int i=0; i<OutputCubes.size(); i++) {
        OutputCubes[i]->Write(ol);
      }
    }
    Process::EndProcess();
  }

  /**
   * Prevents the Original Label blob from being written out to
   * the end of the cube.
   */
  void ProcessImportPds::OmitOriginalLabel(){
    p_keepOriginalLabel = false;
  }


  /**
  * Identify the source of this file PDS or ISIS2.
  *
  * @param inputLabel  The label from the input file.
  *
  */
  void ProcessImportPds::IdentifySource (Isis::Pvl &inputLabel) {

    // Create a temporary Isis::PvlTranslationManager so we can find out what
    // type of input file we have
    stringstream trnsStrm;
    trnsStrm << "Group = PdsFile" << endl;
    trnsStrm << "  InputPosition = ROOT" << endl;
    trnsStrm << "  InputKey = PDS_VERSION_ID" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = Isis2File" << endl;
    trnsStrm << "  InputPosition = ROOT" << endl;
    trnsStrm << "  InputKey = CCSD3ZF0000100000001NJPL3IF0PDS200000001" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "END";

    Isis::PvlTranslationManager sourceXlater (inputLabel, trnsStrm);

    if (sourceXlater.InputHasKeyword("PdsFile")) {
      p_source = PDS;
    }
    else if (sourceXlater.InputHasKeyword("Isis2File")) {
      p_source = ISIS2;
    }
    else {
      p_source = NOSOURCE;
    }

  }

  /**
   * Return true if ISIS2 cube, else return false
   *
   *  @return (bool) returns true if pds file is an Isis2 file
   *
   *  @history 2007-04-12 Tracie Sucharski - New method
   *
   */
  bool ProcessImportPds::IsIsis2() {

    if (p_source == ISIS2) {
      return true;
    }
    else {
      return false;
    }
  }



  /**
   * Translate as many of the ISIS2 labels as possible
   *
   * @param lab The label where the translated Isis2 keywords will
   *            be placed
   *
   */
  void ProcessImportPds::TranslateIsis2Labels (Isis::Pvl &lab) {
    TranslateIsis2BandBin (lab);
    TranslateIsis2Instrument (lab);
  }



  /**
   * Translate as many of the PDS labels as possible
   *
   * @param lab The label where the translated Isis2 keywords will
   *            be placed
   *
   */
  void ProcessImportPds::TranslatePdsLabels (Isis::Pvl &lab) {
    TranslatePdsBandBin (lab);
    TranslatePdsArchive (lab);
  }

  /**
   * Fill as many of the Isis3 BandBin labels as possible
   *
   * @param lab The lable where the translated Isis2 keywords will
   *            be placed
   */
  void ProcessImportPds::TranslateIsis2BandBin (Isis::Pvl &lab) {
    // Set up a translater for Isis2 labels
    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    Isis::iString transDir = (string) dataDir["Base"];

    Isis::Filename transFile (transDir + "/" + "translations/isis2bandbin.trn");
    Isis::PvlTranslationManager isis2Xlater (p_pdsLabel, transFile.Expanded());

    // Add all the Isis2 keywords that can be translated to the requested label
    isis2Xlater.Auto(lab);
  }

  /**
   * Fill as many of the Isis3 instrument labels as possible
   *
   * @param lab The label where the tramslated Isis2 keywords will
   *            be placed
   */
  void ProcessImportPds::TranslateIsis2Instrument (Isis::Pvl &lab) {
    // Set up a translater for Isis2 labels
    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    Isis::iString transDir = (string) dataDir["Base"];
    Isis::Filename transFile (transDir + "/" + "translations/isis2instrument.trn");
    Isis::PvlTranslationManager isis2Xlater (p_pdsLabel, transFile.Expanded());

    // Add all the Isis2 keywords that can be translated to the requested label
    isis2Xlater.Auto(lab);

    //Check StartTime for appended 'z' (Zulu time) and remove
    Isis::PvlGroup &inst = lab.FindGroup("Instrument");

    if (inst.HasKeyword("StartTime")) {
      Isis::PvlKeyword &stkey = inst["StartTime"];
      Isis::iString stime = stkey[0];
      stime = stime.Trim("Zz");
      stkey = stime;
    }
  }

  /**
   * Fill as many of the Isis3 BandBin labels as possible
   *
   * @param lab The lable where the translated PDS keywords will
   *            be placed
   */
  void ProcessImportPds::TranslatePdsBandBin (Isis::Pvl &lab) {
    // Set up a translater for PDS labels
    Isis::Filename transFile (p_transDir + "/" + "translations/pdsImageBandBin.trn");
    Isis::PvlTranslationManager isis2Xlater (p_pdsLabel, transFile.Expanded());

    // Add all the Isis2 keywords that can be translated to the requested label
    isis2Xlater.Auto(lab);
  }

  /**
   * Fill as many of the Isis3 BandBin labels as possible
   *
   * @param lab The lable where the translated PDS keywords will
   *            be placed
   */
  void ProcessImportPds::TranslatePdsArchive (Isis::Pvl &lab) {
    // Set up a translater for PDS labels
    Isis::Filename transFile (p_transDir + "/" + "translations/pdsImageArchive.trn");
    Isis::PvlTranslationManager isis2Xlater (p_pdsLabel, transFile.Expanded());

    // Add all the Isis2 keywords that can be translated to the requested label
    isis2Xlater.Auto(lab);
  }

  /**
   * Read mults and offsets from a def file in order to calculate the upper
   * left x/y.
   *
   * @param[out] xoff    (double &) x offset
   * @param[out] yoff    (double &) y offset
   * @param[out] xmult   (double &) x multiplicative factor
   * @param[out] ymult   (double &) y multiplicative factor
   *
   * @history 2007-04-12 Tracie Sucharski - New Method
   *
   */
  void ProcessImportPds::GetProjectionOffsetMults (double &xoff, double &yoff,
                                                   double &xmult, double &ymult) {

    xmult = -1.0;
    ymult = 1.0;
    xoff = -0.5;
    yoff = -0.5;

    //  Open projectionOffsetMults file
    Isis::Pvl p(p_transDir + "/" + "translations/pdsProjectionLineSampToXY.def");

    Isis::PvlObject &projDef = p.FindObject("ProjectionOffsetMults",
                                            Pvl::Traverse);

    for (int g=0; g<projDef.Groups(); g++) {
      string key = projDef.Group(g)["Keyword"];
      if (p_pdsLabel.HasKeyword(key)) {
        string value = p_pdsLabel[key];
        string pattern = projDef.Group(g)["Pattern"];
        //  If value contains pattern, then set the mults to what is in translation file
        if (value.find(pattern) != std::string::npos) {
          xmult = projDef.Group(g)["xMult"];
          ymult = projDef.Group(g)["yMult"];
          xoff = projDef.Group(g)["xOff"];
          yoff = projDef.Group(g)["yOff"];
          return;
        }
      }
    }
  }


}

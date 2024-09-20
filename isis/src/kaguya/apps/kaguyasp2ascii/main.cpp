/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <bitset>
#include <cstdio>
#include <QString>
#include <QDir>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "FileName.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  UserInterface &ui = Application::GetUserInterface();

  QString inFile = ui.GetFileName("FROM");
  Pvl lab(inFile.toStdString());
  std::string dataFile = lab.findKeyword("FILE_NAME")[0];

  // Detached labels use format keyword = "dataFile" value <unit>
  int keywordIndex = 1;

  if (FileName(inFile.toStdString()).baseName() == FileName(dataFile).baseName()){
    // data files usually do not include path information.  If input basename matches datafile basename, include path information
    // this allows users to specify data that is not in the current directory.
    dataFile = inFile.toStdString();
    // Attached labels use format keyword = value <units>
    keywordIndex = 0;
  } else {
     // data files specification in label usually do not include path
     // information. If label is detached label, data file is located at
     // the same directory as label file. 
     // this allows users to specify data that is not in the current directory.
     std::filesystem::path dir = FileName(inFile.toStdString()).dir();
     dataFile = (dir / dataFile).string();
  }

  ofstream os;
  std::string outFile = FileName(ui.GetFileName("TO").toStdString()).expanded();
  os.open(outFile.c_str(), ios::out);

  int minobs = 1;
  int maxobs = 1000000;
  if (ui.WasEntered("MINOBS")) {
    QString keyval = ui.GetString("MINOBS");
    minobs = keyval.toInt();
    if (minobs < 1) {
      minobs = 1;
    }
  }
  if (ui.WasEntered("MAXOBS")) {
    QString keyval = ui.GetString("MAXOBS");
    maxobs = keyval.toInt();
  }
  if (maxobs < minobs) {
    int temp = minobs;
    minobs = maxobs;
    maxobs = temp;
  }

  int wavptr = 0;
  int rawptr = 0;
  int radptr = 0;
  int refptr1 = 0;
  int refptr2 = 0;
  int qaptr = 0;

  if (lab.hasKeyword("^SP_SPECTRUM_WAV")) {
    wavptr = std::stoi(lab.findKeyword("^SP_SPECTRUM_WAV")[keywordIndex]) - 1;
  }
  if (lab.hasKeyword("^SP_SPECTRUM_RAW")) {
    rawptr = std::stoi(lab.findKeyword("^SP_SPECTRUM_RAW")[keywordIndex]) - 1;
  }
  if (lab.hasKeyword("^SP_SPECTRUM_RAD")) {
    radptr = std::stoi(lab.findKeyword("^SP_SPECTRUM_RAD")[keywordIndex]) - 1;
  }
  //older-format file without calibrated NIR2 data
  if (lab.hasKeyword("^SP_SPECTRUM_REF")) {
    refptr1 = std::stoi(lab.findKeyword("^SP_SPECTRUM_REF")[keywordIndex]) - 1;
  }
  //newer-format file with calibrated NIR2 data and 2 different Reflectances
  if (lab.hasKeyword("^SP_SPECTRUM_REF1")) {
    refptr1 = std::stoi(lab.findKeyword("^SP_SPECTRUM_REF1")[keywordIndex]) - 1;
  }
  if (lab.hasKeyword("^SP_SPECTRUM_REF2")) {
    refptr2 = std::stoi(lab.findKeyword("^SP_SPECTRUM_REF2")[keywordIndex]) - 1;
  }
  if (lab.hasKeyword("^SP_SPECTRUM_QA")) {
    qaptr = std::stoi(lab.findKeyword("^SP_SPECTRUM_QA")[keywordIndex]) - 1;
  }

  FILE *spcptr;
  if ((spcptr = fopen(dataFile.c_str(),"rb")) == 0) {
    std::string msg = "Error opening input Kaguya SP file [" + dataFile + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  union {
    unsigned char ichar[2];
    unsigned short iword;
  } ibuf;

  union {
    unsigned char ichar[2];
    unsigned short iword;
  } obuf;

  if (!lab.hasObject("SP_SPECTRUM_WAV") || !lab.hasObject("SP_SPECTRUM_QA") ||
      !lab.hasObject("SP_SPECTRUM_RAD") || !(lab.hasObject("SP_SPECTRUM_REF") ||
      (lab.hasObject("SP_SPECTRUM_REF1") && lab.hasObject("SP_SPECTRUM_REF2")))) {
    std::string msg = "Input file [" + inFile.toStdString() + "] is not a valid ";
    msg += "Kaguya Spectral Profiler file";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  PvlObject wavobj = lab.findObject("SP_SPECTRUM_WAV");
  int wavlines = std::stoi(wavobj.findKeyword("LINES")[0]);
  int wavsamps = std::stoi(wavobj.findKeyword("LINE_SAMPLES")[0]);
  QString wavtype = QString::fromStdString(wavobj.findKeyword("SAMPLE_TYPE"));
  int wavbits = std::stoi(wavobj.findKeyword("SAMPLE_BITS")[0]);
  if (wavlines != 1 || wavsamps != 296 || wavtype != "MSB_UNSIGNED_INTEGER" ||
      wavbits != 16) {
    std::string msg = "Wavelength data in input file does not meet the following ";
    msg += "requirements: Size=1 row x 296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
    msg += "BitType: 16";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  QString keyval = QString::fromStdString(wavobj.findKeyword("SCALING_FACTOR")[0]);
  bool ok;
  double wavscale = keyval.toDouble(&ok);
  if (!ok) {
    wavscale = 1.0;
  }
  keyval = QString::fromStdString(wavobj.findKeyword("OFFSET")[0]);
  double wavoffset = keyval.toDouble(&ok);
  if (!ok) {
    wavoffset = 0.0;
  }

  double wavelength[296];
  fseek(spcptr,wavptr,SEEK_SET);
  for (int i=0; i<wavlines; i++) {
    for (int j=0; j<wavsamps; j++) {
      size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
      if (results != 1) {
        std::string msg = "Error reading wavelength data from input file";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      obuf.ichar[0] = ibuf.ichar[1];
      obuf.ichar[1] = ibuf.ichar[0];
      wavelength[j] = (float)obuf.iword * wavscale + wavoffset;
    }
  }

  PvlObject rawobj = lab.findObject("SP_SPECTRUM_RAW");
  int rawlines = std::stoi(rawobj.findKeyword("LINES")[0]);
  int rawsamps = std::stoi(rawobj.findKeyword("LINE_SAMPLES")[0]);
  QString rawtype = QString::fromStdString(rawobj.findKeyword("SAMPLE_TYPE"));
  int rawbits = std::stoi(rawobj.findKeyword("SAMPLE_BITS")[0]);
  if (rawsamps != 296 || rawtype != "MSB_UNSIGNED_INTEGER" ||
      rawbits != 16) {
    std::string msg = "Raw data in input file does not meet the following ";
    msg += "requirements: Size=296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
    msg += "BitType: 16";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  keyval = QString::fromStdString(rawobj.findKeyword("SCALING_FACTOR")[0]);
  double rawscale = keyval.toDouble(&ok);
  if (!ok) {
    rawscale = 1.0;
  }
  keyval = QString::fromStdString(rawobj.findKeyword("OFFSET")[0]);
  double rawoffset = keyval.toDouble(&ok);
  if (!ok) {
    rawoffset = 0.0;
  }

  double raw[296*rawlines];
  fseek(spcptr,rawptr,SEEK_SET);
  for (int i=0; i<rawlines; i++) {
    for (int j=0; j<rawsamps; j++) {
      size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
      if (results != 1) {
        std::string msg = "Error reading raw data from input file";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      obuf.ichar[0] = ibuf.ichar[1];
      obuf.ichar[1] = ibuf.ichar[0];
      raw[j+i*296] = (float)obuf.iword * rawscale + rawoffset;
    }
  }

  PvlObject qaobj = lab.findObject("SP_SPECTRUM_QA");
  int qalines = std::stoi(qaobj.findKeyword("LINES")[0]);
  int qasamps = std::stoi(qaobj.findKeyword("LINE_SAMPLES")[0]);
  QString qatype = QString::fromStdString(qaobj.findKeyword("SAMPLE_TYPE"));
  int qabits = std::stoi(qaobj.findKeyword("SAMPLE_BITS")[0]);
  if (qalines != rawlines || qasamps != 296 || qatype != "MSB_UNSIGNED_INTEGER" ||
      qabits != 16) {
    std::string msg = "Quality Assessment data in input file does not meet the ";
    msg += "following requirements: Size=296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
    msg += "BitType=16";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  keyval = QString::fromStdString(qaobj.findKeyword("SCALING_FACTOR")[0]);
  double qascale = keyval.toDouble(&ok);
  if (!ok) {
    qascale = 1.0;
  }
  keyval = QString::fromStdString(qaobj.findKeyword("OFFSET")[0]);
  double qaoffset = keyval.toDouble(&ok);
  if (!ok) {
    qaoffset = 0.0;
  }

  double qa[296*qalines];
  fseek(spcptr,qaptr,SEEK_SET);
  for (int i=0; i<qalines; i++) {
    for (int j=0; j<qasamps; j++) {
      size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
      if (results != 1) {
        std::string msg = "Error reading quality assessment data from input file";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      obuf.ichar[0] = ibuf.ichar[1];
      obuf.ichar[1] = ibuf.ichar[0];
      qa[j+i*296] = (float)obuf.iword * qascale + qaoffset;
    }
  }

  PvlObject radobj = lab.findObject("SP_SPECTRUM_RAD");
  int radlines = std::stoi(radobj.findKeyword("LINES")[0]);
  int radsamps = std::stoi(radobj.findKeyword("LINE_SAMPLES")[0]);
  QString radtype = QString::fromStdString(radobj.findKeyword("SAMPLE_TYPE"));
  int radbits = std::stoi(radobj.findKeyword("SAMPLE_BITS")[0]);
  if (radlines != qalines || radsamps != 296 || radtype != "MSB_UNSIGNED_INTEGER" ||
      radbits != 16) {
    std::string msg = "Radiance data in input file does not meet the following ";
    msg += "requirements: Size=296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
    msg += "BitType=16";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  keyval = QString::fromStdString(radobj.findKeyword("SCALING_FACTOR")[0]);
  double radscale = keyval.toDouble(&ok);
  if (!ok) {
    radscale = 1.0;
  }
  keyval = QString::fromStdString(radobj.findKeyword("OFFSET")[0]);
  double radoffset = keyval.toDouble(&ok);
  if (!ok) {
    radoffset = 0.0;
  }

  double rad[296*radlines];
  fseek(spcptr,radptr,SEEK_SET);
  for (int i=0; i<radlines; i++) {
    for (int j=0; j<radsamps; j++) {
      size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
      if (results != 1) {
        std::string msg = "Error reading radiance data from input file";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      obuf.ichar[0] = ibuf.ichar[1];
      obuf.ichar[1] = ibuf.ichar[0];
      rad[j+i*296] = (float)obuf.iword * radscale + radoffset;
    }
  }

  PvlObject refobj;
  PvlObject refobj2;

  if (lab.hasKeyword("^SP_SPECTRUM_REF")) {
    refobj = lab.findObject("SP_SPECTRUM_REF");
  }
  else {
    refobj = lab.findObject("SP_SPECTRUM_REF1");
    refobj2 = lab.findObject("SP_SPECTRUM_REF2");
  }

  int reflines = std::stoi(refobj.findKeyword("LINES")[0]);
  int refsamps = std::stoi(refobj.findKeyword("LINE_SAMPLES")[0]);
  QString reftype = QString::fromStdString(refobj.findKeyword("SAMPLE_TYPE"));
  int refbits = std::stoi(refobj.findKeyword("SAMPLE_BITS")[0]);
  if (reflines != radlines || refsamps != 296 || reftype != "MSB_UNSIGNED_INTEGER" ||
      refbits != 16) {
    std::string msg = "Reflectance data in input file does not meet the following ";
    msg += "requirements: Size=296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
    msg += "BitType=16";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  keyval = QString::fromStdString(refobj.findKeyword("SCALING_FACTOR")[0]);
  double refscale = keyval.toDouble(&ok);
  if (!ok) {
    refscale = 1.0;
  }
  keyval = QString::fromStdString(refobj.findKeyword("OFFSET")[0]);
  double refoffset = keyval.toDouble(&ok);
  if (!ok) {
    refoffset = 0.0;
  }

  //import reflectance or "reflectance 1" in newer files
  double ref[296*reflines];
  fseek(spcptr,refptr1,SEEK_SET);
  for (int i=0; i<reflines; i++) {
    for (int j=0; j<refsamps; j++) {
      size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
      if (results != 1) {
        std::string msg = "Error reading reflectance data from input file";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      obuf.ichar[0] = ibuf.ichar[1];
      obuf.ichar[1] = ibuf.ichar[0];
      ref[j+i*296] = (float)obuf.iword * refscale + refoffset;
    }
  }

  //import reflectance 2 if it exists
  double *ref2 = NULL;
  if (lab.hasKeyword("^SP_SPECTRUM_REF2")) {
    int reflines2 = std::stoi(refobj2.findKeyword("LINES")[0]);
    int refsamps2 = std::stoi(refobj2.findKeyword("LINE_SAMPLES")[0]);
    QString reftype2 = QString::fromStdString(refobj2.findKeyword("SAMPLE_TYPE"));
    int refbits2 = std::stoi(refobj2.findKeyword("SAMPLE_BITS")[0]);
    if (reflines2 != radlines || refsamps2 != 296 || reftype2 != "MSB_UNSIGNED_INTEGER" ||
     refbits2 != 16) {
     std::string msg = "Reflectance #2 data in input file does not meet the following ";
     msg += "requirements: Size=296 columns, DataType=MSB_UNSIGNED_INTEGER, ";
     msg += "BitType=16";
     throw IException(IException::User, msg, _FILEINFO_);
    }

    keyval = QString::fromStdString(refobj2.findKeyword("SCALING_FACTOR")[0]);
    double refscale2 = keyval.toDouble(&ok);
    if (!ok) {
      refscale2 = 1.0;
    }
    keyval = QString::fromStdString(refobj2.findKeyword("OFFSET")[0]);
    double refoffset2 = keyval.toDouble(&ok);
    if (!ok) {
      refoffset2 = 0.0;
    }

    ref2 = new double[296*reflines2];
    fseek(spcptr,refptr2,SEEK_SET);
    for (int i=0; i<reflines2; i++) {
      for (int j=0; j<refsamps2; j++) {
        size_t results = fread((void *)ibuf.ichar,2,1,spcptr);
        if (results != 1) {
          std::string msg = "Error reading reflectance (Ref2) data from input file";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        obuf.ichar[0] = ibuf.ichar[1];
        obuf.ichar[1] = ibuf.ichar[0];
        ref2[j+i*296] = (float)obuf.iword * refscale2 + refoffset2;
      }
    }
  }

  if (wavsamps != rawsamps || wavsamps != radsamps || wavsamps != refsamps ||
      wavsamps != qasamps || wavsamps != 296) {
    std::string msg = "Number of columns in input file must be 296";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (rawlines < maxobs) {
    maxobs = rawlines;
  }
  if (rawlines < minobs) {
    minobs = rawlines;
  }

  os << "Wavelength";

  //If we have a newer-format file with two Reflectances, output both
  if (ref2 != NULL) {
    for (int i=minobs; i<=maxobs; i++) {
      os << "\t" << "Raw" << i << "\t"<< "Rad" << i << "\t" <<"Ref1_" << i << "\t" << "Ref2_" << i
          << "\t" << "QA" << i;
    }
    os << endl;

    for (int j = 0; j < 296; j++) {
      os << wavelength[j];
      for (int i=minobs-1; i<maxobs; i++) {
        os << "\t" << raw[j+i*296] << "\t" << rad[j+i*296] << "\t" << ref[j+i*296] << "\t"
            << ref2[j+i*296] << "\t" << (std::bitset<16>) qa[j+i*296];
      }
      os << endl;
     }
     delete ref2;
  }
  else {
    for (int i=minobs; i<=maxobs; i++) {
      os << "\t" << "Raw" << i << "\t" << "Rad" << i << "\t" << "Ref" << i << "\t" << "QA" << i;
    }
    os << endl;

    for (int j = 0; j < 296; j++) {
      os << wavelength[j];
      for (int i=minobs-1; i<maxobs; i++) {
        os << "\t" << raw[j+i*296] << "\t" << rad[j+i*296] << "\t" << ref[j+i*296] << "\t"
            << (std::bitset<16>) qa[j+i*296];
      }
       os << endl;
    }
  }

  os.close();
}

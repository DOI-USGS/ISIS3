#if !defined(MdisCalUtils_h)
#define MdisCalUtils_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $
 * $Date: 2008/11/19 22:35:13 $
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       
#include <string>
#include <vector>
#include <cmath>

#include "iString.h"
#include "Filename.h"
#include "CSVReader.h"

namespace Isis {
/** 
 * @brief Helper function to convert values to doubles
 * 
 * @param T Type of value to convert
 * @param value Value to convert
 * 
 * @return double Converted value
 */
template <typename T> double ToDouble(const T &value) {
    return (iString(value).Trim(" \r\t\n").ToDouble());
}

template <typename T> int ToInteger(const T &value) {
    return (iString(value).Trim(" \r\t\n").ToInteger());
}

template <typename T> inline T MIN(const T &A, const T &B) {
  return ( ((A) < (B)) ? (A) : (B));
 }

 template <typename T> inline T MAX(const T &A, const T &B) {
   return ( ((A) > (B)) ? (A) : (B));
 }

 inline std::string Quote(const std::string &value) {
   if (value.empty()) return (value);
   if(value[0] == '"') return (value);
   return (std::string('"'+value+'"'));
 }

/**
 * @brief Load required NAIF kernels required for timing needs
 * 
 * This method maintains the loading of kernels for MESSENGER timing and 
 * planetary body ephemerides to support time and relative positions of planet 
 * bodies. 
 */
static void loadNaifTiming( ) {
  static bool _naifLoaded(false);
  if (!_naifLoaded) {
//  Load the NAIF kernels to determine timing data
    Isis::Filename leapseconds("$base/kernels/lsk/naif????.tls");
    leapseconds.HighestVersion();

    Isis::Filename sclk("$messenger/kernels/sclk/messenger_???.tsc");
    sclk.HighestVersion();

    Isis::Filename pck("$base/kernels/spk/de???.bsp");
    pck.HighestVersion();

//  Load the kernels
    std::string leapsecondsName(leapseconds.Expanded());
    std::string sclkName(sclk.Expanded());
    std::string pckName(pck.Expanded());
    furnsh_c(leapsecondsName.c_str());
    furnsh_c(sclkName.c_str());
    furnsh_c(pckName.c_str());

//  Ensure it is loaded only once
    _naifLoaded = true;
  }
  return;
}

  /**
   * @brief Computes the distance from the Sun to the observed body
   * 
   * This method requires the appropriate NAIK kernels to be loaded that
   * provides instrument time support, leap seconds and planet body ephemeris.
   * 
   * @return double Distance in AU between Sun and observed body 
   */
static bool sunDistanceAU(const std::string &scStartTime,
                          const std::string &target,
                          double &sunDist) {

  //  Ensure NAIF kernels are loaded
  loadNaifTiming();
  sunDist = 1.0;

  //  Determine if the target is a valid NAIF target
  SpiceInt tcode;
  SpiceBoolean found;
  (void) bodn2c_c(target.c_str(), &tcode, &found);
  if (!found) return (false);

  //  Convert starttime to et
  double obsStartTime;
  scs2e_c (-236,scStartTime.c_str(),&obsStartTime);

  //  Get the vector from target to sun and determine its length
  double sunv[3];
  double lt;
  (void) spkpos_c(target.c_str(), obsStartTime, "J2000", "LT+S", "sun",
                  sunv, &lt);
  double sunkm = vnorm_c(sunv);

  //  Return in AU units
  sunDist = sunkm/1.49597870691E8;
  return (true);
}

std::vector<double> loadWACCSV(const std::string &fname, int filter, 
                               int nvalues, bool header = true, int skip = 0) {
  //  Open the CSV file
  Filename csvfile(fname);
  CSVReader csv(csvfile.Expanded(), header, skip);
  for (int i = 0 ; i < csv.rows() ; i++) {
    CSVReader::CSVAxis row = csv.getRow(i);
    if (ToInteger(row[0]) == filter) {
      if ((row.dim1()-1) < nvalues) {
        std::string mess = "Number values (" + iString(row.dim1()-1) + 
                           ") in file " + fname + 
                           " less than number requested (" +
                           iString(nvalues) + ")!";
        throw iException::Message(iException::User, mess.c_str(), _FILEINFO_);
      }
      std::vector<double> rsp;
      for(int i = 0 ; i < nvalues ; i++) {
          rsp.push_back(ToDouble(row[1+i]));
      }
     return (rsp);
    }
  }

  // If it reaches here, the filter was not found
  std::ostringstream mess;
  mess << "CSV Vector MDIS filter " << filter <<  ", not found in file " 
       << fname << "!";
  throw iException::Message(iException::User, mess.str(), _FILEINFO_);
}


 std::vector<double> loadNACCSV(const std::string &fname, int nvalues, 
                                bool header = true, int skip = 0) {
  //  Open the CSV file
  Filename csvfile(fname);
  CSVReader csv(csvfile.Expanded(), header, skip);
  CSVReader::CSVAxis row = csv.getRow(0);
  if (row.dim1() < nvalues) {
    std::string mess = "Number values (" + iString(row.dim1()) + 
                       ") in file " + fname + " less than number requested (" +
                       iString(nvalues) + ")!";
    throw iException::Message(iException::User, mess.c_str(), _FILEINFO_);

  }
  std::vector<double> rsp;
  for(int i = 0 ; i < nvalues ; i++) {
    rsp.push_back(ToDouble(row[i]));
  }
  return (rsp);
}


std::vector<double> loadResponsivity(bool isNAC, bool binned, int filter,  
                               std::string &fname) {

  Filename resfile(fname);
  if(fname.empty()) {
    std::string camstr = (isNAC) ? "NAC" : "WAC";
    std::string binstr = (binned)       ? "_BINNED" : "_NOTBIN";
    std::string base   = "$messenger/calibration/RESPONSIVITY/";
    resfile = base + "MDIS" + camstr + binstr + "_RESP_?.TAB";
    resfile.HighestVersion();
    fname = resfile.OriginalPath() + "/" + resfile.Name();
  }

  // Unfortunately NAC has a slightly different format so must do it
  //  explicitly
  if (isNAC) {
    return (loadNACCSV(fname, 4, false, 0));
  }
  else {
    // Load the WAC parameters
    return (loadWACCSV(fname, filter, 4, false, 0));
  }
}


std::vector<double> loadSolarIrr(bool isNAC, bool binned, int filter,  
                                 std::string &fname)  {

  Filename solfile(fname);
  if(fname.empty()) {
    std::string camstr = (isNAC) ? "NAC" : "WAC";
    std::string base   = "$messenger/calibration/SOLAR/";
    solfile = base + "MDIS" + camstr + "_SOLAR_?.TAB";
    solfile.HighestVersion();
    fname = solfile.OriginalPath() + "/" + solfile.Name();
  }

  if (isNAC) {
    return (loadNACCSV(fname, 3, false, 0));
  }
  else {
    return (loadWACCSV(fname, filter, 3, false, 0));
  }
}

double loadSmearComponent(bool isNAC, int filter, std::string &fname) {

  Filename smearfile(fname);
  if(fname.empty()) {
    std::string camstr = (isNAC) ? "NAC" : "WAC";
    std::string base   = "$messenger/calibration/smear/";
    smearfile = base + "MDIS" + camstr + "_FRAME_TRANSFER_??.TAB";
    smearfile.HighestVersion();
    fname = smearfile.OriginalPath() + "/" + smearfile.Name();
  }

  std::vector<double> smear;
  if (isNAC) {
    smear = loadNACCSV(fname, 1, false, 0);
  }
  else {
    smear = loadWACCSV(fname, filter, 1, false, 0);
  }
  return (smear[0]);
}


};
#endif

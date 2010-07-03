/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/12/29 23:03:54 $                                                                 
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

#include "CalParameters.h"
#include "iString.h"
#include "TextFile.h"
#include "Filename.h"
#include "iException.h"
#include "LeastSquares.h"
#include "Pvl.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include <string>
#include <vector>

using namespace std;
namespace Isis {

  CalParameters::CalParameters(const string &fname) {
    try {
      // Extract Pvl Information from the file
      Pvl pvl(fname);
  
      // Get keywords from input cube label
      PvlGroup &instrument = pvl.FindGroup("INSTRUMENT",Pvl::Traverse);
  
      // Make sure it is a viking mission
      iString spacecraft = (string)instrument["SPACECRAFTNAME"];
      iString mission = spacecraft.Token("_");
      if (mission != "VIKING") {
        string msg = "Invalid Keyword [SpacecraftName]. " +  spacecraft + 
          "must start with 'VIKING'";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
      int spn = 0;
      if ((char)spacecraft[spacecraft.size() - 1] == '1') spn = 1;
      if ((char)spacecraft[spacecraft.size() - 1] == '2') spn = 2;
      if (spn == 0) {
        string msg = "Invalid Keyword [SpacecraftName]. " + spacecraft + 
          "must terminate with '1' or '2'";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
      double clock = instrument["SPACECRAFTCLOCKCOUNT"];
      iString instId = (string)instrument["INSTRUMENTID"];
      int cam;
      // Camera State 4 is used to indicate an extended mission. This is
      // necessary because the dust spot changed position during the extended
      // mission, requiring a new set of calibration files.
      int cs4 = 0;
      if ((char)instId[instId.size() - 1] == 'A') {
        if (spn == 1) cam = 7;
        else cam = 8;
      }
      else if ((char)instId[instId.size() - 1] == 'B') {
        if (spn == 1) {
          cam = 4;
          if (clock > 44800000) cs4 = 1;
        }
        else cam = 6;
      }
      else {
        string msg = "Invalid Keyword [InstrumentID]. " + instId;
        msg += "must terminate with an 'A' or 'B'";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }

      string startTime = instrument["STARTTIME"];
      CalcSunDist(startTime);
      p_labexp = (double)instrument["EXPOSUREDURATION"] * 1000.0;  // convert to msec
      string target = " "; 
      PvlKeyword cs1 = instrument["FLOODMODEID"];
      if ((string)cs1 == "ON") cs1 = 1;
      else if ((string)cs1 == "OFF") cs1 = 0;
      PvlKeyword cs2 = instrument["GAINMODEID"];
      if ((string)cs2 == "LOW") cs2 = 0;
      else if ((string)cs2 == "HIGH") cs2 = 1;
      PvlKeyword cs3 = instrument["OFFSETMODEID"];
      if ((string)cs3 == "ON") cs3 = 1;
      else if ((string)cs3 == "OFF") cs3 = 0;
      PvlKeyword wav = pvl.FindGroup("BANDBIN",Pvl::Traverse)["FILTERID"];

      // Set up calibration, linearity, and offset variables for the input file
      vikcalSetup(mission, spn, target, cam, wav, (int)cs1, (int)cs2, (int)cs3, (int)cs4);
      viklinSetup(mission, spn, target, cam, wav, (int)cs1, (int)cs2, (int)cs3, (int)cs4);
      vikoffSetup(mission, spn, target, cam, clock, (int)cs3);
    }
    catch (iException e) {
      string msg = "Input file [" + fname + "] does not appear to be a viking image";
      throw iException::Message(iException::User,msg, _FILEINFO_);
    }
  }

 /**
  * Finds the correct calibration data values for the input cube in the 
  * vikcal.sav file
  * 
  * @param mission The mission name found in the input cube label
  * @param spn The spacecraft number found in the input cube label
  * @param target The target name found in the input cube label (this value is 
  *               not used in the vikcal.sav file, and is overriden with a space)
  * @param cam The camera number found in the input cube label
  * @param cs1 The state of camera 1 found in the input cube label 
  *            (Light Flood Mode)
  * @param cs2 The state of camera 2 found in the input cube label (Gain Mode)
  * @param cs3 The state of camera 3 found in the input cube label (Offset Mode)
  * @param cs4 The state of camera 4, set only if camera is 4, and it is an 
  *            extended mission
  * 
  * @throws Isis::iException::Programmer - Could not find match in vikcal.sav
  */
  void CalParameters::vikcalSetup(string mission, int spn, string target, 
                     int cam, string wav, int cs1, int cs2, int cs3, int cs4) {
    vector<string> line;

    // Read in vikcal.sav calibration file
    TextFile cal("$viking" + iString(spn) + "/calibration/vikcal.sav",
                 "input",line,0,true);

    // Search for a line in the vikcal.sav file that matches our data from the 
    // input label
    for (int i = 0; i < (int)line.size(); i++) { 
      iString temp = line[i];
      temp.ConvertWhiteSpace();
      iString token = temp.Token(" ");
      temp.TrimHead(" ");
      if (token != mission) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != spn) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cam) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (token != wav) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs1) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs2) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs3) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs4) continue; 


      // The line is a match for our data, so set all the 
      // Calibration variables to their correct values
      p_w0 = temp.Token(" ");
      temp.TrimHead(" ");
      p_dist = temp.Token(" ");
      temp.TrimHead(" ");
      p_gain = temp.Token(" ");
      temp.TrimHead(" ");
      p_offset = temp.Token(" ");
      temp.TrimHead(" ");
      p_exp = temp.Token(" ");
      temp.TrimHead(" ");
      p_gainFile = "$viking" + iString(spn) + "/calibration/" + temp.Token(" ") 
                    + ".cub";
      temp.TrimHead(" ");
      p_offsetFile = "$viking" + iString(spn) + "/calibration/" + 
                      temp.Token(" ") + ".cub";
      return;
    }
    string msg = "Could not find match in [vikcal.sav] calibration file";
    throw iException::Message(iException::Programmer,msg, _FILEINFO_);
  }

 /**
  * Finds the correct linearization values for the input image.  This option is
  * in the Isis2 version of vikcal, but due to the values of B in the viklin.sav
  * file it is never actually used.  If this option is necessary for some reason,
  * place the following code in the vikcal.xml file.
  * @code
  * <group name="Option">
  *     <parameter name="LINEAR">
  *         <type>boolean</type>
  *         <brief>
  *             Linearizer option
  *         </brief>
  *         <description>
  *             Flag to indicate if LINEARIZER option is desired.  LINEAR=TRUE 
  *             indicates linear correction to be applied if there is an entry in the 
  *             viklin.sav file for the mission and camera to be processed.  
  *             LINEAR=FALSE indicates that no linear correction is to be applied.
  *             Defaults to TRUE.            
  *         </description>
  *         <default><item>TRUE</item></default>
  *     </parameter>
  * </group>
  * @endcode
  * 
  * @param mission The mission name found in the input cube label
  * @param spn The spacecraft number found in the input cube label
  * @param target The target found in the input cube (this option is not used, 
  *               and the actual target is overriden with a space)
  * @param cam The camera number found in the input cube
  * @param wav The wave length found in the input cube
  * @param cs1 The state of camera 1 found in the input cube label
  *            (Light Flood Mode)
  * @param cs2 The state of camera 2 found in the input cube label (Gain Mode)
  * @param cs3 The state of camera 3 found in the input cube label (Offset Mode)
  * @param cs4 The state of camera 4, set only if the camera is 4, and it is an
  *            extended mission
  * 
  * @throws Isis::iException::Programmer - Could not find match in viklin.sav
  */
  void CalParameters::viklinSetup(string mission, int spn, string target, 
                     int cam, string wav, int cs1, int cs2, int cs3, int cs4) {
      
    vector<string> line;
    TextFile lin("$viking" + iString(spn) + "/calibration/viklin.sav",
                 "input",line,0,true);

    for (int i = 0; i < (int)line.size(); i++) { 
      iString temp = line[i];
      temp.ConvertWhiteSpace();
      iString token = temp.Token(" ");
      temp.TrimHead(" ");
      if (token != mission) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != spn) continue;
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cam) continue;
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (token != wav) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs1) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs2) continue; 
      token = temp.Token(" ");
      temp.TrimHead(" ");
      if (int(token) != cs3) continue;  
    
      // Set all Linearity variables to the correct values
      p_b = temp.Token(" ");
      temp.TrimHead(" ");
      p_k = temp.Token(" ");
      temp.TrimHead(" ");
      p_normpow = temp.Token(" ");
      return;
    }
    string msg = "Could not find match in [viklin.sav] calibration file";
    throw iException::Message(iException::Programmer,msg, _FILEINFO_);
  }

 /**                                                                       
  * Finds the correct offset data values for the input cube in the 
  * vikoff.sav file                                                                     
  *                                                                        
  * @param mission The mission name found in the input cube label
  * @param spn The spacecraft number found in the input cube label
  * @param target The target found in the input cube (this option is not used, 
  *               and the actual target is overriden with a space)
  * @param cam The camera number found in the input cube
  * @param clock The spacecraft clock count found in the input cube label
  * @param cs3 The state of camera 3 found in the input cube label (Offset Mode)
  *                                                                                                    
  * @throws Isis::iException::Programmer - Could not find match in vikoff.sav
  */
  void CalParameters::vikoffSetup(string mission, int spn, string target, 
                     int cam, double clock, int cs3) {
    vector<string> line;

    // Get the correct offset file - depends on which camera the input image is 
    // from
    string fname = "$viking" + iString(spn) + "/calibration/vikoffcam" + 
                    iString(cam) + ".sav";
    TextFile off(fname,"input",line,0,true);
    vector<double> pp[5], off3;
    double pp1_off[5], pp2_off[5], pp_off[5];
    double rise, run, xm, xb;
    double frm1 = -1.0;
    double frm2 = -1.0;
    for (int i = 0; i < (int)line.size(); i++) { 
      iString temp = line[i];
      temp.ConvertWhiteSpace();
      temp.TrimHead(" ");
      iString token = temp.Token(" ");  
      temp.TrimHead(" ");

      // Go through the first line of the offset file and set all the principle
      // point locations
      if (token == "VIKING") {
        if (int(temp.Token(" ")) == spn) { }
        temp.TrimHead(" ");
        if (int(temp.Token(" ")) == cam) { }
        temp.TrimHead(" ");
        pp[0].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[0].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[1].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[1].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[2].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[2].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[3].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[3].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[4].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp[4].push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        continue;
      }
      if (double(token) < clock) {
        frm1 = token;
        off3.push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp1_off[2] = temp.Token(" ");
        temp.TrimHead(" ");
        pp1_off[0] = pp1_off[2] + (double)temp.Token(" ");
        temp.TrimHead(" ");
        pp1_off[1] = pp1_off[2] + (double)temp.Token(" "); 
        temp.TrimHead(" ");
        pp1_off[3] = pp1_off[2] + (double)temp.Token(" "); 
        temp.TrimHead(" ");
        pp1_off[4] = pp1_off[2] + (double)temp.Token(" ");
        continue;
      } 
      else {
        frm2 = token;
        off3.push_back((double)temp.Token(" "));
        temp.TrimHead(" ");
        pp2_off[2] = temp.Token(" ");
        temp.TrimHead(" ");
        pp2_off[0] = pp2_off[2] + (double)temp.Token(" "); 
        temp.TrimHead(" ");
        pp2_off[1] = pp2_off[2] + (double)temp.Token(" "); 
        temp.TrimHead(" ");
        pp2_off[3] = pp2_off[2] + (double)temp.Token(" "); 
        temp.TrimHead(" ");
        pp2_off[4] = pp2_off[2] + (double)temp.Token(" "); 
      }
      if (frm1 == -1.0 || frm2 == -1.0) {
        string msg = "Could not find match in [vikoff.sav] calibration file";
        throw iException::Message(iException::Programmer,msg, _FILEINFO_);
      }

      // Calculate the offset at each point
      run = frm2 - frm1;
      for (int i = 0; i < 5; i++) {
        rise = pp2_off[i] - pp1_off[i];
        xm = rise / run;
        xb = pp1_off[i] - xm * frm1;
        pp_off[i] = xm * clock + xb;
      }
      p_constOff = 0;
      p_off_off = 0.0;

      // Calculate the constant offset
      if (cs3 == 0) {
        p_constOff = 1;
        rise = off3[1] - off3[0];
        xm = rise / run;
        xb = off3[0] - xm * frm1;
        p_off_off = xm * clock + xb;
      }

      // Found correct clock time, set offset values
      BasisFunction bilinearInterp("leastSquares",5,5);
      LeastSquares lsq(bilinearInterp);
      vector<double> known(5); 
      for (int i=0; i<5;i++) {
        known[0] = pp[i][0];
        known[1] = pp[i][0] * pp[i][0];
        known[2] = pp[i][1];
        known[3] = pp[i][0] * pp[i][1];
        known[4] = 1.0; 
        lsq.AddKnown(known, pp_off[i], 1.0);
      }
      lsq.Solve();
      p_A = bilinearInterp.Coefficient(0);
      p_B = bilinearInterp.Coefficient(1);
      p_C = bilinearInterp.Coefficient(2);
      p_D = bilinearInterp.Coefficient(3);
      p_E = bilinearInterp.Coefficient(4);

      return;
    }
    string msg = "Could not find match in [vikoff.sav] calibration file";
    throw iException::Message(iException::Programmer,msg, _FILEINFO_);
  }

 /**
  * Calculates the distance from the sun at the specified time
  * 
  * @param t iTime
  */
  void CalParameters::CalcSunDist(string t) {
    double sunv[3];
    SpiceDouble lt, et;
    Filename fname1 = (Filename)"$base/kernels/lsk/naif0007.tls";
    Filename fname2 = (Filename)"$base/kernels/spk/de405.bsp";
    string tempfname1 = fname1.Expanded();
    string tempfname2 = fname2.Expanded();
    furnsh_c(tempfname1.c_str());
    furnsh_c(tempfname2.c_str());
    utc2et_c(t.c_str(),&et);
    spkezp_c(10,et,"J2000","LT+S",499,sunv,&lt);
    p_dist1 = sqrt(sunv[0] * sunv[0] + sunv[1] * sunv[1] + sunv[2] * sunv[2]);
  }

} // end namespace isis

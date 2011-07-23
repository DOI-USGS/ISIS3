#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include <QRegExp>

#include "DawnVirCamera.h"
#include "Camera.h"
#include "NaifStatus.h"
#include "iString.h"
#include "iException.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "DawnVirCameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "Kernels.h"

#include "tnt/tnt_array2d_utils.h"

using namespace std;
namespace Isis {
    // constructors
    DawnVirCamera::DawnVirCamera(Pvl &lab) : LineScanCamera(lab) {


      PvlGroup &archive = lab.FindGroup("Archive", Isis::Pvl::Traverse);
      int procLevel = archive["ProcessingLevelId"];
      m_is1BCalibrated = (procLevel > 2) ? true : false;

      // Get the start time from labels
      PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      string channelId = inst["ChannelId"];

      // Check for presence of articulation kernel
      bool hasArtCK = hasArticulationKernel(lab);

      // Set proper end frame
      int virFrame(0);
      if (channelId == "VIS") { 
        // Frame DAWN_VIR_VIS : DAWN_VIR_VIS_ZERO
        virFrame = (hasArtCK) ? -203211 : -203221; 
      }
      else { // (channelId == "IR)
        // Frame DAWN_VIR_IR : DAWN_VIR_IR_ZERO
        virFrame = (hasArtCK) ? -203213 : -203223; 
      } 

      InstrumentRotation()->SetFrame(virFrame);

      // We do not want to downsize the cache
      InstrumentRotation()->MinimizeCache(SpiceRotation::No);

      // Set up the camera info from ik/iak kernels
      SetFocalLength();
      SetPixelPitch();

      // Get other info from labels
      PvlKeyword &frameParam = inst["FrameParameter"];
      double m_lineRate = frameParam[0].ToDouble();
      double m_summing  = frameParam[1].ToDouble();

      // Setup detector map
      //  Get the line scane rates/times
      readHouseKeeping(lab.Filename(), m_lineRate);
      new VariableLineScanCameraDetectorMap(this, m_lineRates);
      DetectorMap()->SetDetectorSampleSumming(m_summing);

      // Setup focal plane map
      new CameraFocalPlaneMap(this, NaifIkCode());

      //  Retrieve boresight location from instrument kernel (IK) (addendum?)
      iString ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      FocalPlaneMap()->SetDetectorOrigin(sampleBoreSight, lineBoreSight);

      // Setup distortion map
      new DawnVirCameraDistortionMap(this);

      // Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      // Set initial start time always (label start time is inaccurate)
      SetTime(iTime(startTime()-(exposureTime()/2.0)));    // Isis3nightly
//      SetEphemerisTime(startTime()-(exposureTime()/2.0));  // Isis3.2.1

      //  Now check to determine if we have a cache already.  If we have a cache
      //  table, we are beyond spiceinit and have already computed the proper
      //  point table from the housekeeping data
      if (!InstrumentRotation()->IsCached() && !hasArtCK) {

        // Create new table here prior to creating normal caches
        Table quats = getPointingTable(channelId, virFrame);

        // Create all system tables - all kernels closed after this
        LoadCache();
        InstrumentRotation()->LoadCache(quats);
      }
      else {
        LoadCache();
      }
    }

    /** Returns CK frame identifier  */
    int DawnVirCamera::CkFrameId() const {  return (-203000); }
    /** Returns CK reference frame */
    int DawnVirCamera::CkReferenceId() const { return (1); }
    /** Return PK reference frame */
    int DawnVirCamera::SpkReferenceId() const { return (1); }


   /**
    * @brief Scrubs a string coming out of the housekeeping table 
    *  
    * This routine is needed to clean up strings from the housekeeping table. 
    * There apparently are extraneous characters in the text fields in this 
    * table. 
    *  
    * This method removes all non-printable characters and the space character. 
    * 
    * @param text         String to scrub
    * 
    * @return std::string Returns scrubs strings
    */
   std::string DawnVirCamera::scrub(const std::string &text) const {
     string ostr;
     for (unsigned int i = 0 ; i < text.size() ; i++) {
       if ((text[i] > ' ') &&  (text[i] <= 'z')) ostr.push_back(text[i]);
     }
     return (ostr);
   }

   /** Return the pixel summing rate */
   int DawnVirCamera::pixelSumming() const {
     return (m_summing);
   }

   /** Return the line scan rate */ 
   double DawnVirCamera::exposureTime() const {
     return (m_lineRate);
   }

   /** Return start time */
   double DawnVirCamera::startTime() const {
     return (m_mirrorData[0].m_scanLineEt);
   }


   /** Return end time after all lines are acquired */
   double DawnVirCamera::endTime() const {
    return (m_mirrorData[hkLineCount()-1].m_scanLineEt);
   }


   /** Returns number of housekeeping records found in the cube Table */
   int    DawnVirCamera::hkLineCount() const {
     return (m_mirrorData.size());
   }

   /**
   * @brief Read the VIR houskeeping table from cube
   *
   * This method reads an ISIS Table object from the cube.  This table named,
   * "VIRHouseKeeping", contains four fields: ScetTimeClock, ShutterStatus,
   * MirrorSin, and MirrorCos.  These fields contain the scan line time in
   * SCLK, status of shutter - open, closed (dark), sine and cosine of the
   * scan mirror, respectively.
   *
   * @history 2011-07-22 Kris Becker
   */
   void DawnVirCamera::readHouseKeeping(const std::string &filename, 
                                       double lineRate) { 
    //  Open the ISIS table object
    Table hktable("VIRHouseKeeping", filename);

    m_lineRates.clear();
    int lineno(1);
    for (int i = 0; i < hktable.Records(); i++) {
      TableRecord &trec = hktable[i];
      string scet = scrub(trec["ScetTimeClock"]);
      string shutterMode = scrub(trec["ShutterStatus"]);


      double mirrorSin = trec["MirrorSin"];
      double mirrorCos = trec["MirrorCos"];
      double scanElecDeg = atan(mirrorSin/mirrorCos) * dpr_c();
      double optAng = ((scanElecDeg - 3.7996979) * 0.25/0.257812);
      optAng /= 1000.0;  // Convert mrads to radians

      ScanMirrorInfo smInfo;
      double lineStart;
     //scs2e_c(NaifSpkCode(), scet.c_str(), &lineStart);
      lineStart = getClockTime(scet, NaifSpkCode()).Et();
      bool isDark = iString::Equal("closed", shutterMode);

      smInfo.m_lineNum = lineno;
      smInfo.m_scanLineEt = lineStart;
      smInfo.m_mirrorSin = mirrorSin;
      smInfo.m_mirrorCos = mirrorCos;
      smInfo.m_opticalAngle = optAng;
      smInfo.m_isDarkCurrent = isDark;

      if ((!m_is1BCalibrated) || (!(m_is1BCalibrated && isDark))) {
        m_lineRates.push_back(LineRateChange(lineno, lineStart, lineRate));
        m_mirrorData.push_back(smInfo);
        lineno++;
      }
    }

    if ((int) m_lineRates.size() != Lines()) {
      ostringstream mess;
      mess << "Number housekeeping lines determined (" << m_lineRates.size() 
           << ") is not equal to image lines(" << Lines() << ")";
      throw iException::Message(iException::Programmer, mess.str(), _FILEINFO_);
    }
  }

   /**
   * @brief Compute the pointing table for each line
   *
   * From the VIR housekeeping data, compute the pointing table for each line
   *  in the image.  This table is for InstrumentRotation(Table &) to establish
   *  line/sample pointing information.
   *
   * @history 2011-07-22 Kris Becker
   */
  Table DawnVirCamera::getPointingTable(const std::string &virChannel,
                                        const int zeroFrame)  {

    // Create Spice Pointing table
    TableField q0("J2000Q0", TableField::Double);
    TableField q1("J2000Q1", TableField::Double);
    TableField q2("J2000Q2", TableField::Double);
    TableField q3("J2000Q3", TableField::Double);
    TableField av1("AV1", TableField::Double);
    TableField av2("AV2", TableField::Double);
    TableField av3("AV3", TableField::Double);
    TableField t("ET", TableField::Double);

    TableRecord record;
    record += q0;
    record += q1;
    record += q2;
    record += q3;
    record += av1;
    record += av2;
    record += av3;
    record += t;
  
    // Get pointing table
    Table quats("SpiceRotation", record);
    int nfields = record.Fields();

    string virId = "DAWN_VIR_" + virChannel;
    string virZero = virId + "_ZERO";

    // Allocate output arrays
    int nvals = nfields - 1;
    int nlines = m_lineRates.size();

    SpiceDouble eulang[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    SpiceDouble xform[6][6], xform2[6][6];
    SpiceDouble m[3][3];
    SpiceDouble q_av[7], *av(&q_av[4]);

    for (int i = 0 ; i < nlines ; i++) {
      double etTime = m_mirrorData[i].m_scanLineEt;
      double optAng = m_mirrorData[i].m_opticalAngle;
      try {
        // J2000 -> DAWN_VIR_{channel}_ZERO
        SMatrix state = getStateRotation("J2000", virZero, etTime);

        // Set rotation of optical scan mirror (in radians)
        eulang[1] = optAng;
        eul2xf_c(eulang, 1, 2, 3, xform);
        mxmg_c(xform, &state[0][0], 6, 6, 6, xform2);
      
        // Transform to output format
        xf2rav_c(xform2, m, av);  // Transfers AV to output q_av via pointer
        m2q_c(m, q_av);          // Transfers quaternion

        //  Now populate the table record with the line pointing
        for (int k = 0 ; k < nvals ; k++) {
          record[k] = q_av[k];
        }

        // Add time to record; record to table
        record[nvals] = etTime;
        quats += record;
      }
      catch (iException &ie) {
        ostringstream mess;
        mess << "Failed to get point state for line " << i+1;
        ie.Message(iException::User, mess.str(), _FILEINFO_);
        throw;
      }
    }

    // Add some necessary keywords
    quats.Label() += PvlKeyword("CkTableStartTime", startTime()-(exposureTime()/2.0));
    quats.Label() += PvlKeyword("CkTableEndTime", endTime()+(exposureTime()/2.0));
    quats.Label() += PvlKeyword("CkTableOriginalSize", quats.Records());

    // Create the time dependant frames keyword
    int virZeroId = GetInteger("FRAME_" + virZero);
    PvlKeyword tdf("TimeDependentFrames", virZeroId); // DAWN_VIR_{ID}_ZERO 
    tdf.AddValue(-203200);  // DAWN_VIR
    tdf.AddValue(-203000);  // DAWN_SPACECRAFT
    tdf.AddValue(1);        // J2000
    quats.Label() += tdf;

    //  Create constant rotation frames
    PvlKeyword cf("ConstantFrames", virZeroId);
    cf.AddValue(virZeroId);
    quats.Label() += cf;

    SpiceDouble identity[3][3];
    ident_c(identity);

    //  Store DAWN_VIR_{ID}_ZERO -> DAWN_VIR_{ID}_ZERO identity rotation
    PvlKeyword crot("ConstantRotation");
    for (int i = 0 ; i < 3 ; i++) {
      for (int j = 0 ; j < 3 ; j++) {
        crot.AddValue(identity[i][j]);
      }
    }

    quats.Label() += crot;

    return (quats);
  }

   /**
   * @brief Compute the state rotation at a given time for given frames
   *
   *  Compute a 6x6 rotation state matrix between the two frames at the
   *  specified time.
   *
   *
   * @history 2011-07-22 Kris Becker
   */

  DawnVirCamera::SMatrix DawnVirCamera::getStateRotation(const std::string &frame1,
                                                         const std::string &frame2,
                                                         const double &etTime) 
                                                         const { 
    SMatrix state(6,6);
    NaifStatus::CheckErrors();
    try {
      // Get pointing w/AVs
      sxform_c(frame1.c_str(), frame2.c_str(), etTime, 
               (SpiceDouble (*)[6]) state[0]);
      NaifStatus::CheckErrors();
    } catch ( iException &ie ) {
      ie.Clear();
      try {
        SMatrix rot(3,3);
        pxform_c(frame1.c_str(), frame2.c_str(), etTime, 
                 (SpiceDouble (*)[3]) rot[0]);
        NaifStatus::CheckErrors();
        SpiceDouble av[3] = {0.0, 0.0, 0.0 };
        rav2xf_c((SpiceDouble (*)[3]) rot[0], av, 
                 (SpiceDouble (*)[6]) state[0]);
      } catch ( iException &ie2 ) {
        ostringstream mess;
        mess << "Could not get state rotation for Frame1 (" << frame1 
             << ") to Frame2 (" <<  frame2 <<  ") at time " << etTime;
        ie2.Message(iException::User, mess.str(), _FILEINFO_);
        throw;
      }
    }
    return (state);
  }

  /** 
  * @brief determine if the CK articulation kernels are present/given
  *  
  *  This method will determine if the CK articulation kernels are present in
  *  the labels.  If a kernel with the file pattern "dawn_vir_?????????_?.bc"
  *  is present as a CK kernel, then that kernel contains mirror scan angles
  *  for each line.  

  *  If the kernel does not exist, this camera model will provide these angles
  *  from the VIR housekeeping data.
  *
  * @history 2011-07-22 Kris Becke
  *
  */
  bool DawnVirCamera::hasArticulationKernel(Pvl &label) const {
    Kernels kerns(label);
    std::vector<std::string> cks = kerns.getKernelList("CK");
    QRegExp virCk("*dawn_vir_?????????_?.bc");
    virCk.setPatternSyntax(QRegExp::Wildcard);
    for (unsigned int i = 0 ; i < cks.size() ; i++) {
      if ( virCk.exactMatch(iString::ToQt(cks[i])) ) return (true);
    }
    return (false);
  }

}

/** Instantiate a new DawnVirCamera model for the given label content */
extern "C" Isis::Camera *DawnVirCameraPlugin(Isis::Pvl &lab) {
  return new Isis::DawnVirCamera(lab);
}

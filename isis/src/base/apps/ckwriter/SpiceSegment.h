#if !defined(SpiceSegment_h)
#define SpiceSegment_h
/**                                                                       
 * @file                                                                  
 * $Revision$ 
 * $Date$
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
 *  
 *   $Id$
 */                                                                       

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "naif/SpiceZdf.h"
#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array1d_utils.h"
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array2d_utils.h"

#include "Kernels.h"
#include "iString.h"
#include "IException.h"


namespace Isis {

class Cube;
class Camera;
class Table;
class PvlObject;

/**
 * @brief Maintain a SPICE kernel segment for conversions and export
 * 
 * This class is designed to read SPICE data from ISIS cube blobs and convert
 * them to proper formats for export to NAIF formatted SPICE kernel files.  This
 * particular implementation supports NAIF CK kernel types 1, 2 and 3.
 *  
 * @author 2010-11-10 Kris Becker 
 * @internal 
 * @history 2010-12-09 Kris Becker Added more documentation
 * @history 2011-05-04 Kris Becker Added pad time. 
 * @history 2011-05-29 Debbie A. Cook Changed name of local Kernels class to
 *                                    LocalKernels to avoid confusion
 * @history 2011-06-15 Kris Becker Removed use of LocalKernels and use version 
 *                                  in system; added loading of IAK kernel
 *                                  specifically for Cassini support; removed
 *                                  conditionalized obsolete code.
 * @history 2011-09-19 Debbie A. Cook Added section of code to ignore
 *                                  Instrument group and use OriginalInstrument
 *                                  group in the case of the IdealCamera.  The
 *                                  output kernel will be based on the original
 *                                  instrument code since the IdealCamera has
 *                                  no code.  This is safe to do since the
 *                                  camera is not being used to manipulate
 *                                  pixels. 
 * 
 */
class SpiceSegment {
  public:
    typedef TNT::Array1D<SpiceDouble> SVector;       //!<  1-D Buffer
    typedef TNT::Array2D<SpiceDouble> SMatrix;       //!<  2-D buffer

    SpiceSegment();
    SpiceSegment(const std::string &fname);
    SpiceSegment(Cube &cube, const std::string &tblname = "SpiceSegment");
    virtual ~SpiceSegment() { }
    
    /** Returns the number of elements in the quaternions */
    int size() const { return (size(_quats)); }

    /** Returns the name of the segment, typically the ProductId */
    std::string Id() const { return (_name); }
    void setId(const std::string &id);

    /** Start time of segment in ET */
    double startTime() const { return (_startTime); }
    /** End time of segment in ET */
    double endTime() const { return (_endTime); }

    bool operator<(const SpiceSegment &segment) const {
      return (startTime() < segment.startTime());
    }

    // Elements for writing NAIF SPICE kernels
    int FurnshKernelType(const std::string &ktypes) const;
    int UnloadKernelType(const std::string &ktypes = "") const;

    /** Returns CK segment reference frame */
    std::string ReferenceFrame() const { return (_refFrame); }
    /** NAIF SPICE instrument code */
    int InstCode() const { return (_instCode); }
    /** Returns instance of quaternions */
    const SMatrix &Quaternions() const { return (_quats); }
    /** Determines if angular velocites exist */
    bool hasAngularVelocities() const { return ((size(_avvs) > 0)); }
    /** Returns instance of angular velocity vectors */
    const SMatrix &AngularVelocities() const { return (_avvs); }

    /** Returns times in units of SCLK */
    const SVector &SCLKTimes() const { return (_times); }

    /** Returns a comment summarizing the segment */
    std::string getComment() const;

    // Mostly type 2 needs but can be used for type 3 (start times anyway)
    virtual SVector SCLKStartIntervals() const;
    virtual SVector SCLKStopIntervals() const;
    virtual SVector TickRate() const;

  private:
    typedef TNT::Array1D<SMatrix>     SMatSeq;       //!<  Time ordered matrices

    // Mutable for full loading/unloading of kernels w/o restrictions
    mutable Kernels  _kernels;
    int         _camVersion;
    std::string _name;
    std::string _fname;
    double      _startTime;
    double      _endTime;
    std::string _utcStartTime; //  Need to store these as conversion from ET
    std::string _utcEndTime;   //  requires leap seconds kernel
    std::string _instId;       //  Instrument ID
    std::string _target;       //  Target name
    int         _instCode;     //  NAIF instrument code of the SPICE segment
    std::string _instFrame;    //  NAIF instrument frame
    std::string _refFrame;     //  NAIF reference frame
    SMatrix     _quats;
    SMatrix     _avvs;
    SVector     _times;
    double      _tickRate;     // Number of seconds per tick for s/c


    // Internal processing methods
    void init();
    template <class TNTSTORE> int size(const TNTSTORE &t) const { return (t.dim1()); }

    std::string getKeyValue(PvlObject &label, const std::string &keyword);
    void import(Cube &cube, const std::string &tblname = "SpiceSegment");
    SMatrix load(Table &cache);
    SMatrix getQuaternions(const SMatrix &spice) const;
    SMatrix getAngularVelocities(const SMatrix &spice) const;
    SVector getTimes(const SMatrix &spice) const;

    bool getTimeDependentFrameIds(Table &table, int &toId, int &fromId) const;
    std::string getFrameName(int frameid) const;
    SMatrix getConstantRotation(Table &table) const;
    SMatrix getIdentityRotation(const int &nelements = 3) const;
    SMatrix computeStateRotation(const std::string &frame1, 
                                 const std::string &frame2, 
                                 double etTime) const;
    void getRotationMatrices(Cube &cube, Camera &camera, Table &table,
                             SMatSeq &lmats, SMatSeq &rmat, SVector &sclks);
    SVector convertTimes(int scCode, const SVector &etTimes);
    void convert(const SMatrix &quats, const SMatrix &avvs,
                 const SMatSeq &lmats, const SMatSeq &rmats, 
                 SMatrix &ckQuats, SMatrix &ckAvvs) const;
    const SMatrix &getMatrix(const SMatSeq &seq, const int &nth) const;

    SMatrix expand(int ntop, int nbot, const SMatrix &matrix) const;
    SVector expand(int ntop, int nbot, const SVector &vec) const;

    double SCLKtoET(SpiceInt scCode, double sclk) const;
    double ETtoSCLK(SpiceInt scCode, double et) const;

    std::string toUTC(const double &et) const;

};

};     // namespace Isis
#endif


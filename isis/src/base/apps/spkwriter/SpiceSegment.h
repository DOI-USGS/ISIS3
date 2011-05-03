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
#include "Cube.h"
#include "iException.h"


namespace Isis {

/**
 * @brief Container for SPICE kernel segment used in conversions and export
 * 
 * This class is designed to contain SPICE data from ISIS cube blobs in proper 
 * formats for export to NAIF formatted SPICE kernel files.  It is intended to 
 * be used as a base class that can be augmented to specific implementations of 
 * CK and SPK type kernels. 
 *  
 * @author 2010-11-10 Kris Becker 
 * @internal 
 * @history 2010-12-09 Kris Becker Added more documentation
 * 
 */
class SpiceSegment {
  public:
    typedef TNT::Array1D<SpiceDouble> SVector;       //!<  1-D Buffer
    typedef TNT::Array2D<SpiceDouble> SMatrix;       //!<  2-D buffer
    
    SpiceSegment();
    SpiceSegment(Cube &cube);
    virtual ~SpiceSegment() { }

    /** Returns size of elements in segment */
    virtual int size() const = 0;

    /** Return name of cube file associated with segment */
    std::string Source() const { return (_fname);     }

    /** Returns the name of the segment, typically the ProductId */
    std::string Id() const { return (_name); }
    void setId(const std::string &id);

    /** Return name of instrument */
    std::string Instrument() const { return (_instId); }
    /** Return name of target */
    std::string Target() const { return (_target); }

    /** Start time of segment in ET */
    double startTime() const { return (_startTime); }
    /** End time of segment in ET */
    double endTime() const { return (_endTime); }

    std::string utcStartTime() const { return (_utcStartTime);  }
    std::string utcEndTime() const { return (_utcEndTime);  }

    inline bool operator<(const SpiceSegment &segment) const {
      return (startTime() < segment.startTime());
    }

    virtual bool HasVelocityVectors() const { return (false);  }

    // Elements for writing NAIF SPICE kernels
    int LoadKernelType(const std::string &ktypes) const;
    int UnloadKernelType(const std::string &ktypes = "") const;
    int CameraVersion() const { return (_kernels.CameraVersion()); }

    /** Returns a comment summarizing the segment */
    virtual std::string getComment() const = 0;

  protected:
    void init(Cube &cube); 
    std::string getKeyValue(PvlObject &label, const std::string &keyword);
    const Kernels &getKernels() const { return (_kernels); }
    bool getImageTimes(Pvl &lab, double &start, double &end) const;
    SMatrix expand(int ntop, int nbot, const SMatrix &matrix) const;
    SVector expand(int ntop, int nbot, const SVector &vec) const;

    void setStartTime(double et);
    void setEndTime(double et);
     
    std::string getNaifName(int naifid) const;
    std::string toUTC(const double &et) const;
    double UTCtoET(const std::string &utc) const;

  private:
    std::string _name;
    std::string _fname;
    std::string _instId;
    std::string _target;
    
    double      _startTime;
    double      _endTime;
    std::string _utcStartTime; //  Need to store these as conversion from ET
    std::string _utcEndTime;   //  requires leap seconds kernel 

    mutable Kernels _kernels;  // Kernel manager
    void init();
    
};

};     // namespace Isis
#endif


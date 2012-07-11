#if !defined(SpkSegment_h)
#define SpkSegment_h
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

#include "SpiceSegment.h"
#include "iString.h"
#include "IException.h"


namespace Isis {

class Cube;
class Camera;
class Table;
class PvlObject;

/**
 * @brief Maintain a SPK SPICE kernel segment for conversions and export
 * 
 * This class is designed to read SPICE data from ISIS cube blobs and convert
 * them to proper formats for export to NAIF formatted SPK SPICE kernel files. 
 * This particular implementation supports NAIF SPK kernel types 9 and 13. 
 *  
 * @author 2011-02-12 Kris Becker 
 * @internal 
 *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
 *                          coding standards. References #972.
 * 
 */
class SpkSegment : public SpiceSegment {
  public:
    typedef SpiceSegment::SVector SVector;
    typedef SpiceSegment::SMatrix SMatrix;

    SpkSegment();
    SpkSegment(const std::string &fname);
    SpkSegment(Cube &cube);
    virtual ~SpkSegment() { }

    void import(Cube &cube);
    
    /** Returns the number of elements in the vectors */
    int size() const { return (size(_states)); }

    /** Returns SPK segment reference frame */
    int BodyCode() const { return (_body); }
    /** NAIF SPICE instrument code */
    int CenterCode() const { return (_center); }
    /** Returns CK segment reference frame */
    std::string ReferenceFrame() const { return (_refFrame); }
    /** Get times of each entry */
    SVector Epochs() const { return (_epochs); }
    /** Returns instance of quaternions */
    const SMatrix &States() const { return (_states); }

    int Degree() const { return (_degree);   }

    bool HasVelocityVectors() const { return (_hasVV); }

    /** Returns a comment summarizing the segment */
    std::string getComment() const;

  private:
    enum { MinimumStates = 3, MaximumDegree = 7};  // Sensible? NAIF extremes

    int         _body;         //  NAIF body code of the SPICE segment
    std::string _bodyFrame;    //  NAIF body frame
    int         _center;       //  NAIF center code of the SPICE segment
    std::string _centerFrame;  //  NAIF center frame
    std::string _refFrame;     //  NAIF reference frame
    SMatrix     _states;       //  Position states
    SVector     _epochs;       //  ET times of records
    bool        _hasVV;        //  Has velocity vectors?
    int         _degree;       //  Degree of polynomial to fit in NAIF kernel

    // Internal processing methods
    void init();
    template <class TNTSTORE> int size(const TNTSTORE &t) const { return (t.dim1()); }

    SMatrix load(Table &cache);
    void getStates(Camera &camera, const SMatrix &spice, SMatrix &states, 
                   SVector &epochs, bool &hasVV) const; 
    SVector makeState(SpicePosition *position, const double &time0,
                      const SVector &stateT, const double &timeT) const;
    SVector adjustTimes(Camera &camera, const SVector &epochs) const;

};

};     // namespace Isis
#endif


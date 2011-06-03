/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
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
#include <numeric>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>


#include "SpkSegment.h"
#include "iString.h"
#include "Filename.h"
#include "Cube.h"
#include "Camera.h"
#include "Table.h"
#include "NaifStatus.h"
#include "iException.h"

#include "naif/SpiceUsr.h"
#define PURE_METHODS_PRESENT 1

using namespace std;

namespace Isis {

/** Default constructor */
SpkSegment::SpkSegment() : SpiceSegment() { 
  init(); 
}

/** Constructor from ISIS cube file by name of the cube */
SpkSegment::SpkSegment(const std::string &fname) : SpiceSegment() {
  init();
  Cube cube;
  cube.Open(fname);
  SpiceSegment::init(cube);
  import(cube);
}

/** Constructor from ISIS cube object */
SpkSegment::SpkSegment(Cube &cube) : SpiceSegment(cube) {
  init();
  import(cube);
}

/**
 * @brief Load and process SPICE data from an ISIS cube object 
 *  
 * This method extracts SPK SPICE date from an ISIS cube object.  This object 
 * must have been spiceinit'ed at a minimum and, by definition, have a 
 * supporting camera model. 
 *  
 * SPK data is extracted from the SpicePosition object via the Table it provides 
 * of this information.  The SPICE position state vectors are potentially 
 * trasformed to the proper state for target body, center body and reference 
 * frame. 
 * 
 * @author kbecker (5/2/2011)
 * 
 * @param cube  Cube to generate SPICE segment from 
 *
 * @internal
 *   @history 2011-05-27 Debbie A. Cook - Added call to create Hermite cache
 *                           to make this option available for testing. 
 *                           This will need to be an option in the future.
 */
void SpkSegment::import(Cube &cube) {
  typedef std::vector<std::string>  StrList;

  //  Extract ISIS SPK blob and transform to CK 3 content
  NaifStatus::CheckErrors();
  try {

    Camera *camera(cube.Camera());
    Kernels kernels = getKernels();

    // Load necessary kernels and id frames
    kernels.Load("PCK,LSK,FK,SPK,EXTRA");
#if defined(PURE_METHODS_PRESENT)
    _body = camera->SpkTargetId();
    _center = camera->SpkCenterId();
    _refFrame = getNaifName(camera->SpkReferenceId());
#else
    // This probably covers 95% of the missions - really needs the pure methods
    _body = camera->NaifSpkCode();
    _center = camera->NaifBodyCode();
    _refFrame = getNaifName(1);
#endif
    _bodyFrame = getNaifName(_body);
    _centerFrame = getNaifName(_center);

    //  Get the SPICE data
//       spkCache = camera->InstrumentPosition()->LineCache("SpkSegment");
      Table spkCache = camera->InstrumentPosition()->LoadHermiteCache("SpkSegment");
    getStates(*camera, load(spkCache), _states, _epochs, _hasVV);

      // Save current time
    SpicePosition *ipos(camera->InstrumentPosition());
    double currentTime = ipos->EphemerisTime();



#if 0
    ////  This is currently determined to not be required and needs to bei
    ////   turned off.  (2011-05-17 KJB)

    //  Adjust times for aberration and light time corrections.  Note this
    //  is a kludge only needed until we correct this on read by spiceinit
    _epochs = adjustTimes(*camera, _epochs);
#endif

    // Add records with 3 milliseconds padding to top and bottom of records
//     const double Epsilon(3.0e-3);
    const double Epsilon(3.0e-3);

    // Pad the top and bottom of the 
    _states = expand(1, 1, _states);
    _epochs = expand(1, 1, _epochs);

    int ndim1(_states.dim1());
    int ndim2(_states.dim2());

    // Add record to top of states
    SVector stateT(ndim2, _states[1]);  // Map to to first record
    double sTime = _epochs[1] - Epsilon;
    SVector s0 = makeState(ipos, _epochs[1], stateT, sTime);
    SVector(ndim2, _states[0]).inject(s0);
    _epochs[0] = sTime; 

    // Add record to bottom of states
    stateT = SVector(ndim2, _states[ndim1-2]);
    sTime = _epochs[ndim1-2] + Epsilon;
    s0 = makeState(ipos, _epochs[ndim1-2], stateT, sTime);
    SVector(ndim2, _states[ndim1-1]).inject(s0);
    _epochs[ndim1-1] = sTime;

    //  Restore saved time and determine degree of NAIF interpolation
    ipos->SetEphemerisTime(currentTime);
    _degree = std::min((int) MaximumDegree, _states.dim1()-1);
    //  Ensure the degree is odd and less that the even value
    _degree = (((_degree - 1) / 2) * 2) + 1;


#if 0
    // Prints state vectors
    for ( int i = 0 ; i < size() ; i++ ) {
      cout << i << ":";
      for ( int j = 0 ; j < _states.dim2() ; j++ ) {
        cout << " " << setw(26) << setprecision(13) << _states[i][j];
      }
      cout << " " << setw(26) << setprecision(13) << _epochs[i] << "\n";
    }
#endif
    setStartTime(_epochs[0]);
    setEndTime(_epochs[size(_epochs)-1]);

  } catch ( iException &ie  ) {
    ostringstream mess;
    mess << "Failed to construct SPK content from ISIS file " << Source();
    ie.Message(iException::User, mess.str(), _FILEINFO_);
    throw;
  }

  return;
}

/**
 * @brief Convert J2000 positions to frame relative to center body 
 *  
 * This method converts the data from SpicePostion to state vectors relative to 
 * the center of motion of the object identified by body.  The return results 
 * will be ready to write to (at least) SPK kernels of type 9 and 13. 
 *  
 * The "states" parameters will be a matrix of the form states[nrecs][6], where 
 * "nrecs" is the number of states in the table, and times[nrecs] corresponds 
 * to TDB time for each nrecs record. 
 *  
 * PreRequisites: 
 *   The internal _body, _center and _refFrame are required to be defined prior
 *     to calling this routine. 
 *   The FK kernel is likely to be required to be loaded in the NAIF kernel
 *     pool so that frame translations can occur.  The caller is burdened with
 *     ensuring the kernel is loaded.
 *  
 * @author Kris Becker - 2/21/2011
 * 
 * @param camera Camera object for the data
 * @param spice  Data from the SpicePosition::LineCache method
 * @param states Matrix containing "nrecs" states vectors
 * @param times  TBD epoch times for each record
 */
void SpkSegment::getStates(Camera &camera, const SMatrix &spice,
                           SMatrix &states, SVector &epochs, bool &hasVV) 
                           const { 
  int nrecs = size(spice);
  int nelems = spice.dim2();
  states = SMatrix(nrecs, 6, 0.0);  // Initialize to 0 should no deltas exist
  epochs = SVector(nrecs);
  hasVV = (nelems == 7);

  // cout << "Number Records: " << nrecs << "\n";

  // Extract contents
  for ( int i = 0 ; i < nrecs ; i++ ) {
 //   cout  << "Rec: " << i;
    for ( int j = 0 ; j < (nelems-1) ; j++ ) {
      states[i][j] = spice[i][j];
 //     cout << " " << setw(26) << setprecision(13) << _states[i][j];         
    }
    epochs[i] = spice[i][nelems-1];
 //   cout << " " << setw(26) << setprecision(13) << _epochs[i] << "\n";
  }

  //  Compute state rotations relative to the reference frame
  string j2000 = getNaifName(1);  // ISIS stores in J2000
  if (j2000 != _refFrame) {
    for (int n = 0 ; n < nrecs ; n++) {
      SpiceDouble xform[6][6];
      sxform_c(j2000.c_str(), _refFrame.c_str(), epochs[n], xform);
      mxvg_c(xform, states[n], 6, 6, states[n]);
    }
  }

  return;
}

/**
 * @brief Make a new state vector from the current state and time 
 *  
 * This method creates a new state from the given state0 using the position 
 * object and current time, time0, at that position.  timeT is the new time of 
 * the desired state.   
 * 
 * @author kbecker (5/2/2011)
 * 
 * @param position  Position object for the state 
 * @param time0     Current time for give state
 * @param state0    Current state vector
 * @param timeT     Desired time of the new state
 * 
 * @return SpkSegment::SVector New state at timeT
 * @internal
 * @history 2011-06-03 Debbie A. Cook Put extrapolation code back
 *                                    in use since it gives the best results.
 */
SpkSegment::SVector SpkSegment::makeState(SpicePosition *position, const double &time0,
                                          const SVector &state0, const double &timeT) const {


  SVector stateT = state0.copy();
  // The code below seems to be working well for fixing the ends so I am putting it back in DAC
#if 1
  position->SetEphemerisTime(time0);
  std::vector<double> tstate = position->Extrapolate(timeT);
  int nElems = std::min((int) tstate.size(), state0.dim1());
  for ( int i = 0 ; i < nElems ; i++ ) {
    stateT[i] = tstate[i];
  }
#endif
//   for ( int i = 3 ; i < stateT.dim1() ; i++ ) {
//     stateT[i] = 0.0;  
//   }

  return (stateT);
}

/**
 * @brief Construct a comment for the given segment
 * 
 * @author kbecker (5/2/2011)
 * 
 * @return std::string Comment string for the segment
 */
std::string SpkSegment::getComment() const {
  ostringstream comment;

  comment << 
"\n-----------------------------------------------------------------------\n" <<
"  File:        " << Filename(Source()).Name() << endl <<
"  Segment ID:  " << Id() << " (ProductId)" << endl <<
"  StartTime:   " << utcStartTime() << endl <<
"  EndTime:     " << utcEndTime() << endl <<
"  Target Body: " << "Body " << _body << ", " << _bodyFrame << endl <<
"  Center Body: " << "Body " << _center << ", " << _centerFrame << endl << 
"  RefFrame:    " << _refFrame << endl <<
"  Records:     " << size() << endl;

  string hasVV = (_hasVV) ? "YES" : "NO";
  comment <<
"  HasVV:       " << hasVV << endl;

  comment <<                       
"  PolyDegree:  " << _degree << endl <<
"  CamVersion:  " << CameraVersion() << endl;
  std::vector<std::string> klist = getKernels().getKernelList();
  if ( klist.size() > 0 ) {
    comment << 
"  Kernels:     \n";
    for ( unsigned int i = 0 ; i < klist.size() ; i++  ) {
      comment <<
"    " << klist[i] << endl;
    }
  }

  return (string(comment.str()));
}

/**
 * @brief Initialize object parameters
 * 
 * @author kbecker (5/2/2011)
 */
void SpkSegment::init() {
  _body = _center = 1;
  _bodyFrame = _centerFrame = _refFrame = "";
  _states = SMatrix(0,0);
  _epochs = SVector(0);
  _hasVV = false;
  _degree = 1;
  return;
}

/**
 * @brief Load the SPK segements from the ISIS table object 
 *  
 * This method extracts position vectors, velocity vectors (if they exist) and 
 * epochs (times) from an ISIS SpicePosition BLOB/table.  The table content 
 * (number of fields) determine if the vectors exist. 
 * 
 * @author kbecker (5/2/2011)
 * 
 * @param table   SpicePosition Table to extract SPK data
 * 
 * @return SpkSegment::SMatrix Returns SPK data 
 */
SpkSegment::SMatrix SpkSegment::load(Table &table) {

//  Allocate the internal cache and transfer.
  //  Makes some assumptions about the format of the SPICE table in that
  //  all fields are double.
  int nrecs = table.Records();
  TableRecord &rec = table[0];
  int nvals = rec.Fields();

  // Ensure the table has the expected format, error out if not valid.
//   if ( !((nvals == 7) || (nvals == 4)) ) {
//     ostringstream mess;
//     mess << "SPICE (SPK) Table " << table.Name() 
//          << " must have 7 (with velocity vectors) or 4 fields but has " 
//          << nvals;
//     throw iException::Message(iException::User, mess.str(), _FILEINFO_);
//   }

  // Extract contents
  SMatrix spice = SMatrix(nrecs,nvals);
  for ( int i = 0 ; i < nrecs ; i++ ) {
    TableRecord &rec = table[i];
    for ( int f = 0 ; f < rec.Fields() ; f++ ) {
      TableField &field = rec[f];
      spice[i][f] = (double) field;
    }
  }
  return (spice);
}

/**
 * @brief Adjust times for stellar aberration and light time 
 *  
 * This method adjusts the times for stellar aberration and light time 
 * correction if needed.  This is only a temporary fix until it is corrected in 
 * the SpicePosition class. 
 *  
 * Prerequisites:  Upon calling this method, the appropriate SPK must be loaded. 
 * The reference frame (_refFrame), typically "J2000", must also be defined. 
 *  
 * Currently, several missions do not apply these corrections.  These are 
 * LunarOrbiter and Mariner10. 
 *  
 * @author kbecker (5/2/2011)
 * 
 * @param epochs Times to correct for aberration and light time
 * 
 * @return SpkSegment::SVector Corrected/adjust times
 */
SpkSegment::SVector SpkSegment::adjustTimes(Camera &camera, 
                                            const SVector &epochs) const { 

  SpiceInt observer = camera.NaifSpkCode();

  // Don't adjust the following spacecraft:
  SpiceInt LunarOrbiter(-533), Mariner10(-76);
  if (observer == LunarOrbiter) return (epochs);
  if (observer == Mariner10)    return (epochs);

  SpiceInt target   = camera.NaifBodyCode();
  SVector ltEpochs(epochs.dim1());
  for ( int i = 0 ; i < epochs.dim1() ; i++ ) {
    SpiceDouble lt;
    SpiceDouble state[6];
    spkez_c(observer, epochs[i], _refFrame.c_str(), "LT+S", target, state, &lt);
    ltEpochs[i] = epochs[i] - lt;
  }
  return (ltEpochs);
}
};  // namespace Isis


/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2009/12/29 23:03:52 $                                                                 
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

#include <string>
#include <sstream>
#include <iostream>

#include "naif/SpiceUsr.h"

#include "HiJitCube.h"
#include "Instrument.hh"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Filename.h"

using namespace UA::HiRISE;
using std::string;
using std::endl;
using std::ostringstream;


namespace Isis {


static  geos::geom::GeometryFactory geosFactory;
bool HiJitCube::naifLoaded = false;
int npSamp0[] = {0,1971,3964,5963,7970,7971,7971,9975,9976,9976,11981,13986,15984,17982};
int npSamps[] = {2021,2043,2048,2052,2055,2053,2053,2053,2054,2055,2051,2049,2043,2018};
bool sampinit=false;
bool originst;

/**
 *  @brief Default constructor with no cube
 */
HiJitCube::HiJitCube() {
  initLocal();
}

/**
 *  @brief Constructor with file to open
 */
HiJitCube::HiJitCube(const std::string &filename) {
  initLocal();
  OpenCube(filename);
}

/**
 *  @brief Constructor with file to open and potential shift applied
 */
HiJitCube::HiJitCube(const std::string &filename, PvlObject &shift) {
  initLocal();
  OpenCube(filename, shift);
}


/**
 *  @brief Destructor
 */
HiJitCube::~HiJitCube() {
  delete fpGeom;
  Close();
}

void HiJitCube::setSampleOffset(int soff) {
  jdata.sampOffset = soff;
  if (IsOpen()) computePoly();
  return;
}

void HiJitCube::setLineOffset(int loff) {
  jdata.lineOffset = loff;
  if (IsOpen()) computePoly();
  return;
}


void HiJitCube::OpenCube(const std::string &filename) {
  Open(filename);
  Init();
  return;
}

void HiJitCube::OpenCube(const std::string &filename, PvlObject &shift) {
  OpenCube(filename);

//  Determine if a shift of the CCD exists in the definitions file
  if (shift.HasGroup(jdata.ccdName)) {
    PvlGroup &ccddef = shift.FindGroup(jdata.ccdName, Pvl::Traverse);
    if (ccddef.HasKeyword("SampleOffset")) {
      jdata.sampOffset = (int) ccddef["SampleOffset"];
    }
    if (ccddef.HasKeyword("LineOffset")) {
      jdata.lineOffset = (int) ccddef["LineOffset"];
    }
    computePoly();
  }

  return;
}

double HiJitCube::getLineTime(double line) const {
  return (((line-1.0) * jdata.linerate) + jdata.obsStartTime);
}

void HiJitCube::Compatable(HiJitCube &cube) throw (iException &) {
  JitInfo other = cube.GetInfo();
  
  if (jdata.summing != other.summing) {
    ostringstream msg;
    msg << "Summing mode (" << jdata.summing 
        << ") in file " << Filename() << " is not equal to summing mode (" 
        << other.summing << ") in file " << cube.Filename() << endl;
    throw iException::Message(iException::User,msg.str(),_FILEINFO_);
  }
  return;
}
       

bool HiJitCube::intersects(const HiJitCube &cube) const {
  return (fpGeom->intersects(cube.Poly()));
}

bool HiJitCube::overlap(const HiJitCube &cube, Corners &ovlCorners) {
  geos::geom::Geometry *ovl = fpGeom->intersection(cube.Poly());
//  cout << "Overlap: " << ovl->toString() << std::endl;
  ovlCorners = FocalPlaneToImage(getCorners(*ovl));
  delete ovl;
  return (ovlCorners.good);
}


void HiJitCube::loadNaifTiming( ) {
  if (!naifLoaded) {
//  Load the NAIF kernels to determine timing data
    Isis::Filename leapseconds("$base/kernels/lsk/naif????.tls");
    leapseconds.HighestVersion();

    Isis::Filename sclk("$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc");
    sclk.HighestVersion();

//  Load the kernels
    string lsk = leapseconds.Expanded();
    string sClock = sclk.Expanded();
    furnsh_c(lsk.c_str());
    furnsh_c(sClock.c_str());

//  Ensure it is loaded only once
    naifLoaded = true;
  }
  return;
}

void HiJitCube::computeStartTime() {
  loadNaifTiming();

//  Compute the unbinned and binned linerates in seconds
  jdata.unBinnedRate = ((Instrument::LINE_TIME_PRE_OFFSET + 
                   (jdata.dltCount/Instrument::HIRISE_CLOCK_SUBTICK_MICROS)) 
                   / 1000000.0);
  jdata.linerate = jdata.unBinnedRate * ((double) jdata.summing);

  if (!jdata.scStartTime.empty()) {
    string scStartTimeString = jdata.scStartTime;
    scs2e_c (-74999,scStartTimeString.c_str(),&jdata.obsStartTime);
    // Adjust the start time so that it is the effective time for
    // the first line in the image file
    jdata.obsStartTime -= (jdata.unBinnedRate * (((double(jdata.tdiMode/2.0)
                                                     - 0.5))));
    // Effective observation
    // time for all the TDI lines used for the
    // first line before doing binning
    jdata.obsStartTime += (jdata.unBinnedRate * (((double) jdata.summing/2.0)
                                                  - 0.5));
  }
  return;
}

void HiJitCube::initLocal() {
  jdata.filename = "_none_";
  jdata.productId = "__undetermined__";
  jdata.lines = 0;
  jdata.samples = 0;
  jdata.sampOffset = 0;
  jdata.lineOffset = 0;
  jdata.tdiMode = 0;
  jdata.summing = 0;
  jdata.channelNumber = 0;
  jdata.cpmmNumber = 0;
  jdata.ccdName ="_unknown_";
  jdata.dltCount = 0.0;
  jdata.UTCStartTime = "";
  jdata.scStartTime = "";
  jdata.obsStartTime = 0.0;
  jdata.unBinnedRate = 0.0;
  jdata.linerate = 0.0;
  jdata.fpSamp0 = 0;
  jdata.fpLine0 = 0;
  jdata.pixpitch = 0.0;

  fpGeom = 0;
}

void HiJitCube::Init() throw (iException &) {
   // Get required keywords from instrument group
  Pvl *label(Label());
  Isis::PvlGroup inst;
  Isis::PvlGroup idinst;
  jdata.filename = Filename();
  Isis::PvlGroup &archive = label->FindGroup ("Archive",Isis::Pvl::Traverse);
  jdata.productId = (string) archive["ProductId"];

  jdata.lines = Lines();
  if (label->FindObject("IsisCube").HasGroup("OriginalInstrument")) {
    inst = label->FindGroup ("OriginalInstrument",Isis::Pvl::Traverse);
    originst = true;
  } else {
    inst = label->FindGroup ("Instrument",Isis::Pvl::Traverse);
    originst = false;
  }
  jdata.tdiMode = inst["Tdi"];
  jdata.summing = inst["Summing"];
  if (label->FindObject("IsisCube").HasGroup("Instrument") && originst) {
    idinst = label->FindGroup ("Instrument",Isis::Pvl::Traverse);
    jdata.pixpitch = idinst["PixelPitch"];
    jdata.summing = (int) (jdata.pixpitch/.012);
  }
  if (originst && jdata.summing != 1 && !sampinit) {
    for (int i=0; i<14; i++) {
      npSamps[i] = (int) (npSamps[i]/(float)jdata.summing + 0.5);
      npSamp0[i] = (int) (npSamp0[i]/(float)jdata.summing + 0.5);
    }
    sampinit = true;
  }
  jdata.channelNumber = inst["ChannelNumber"];
  jdata.cpmmNumber = inst["CpmmNumber"];
  if (originst) {
    jdata.samples = npSamps[jdata.cpmmNumber];
  } else {
    jdata.samples = Samples();
  }
  jdata.ccdName = Instrument::CCD_NAMES[jdata.cpmmNumber];
  jdata.dltCount = inst["DeltaLineTimerCount"];
  jdata.UTCStartTime = (string) inst["StartTime"];
  jdata.scStartTime = (string) inst["SpacecraftClockStartCount"];

  try {
      if (originst) {
        jdata.fpSamp0 = npSamp0[jdata.cpmmNumber];
      } else {
        jdata.fpSamp0 = Instrument::focal_plane_x_offset(jdata.cpmmNumber,
	                                               jdata.summing);
      }
  } catch (Exception hiExc) {
    ostringstream msg;
    msg << "Summing mode (" << jdata.summing 
        << ") is illegal (must be > 0) or CPMM number (" << jdata.cpmmNumber
        << ") is invalid in file " << Filename() << endl;
    throw iException::Message(iException::User,msg.str(),_FILEINFO_);
  }

//  It is assumed all images start at the line location in the focal plane
  jdata.fpLine0 = 0;

//  Validate channel number and adjust starting sample
  if ((jdata.channelNumber > 2) || (jdata.channelNumber < 0)) {
    ostringstream msg;
    msg << "Channel number (" << jdata.channelNumber 
        << ") is invalid (must be 0, 1 or 2) in file " << Filename() << endl;
    throw iException::Message(iException::User,msg.str(),_FILEINFO_);
  }
  else {
    if (originst) {
      if (jdata.channelNumber == 0) jdata.fpSamp0 += npSamps[jdata.cpmmNumber];
    } else {
      if (jdata.channelNumber == 0) jdata.fpSamp0 += Samples();
    }
  }

// Determine starting time of image and compute the binning rates
  computeStartTime();

//  Compute the focal plane polygon for this image
  computePoly();
  return;
}


int HiJitCube::getBinModeIndex(int summing) const throw (iException &) {
  for (unsigned int i = 0 ; i < Instrument::TOTAL_BINNING_FACTORS ; i++) {
    int binFactor = Instrument::BINNING_FACTORS[i];
    if (binFactor == summing) return (i);
  }

  ostringstream msg;
  msg << "Invalid summing mode (" << summing << ") for file " <<
         Filename() << std::endl;
  throw iException::Message(iException::User,msg.str(),_FILEINFO_);
}


void HiJitCube::computePoly() {

//  Compute sample and line coordinates in the focal plane
  int samp0,sampN;
  if (originst) {
    samp0=jdata.fpSamp0 + jdata.sampOffset;
    sampN=samp0 + npSamps[jdata.cpmmNumber] - 1;
  } else {
    samp0=jdata.fpSamp0 + jdata.sampOffset;
    sampN=samp0 + Samples() - 1;
  }
  int line0(jdata.fpLine0 + jdata.lineOffset), lineN(line0 + Lines() - 1);

//  Allocate a new coordinate sequence and define it
  geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();
  pts->add(geos::geom::Coordinate(samp0, lineN));
  pts->add(geos::geom::Coordinate(sampN, lineN));
  pts->add(geos::geom::Coordinate(sampN, line0));
  pts->add(geos::geom::Coordinate(samp0, line0));
  pts->add(geos::geom::Coordinate(samp0, lineN));

  
//  Make this reentrant and delete previous one if it exists and get the
//  new one
  delete fpGeom;
  fpGeom = geosFactory.createPolygon(geosFactory.createLinearRing(pts), 0);
  return;
}

HiJitCube::Corners HiJitCube::FocalPlaneToImage(const Corners &fp) const {
  Corners image;
  if (fp.good) {
    double samp0;
//  Compute sample and line coordinates in the focal plane
    if (originst) {
      samp0 = 0;
    } else {
      samp0 = jdata.fpSamp0 + jdata.sampOffset;
    }
    double line0(jdata.fpLine0 + jdata.lineOffset);

    image.topLeft.sample = fp.topLeft.sample - samp0 + 1.0;
    image.topLeft.line   = fp.topLeft.line   - line0 + 1.0;
    image.lowerRight.sample = fp.lowerRight.sample - samp0 + 1.0;
    image.lowerRight.line   = fp.lowerRight.line   - line0 + 1.0;
    image.good = true;
  }
  return (image);
}

HiJitCube::Corners HiJitCube::getCorners(const geos::geom::Geometry &poly) const {
  Corners corners;
  if ((poly.isValid()) && (!poly.isEmpty())) {

//  Get the coordinate list
    geos::geom::CoordinateSequence *clist = poly.getCoordinates();
    const geos::geom::Coordinate *minpt = clist->minCoordinate();
//    cout << "MinPoint: " << minpt->x << ", " << minpt->y << std::endl;

    geos::geom::Coordinate maxpt(clist->getAt(0));
    for (unsigned int i = 1 ; i < clist->getSize() ; i++) {
      geos::geom::Coordinate pt = clist->getAt(i);
      if ((pt.x >= maxpt.x) && (pt.y >= maxpt.y)) maxpt = pt;
    }
//    cout << "MaxPoint: " << maxpt.x << ", " << maxpt.y << std::endl;

    corners.topLeft.sample    = minpt->x;
    corners.topLeft.line      = minpt->y;
    corners.lowerRight.sample = maxpt.x;
    corners.lowerRight.line   = maxpt.y;
    corners.good = true;

    delete clist;
  }
  return (corners);
}


}

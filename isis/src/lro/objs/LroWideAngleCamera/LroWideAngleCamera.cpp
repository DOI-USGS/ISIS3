/**
 * @file
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

#include "LroWideAngleCamera.h"
#include "LroWideAngleCameraDistortionMap.h"

#include <sstream>
#include <iomanip>

#include "CameraFocalPlaneMap.h"
#include "CameraSkyMap.h"
#include "CollectorMap.h"
#include "IException.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "PushFrameCameraDetectorMap.h"
#include "PushFrameCameraGroundMap.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the LRO WAC Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @throws Isis::iException::User - The image does not appear to be a Lunar
   *             Reconaissance Orbiter Wide Angle Camera image
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  LroWideAngleCamera::LroWideAngleCamera(Cube &cube) :
      PushFrameCamera(cube) {
    NaifStatus::CheckErrors();
    // Set up the camera characteristics
    instrumentRotation()->SetFrame(naifIkCode());
    SetFocalLength();
    SetPixelPitch();

    // Get the ephemeris time from the labels
    double et;
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockStartCount"];
    et = getClockTime(stime).Et();

    p_exposureDur = inst["ExposureDuration"];
    // TODO:  Changed et - exposure to et + exposure.
    //   Think about if this is correct
    p_etStart = et + ((p_exposureDur / 1000.0) / 2.0);

    // Compute the framelet size and number of framelets
    IString instId = QString((QString) inst["InstrumentId"]).toUpper();

    int frameletSize = 14;
    int sumMode = 1;

    if(instId == "WAC-UV") {
      sumMode = 4;
      frameletSize = 16;
    }
    else if(instId == "WAC-VIS") {
      sumMode = 1;
      frameletSize = 14;
    }
    else {
      string msg = "Invalid value [" + instId
                   + "] for keyword [InstrumentId]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_nframelets = ParentLines() / (frameletSize / sumMode);

    // Setup the line detector offset map for each filter
    int nbands = (int) lab.findKeyword("Bands", PvlObject::Traverse);
    const PvlGroup &bandBin = lab.findGroup("BandBin", Pvl::Traverse);
    const PvlKeyword &filtNames = bandBin["Center"];
    // Sanity check
    if(nbands != filtNames.size()) {
      ostringstream mess;
      mess << "Number bands in (file) label (" << nbands
           << ") do not match number of values in BandBin/Center keyword ("
           << filtNames.size() << ") - required for band-dependent geoemtry";
      throw IException(IException::User, mess.str(), _FILEINFO_);
    }

    // Is the data flipped?
    bool dataflipped = (inst["DataFlipped"][0].toUpper() == "YES");

    //  Now create detector offsets
    QString instCode = "INS" + toString(naifIkCode());
    QString ikernKey = instCode + "_FILTER_BANDCENTER";
    vector<int> fbc = GetVector(ikernKey);
    ikernKey = instCode + "_FILTER_OFFSET";
    vector<int> foffset = GetVector(ikernKey);


    // Create a map of filter wavelength to offset.  Also needs a reverse
    // lookup to order the offset into the CCD (ascending sort provided
    // automagically be CollectorMap).
    CollectorMap<int, int> filterToDetectorOffset, wavel;
    for(unsigned int i = 0 ; i < foffset.size() ; i++) {
      filterToDetectorOffset.add(fbc[i], foffset[i]);
      wavel.add(foffset[i], fbc[i]);
    }

    // Construct special format for framelet offsets into CCD.  Uses the above
    // reverse map.  Need only get the value (wavelength) of the map as the
    // key (offset) is sorted above.
    int frameletOffsetFactor = inst["ColorOffset"];
    if(dataflipped) frameletOffsetFactor *= -1;
    CollectorMap<int, int> filterToFrameletOffset;
    for(int j = 0 ; j < wavel.size() ; j++) {
      int wavelen = wavel.getNth(j);
      filterToFrameletOffset.add(wavelen, j * frameletOffsetFactor);
    }

    //  Now map the actual filter that exist in cube
    for(int i = 0; i < filtNames.size(); i++) {
      if(!filterToDetectorOffset.exists((IString)filtNames[i])) {
        QString msg = "Unrecognized filter name [" + filtNames[i] + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      p_detectorStartLines.push_back(filterToDetectorOffset.get((IString)filtNames[i]));
      p_frameletOffsets.push_back(filterToFrameletOffset.get((IString)filtNames[i]));
    }

    // Setup detector map
    double frameletRate = (double) inst["InterframeDelay"] / 1000.0;
    PushFrameCameraDetectorMap *dmap = new PushFrameCameraDetectorMap(this,
        p_etStart, frameletRate, frameletSize);
    dmap->SetDetectorSampleSumming(sumMode);
    dmap->SetDetectorLineSumming(sumMode);

    // flipping disabled if already flipped
    bool flippedFramelets = dataflipped;
    dmap->SetFlippedFramelets(flippedFramelets, p_nframelets);

    // Setup focal plane map
    new CameraFocalPlaneMap(this, naifIkCode());

    // The line detector origin varies based on instrument mode
    double detectorOriginLine;
    double detectorOriginSamp;

    dmap->SetGeometricallyFlippedFramelets(false);

    ikernKey = instCode + "_BORESIGHT_SAMPLE";
    double sampleBoreSight = getDouble(ikernKey);

    ikernKey = instCode + "_BORESIGHT_LINE";
    double lineBoreSight = getDouble(ikernKey);

    //  get instrument-specific sample offset
    QString instModeId = ((QString)inst["InstrumentModeId"]).toUpper();
    // For BW mode, add the mode (0,1 (non-polar) or 2,3 (polar)) used to
    // acquire image
    if (instModeId == "BW") {
      instModeId += inst["Mode"][0];
      // There are no offsets for BW mode.. there can only be 1 filter
      //   and there must be 1 filter.
      p_frameletOffsets[0] = 0;
    }
    ikernKey = instCode + "_" + instModeId + "_SAMPLE_OFFSET";
    int sampOffset = getInteger(ikernKey);

    detectorOriginSamp = sampleBoreSight + 1;
    detectorOriginLine = lineBoreSight + 1;

    FocalPlaneMap()->SetDetectorOrigin(detectorOriginSamp,
                                       detectorOriginLine);
    dmap->SetStartingDetectorSample(sampOffset+1);

    // Setup distortion map
    new LroWideAngleCameraDistortionMap(this, naifIkCode());

    // Setup the ground and sky map
    bool evenFramelets = (QString(inst["Framelets"][0]).toUpper()
                          == "EVEN");

    new PushFrameCameraGroundMap(this, evenFramelets);

    new CameraSkyMap(this);
    LoadCache();
    NaifStatus::CheckErrors();

    if(instId == "WAC-UV") {
      // geometric tiling is not worth trying for 4-line framelets
      SetGeometricTilingHint(2, 2);
    }
    else {
      SetGeometricTilingHint(8, 4);
    }
  }

  /**
   * Sets the band in the camera model
   *
   * @param vband The band number to set
   */
  void LroWideAngleCamera::SetBand(const int vband) {

    // Sanity check on requested band
    int maxbands = min(p_detectorStartLines.size(), p_frameletOffsets.size());
    if((vband <= 0) || (vband > maxbands)) {
      ostringstream mess;
      mess << "Requested virtual band (" << vband
           << ") outside valid (BandBin/Center) limits (1 - " << maxbands
           <<  ")";
      throw IException(IException::Programmer, mess.str(), _FILEINFO_);
    }

    //  Set up valid band access
    Camera::SetBand(vband);
    PushFrameCameraDetectorMap *dmap = NULL;
    dmap = (PushFrameCameraDetectorMap *) DetectorMap();
    dmap->SetBandFirstDetectorLine(p_detectorStartLines[vband - 1]);
    dmap->SetFrameletOffset(p_frameletOffsets[vband - 1]);
  }

  /**
   * @param key
   * @return @b int Pool key size
   */
  int LroWideAngleCamera::PoolKeySize(const QString &key) const {
    SpiceBoolean found;
    SpiceInt n;
    SpiceChar ctype[1];
    dtpool_c(key.toAscii().data(), &found, &n, ctype);
    if(!found) n = 0;
    return (n);
  }

  /**
   * @param key
   * @return @b vector < @b int >
   */
  vector<int> LroWideAngleCamera::GetVector(const QString &key) {
    QVariant poolKeySize = getStoredResult(key + "_SIZE", SpiceIntType);

    int nvals = poolKeySize.toInt();

    if(nvals == 0) {
      nvals = PoolKeySize(key);
      storeResult(key + "_SIZE", SpiceIntType, nvals);
    }

    if(nvals <= 0) {
      QString mess = "Kernel pool keyword " + key + " not found!";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    vector<int> parms;
    for(int i = 0 ; i < nvals ; i++) {
      parms.push_back(getInteger(key, i));
    }

    return (parms);
  }

}


// Plugin
/**
 * This is the function that is called in order to instantiate a
 * LroWideAngleCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* LroWideAngleCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Lro namespace.
 */
extern "C" Isis::Camera *LroWideAngleCameraPlugin(Isis::Cube &cube) {
  return new Isis::LroWideAngleCamera(cube);
}

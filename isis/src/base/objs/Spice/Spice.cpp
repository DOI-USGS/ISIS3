/**
 * @file
 * $Revision: 1.24 $
 * $Date: 2010/04/09 22:31:16 $
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
#include "Spice.h"

#include <cfloat>
#include <iomanip>

#include <QVector>

#include "Constants.h"
#include "Distance.h"
#include "EndianSwapper.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "iTime.h"
#include "Longitude.h"
#include "NaifStatus.h"

#include "getSpkAbCorrState.hpp"

using namespace std;

namespace Isis {
  /**
   * Constructs a Spice object and loads SPICE kernels using information from the
   * label object. The constructor expects an Instrument and Kernels group to be
   * in the labels.
   *
   * @param lab Label containing Instrument and Kernels groups.
   *
   * @internal
   * @history 2005-10-07 Jim Torson  -   Modified the constructor so it can
   *                                      handle multiple SpacecraftPosition and
   *                                      multiple SpacecraftPointing kernel files
   * @history 2005-11-29 Debbie A. Cook - Added loop to allow multiple frames
   *                                      kernels and code to initialize Naif
   *                                      codes when no platform is used (landers)
   * @history 2006-01-03 Debbie A. Cook - Added loop to allow multiple spacecraft
   *                                      clock kernels (for Viking)
   * @history 2006-02-21 Jeff Anderson/Debbie Cook - Refactor to use SpicePosition
   *                                                 and SpiceRotation classes
   * @history 2009-03-18 Tracie Sucharski - Remove code for old keywords.
   */

  // TODO: DOCUMENT EVERYTHING
  Spice::Spice(Pvl &lab) {
    PvlGroup kernels = lab.FindGroup("Kernels", Pvl::Traverse);
    bool hasTables = (kernels["TargetPosition"][0] == "Table");

    Init(lab, !hasTables);
  }

  /**
   * Constructs a Spice object.
   *
   * @param lab  Pvl labels.
   * @param noTables Indicates the use of tables.
   */
  Spice::Spice(Pvl &lab, bool noTables) {
    Init(lab, noTables);
  }

  /**
   * Initialization of Spice object.
   *
   * @param lab  Pvl labels
   * @param noTables Indicates the use of tables.
   *
   * @throw Isis::Exception::Io - "Can not find NAIF code for NAIF target"
   * @throw Isis::Exception::Camera - "No camera pointing available"
   * @throw Isis::Exception::Camera - "No instrument position available"
   *
   * @internal
   *   @history 2011-02-08 Jeannie Walldren - Initialize pointers to null.
   */
  void Spice::Init(Pvl &lab, bool noTables) {
    NaifStatus::CheckErrors();

    // Initialize members
    p_radii = new Distance[3];

    p_solarLongitude = new Longitude;
    p_et = NULL;
    p_kernels = new QVector<iString>;
    p_target = new iString;

    p_startTime = new iTime;
    p_endTime = new iTime;
    p_cacheSize = new SpiceDouble;
    *p_cacheSize = 0;

    p_startTimePadding = new SpiceDouble;
    *p_startTimePadding = 0;
    p_endTimePadding = new SpiceDouble;
    *p_endTimePadding = 0;

    p_instrumentPosition = NULL;
    p_instrumentRotation = NULL;
    p_sunPosition = NULL;
    p_bodyRotation = NULL;

    p_allowDownsizing = false;

    p_bodyCode = new SpiceInt;
    p_spkCode = new SpiceInt;
    p_ckCode = new SpiceInt;
    p_ikCode = new SpiceInt;
    p_sclkCode = new SpiceInt;
    p_spkBodyCode = new SpiceInt;

    p_naifKeywords = new PvlObject("NaifKeywords");

    p_sky = false;

    // Get the kernel group and load main kernels
    PvlGroup kernels = lab.FindGroup("Kernels", Pvl::Traverse);

    // Get the time padding first
    if(kernels.HasKeyword("StartPadding")) {
      *p_startTimePadding = kernels["StartPadding"][0];
    }
    else {
      *p_startTimePadding = 0.0;
    }

    if(kernels.HasKeyword("EndPadding")) {
      *p_endTimePadding  = kernels["EndPadding"][0];
    }
    else {
      *p_endTimePadding = 0.0;
    }

    // p_usingNaif = !lab.HasObject("NaifKeywords") || noTables;
    p_usingNaif = !lab.HasObject("NaifKeywords") || noTables
      || lab.FindObject("IsisCube").HasGroup("OriginalInstrument");

//  Modified  to load planetary ephemeris SPKs before s/c SPKs since some
//  missions (e.g., MESSENGER) may augment the s/c SPK with new planet
//  ephemerides. (2008-02-27 (KJB))
    if(p_usingNaif) {
      if(noTables) {
        Load(kernels["TargetPosition"], noTables);
        Load(kernels["InstrumentPosition"], noTables);
        Load(kernels["InstrumentPointing"], noTables);
      }

      if(kernels.HasKeyword("Frame")) {
        Load(kernels["Frame"], noTables);
      }

      Load(kernels["TargetAttitudeShape"], noTables);
    if(kernels.HasKeyword("Instrument"))
    Load(kernels["Instrument"], noTables);
      // Always load after instrument
    if(kernels.HasKeyword("InstrumentAddendum"))
    Load(kernels["InstrumentAddendum"], noTables);
      Load(kernels["LeapSecond"], noTables);
    if( kernels.HasKeyword("SpacecraftClock"))
    Load(kernels["SpacecraftClock"], noTables);

// Modified to load extra kernels last to allow overriding default values
// (2010-04-07) (DAC)
      if(kernels.HasKeyword("Extra")) {
        Load(kernels["Extra"], noTables);
      }
    }
    else {
      *p_naifKeywords = lab.FindObject("NaifKeywords");
    }

    // Get NAIF ik, spk, sclk, and ck codes
    //
    //    Use ikcode to get parameters from instrument kernel such as focal
    //    length, distortions, focal plane maps, etc
    //
    //    Use spkcode to get spacecraft position from spk file
    //
    //    Use sclkcode to transform times from et to tics
    //
    //    Use ckcode to transform between frames
    //
    //    Use bodycode to obtain radii and attitude (pole position/omega0)
    //
    //    Use spkbodycode to read body position from spk

    string trykey = "NaifIkCode";
    if(kernels.HasKeyword("NaifFrameCode")) trykey = "NaifFrameCode";
    *p_ikCode = (int) kernels[trykey];

    *p_spkCode  = *p_ikCode / 1000;
    *p_sclkCode = *p_spkCode;
    *p_ckCode   = *p_ikCode;

    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    *p_target = inst["TargetName"][0];

    if(iString(*p_target).UpCase() == "SKY") {
      *p_bodyCode = *p_spkCode;
      p_radii[0] = p_radii[1] = p_radii[2] = Distance(1000.0, Distance::Meters);
      p_sky = true;
    }
    else {
      *p_bodyCode = NaifBodyCode();
      iString radiiKey = "BODY" + iString((BigInt)*p_bodyCode) + "_RADII";

      p_radii[0] = Distance(GetDouble(radiiKey, 0), Distance::Kilometers);
      p_radii[1] = Distance(GetDouble(radiiKey, 1), Distance::Kilometers);
      p_radii[2] = Distance(GetDouble(radiiKey, 2), Distance::Kilometers);

      p_sky = false;
    }
    *p_spkBodyCode = *p_bodyCode;

    // Override them if they exist in the labels
    if(kernels.HasKeyword("NaifSpkCode"))
      *p_spkCode = (int) kernels["NaifSpkCode"];

    if(kernels.HasKeyword("NaifCkCode"))
      *p_ckCode = (int) kernels["NaifCkCode"];

    if(kernels.HasKeyword("NaifSclkCode"))
      *p_sclkCode = (int) kernels["NaifSclkCode"];

    if(kernels.HasKeyword("NaifBodyCode"))
      *p_bodyCode = (int) kernels["NaifBodyCode"];

    if(!p_sky) {
      if(kernels.HasKeyword("NaifSpkBodyCode"))
        *p_spkBodyCode = (int) kernels["NaifSpkBodyCode"];
    }

    if(p_sky) {
      // Create the identity rotation for sky targets
      // Everything in bodyfixed will really be J2000
      p_bodyRotation = new SpiceRotation(1);
    }
    else {
      // JAA - Modified to stored and look for the frame body code in the
      // cube labels
      SpiceInt frameCode;
      if((p_usingNaif) || (!p_naifKeywords->HasKeyword("BODY_FRAME_CODE"))) {
        char frameName[32];
        SpiceBoolean found;
        cidfrm_c(*p_spkBodyCode, sizeof(frameName), &frameCode, frameName,
                 &found);

        if(!found) {
          iString naifTarget = "IAU_" + iString(*p_target).UpCase();
          namfrm_c(naifTarget.c_str(), &frameCode);
          if(frameCode == 0) {
            string msg = "Can not find NAIF code for [" + naifTarget + "]";
            throw iException::Message(iException::Io, msg, _FILEINFO_);
          }
        }

        QVariant result = (int)frameCode;
        storeValue("BODY_FRAME_CODE",0,SpiceIntType,result);
      }
      else {
        frameCode = GetInteger("BODY_FRAME_CODE",0);
      }

      p_bodyRotation = new SpiceRotation(frameCode);
    }

    p_instrumentRotation = new SpiceRotation(*p_ckCode);
    p_instrumentPosition = new SpicePosition(*p_spkCode, *p_spkBodyCode);
    p_sunPosition = new SpicePosition(10, *p_bodyCode);

    // Check to see if we have nadir pointing that needs to be computed &
    // See if we have table blobs to load
    if(kernels["TargetPosition"][0].UpCase() == "TABLE") {
      Table t("SunPosition", lab.Filename(), lab);
      p_sunPosition->LoadCache(t);

      Table t2("BodyRotation", lab.Filename(), lab);
      p_bodyRotation->LoadCache(t2);
      if(t2.Label().HasKeyword("SolarLongitude")) {
        *p_solarLongitude = Longitude(t2.Label()["SolarLongitude"],
            Angle::Degrees);
      }
      else {
        SolarLongitude();
      }
    }

    //  We can't assume InstrumentPointing & InstrumentPosition exist, old
    //  files may be around with the old keywords, SpacecraftPointing &
    //  SpacecraftPosition.  The old keywords were in existance before the
    //  Table option, so we don't need to check for Table under the old
    //  keywords.

    if(kernels["InstrumentPointing"].Size() == 0) {
      throw iException::Message(iException::Camera,
                                "No camera pointing available",
                                _FILEINFO_);
    }

    //  2009-03-18  Tracie Sucharski - Removed test for old keywords, any files
    // with the old keywords should be re-run through spiceinit.
    if(kernels["InstrumentPointing"][0].UpCase() == "NADIR") {
      if(p_instrumentRotation) {
        delete p_instrumentRotation;
        p_instrumentRotation = NULL;
      }

      p_instrumentRotation = new SpiceRotation(*p_ikCode, *p_spkBodyCode);
    }
    else if(iString((std::string)kernels["InstrumentPointing"]).UpCase() == "TABLE") {
      Table t("InstrumentPointing", lab.Filename(), lab);
      p_instrumentRotation->LoadCache(t);
    }

    if(kernels["InstrumentPosition"].Size() == 0) {
      throw iException::Message(iException::Camera,
                                "No instrument position available",
                                _FILEINFO_);
    }

    if(iString((std::string)kernels["InstrumentPosition"]).UpCase() == "TABLE") {
      Table t("InstrumentPosition", lab.Filename(), lab);
      p_instrumentPosition->LoadCache(t);
    }

    NaifStatus::CheckErrors();
  }


  /**
   * Loads/furnishes NAIF kernel(s)
   *
   * @param key PvlKeyword
   * @param noTables Indicates the use of tables.
   *
   * @throw Isis::iException::Io - "Spice file does not exist."
   */
  void Spice::Load(PvlKeyword &key, bool noTables) {
    NaifStatus::CheckErrors();

    for(int i = 0; i < key.Size(); i++) {
      if(key[i] == "") continue;
      if(iString(key[i]).UpCase() == "NULL") break;
      if(iString(key[i]).UpCase() == "NADIR") break;
      if(iString(key[i]).UpCase() == "TABLE" && !noTables) break;
      if(iString(key[i]).UpCase() == "TABLE" && noTables) continue;
      Filename file(key[i]);
      if(!file.exists()) {
        string msg = "Spice file does not exist [" + file.Expanded() + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
      string fileName(file.Expanded());
      furnsh_c(fileName.c_str());
      p_kernels->push_back((string)key[i]);
    }

    NaifStatus::CheckErrors();
  }

  /**
   * Destroys the Spice object
   */
  Spice::~Spice() {
    NaifStatus::CheckErrors();

    if(p_radii != NULL) {
      delete [] p_radii;
      p_radii = NULL;
    }

    if(p_solarLongitude != NULL) {
      delete p_solarLongitude;
      p_solarLongitude = NULL;
    }

    if(p_et != NULL) {
      delete p_et;
      p_et = NULL;
    }

    if(p_target != NULL) {
      delete p_target;
      p_target = NULL;
    }

    if(p_startTime != NULL) {
      delete p_startTime;
      p_startTime = NULL;
    }

    if(p_endTime != NULL) {
      delete p_endTime;
      p_endTime = NULL;
    }

    if(p_cacheSize != NULL) {
      delete p_cacheSize;
      p_cacheSize = NULL;
    }

    if(p_startTimePadding != NULL) {
      delete p_startTimePadding;
      p_startTimePadding = NULL;
    }

    if(p_endTimePadding != NULL) {
      delete p_endTimePadding;
      p_endTimePadding = NULL;
    }

    if(p_instrumentPosition != NULL) {
      delete p_instrumentPosition;
      p_instrumentPosition = NULL;
    }

    if(p_instrumentRotation != NULL) {
      delete p_instrumentRotation;
      p_instrumentRotation = NULL;
    }

    if(p_sunPosition != NULL) {
      delete p_sunPosition;
      p_sunPosition = NULL;
    }

    if(p_bodyRotation != NULL) {
      delete p_bodyRotation;
      p_bodyRotation = NULL;
    }

    if(p_bodyCode != NULL) {
      delete p_bodyCode;
      p_bodyCode = NULL;
    }

    if(p_spkCode != NULL) {
      delete p_spkCode;
      p_spkCode = NULL;
    }

    if(p_ckCode != NULL) {
      delete p_ckCode;
      p_ckCode = NULL;
    }

    if(p_ikCode != NULL) {
      delete p_ikCode;
      p_ikCode = NULL;
    }

    if(p_sclkCode != NULL) {
      delete p_sclkCode;
      p_sclkCode = NULL;
    }

    if(p_spkBodyCode != NULL) {
      delete p_spkBodyCode;
      p_spkBodyCode = NULL;
    }

    // Unload the kernels (TODO: Can this be done faster)
    for(int i = 0; p_kernels && i < p_kernels->size(); i++) {
      Filename file(p_kernels->at(i));
      string fileName(file.Expanded());
      unload_c(fileName.c_str());
    }

    if(p_kernels != NULL) {
      delete p_kernels;
      p_kernels = NULL;
    }

    NaifStatus::CheckErrors();
  }

  /**
   * This method creates an internal cache of spacecraft and sun positions over a
   * specified time range. The SPICE kernels are then immediately unloaded. This
   * allows multiple instances of the Spice object to be created as the NAIF
   * toolkit can clash if multiple sets of SPICE kernels are loaded. Note that
   * the cache size is specified as an argument. Therefore, times requested via
   * SetTime() which are not directly loaded in the cache will be interpolated.
   * If the instrument position is not cached and cacheSize is greater than 3,
   * the tolerance is passed to the SpicePosition Memcache2HermiteCache()
   * method.
   *
   * @b Note:  Before this method is called, the private variables p_cacheSize,
   * p_startTime and p_endTime must be set.  This is done in the Camera classes
   * using the methods SetCacheSize() and SetStartEndEphemerisTime().
   *
   * @param startTime Starting ephemeris time to cache
   * @param endTime Ending ephemeris time to cache
   * @param size Size of the cache.
   * @param tol Tolerance.
   *
   * @throw Isis::iException::Programmer - "Argument cacheSize must be greater
   *        than zero"
   * @throw Isis::iException::Programmer - "Argument startTime must be less than
   *        or equal to endTime"
   * @throw Isis::iException::User - "This instrument does not support time
   *             padding"
   *
   * @history 2011-04-10 Debbie A. Cook - Updated to only create cache for
   *          instrumentPosition if type is Spice.
   */
  void Spice::CreateCache(iTime startTime, iTime endTime,
      int cacheSize, double tol) {
    NaifStatus::CheckErrors();

    // Check for errors
    if(cacheSize <= 0) {
      string msg = "Argument cacheSize must be greater than zero";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(startTime > endTime) {
      string msg = "Argument startTime must be less than or equal to endTime";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(*p_cacheSize > 0) {
      string msg = "A cache has already been created";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(cacheSize == 1 && (*p_startTimePadding != 0 || *p_endTimePadding != 0)) {
      string msg = "This instrument does not support time padding";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    string abcorr;
    if(getSpkAbCorrState(abcorr) )
      InstrumentPosition()->SetAberrationCorrection("NONE");

    iTime avgTime((startTime.Et() + endTime.Et()) / 2.0);
    ComputeSolarLongitude(avgTime);

    // Cache everything
    if(!p_bodyRotation->IsCached()) {
      int bodyRotationCacheSize = cacheSize;
      if(cacheSize > 2) bodyRotationCacheSize = 2;
      p_bodyRotation->LoadCache(
          startTime.Et() - *p_startTimePadding,
          endTime.Et() + *p_endTimePadding,
          bodyRotationCacheSize);
    }

    if(p_instrumentRotation->GetSource() < SpiceRotation::Memcache) {
      if(cacheSize > 3) p_instrumentRotation->MinimizeCache(SpiceRotation::Yes);
      p_instrumentRotation->LoadCache(
          startTime.Et() - *p_startTimePadding,
          endTime.Et() + *p_endTimePadding,
          cacheSize);
    }

    if(p_instrumentPosition->GetSource() < SpicePosition::Memcache) {
      p_instrumentPosition->LoadCache(
          startTime.Et() - *p_startTimePadding,
          endTime.Et() + *p_endTimePadding,
          cacheSize);
      if(cacheSize > 3) p_instrumentPosition->Memcache2HermiteCache(tol);
    }

    if(!p_sunPosition->IsCached()) {
      int sunPositionCacheSize = cacheSize;
      if(cacheSize > 2) sunPositionCacheSize = 2;
      p_sunPosition->LoadCache(
          startTime.Et() - *p_startTimePadding,
          endTime.Et() + *p_endTimePadding,
          sunPositionCacheSize);
    }

    // Save the time and cache size
    *p_startTime = startTime;
    *p_endTime = endTime;
    *p_cacheSize = cacheSize;
    p_et = NULL;

    // Unload the kernels (TODO: Can this be done faster)
    for(int i = 0; i < p_kernels->size(); i++) {
      Filename file(p_kernels->at(i));
      string fileName(file.Expanded());
      unload_c(fileName.c_str());
    }

    p_kernels->clear();

    NaifStatus::CheckErrors();
  }


  /**
   * Accessor method for the cache start time.
   * @return @b iTime Start time for the image.
   * @author Steven Lambright
   * @internal
   *   @history 2011-02-09 Steven Lambright - Original version.
   */
  iTime Spice::CacheStartTime() const {
    if(p_startTime)
      return *p_startTime;

    return iTime();
  }

  /**
   * Accessor method for the cache end time.
   * @return @b iTime End time for the image.
   * @author Steven Lambright
   * @internal
   *   @history 2011-02-09 Steven Lambright - Original version.
   */
  iTime Spice::CacheEndTime() const {
    if(p_endTime)
      return *p_endTime;

    return iTime();
  }


  //NO CALL TO THIS METHOD IS FOUND IN ISIS.  COMMENT OUT AND SAVE FOR AT LEAST 3 MONTHS
  //IF NO NEED IS FOUND FOR IT, DELETE METHOD.
  // 2011-02-08 JEANNIE WALLDREN
//???
//???  /**
//???   * See previous CreateCache method. This method simply invokes that one with
//???   * the same start and end time and a cache size of one.
//???   *
//???   * @param time Ephemeris time to cache
//???   */
//???  void Spice::CreateCache(double time, double tol) {
//???    CreateCache(time, time, 1, tol);
//???  }

  /**
   * Sets the ephemeris time and reads the spacecraft and sun position from the
   * kernels at that instant in time.
   *
   * @param et Ephemeris time (read NAIF documentation for a detailed
   *           description)
   *
   * @see http://naif.jpl.nasa.gov/naif/
   * @internal
   *   @history 2005-11-29 Debbie A. Cook - Added alternate code for processing
   *                                        instruments without a platform
   *   @history 2011-02-09 Steven Lambright - Changed name from
   *                                        SetEphemerisTime()
   */
  void Spice::SetTime(const iTime &et) {

    if(p_et == NULL) {
      p_et = new iTime();
      // Before the Spice is cached, but after the camera aberration correction
      // is set, check to see if the instrument position kernel was created
      // by spkwriter.  If so turn off aberration corrections because the camera
      // set aberration corrections are included in the spk already.
      string abcorr;
      if(*p_cacheSize == 0) {
        if(p_startTime->Et() == 0.0  && p_endTime->Et() == 0.0  &&
           getSpkAbCorrState(abcorr))
          InstrumentPosition()->SetAberrationCorrection("NONE");
      }
    }

    *p_et = et;

    p_bodyRotation->SetEphemerisTime(et.Et());  
    p_instrumentRotation->SetEphemerisTime(et.Et()); 
    p_instrumentPosition->SetEphemerisTime(et.Et());    
    p_sunPosition->SetEphemerisTime(et.Et());

    std::vector<double> uB = p_bodyRotation->ReferenceVector(p_sunPosition->Coordinate());
    p_uB[0] = uB[0];
    p_uB[1] = uB[1];
    p_uB[2] = uB[2];

    ComputeSolarLongitude(*p_et);
  }

  /**
   * Returns the spacecraft position in body-fixed frame km units.
   *
   * @param p[] Spacecraft position
   *
   * @see SetTime()
   *
   * @throw Isis::iException::Programmer - "You must call SetTime first"
   */
  void Spice::InstrumentPosition(double p[3]) const {
    if(p_et == NULL) {
      std::string msg = "You must call SetTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    std::vector<double> sB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Coordinate());
    p[0] = sB[0];
    p[1] = sB[1];
    p[2] = sB[2];
  }


  /**
   * Returns the spacecraft velocity in body-fixed frame km/sec units.
   *
   * @param v[] Spacecraft velocity
   */
  void Spice::InstrumentVelocity(double v[3]) const {
    if(p_et == NULL) {
      std::string msg = "You must call SetTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    std::vector<double> vB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Velocity());
    v[0] = vB[0];
    v[1] = vB[1];
    v[2] = vB[2];
  }


  /**
    * Returns the ephemeris time in seconds which was used to obtain the
    * spacecraft and sun positions.
    *
    * @return double
    */
  iTime Spice::Time() const {
    return *p_et;
  }

  /**
   * Fills the input vector with sun position information, in either body-fixed
   * or J2000 reference frame and km units.
   *
   * @param p[] Sun position
   *
   * @see SetTime()
   */
  void Spice::SunPosition(double p[3]) const {
    if(p_et == NULL) {
      std::string msg = "You must call SetTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    p[0] = p_uB[0];
    p[1] = p_uB[1];
    p[2] = p_uB[2];
  }

  /**
   * Calculates and returns the distance from the spacecraft to the target center
   *
   * @return double Distance to the center of the target from the spacecraft
   */
  double Spice::TargetCenterDistance() const {
    std::vector<double> sB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Coordinate());
    return sqrt(pow(sB[0], 2) + pow(sB[1], 2) + pow(sB[2], 2));
  }

  /**
   * Returns the radii of the body in km. The radii are obtained from the
   * appropriate SPICE kernel for the body specified by TargetName in the
   * Instrument group of the labels.
   *
   * @param r[] Radii of the target in kilometers
   */
  void Spice::Radii(Distance r[3]) const {
    r[0] = p_radii[0];
    r[1] = p_radii[1];
    r[2] = p_radii[2];
  }

  /**
   * This returns the NAIF body code of the target indicated in the labels.
   *
   * @return @b SpiceInt NAIF body code
   *
   */
  SpiceInt Spice::NaifBodyCode() const {
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c(p_target->c_str(), &code, &found);
    if(!found) {
      string msg = "Could not convert Target [" + *p_target +
                   "] to NAIF code";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    return (int) code;
  }

  /**
   * This returns the NAIF SPK code to use when reading from SPK kernels.
   *
   * @return @b SpiceInt NAIF SPK code
   */
  SpiceInt Spice::NaifSpkCode() const {
    return *p_spkCode;
  }

  /**
   * This returns the NAIF CK code to use when reading from CK kernels.
   *
   * @return @b SpiceInt NAIF CK code
   */
  SpiceInt Spice::NaifCkCode() const {
    return *p_ckCode;
  }

  /**
   * This returns the NAIF IK code to use when reading from instrument kernels.
   *
   * @return @b SpiceInt NAIF IK code
   */
  SpiceInt Spice::NaifIkCode() const {
    return *p_ikCode;
  }

  /**
   * This returns the NAIF SCLK code to use when reading from instrument
   * kernels.
   *
   * @return @b SpiceInt NAIF SCLK code
   */
  SpiceInt Spice::NaifSclkCode() const {
    return *p_sclkCode;
  }

  /**
   * This returns the PvlObject that stores all of the requested Naif data
   *   and can be a replacement for furnishing text kernels.
   */
  PvlObject Spice::getStoredNaifKeywords() const {
    return *p_naifKeywords;
  }


  /**
   * This returns a value from the NAIF text pool. It is a static convience
   *
   * @param key Name of NAIF keyword to obtain from the pool
   * @param index If the keyword is an array, the element to obtain.
   *              Defaults to 0
   *
   * @return @b SpiceInt Spice integer from NAIF text pool
   *
   * @throw Isis::iException::Io - "Can not find key in instrument kernels
   */
  SpiceInt Spice::GetInteger(const iString &key, int index) {
    return readValue(key, SpiceIntType, index).toInt();
  }

  /**
   * This returns a value from the NAIF text pool. It is a static convience method
   *
   * @param key Name of NAIF keyword to obtain from the pool
   * @param index If the keyword is an array, the element to obtain. Defaults to 0
   *
   * @return @b SpiceDouble Spice double from NAIF text pool
   *
   * @throw Isis::iException::Io - "Can not find key in instrument kernels."
   */
  SpiceDouble Spice::GetDouble(const iString &key, int index) {
    return readValue(key, SpiceDoubleType, index).toDouble();
  }


  /**
   * This converts the spacecraft clock ticks value (clockValue) to an iTime.
   *
   * Use this when possible because naif calls (such as scs2e_c) cannot be
   *   called when not using naif.
   */
  iTime Spice::getClockTime(iString clockValue, int sclkCode) {
    if(sclkCode == -1)
      sclkCode = NaifSclkCode();

    iTime result;

    iString key = "CLOCK_ET_" + iString(sclkCode) + "_" + clockValue;
    QVariant storedClockTime = getStoredResult(key, SpiceDoubleType);

    if(storedClockTime.isNull()) {
      SpiceDouble timeOutput;
      scs2e_c(sclkCode, clockValue.c_str(), &timeOutput);
      storedClockTime = timeOutput;
      storeResult(key, SpiceDoubleType, timeOutput);
    }

    result = storedClockTime.toDouble();

    return result;
  }


  /**
   * This should be used for reading ALL text naif kernel values. This will
   *   read it from Naif if we're using naif/not attached kernels. If we have
   *   attached kernels and a NaifKeywords label object we will grab it from
   *   there instead. This allows us to not furnish kernels after spiceinit.
   *
   * @param key The naif keyword/value name
   * @param type The naif value's primitive type
   * @param index The index into the naif keyword array to read
   */
  QVariant Spice::readValue(iString key, SpiceValueType type, int index) {
    QVariant result;

    if(p_usingNaif) {
      NaifStatus::CheckErrors();

      // This is the success status of the naif call
      SpiceBoolean found = false;

      // Naif tells us how many values were read, but we always just read one.
      //   Use this variable to make naif happy.
      SpiceInt numValuesRead;

      if(type == SpiceDoubleType) {
        SpiceDouble kernelValue;
        gdpool_c(key.c_str(), (SpiceInt)index, 1,
                 &numValuesRead, &kernelValue, &found);
        result = kernelValue;
      }
      else if(type == SpiceStringType) {
        char kernelValue[512];
        gcpool_c(key.c_str(), (SpiceInt)index, 1, sizeof(kernelValue),
                 &numValuesRead, kernelValue, &found);
        result = kernelValue;
      }
      else if(type == SpiceIntType) {
        SpiceInt kernelValue;
        gipool_c(key.c_str(), (SpiceInt)index, 1, &numValuesRead,
                 &kernelValue, &found);
        result = (int)kernelValue;
      }

      if(!found) {
        string msg = "Can not find [" + key + "] in text kernels";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      storeValue(key, index, type, result);
    }
    else {
      // Read from PvlObject that is our naif keywords
      result = readStoredValue(key, type, index);

      if(result.isNull()) {
        iString msg = "The camera is requesting spice data [" + key + "] that "
            "was not attached, please re-run spiceinit";
        throw iException::Message(iException::Spice, msg, _FILEINFO_);
      }
    }

    return result;
  }


  void Spice::storeResult(iString name, SpiceValueType type, QVariant value) {
    if(type == SpiceDoubleType) {
      EndianSwapper swapper("LSB");

      double doubleVal = value.toDouble();
      doubleVal = swapper.Double(&doubleVal);
      QByteArray byteCode((char *) &doubleVal, sizeof(double));
      value = byteCode;
      type = SpiceByteCodeType;
    }

    storeValue(name + "_COMPUTED", 0, type, value);
  }


  QVariant Spice::getStoredResult(iString name, SpiceValueType type) {
    bool wasDouble = false;

    if(type == SpiceDoubleType) {
      wasDouble = true;
      type = SpiceByteCodeType;
    }

    QVariant stored = readStoredValue(name + "_COMPUTED", type, 0);

    if(wasDouble && !stored.isNull()) {
      EndianSwapper swapper("LSB");
      double doubleVal = swapper.Double((void *)QByteArray::fromHex(
          stored.toByteArray()).data());
      stored = doubleVal;
    }

    return stored;
  }


  void Spice::storeValue(iString key, int index, SpiceValueType type,
                         QVariant value) {
    if(!p_naifKeywords->HasKeyword(key))
      p_naifKeywords->AddKeyword(PvlKeyword(key));

    PvlKeyword &storedKey = p_naifKeywords->FindKeyword(key);

    while(index >= storedKey.Size()) {
      storedKey.AddValue("");
    }

    if(type == SpiceByteCodeType) {
      storedKey[index] = iString(value.toByteArray().toHex().data());
    }
    else if(type == SpiceStringType) {
      storedKey[index] = value.toString();
    }
    else if(type == SpiceDoubleType) {
      storedKey[index] = value.toDouble();
    }
    else if(type == SpiceIntType) {
      storedKey[index] = value.toInt();
    }
    else {
      iString msg = "Unable to store variant in labels for key [" + key + "]";
      throw iException::Message(iException::Spice, msg, _FILEINFO_);
    }
  }


  QVariant Spice::readStoredValue(iString key, SpiceValueType type,
                                  int index) {
    // Read from PvlObject that is our naif keywords
    QVariant result;

    if(p_naifKeywords->HasKeyword(key) && !p_usingNaif) {
      PvlKeyword &storedKeyword = p_naifKeywords->FindKeyword(key);

      try {
        if(type == SpiceDoubleType)
          result = (double)storedKeyword[index];
        else if(type == SpiceStringType)
          result = QString::fromStdString(storedKeyword[index]);
        else if(type == SpiceByteCodeType || SpiceStringType)
          result = QByteArray(
              string(storedKeyword[index]).c_str());
        else if(type == SpiceIntType)
          result = (int)storedKeyword[index];
      }
      catch(iException &e) {
        e.Clear();
      }
    }

    return result;
  }


  /**
   * This returns a value from the NAIF text pool. It is a static convience
   * method
   *
   * @param key Name of NAIF keyword to obtain from the pool
   * @param index If the keyword is an array, the element to obtain. Defaults to 0
   *
   * @return @b string Value from the NAIF text pool
   *
   * @throw Isis::iException::Io - "Can not find key in instrument kernels."
   */
  iString Spice::GetString(const iString &key, int index) {
    return readValue(key, SpiceStringType, index).toString();
  }


  /**
   * Returns the sub-spacecraft latitude/longitude in universal coordinates
   * (0-360 positive east, ocentric)
   *
   * @param lat Sub-spacecraft latitude
   *
   * @param lon Sub-spacecraft longitude
   *
   * @see SetTime()
   * @throw Isis::iException::Programmer - "You must call SetTime
   *             first."
   */
  void Spice::SubSpacecraftPoint(double &lat, double &lon) {
    NaifStatus::CheckErrors();

    if(p_et == NULL) {
      std::string msg = "You must call SetTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    SpiceDouble usB[3], dist;
    std::vector<double> vsB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Coordinate());
    SpiceDouble sB[3];
    sB[0] = vsB[0];
    sB[1] = vsB[1];
    sB[2] = vsB[2];
    unorm_c(sB, usB, &dist);

    SpiceDouble a = p_radii[0].GetKilometers();
    SpiceDouble b = p_radii[1].GetKilometers();
    SpiceDouble c = p_radii[2].GetKilometers();

    SpiceDouble originB[3];
    originB[0] = originB[1] = originB[2] = 0.0;

    SpiceBoolean found;
    SpiceDouble subB[3];
    surfpt_c(originB, usB, a, b, c, subB, &found);

    SpiceDouble mylon, mylat;
    reclat_c(subB, &a, &mylon, &mylat);
    lat = mylat * 180.0 / PI;
    lon = mylon * 180.0 / PI;
    if(lon < 0.0) lon += 360.0;

    NaifStatus::CheckErrors();
  }

  /**
   * Returns the sub-solar latitude/longitude in universal coordinates (0-360
   * positive east, ocentric)
   *
   * @param lat Sub-solar latitude
   * @param lon Sub-solar longitude
   *
   * @see SetTime()
   * @throw Isis::iException::Programmer - "You must call SetTime
   *             first."
   */
  void Spice::SubSolarPoint(double &lat, double &lon) {
    NaifStatus::CheckErrors();

    if(p_et == NULL) {
      std::string msg = "You must call SetTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    SpiceDouble uuB[3], dist;
    unorm_c(p_uB, uuB, &dist);

    SpiceDouble a = p_radii[0].GetKilometers();
    SpiceDouble b = p_radii[1].GetKilometers();
    SpiceDouble c = p_radii[2].GetKilometers();

    SpiceDouble originB[3];
    originB[0] = originB[1] = originB[2] = 0.0;

    SpiceBoolean found;
    SpiceDouble subB[3];
    surfpt_c(originB, uuB, a, b, c, subB, &found);

    SpiceDouble mylon, mylat;
    reclat_c(subB, &a, &mylon, &mylat);
    lat = mylat * 180.0 / PI;
    lon = mylon * 180.0 / PI;
    if(lon < 0.0) lon += 360.0;

    NaifStatus::CheckErrors();
  }


  /**
    * Returns the string name of the target
    *
    * @return string
    */
  iString Spice::Target() const {
    return *p_target;
  }


  /**
   * Computes the solar longitude for the given ephemeris time.  If the target
   * is sky, the longitude is set to -999.0.
   *
   * @param et Ephemeris time
   */
  void Spice::ComputeSolarLongitude(iTime et) {
    NaifStatus::CheckErrors();

    if(iString(Target()).UpCase() == "SKY") {
      *p_solarLongitude = Longitude();
      return;
    }

    if(p_bodyRotation->IsCached()) return;

    double tipm[3][3], npole[3];
    char frameName[32];
    SpiceInt frameCode;
    SpiceBoolean found;

    cidfrm_c(*p_spkBodyCode, sizeof(frameName), &frameCode, frameName, &found);

    if(found) {
      pxform_c("J2000", frameName, et.Et(), tipm);
    }
    else {
      tipbod_c("J2000", *p_spkBodyCode, et.Et(), tipm);
    }

    for(int i = 0; i < 3; i++) {
      npole[i] = tipm[2][i];
    }

    double state[6], lt;
    spkez_c(*p_spkBodyCode, et.Et(), "J2000", "NONE", 10, state, &lt);

    double uavel[3];
    ucrss_c(state, &state[3], uavel);

    double x[3], y[3], z[3];
    vequ_c(uavel, z);
    ucrss_c(npole, z, x);
    ucrss_c(z, x, y);

    double trans[3][3];
    for(int i = 0; i < 3; i++) {
      trans[0][i] = x[i];
      trans[1][i] = y[i];
      trans[2][i] = z[i];
    }

    spkez_c(10, et.Et(), "J2000", "LT+S", *p_spkBodyCode, state, &lt);

    double pos[3];
    mxv_c(trans, state, pos);

    double radius, ls, lat;
    reclat_c(pos, &radius, &ls, &lat);

    *p_solarLongitude = Longitude(ls, Angle::Radians).Force360Domain();

    NaifStatus::CheckErrors();
  }


  /**
   * Returns the solar longitude
   *
   * @return @b double The Solar Longitude
   */
  Longitude Spice::SolarLongitude() {
    if(p_et) {
      ComputeSolarLongitude(*p_et);
      return *p_solarLongitude;
    }

    return Longitude();
  }


  /**
   * Returns true if the kernel group has kernel files
   *
   * @param lab Label containing Instrument and Kernels groups.
   *
   * @return @b bool status of kernel files in the kernel group
   */
  bool Spice::HasKernels(Pvl &lab) {

    // Get the kernel group and check main kernels
    PvlGroup kernels = lab.FindGroup("Kernels", Pvl::Traverse);
    std::vector<string> keywords;
    keywords.push_back("TargetPosition");

    if(kernels.HasKeyword("SpacecraftPosition")) {
      keywords.push_back("SpacecraftPosition");
    }
    else {
      keywords.push_back("InstrumentPosition");
    }

    if(kernels.HasKeyword("SpacecraftPointing")) {
      keywords.push_back("SpacecraftPointing");
    }
    else {
      keywords.push_back("InstrumentPointing");
    }

    if(kernels.HasKeyword("Frame")) {
      keywords.push_back("Frame");
    }

    if(kernels.HasKeyword("Extra")) {
      keywords.push_back("Extra");
    }

    PvlKeyword key;
    for(int ikey = 0; ikey < (int) keywords.size(); ikey++) {
      key = kernels[ikey];

      for(int i = 0; i < key.Size(); i++) {
        if(key[i] == "") return false;
        if(iString(key[i]).UpCase() == "NULL") return false;
        if(iString(key[i]).UpCase() == "NADIR") return false;
        if(iString(key[i]).UpCase() == "TABLE") return false;
      }
    }
    return true;
  }
}

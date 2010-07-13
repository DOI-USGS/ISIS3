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

#include <cfloat>
#include "Spice.h"
#include "iString.h"
#include "iException.h"
#include "Filename.h"
#include "Constants.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Spice object and loads SPICE kernels using information from the
   * label object. The constructor expects an Instrument and Kernels group to be
   * in the labels.
   *
   * @param lab Label containing Instrument and Kernels groups.
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
  Spice::Spice(Isis::Pvl &lab) {
    Isis::PvlGroup kernels = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
    bool hasTables = (kernels["TargetPosition"][0] == "Table");

    Init(lab, !hasTables);
  }

  Spice::Spice(Isis::Pvl &lab, bool notab) {
    Init(lab, notab);
  }

  void Spice::Init(Isis::Pvl &lab, bool notab) {
    NaifStatus::CheckErrors();

    // Initialization
    p_startTime = 0.0;
    p_endTime = 0.0;
    p_cacheSize = 0;
    p_et = -DBL_MAX;
    p_allowDownsizing = false;

    // Get the kernel group and load main kernels
    Isis::PvlGroup kernels = lab.FindGroup("Kernels", Isis::Pvl::Traverse);

    // Get the time padding first
    if(kernels.HasKeyword("StartPadding")) {
      p_startTimePadding = kernels["StartPadding"][0];
    }
    else {
      p_startTimePadding = 0.0;
    }

    if(kernels.HasKeyword("EndPadding")) {
      p_endTimePadding  = kernels["EndPadding"][0];
    }
    else {
      p_endTimePadding = 0.0;
    }


//  Modified  to load planetary ephemeris SPKs before s/c SPKs since some
//  missions (e.g., MESSENGER) may augment the s/c SPK with new planet
//  ephemerides. (2008-02-27 (KJB))
    if(notab) {
      Load(kernels["TargetPosition"], notab);
      Load(kernels["InstrumentPosition"], notab);
      Load(kernels["InstrumentPointing"], notab);
    }

    if(kernels.HasKeyword("Frame")) {
      Load(kernels["Frame"], notab);
    }

    Load(kernels["TargetAttitudeShape"], notab);
    Load(kernels["Instrument"], notab);
    Load(kernels["InstrumentAddendum"], notab);  // Always load after instrument
    Load(kernels["LeapSecond"], notab);
    Load(kernels["SpacecraftClock"], notab);

// Modified to load extra kernels last to allow overriding default values
// (2010-04-07) (DAC)
    if(kernels.HasKeyword("Extra")) {
      Load(kernels["Extra"], notab);
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
    p_ikCode = (int) kernels[trykey];

    p_spkCode = p_ikCode / 1000;
    p_sclkCode = p_spkCode;
    p_ckCode = p_ikCode;

    Isis::PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
    p_target = (string) inst["TargetName"];

    if(iString(p_target).UpCase() == "SKY") {
      p_bodyCode = p_spkCode;
      p_radii[0] = p_radii[1] = p_radii[2] = 1.0;
      p_sky = true;
    }
    else {
      p_bodyCode = NaifBodyCode();
      SpiceInt n;
      bodvar_c(p_bodyCode, "RADII", &n, p_radii);
      p_sky = false;
    }
    p_spkBodyCode = p_bodyCode;

    // Override them if they exist in the labels
    if(kernels.HasKeyword("NaifSpkCode")) p_spkCode = (int) kernels["NaifSpkCode"];
    if(kernels.HasKeyword("NaifCkCode")) p_ckCode = (int) kernels["NaifCkCode"];
    if(kernels.HasKeyword("NaifSclkCode")) p_sclkCode = (int) kernels["NaifSclkCode"];
    if(kernels.HasKeyword("NaifBodyCode")) p_bodyCode = (int) kernels["NaifBodyCode"];
    if(!p_sky) {
      if(kernels.HasKeyword("NaifSpkBodyCode")) p_spkBodyCode = (int) kernels["NaifSpkBodyCode"];
    }

    // Create our SpicePosition and SpiceRotation objects
    p_bodyRotation = 0;
    p_instrumentRotation = 0;
    p_instrumentPosition = 0;
    p_sunPosition = 0;

    if(p_sky) {
      // Create the identity rotation for sky targets
      // Everything in bodyfixed will really be J2000
      p_bodyRotation = new SpiceRotation(1);
    }
    else {
      char frameName[32];
      SpiceInt frameCode;
      SpiceBoolean found;
      cidfrm_c(p_spkBodyCode, sizeof(frameName), &frameCode, frameName, &found);

      if(!found) {
        string naifTarget = string("IAU_") + iString(p_target).UpCase();
        namfrm_c(naifTarget.c_str(), &frameCode);
        if(frameCode == 0) {
          string msg = "Can not find NAIF code for [" + naifTarget + "]";
          throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
        }
      }
      p_bodyRotation = new SpiceRotation(frameCode);
    }
    p_instrumentRotation = new SpiceRotation(p_ckCode);
    p_instrumentPosition = new SpicePosition(p_spkCode, p_spkBodyCode);
    p_sunPosition = new SpicePosition(10, p_bodyCode);

    // Check to see if we have nadir pointing that needs to be computed &
    // See if we have table blobs to load
    if(iString((std::string)kernels["TargetPosition"]).UpCase() == "TABLE") {
      Table t("SunPosition", lab.Filename());
      p_sunPosition->LoadCache(t);

      Table t2("BodyRotation", lab.Filename());
      p_bodyRotation->LoadCache(t2);
      if(t2.Label().HasKeyword("SolarLongitude")) {
        p_solarLongitude = t2.Label()["SolarLongitude"];
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
    if(iString((std::string)kernels["InstrumentPointing"]).UpCase() == "NADIR") {
      delete p_instrumentRotation;
      p_instrumentRotation = new SpiceRotation(p_ikCode, p_spkBodyCode);
    }
    else if(iString((std::string)kernels["InstrumentPointing"]).UpCase() == "TABLE") {
      Table t("InstrumentPointing", lab.Filename());
      p_instrumentRotation->LoadCache(t);
    }

    if(kernels["InstrumentPosition"].Size() == 0) {
      throw iException::Message(iException::Camera,
                                "No instrument position available",
                                _FILEINFO_);
    }

    if(iString((std::string)kernels["InstrumentPosition"]).UpCase() == "TABLE") {
      Table t("InstrumentPosition", lab.Filename());
      p_instrumentPosition->LoadCache(t);
    }

    NaifStatus::CheckErrors();
  }


  //! Load/furnish NAIF kernel(s)
  void Spice::Load(Isis::PvlKeyword &key, bool notab) {
    NaifStatus::CheckErrors();

    for(int i = 0; i < key.Size(); i++) {
      if(key[i] == "") continue;
      if(iString(key[i]).UpCase() == "NULL") break;
      if(iString(key[i]).UpCase() == "NADIR") break;
      if(iString(key[i]).UpCase() == "TABLE" && !notab) break;
      if(iString(key[i]).UpCase() == "TABLE" && notab) continue;
      Isis::Filename file(key[i]);
      if(!file.exists()) {
        string msg = "Spice file does not exist [" + file.Expanded() + "]";
        throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
      }
      string fileName(file.Expanded());
      furnsh_c(fileName.c_str());
      p_kernels.push_back((string)key[i]);
    }

    NaifStatus::CheckErrors();
  }

  /**
   * Destroys the Spice object
   */
  Spice::~Spice() {
    NaifStatus::CheckErrors();

    if(p_bodyRotation != 0) delete p_bodyRotation;
    if(p_instrumentRotation != 0) delete p_instrumentRotation;
    if(p_instrumentPosition != 0) delete p_instrumentPosition;
    if(p_sunPosition != 0) delete p_sunPosition;

    // Unload the kernels (TODO: Can this be done faster)
    for(unsigned int i = 0; i < p_kernels.size(); i++) {
      Isis::Filename file(p_kernels[i]);
      string fileName(file.Expanded());
      unload_c(fileName.c_str());
    }

    NaifStatus::CheckErrors();
  }

  /**
   * This method creates an internal cache of spacecraft and sun positions over a
   * specified time range. The SPICE kernels are then immediately unloaded. This
   * allows multiple instances of the Spice object to be created as the NAIF
   * toolkit can clash if multiple sets of SPICE kernels are loaded. Note that
   * the cache size is specified as an argument. Therefore, times requested via
   * SetEphemerisTime which are not directly loaded in the cache will be
   * interpolated.
   *
   * @param startTime Starting ephemeris time to cache
   *
   * @param endTime Ending ephemeris time to cache
   *
   * @param size Size of the cache.
   *
   * @throws Isis::iException::Programmer
   */
  void Spice::CreateCache(double startTime, double endTime, int cacheSize, double tol) {
    NaifStatus::CheckErrors();

    // Check for errors
    if(cacheSize <= 0) {
      string msg = "Argument cacheSize must be greater than zero";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    if(startTime > endTime) {
      string msg = "Argument startTime must be less than or equal to endTime";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    if(p_cacheSize > 0) {
      string msg = "A cache has already been created";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    if(cacheSize == 1 && (p_startTimePadding != 0 || p_endTimePadding != 0)) {
      string msg = "This instrument does not support time padding";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    double avgTime = (startTime + endTime) / 2.0;
    ComputeSolarLongitude(avgTime);

    // Cache everything
    if(!p_bodyRotation->IsCached()) {
      int bodyRotationCacheSize = cacheSize;
      if(cacheSize > 2) bodyRotationCacheSize = 2;
      p_bodyRotation->LoadCache(startTime - p_startTimePadding, endTime + p_endTimePadding, bodyRotationCacheSize);
    }

    if(p_instrumentRotation->GetSource() < SpiceRotation::Memcache) {
      if(cacheSize > 3) p_instrumentRotation->MinimizeCache(SpiceRotation::Yes);
      p_instrumentRotation->LoadCache(startTime - p_startTimePadding, endTime + p_endTimePadding, cacheSize);
    }

    if(!p_instrumentPosition->IsCached()) {
      p_instrumentPosition->LoadCache(startTime - p_startTimePadding, endTime + p_endTimePadding, cacheSize);
      if(cacheSize > 3) p_instrumentPosition->Memcache2HermiteCache(tol);
    }

    if(!p_sunPosition->IsCached()) {
      int sunPositionCacheSize = cacheSize;
      if(cacheSize > 2) sunPositionCacheSize = 2;
      p_sunPosition->LoadCache(startTime - p_startTimePadding, endTime + p_endTimePadding, sunPositionCacheSize);
    }

    // Save the time and cache size
    p_startTime = startTime;
    p_endTime = endTime;
    p_cacheSize = cacheSize;
    p_et = -DBL_MAX;

    // Unload the kernels (TODO: Can this be done faster)
    for(unsigned int i = 0; i < p_kernels.size(); i++) {
      Isis::Filename file(p_kernels[i]);
      string fileName(file.Expanded());
      unload_c(fileName.c_str());
    }
    p_kernels.clear();

    NaifStatus::CheckErrors();
  }

  /**
   * See previous CreateCache method. This method simply invokes that one with
   * the same start and end time and a cache size of one.
   *
   * @param time Ephemeris time to cache
   */
  void Spice::CreateCache(double time, double tol) {
    CreateCache(time, time, 1, tol);
  }

  /**
   * Sets the ephemeris time and reads the spacecraft and sun position from the
   * kernels at that instant in time.
   *
   * @param et Ephemeris time(read NAIF documentation for a detailed description)
   *
   * @throws Isis::iException::Message
   *
   * @internal
   * @history 2005-11-29 Debbie A. Cook - Added alternate code for processing
   *                                      instruments without a platform
   */
  void Spice::SetEphemerisTime(const double et) {
    p_et = et;

    p_bodyRotation->SetEphemerisTime(et);
    p_instrumentRotation->SetEphemerisTime(et);
    p_instrumentPosition->SetEphemerisTime(et);
    p_sunPosition->SetEphemerisTime(et);

    std::vector<double> uB = p_bodyRotation->ReferenceVector(p_sunPosition->Coordinate());
    p_uB[0] = uB[0];
    p_uB[1] = uB[1];
    p_uB[2] = uB[2];

    ComputeSolarLongitude(p_et);
  }

  /**
   * Returns the spacecraft position in body-fixed frame km units.
   *
   * @param p[] Spacecraft position
   */
  void Spice::InstrumentPosition(double p[3]) const {
    if(p_et == -DBL_MAX) {
      std::string msg = "You must call SetEphemerisTime first";
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
   * @param p[] Spacecraft velocity
   */
  void Spice::InstrumentVelocity(double v[3]) const {
    if(p_et == -DBL_MAX) {
      std::string msg = "You must call SetEphemerisTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    std::vector<double> vB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Velocity());
    v[0] = vB[0];
    v[1] = vB[1];
    v[2] = vB[2];
  }

  /**
  * Returns the sun position in either body-fixed or J2000 reference frame and
  * km units.
  *
  * @param p[] Sun position
  */
  void Spice::SunPosition(double p[3]) const {
    if(p_et == -DBL_MAX) {
      std::string msg = "You must call SetEphemerisTime first";
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
  void Spice::Radii(double r[3]) const {
    r[0] = p_radii[0];
    r[1] = p_radii[1];
    r[2] = p_radii[2];
  }

  /**
   * This returns the NAIF body code of the target indicated in the labels.
   *
   * @return SpiceInt
   *
   * @throws Isis::iException::Io
   */
  SpiceInt Spice::NaifBodyCode() const {
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c(p_target.c_str(), &code, &found);
    if(!found) {
      string msg = "Could not convert Target [" + p_target +
                   "] to NAIF code";
      throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
    }

    return (int) code;
  }

  /**
   * This returns the NAIF SPK code to use when reading from SPK kernels.
   *
   * @return SpiceInt
   */
  SpiceInt Spice::NaifSpkCode() const {
    return p_spkCode;
  }

  /**
   * This returns the NAIF CK code to use when reading from CK kernels.
   *
   * @return SpiceInt
   */
  SpiceInt Spice::NaifCkCode() const {
    return p_ckCode;
  }

  /**
   * This returns the NAIF IK code to use when reading from instrument kernels.
   *
   * @return SpiceInt
   */
  SpiceInt Spice::NaifIkCode() const {
    return p_ikCode;
  }

  // Return naif sclk code
  SpiceInt Spice::NaifSclkCode() const {
    return p_sclkCode;
  }
  /**
   * This returns a value from the NAIF text pool. It is a static convience
   *
   * @param key Name of NAIF keyword to obtain from the pool
   *
   * @param index If the keyword is an array, the element to obtain.
   *              Defaults to 0
   *
   * @return SpiceInt
   *
   * @throws Isis::iException::Io
   */
  SpiceInt Spice::GetInteger(const std::string &key, int index) {
    NaifStatus::CheckErrors();

    SpiceBoolean found;
    SpiceInt value;
    SpiceInt n;
    gipool_c(key.c_str(), (SpiceInt)index, 1, &n, &value, &found);
    if(!found) {
      string msg = "Can not find [" + key + "] in instrument kernels";
      throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
    }

    NaifStatus::CheckErrors();

    return value;
  }

  /**
   * This returns a value from the NAIF text pool. It is a static convience method
   *
   * @param key Name of NAIF keyword to obtain from the pool
   *
   * @param index If the keyword is an array, the element to obtain. Defaults to 0
   *
   * @return SpiceDouble
   *
   * @throws Isis::iException::Io
   */
  SpiceDouble Spice::GetDouble(const std::string &key, int index) {
    NaifStatus::CheckErrors();

    SpiceBoolean found;
    SpiceDouble value;
    SpiceInt n;
    gdpool_c(key.c_str(), (SpiceInt)index, 1, &n, &value, &found);
    if(!found) {
      string msg = "Can not find [" + key + "] in instrument kernels";
      throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
    }

    NaifStatus::CheckErrors();

    return value;
  }

  /**
   * This returns a value from the NAIF text pool. It is a static convience
   * method
   *
   * @param key Name of NAIF keyword to obtain from the pool
   *
   * @param index If the keyword is an array, the element to obtain. Defaults to 0
   *
   * @return string
   *
   * @throws Isis::iException::Io
   */
  string Spice::GetString(const std::string &key, int index) {
    NaifStatus::CheckErrors();

    SpiceBoolean found;
    SpiceInt n;
    char cstr[512];
    gcpool_c(key.c_str(), (SpiceInt)index, 1, 512, &n, cstr, &found);
    if(!found) {
      string msg = "Can not find [" + key + "] in instrument kernels";
      throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
    }

    NaifStatus::CheckErrors();

    return string(cstr);
  }

  /**
   * Returns the sub-spacecraft latitude/longitude in universal coordinates
   * (0-360 positive east, ocentric)
   *
   * @param lat Sub-spacecraft latitude
   *
   * @param lon Sub-spacecraft longitude
   */
  void Spice::SubSpacecraftPoint(double &lat, double &lon) {
    NaifStatus::CheckErrors();

    if(p_et == -DBL_MAX) {
      std::string msg = "You must call SetEphemerisTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    SpiceDouble usB[3], dist;
    std::vector<double> vsB = p_bodyRotation->ReferenceVector(p_instrumentPosition->Coordinate());
    SpiceDouble sB[3];
    sB[0] = vsB[0];
    sB[1] = vsB[1];
    sB[2] = vsB[2];
    unorm_c(sB, usB, &dist);

    SpiceDouble a = p_radii[0];
    SpiceDouble b = p_radii[1];
    SpiceDouble c = p_radii[2];

    SpiceDouble originB[3];
    originB[0] = originB[1] = originB[2] = 0.0;

    SpiceBoolean found;
    SpiceDouble subB[3];
    surfpt_c(originB, usB, a, b, c, subB, &found);

    SpiceDouble mylon, mylat;
    reclat_c(subB, &a, &mylon, &mylat);
    lat = mylat * 180.0 / Isis::PI;
    lon = mylon * 180.0 / Isis::PI;
    if(lon < 0.0) lon += 360.0;

    NaifStatus::CheckErrors();
  }

  /**
   * Returns the sub-solar latitude/longitude in universal coordinates (0-360
   * positive east, ocentric)
   *
   * @param lat Sub-solar latitude
   *
   * @param lon Sub-solar longitude
   */
  void Spice::SubSolarPoint(double &lat, double &lon) {
    NaifStatus::CheckErrors();

    if(p_et == -DBL_MAX) {
      std::string msg = "You must call SetEphemerisTime first";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    SpiceDouble uuB[3], dist;
    unorm_c(p_uB, uuB, &dist);

    SpiceDouble a = p_radii[0];
    SpiceDouble b = p_radii[1];
    SpiceDouble c = p_radii[2];

    SpiceDouble originB[3];
    originB[0] = originB[1] = originB[2] = 0.0;

    SpiceBoolean found;
    SpiceDouble subB[3];
    surfpt_c(originB, uuB, a, b, c, subB, &found);

    SpiceDouble mylon, mylat;
    reclat_c(subB, &a, &mylon, &mylat);
    lat = mylat * 180.0 / Isis::PI;
    lon = mylon * 180.0 / Isis::PI;
    if(lon < 0.0) lon += 360.0;

    NaifStatus::CheckErrors();
  }

  void Spice::ComputeSolarLongitude(double et) {
    NaifStatus::CheckErrors();

    if(iString(Target()).UpCase() == "SKY") {
      p_solarLongitude = -999.0;
      return;
    }

    if(p_bodyRotation->IsCached()) return;

    double tipm[3][3], npole[3];
    char frameName[32];
    SpiceInt frameCode;
    SpiceBoolean found;

    cidfrm_c(p_spkBodyCode, sizeof(frameName), &frameCode, frameName, &found);

    if(found) {
      pxform_c("J2000", frameName, et, tipm);
    }
    else {
      tipbod_c("J2000", p_spkBodyCode, et, tipm);
    }

    for(int i = 0; i < 3; i++) {
      npole[i] = tipm[2][i];
    }

    double state[6], lt;
    spkez_c(p_spkBodyCode, et, "J2000", "NONE", 10, state, &lt);

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

    spkez_c(10, et, "J2000", "LT+S", p_spkBodyCode, state, &lt);

    double pos[3];
    mxv_c(trans, state, pos);

    double radius, ls, lat;
    reclat_c(pos, &radius, &ls, &lat);
    ls *= 180.0 / Isis::PI;
    if(ls < 0.0) ls += 360.0;
    else if(ls > 360.0) ls -= 360.0;
    p_solarLongitude = ls;

    NaifStatus::CheckErrors();
  }


  /**
   * Returns the solar longitude
   *
   * @return double - The Solar Longitude
   */
  double Spice::SolarLongitude() {
    ComputeSolarLongitude(p_et);
    return p_solarLongitude;
  }


  /**
   * Returns true if the kernel group has kernel files
   *
   * @param lab Label containing Instrument and Kernels groups.
   * @return bool - status of kernel files in the kernel group
   */
  bool Spice::HasKernels(Isis::Pvl &lab) {

    // Get the kernel group and check main kernels
    Isis::PvlGroup kernels = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
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

    Isis::PvlKeyword key;
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

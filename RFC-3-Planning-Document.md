**This is simply a raw plain text dump of the internal document put together by ISIS developers to determine the scope of changes required in the work proposed in RFC3**

________________


S - Spice
M - Memcache
HC - HermiteCache
PF - PolyFunction
PFH - PolyFunctionOverHermiteConstant






Functions Within SpicePosition


SpicePosition() / init() → S


LoadCache(double, double, int) → M


LoadCache(Table) → M, HC, PF
* This one depends on the label in the Table being passed in
* No PFH?
* Also may call SetPolynomial(std::vector<double>, std::vector<double>, std::vector<double>, Source) 
   * Can this change source unintentionally? 


ReloadCache() → M


ReloadCache(Table) → M, HC, PF
* This one depends on the label in the Table being passed in
* This method seems to just be an override for a fail-safe built into the LoadCache(Table) method; by setting the Source to Spice before calling LoadCache(Table), program does not fail if there has already been a cache loaded before


LoadHermiteCache(QString) → HC


SetPolynomial(std::vector<double>, std::vector<double>, std::vector<double>, Source) → (Source)
* This Source defaults to PF
* I believe the only other option is PFH, but not confirmed


SetPolynomial(Source) → (Source)
* Calls SetPolynomial(std::vector<double>, std::vector<double>, std::vector<double>, Source) just before returning


Memcache2HermiteCache(double) → HC


LineCache(QString)
* May call ReloadCache()


SetPolynomialDegree(int)
* May call SetPolynomial(std::vector<double>, std::vector<double>, std::vector<double>, Source) 
   * Can this change the Source unintentionally?






Where in ISIS the SpicePosition/SpacecraftPosition Source is Changing


* apolloPanInit/main.cpp
   * S ( init ) → M ( LoadCache(double, double, int) )
* Spice.cpp
   * Has two member variables of type SpicePosition: m_instrumentPosition and m_sunPosition
   * Spice::init(Pvl, bool)
      * Both m_intrumentPosition and m_sunPosition: S ( init ) → M, HC, or PF ( LoadCache(Table) )
      * (SpiceRotation is also used here…)
   * Spice::createCache(iTime, iTime, int, double)
      * Both m_intrumentPosition and m_sunPosition: M, HC or PF (Whatever was loaded in Spice::init) → M ( LoadCache(doube, double, int) )
      * m_instrumentPosition: M → HM ( Memcache2HermiteCache(double) )
         * Optional - only if cache-size > 3
* BundleObservation.cpp
   * Has one member variable of type SpicePosition: m_instrumentPosition, as well as a couple local variables
      * m_instrumentPosition may or may not be initialized in constructor depending on which one is called
      * It does not appear that m_instrumentPosition’s Source ever changes within this file
   * initializeExteriorOrientation()
      * Local variable spicePosition: S ( init ) → (Whatever was specified as the positionInteroplationType in the solve settings for this observation) ( SetPolynomial(Source) )
         * (This may not be relevant)
   * applyParameterCorrections(LinearAlgebra::Vector)
      * (saa)
* BundleObservationSolveSettings.cpp
   * This class only really has a member variable, m_positionInterpolationType, that is of type SpicePosition::Source and is set in some of the constructors, or in setInstrumentPositionSettings(InstrumentPositionSolveOption)
Replace spiceinit Logic with ALE
Scope - 1500 PersonHours
Jesse
Summer
Kristin
Adam
Kelvin
* Eliminate keeping kernels loaded post spiceinit
   * All data needed from kernels needs to be cached during spiceinit
   * 99% of the time this is already the case (spiceinit attach=true)
* Move all SPICE kernel calls into ALE
   * This will remove 99% of SPICE calls from ISIS camera models
   * Currently, different parts of the camera models request the kernel data that they need
   * We will need to consolidate all of the different SPICE data camera models require into ALE
* Write an ALE driver for every ISIS camera model (45 plugins)
   * Halfway step to generating USGS CSM ISDs from any ISIS cube
   * Many smaller tasks
   * 400
   * 240
   * 360
   * 400
   * 350
* Rewrite ISIS kernel selection logic in ALE
   * We will use the existing configuration files and directory structure
   * 80
   * 100
   * 50
   * 60
   * 40
* Change the Spice class implementation to use ALE and a json object instead of SPICE class and the NAIF keywords objects
   * Keep the majority of the current Spice class API
   * 160
   * 100
   * 80
   * 80
   * 80
* SpicePosition and SpiceRotation will also change to using ALE and the json object instead of SPICE calls
   * Will benchmark for speed concerns here
   * 160
   * 100
   * 160
   * 100
   * 160
* Spiceserver will be replaced with pfeffernusse running ALE
   * The current spiceserver will need to be kept running to support older versions
   * 40
   * 60
   * 60
   * 60
   * 60
* Move Target class functionality into Spice class (which in turn gets its data from ALE label)
   * 40
   * 60
   * 40
   * 30
   * 40
* Final integration testing
   * 120
   * 120
   * 80
   * 80
   * 120
Benefits
* Testing logic in ALE drivers is easier than testing logic in ISIS camera models
   * Python tests are simpler because it’s a more flexible language
   * Pytest is better suited for testing with “fake” data
* More commonly used language amongst our users
   * Help foster open source contributions
* Isolating SPICE to one location and using an easier to read language will make developing understanding quicker
* ISIS kernel selection, camera interior, and exterior orientation will be available through just ALE
* Maintain a single web service for both USGS CSM and ISIS
* Maintain a single library for generating camera interior and exterior orientation that can be used in USGS CSM and ISIS
* Camera exterior and interior orientation can come from non SPICE data sources
* Exploratory design and research will largely be shared with CSM integration in ISIS
   * CSM is high level integration camera model
   * ALE is low level integration camera model
* A significant step toward modularizing ISIS
Risks
* No forward compatibility 
   * This will be a pain point for a little while after release
* Need to maintain backwards compatibility
   * ALE will allow for homogenization of old spiceinit’d cubes and new spiceinit’d cubes.
   * Already partially implemented
* This impacts 99% of processes in ISIS
   * Two types of problems:
      * Missing data - This will be immediately obvious if it happens, exceptions will be thrown
      * Incorrect data - This will be found through differences in test outputs
* There are a large number of ISIS camera models
   * The majority of camera models use a small, well defined set of SPICE data
   * Python is a more flexible language than c++, implementing specialized logic in ALE will be less complex than its ISIS implementation
Notes


Make some kind of a “spiceinit just calls ale” wrapper to not break mission processing pipelines with a spiceinit step. 


If we create a geoTIFF writer would we put it in ALE? PDS file -> ALE -> ISD -> CSM -> Ground Extents -> geoTIFF


Will ALE be useful if we don’t incorporate “2isis” functionality into it? If we can make it easier to develop new ingestion tools, other programmers can work on them.

When caching of SPICE data was added to ISIS, why wasn’t it all done at once?
Classes/Apps to be modified
* LightTimeCorretionState
   * Move logic to ALE
* KernelDb
   * Move logic to ALE
* kerneldbgen
   * Move into ale, if time available. 
* Bundle
   * BundleTargetBody
      * Could change due to target being moved into ALE
   * BundleObservation
      * Will change due to the overhaul of SpicePosition, and SpiceRotation
   * BundleObservationSolveSettings
      * Will change due to the overhaul of SpicePosition, and SpiceRotation
* MiniRF
   * Will change due to the overhaul of SpicePosition, and SpiceRotation
* apollopaninit/main
   * Will change due to the overhaul of SpicePosition, and SpiceRotation
* spiceinit 
   * Remove. Functionality now handled by ALE
* Spice
   * Much of the logic will now be handled by making calls to ALE
   * (see below)
* Target
   * Remove and add functionality to ALE
   * (see below)
* Sensor
   * Potential changes due to the reduction of the target class into ALE along with small changes from the spice API
* ShadowFunctor
   * Small changes due to spice API changes
* SpicePosition and SpiceRotation
   * Remove and move logic to ALE
* SpacecraftPosition and LineScanCameraRotation
   * (Inherit from SpicePosition and SpiceRotation respectively)
* ShapeModelFactory
   * Updated to make use of a Spice object rather than the removed Target object


Potential Changes in Spice
Public Methods
* void setTime(const iTime &time);
   * No change
* void instrumentPosition(double p[3]) const;
   * This can be replaced everywhere by instrumentBodyFixedPosition
* void instrumentBodyFixedPosition(double p[3]) const;
   * No change
* void sunPosition(double p[3]) const;
   * This can be replaced with json and ALE calls
* double targetCenterDistance() const;
   * This can be replaced with json and ALE calls
* Longitude solarLongitude(); and void Spice::computeSolarLongitude(iTime et)
   * This can be replaced with json and ALE calls
* void instrumentBodyFixedVelocity(double v[3]) const;
   * This can be replaced with json and ALE calls
* iTime time() const;
   * No change
* void radii(Distance r[3]) const;
   * This is a wrapper around the Target class
   * Can we merge the Target class into Spice?
* void createCache(iTime startTime, iTime endTime, const int size, double tol);
   * This is going away
* iTime cacheStartTime() const;
   * This can be replaced with json and ALE calls
* iTime cacheEndTime() const;
   * This can be replaced with json and ALE calls
* void subSpacecraftPoint(double &lat, double &lon);
   * This can be replaced with json and ALE calls
* void subSolarPoint(double &lat, double &lon);
   * This can be replaced with json and ALE calls
* Target *target() const;
   * This depends on how Target and Spice are related
* QString targetName() const;
   * This depends on how Target and Spice are related
* iTime getClockTime(QString clockValue, int sclkCode = -1);
   * This can be replaced with json and ALE call
   * There are several calls to this in camera model implementations that will need to be baked into ALE drivers
* SpiceDouble getDouble(const QString &key, int index = 0);
* SpiceInt getInteger(const QString &key,   int index = 0);
* QString getString(const QString &key,         int index = 0);
   * These three will be replaced with json calls
* SpicePosition *sunPosition() const;
   * This can potentially be removed
* SpicePosition *instrumentPosition() const;
   * This can potentially be removed
* SpiceRotation *bodyRotation() const;
   * This can potentially be removed
* SpiceRotation *instrumentRotation() const;
   * This can potentially be removed
   * All four of these methods can be replaced with methods in the Spice class that provide the functionality these are used for.
* bool hasKernels(Pvl &lab);
   * This can potentially be removed
* bool isTimeSet();
   * This will be unchanged
* SpiceInt naifBodyCode() const;
   * This depends on how Target and Spice are related
* SpiceInt naifSpkCode() const;
   * This can be replaced with json and ALE calls
* SpiceInt naifCkCode() const;
   * This can be replaced with json and ALE calls
* SpiceInt naifIkCode() const;
   * This can be replaced with json and ALE calls
* SpiceInt naifSclkCode() const;
   * This can be replaced with json and ALE calls
* SpiceInt naifBodyFrameCode() const;
   * This can be replaced with json and ALE calls
* PvlObject getStoredNaifKeywords() const;
   * This can be replaced with with returning the json
   * This will require small changes to hideal2pds
* virtual double resolution();
   * This is a virtual function that just returns 1.0
   * This should probably become pure virtual
Private Methods
* QVariant readValue(QString key, SpiceValueType type, int index = 0);
* void storeResult(QString name, SpiceValueType type, QVariant value);
* QVariant getStoredResult(QString name, SpiceValueType type);
* void storeValue(QString key, int index, SpiceValueType type, QVariant value);
* QVariant readStoredValue(QString key, SpiceValueType type, int index);
   * These will all be replaced by the JSON/ALE scheme
* Spice(const Spice &other);
* Spice &operator=(const Spice &other);
   * These will remain to prevent copying
* void init(Pvl &lab, bool noTables);
   * This will be re-written due to how many of the member variables are changing
* void load(PvlKeyword &key, bool notab);
   * This logic will be moved into ALE and used only in spiceinit
* void computeSolarLongitude(iTime et);
   * This can be replaced with json and ALE calls
Potential Changes in Target
Proposed change: Remove the Target class and move functionality into Spice class


* naifBodyCode()
   * Only being called in Spice.cpp and TargetBody.cpp, should most likely be moved into Spice.cpp or removed depending on how we agree to handle body codes in the ALE label. 
* lookupNaifBodyCode(PVL lab)
   * Acquires target from PVL label and returns naif body code. This method is in the same boat as naifBodyCode()
* init()
   * Sets pointers to null, no reason this can’t be moved or removed 
* name()
   * Returns instrument name from label, already exists in ALE label
* SystemName()
   * Returns Body name from naif code, should be removed and replaced with it being acquired from ALE label. Only being called in Widgets and TargetBody.cpp
* setRadii(Distance)
   * Radii Only changes through setRadii, commented out in Target’s constructor. Should be moved to Spice. 
* shape()
   * shapeModelFactory call should be moved into Spice 
* The rest are just wrappers for spice calls
   * frameType
   * poleRaCoefs
   * poleDecCoefs
   * pmCoefs
   * poleRaNutPrecCoefs
   * poleDecNutPrecCoefs
   * pmNutPrecCoefs
   * sysNutPrecConstants
   * sysNutPrecCoefs


Doing the simple refactor of SpicePositon and SpiceRotation
Scope - 500 PersonHours
* In ISIS3, do not modify the Spice class or spiceinit 
* ISIS handles all kernel loading and unloading
   * ALE just makes SPICE calls to query data
* In ISIS3, keep the API of the SpicePosition and SpiceRotation classes, but change the underlying implementation to use ALE (See: https://github.com/DOI-USGS/ISIS3/wiki/SpicePosition-requirements)
   * Add any functionality to ale which is necessary to do this (See: https://github.com/DOI-USGS/ale/labels/required)
Benefits
* We already have a proof-of-concept PR that demonstrates that SpicePosition::SetEphemerisTimePolyFunction() works with some of the existing code replaced with ale::getPosition() and ale::getVelocity().
* Maintain a single library for generating exterior orientation that can be used in USGS CSM and ISIS
* Better testing of exterior orientation generation
* Greater opportunities for open source contribution to exterior orientation generation
Risks
* Work impacts a large portion of ISIS
   * Work is isolated to two class hierarchies. Easy to test against class APIs and ensure all functionality is honored correctly.
Classes to be modified
* SpicePosition and SpiceRotation
   * Logic will be replaced by ALE
   * API will largely remain the same
* Spice
   * Modified to handle minor changes to SpicePositon and SpiceRotation API
Notes
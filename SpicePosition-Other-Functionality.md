# What is this document?
This is the companion to the core SpicePosition requirements [document](https://github.com/DOI-USGS/ISIS3/wiki/SpicePosition-requirements). This document contains every public method that is not mentioned in the other document. This will be an evolving document where we can collect what each method does, where it is used, and why it is used.

# Methods

## Time Bias (**Candidate For Removal**)
* SetTimeBias(double timeBias)
    * ~~Used by `MocNarrowAngleCamera` and `MocWideAngleCamera`~~
    * NOT used. There is a function with this name used from `SpiceRotation`, but `SpicePosition`'s is not used.
* GetTimeBias() const
    * Only used within `SpicePosition`! We could potentially eliminate this from the API, as it is only used within `SpicePosition`. 

## LightTime and Aberration Correction
* SetAberrationCorrection(const QString &correction)
    * Used by `SpacecraftPosition`, `Spice`, `gllssical/main`, `LoHighCamera`, `LoMediumCamera`, `Mariner10Camera`
* QString GetAberrationCorrection()
    * Not used anywhere (`SpacecraftPosition` has something similar and should probably use it but doesn't)
* GetLightTime()
    * Not used anywhere (`SpacecraftPosition` has something similar and should probably use it but doesn't)

## GetCenterCoordinate
* GetCenterCoordinate()
    * Used by `BundleObservation`
    * Gets the gets the mid point in time between the start and end of the cached position data, and returns that coordinate
    * Used to get the center position of the instrument when looking at a section of ephemeris data

## HasVelocity (Candidate For Removal)
* HasVelocity()
    * Not used, the private variable is getting accessed directly from within `SpicePosition`
    * Returns the private variable p_hasVelocity from a SpicePosition object
    * Doesn't provide any functionality at the moment

## IsCached
* IsCached()
    * Used by `Spice`
    * Checks if there is something stored within the cache
    * Used to determine whether or not to read/write from and to cache

## SetPolynomialDegree
* SetPolynomialDegree(int degree)
    * Used by `BundleObservation`
    * Sets the degree of the internal polynomial state
    * Used to set and change the polynomials degree, consequently expanding or reducing the polynomial if the polynomials have been "applied"(?)

## GetSource
* GetSource()
    * Used by `Spice`
    * Returns the private variable p_source from a `SpicePosition` object
    * Used to determine how the data in `SpicePosition` is stored

## Base Time and Time Scaling
* ComputeBaseTime()
    * Only used within `SpicePosition`
    * Computes the base time based on the private variable p_override
    * Adjust the p_basetime and p_timescale variables in SpicePosition when SetPolynomial is run or when memcache2hermitecache is run
* GetBaseTime()
    * Used in `BundleObservation`
    * Returns the private variable p_basetime
    * Used to get the current base time for fit equations
* SetOverrideBaseTime(double baseTime, double timeScale)
    * Used in `BundleObservation`
    * Sets the p_overrideBaseTime to the given baseTime, sets p_overrideTimeScale to the given time scale, and sets p_override to timeScale
    * Used in LoadCache to set the above variables if p_source is not a PolyFunction
* GetTimeScale()
    * Used in `BundleObservation`
    * Returns the private variable p_timeScale
    * Used to set a variable in `BundleObservation::initializeExteriorOrientation`

## DPolynomial 
* DPolynomial(const int coeffIndex)
    * Only used within `SpicePosition`
    * Takes the derivative of a polynomial in respect to a given coefficient index
    * Used to compute partial derivatives in `SpicePosition`

## Extrapolate
* Extrapolate(double timeEt)
    * Used in `SpkSegment`
    * Extrapolate position for a given time assuming a constant velocity (From doc string)
    * Used to extrapolate a new position based on a known position and assumed constant velocity

## HermiteCoordinate()
* HermiteCoordinate
    * Not used
# What is this document?
This is the companion to the core SpicePosition requirements [document](https://github.com/USGS-Astrogeology/ISIS3/wiki/SpicePosition-requirements). This document contains every public method that is not mentioned in the other document. This will be an evolving document where we can collect what each method does, where it is used, and why it is used.

# Methods

## Time Bias
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

## HasVelocity
* HasVelocity()
    * Only used within `SpicePosition`

## IsCached
* IsCached()
    * Used by `Spice`

## SetPolynomialDegree
* SetPolynomialDegree(int degree)
    * Used by `BundleObservation`

## GetSource
* GetSource()
    * Used by `Spice`

## Base Time and Time Scaling
* ComputeBaseTime()
    * Only used within `SpicePosition`
* GetBaseTime()
    * Used in `BundleObservation`
* SetOverrideBaseTime(double baseTime, double timeScale)
    * Used in `BundleObservation`
* GetTimeScale()
    * Used in `BundleObservation`

## DPolynomial
* DPolynomial(const int coeffIndex)
    * Only used within `SpicePosition`

## Extrapolate
* Extrapolate(double timeEt)
    * Used in `SpkSegment`

## HermiteCoordinate()
* HermiteCoordinate
    * Not used
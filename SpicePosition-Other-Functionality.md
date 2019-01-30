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
* QString GetAberrationCorrection()
* GetLightTime()

## GetCenterCoordinate
* GetCenterCoordinate()

## HasVelocity
* HasVelocity()

## IsCached
* IsCached()

## SetPolynomialDegree
* SetPolynomialDegree(int degree)

## GetSource
* GetSource()

## Base Time and Time Scaling
* ComputeBaseTime()
* GetBaseTime()
* SetOverrideBaseTime(double baseTime, double timeScale)
* GetTimeScale()

## DPolynomial
* DPolynomial(const int coeffIndex)

## Extrapolate
* Extrapolate(double timeEt)

## HermiteCoordinate()
* HermiteCoordinate
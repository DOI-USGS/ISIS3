* Feature/Process Name: Spice Data
* Start Date: Late July - Mid August
* RFC PR: (empty until a PR is opened)
* Issue: https://github.com/DOI-USGS/ISIS3/issues/2370
* Authors: Kristin Berry, Jesse Mapel, Adam Paquette, Kelvin Rodriguez, Summer Stapleton

## Summary

The part of ISIS3 that handles reading SPICE kernels and position/orientation calculations will be pulled out into an external library: the Abstraction Layer for Ephemerides (ALE) which implements a JSON based approach. ISIS3 will have minimal modifications to its Spice class API but internally will make calls to ALE. Backward, but not forwards, compatibility will be maintained.

This RFC is intended for our technical contributors and API consumers. A subsequent RFC will address the SPICE refactor at an application level.

## Motivation

SPICE kernels are one primary source of spacecraft ephemeris information necessary for parametrizing planetary sensor models. The primary source of ephemeris information for the ISIS3 library are SPICE kernels. There are other libraries within Astro, namely the ASC implementation of the Community Sensor Model API (usgscsm), that would like to parametrize sensor models from SPICE kernels as ISIS3 does. As other uses for camera models are developed both within and outside of the Astrogeology Science Center (ASC), we would like to provide the ability to easily read and extract data from SPICE kernels to others without the need for the entire ISIS3 code base.

ISIS3’s current SPICE implementation is difficult to exploit in other libraries. To this end, we have started work on the Abstraction Library for Ephemerides (ALE). ALE contains two components: 1) an IO module written in Python, which encapsulates SPICE data into a human-readable JSON string, and 2) a math module written in C++ that supports evaluation, interpolation, and fitting of ephemeris data.
The new JSON storage format for ephemeris data better affords interoperability across different tools (e.g. usgscsm will be able to access the same camera model data as ISIS3). Redoing both the IO and ephemeris interpolation logic by refactoring them into a new library will remove much of the technical debt currently present in ISIS3’s Spice, SpiceRotation and SpicePosition classes. A new ALE IO module with a higher-level Python implementation affords easier and better testing, the rapid development of new instrument ingestion tools, better readability of the SPICE reading/writing process, and easier training for new developers.  

## Proposed Solution / Explanation
### Terms
* Mixin - An object-oriented programming design pattern. Mixins are, intuitively, interfaces with implementations. Each mixin has a role orthogonal to each other. This allows a developer to inject code into classes by mixing and matching mixins. Here we are only concerned with the Python implementation of mixins via multiple inheritance. That is, in Python, you combine mixins into another class by inheriting from each of the desired mixin classes.  https://en.wikipedia.org/wiki/Mixin
* ALE Driver - Class for reading in a data file written using a mixin design pattern. Each mixin handles a common implementation, e.g. Framer and LineScanner mixins acquire/compute key values that most framer and line scanner sensor types need respectively, there is also a SPICE mixin for generic SPICE data, a PDS3 mixin for generic PDS3 data. For example, a driver for reading an LROC PDS3 image label would mix the LineScanner (containing logic for the number of ephemerides, time dependence, etc.), PDS3 (containing logic for parsing PDS3 keys) and SPICE (containing logic to make the necessary SPICE API calls) mixins in a new class where only exceptional logic needs to be written into methods (e.g. how to translate the PDS3 instrument ID into something bod2c can understand, which is unique to each instrument). This minimizes the amount of new handwritten code for each new instrument/file type that needs to be supported.
* ALE IO Module - A Python module in ALE that encapsulates all ALE drivers. 
* ALE ISD - ALE’s JSON based instrument support data (ISD) format. The output of every ALE driver. This encapsulates all data acquired from original image labels and SPICE kernels required for creating a camera model. 
* Core ISIS3 SPICE Classes - Shorthand for ISIS3’s Spice, SpicePostion and SpiceRotation classes which handle reading from SPICE kernels, reading ISIS3’s binary table format for ephemeris data and interpolating over positional/rotational data.

The proposed refactor accomplishes the following:
* Moves kernel management and querying from ISIS into ALE, which includes both the kernel selection process and NAIF SPICE Toolkit C API calls. 
* Augments ISIS3’s labels with an ALE ISD replacing ISIS3’s kernel PVL groups and binary tables.
   * Requires that, for each instrument ISIS3 currently supports, we must create a new ALE driver. 
   * Requires we reimplement ISIS3’s Kernel selection process in ALE. 
* Reduces the statefulness in core ISIS3 SPICE classes.
* Reduces responsibility of core ISIS3 SPICE classes by moving evaluation, and fitting of positional/rotational data into ALE.
* Relieves ISIS3’s testing burden for the spiceinit app from ISIS3 to ALE
* User workflows will remain unchanged while developers will have to learn the new ALE ISD format. 

This refactor will be a substantial change to the ISIS API, primarily the API for core ISIS3 SPICE classes. All of the SPICE calls in the core ISIS3 SPICE classes along with data interpolation, evaluation, and fitting will move to ALE. Therefore, the core ISIS3 SPICE classes will become consumers of ALE. As stated above, this modularization reduces code coupling, code complexity, and testing complexity. This change also reduces the need to write standard string parsing methods in a low-level (C++) language and instead pushes this work into a higher level (Python) language with good native support for text manipulation.
Currently, ISIS3 handles kernel loading and unloading to obtain data for instantiating sensor models in the Spice class hierarchy (Spice, Sensor, Camera, etc.). The position and orientation information (CKs, SPKs, alternatively "exterior orientation") is stored either in a table of data points, a table with a reduced set of points and an associated Hermite spline, or a set of coefficients for a polynomial fit. Interior orientation information stored in the SPICE kernels, such as focal length, CCD center, principal point offset, etc., are stored as PVL key-value pairs in the NaifKeywords PVL group in the ISIS3 cube label. The proposed change will extract kernel loading into ALE drivers, and replace the current storage method of SPICE data with a new ALE ISD.

The burden of selecting kernels, loading/unloading kernels and handling edge cases present in specific instruments will be moved to ALE’s IO module. This change requires reimplementing ISIS3’s kernel selection process and creating new ALE drivers for ISIS3’s 40+ supported instruments. 

We suggest replacing PVL/binary tables with an ALE ISD appended to an ISIS3 label or as a sidecar file. The new ALE JSON label will contain all SPICE acquired metadata and relevant support data (e.g. positions, orientations, NAIF keys, CCD Center, distortion model information, etc.). The spiceinit application will be modified to use ALE to generate an ALE ISD from the input ISIS cube. spiceinit will then attach the ALE ISD to the ISIS cube label. From this point on, all subsequent SPICE calls will be redirected to read from the data cached in the ALE ISD. This requires refactoring all calls within the ISIS3 library that make direct access to the SPICE API to now make calls to a single interface - ALE.

ISIS3’s Spice class is the entry point for reading and managing all SPICE related sensor data. The Spice class makes the necessary SPICE calls across different member functions and then stores the results in several member variables. In the proposed change, ISIS3’s Spice class would only need to read a JSON label into a single member variable, then access that variable to acquire SPICE data. This form of data access will propagate to other classes like SpicePosition and SpiceRotation since both use the member variables that are stored in the present implementation of the Spice class, eliminating tight coupling between the Spice, SpiceRotation and SpicePosition classes. This refactors the Spice, SpiceRotation, and SpiceRotation classes into wrappers for ALE, requiring re-implementation of tests in ALE. To this end, we plan on testing ALE’s logic against ISIS3’s current tests, mimicking them in ALE and ultimately deleting them in ISIS3. 

## Drawbacks

* In preparation for this RFC, we have enumerated what we believe are a majority of the locations (methods, member variables, etc.) that must be changed in ISIS3 core SPICE classes and are confident this work is tractable (see the [RFC 3 Planning Document](https://github.com/DOI-USGS/ISIS3/wiki/RFC-3-Planning-Document) for details). But we must keep in mind that making major changes in ISIS can be unwieldy considering ISIS3’s technical debt (specifically, an interwoven web of dependencies). Refactoring out ISIS3 SPICE data management, which is a central part of the ISIS3 code base, has potentially wide-reaching ramifications that are challenging to fully account for and additional changes we have yet to explicitly account for. 
* Attempting to remove technical debt via small changes can introduce more technical debt everywhere those small changes interact with existing code. By refactoring large, interconnected components of ISIS3, we reduce the number of places where the refactored code must interact with the existing code.
* This is an API breaking change that has x ramifications. (1) Based on the recently adopted semantic versioning scheme, we are required to increment the major version to 4.x.x when inducing an API breaking change. A subsequent, non-technical RFC will be published describing the rationale and anticipated impact. (2) While this change is backwards compatible, it is not forwards compatible. The 4.x.x library will be able to read ISIS3 compatible cubes, but the ISIS3 library will not be able to read cubes from the 4.x.x library natively.  Allowing both forward and backward compatibility will require a non-trivial amount of additional work without a known proportional benefit to the ISIS3 user community.

## Alternatives

* Reduced project scope by only factoring SpicePosition and SpiceRotation out of ISIS and into ALE
   * Benefits
      * Available proof of concept
      * Single library for generating exterior orientation
      * Better testing for exterior orientation generation
   * Lose
      * A significant step toward modularizing ISIS
      * Single library for SPICE selection, load/unloading and both interior/exterior orientation data exploitable outside of ISIS. 
      * Better testing for interior orientation generation along with spice data selection logic
      * Isolate SPICE to one location in an easier to develop language
* Reduced scope refactor followed by larger refactor
   * It is possible to break the work into smaller tasks that can be done over a longer period of time. In considering this option, we would rather not spend extra time from transitioning on/off the project, leaving ISIS3 in intermediate states.
* Retain status quo
   * ISIS3 technical debt remains the same and no community tool for exploiting ISIS3’s SPICE logic outside of ISIS will exist. 

## Unresolved Questions
                          
1) What will the new JSON format for SPICE data look like? The ALE ISD format isn’t fully actualized. 
2) How to handle kernel writers? An important part of ISIS3’s functionality is to write new adjusted kernels. This effort is out of scope for this RFC but to fully extract SPICE kernel IO from ISIS3, we need to talk about handling kernel writing in the future. 

## Future Possibilities

The potential exists for ALE to support other label file formats, namely: PDS4, FITS, GeoTiffs etc. An argument could also be made for supporting other types of ephemeris data formats like STK, OEM, etc. Supporting other formats of both ephemeris data and labels would make ALE more useful within the Planetary Science community.
Combining logic from ISIS3 ingestion applications with the new camera model drivers allows for the generation of ALE ISDs from PDS3/4 files instead of ISIS3 cubes. This functionality would allow for instantiating USGSCSM sensor models directly from PDS4 labels. Additional work in ISIS3 would be required to instantiate an ISIS sensor model directly from a PDS4 label, due to the coupling between ISIS3 cubes and ISIS3 sensor models.
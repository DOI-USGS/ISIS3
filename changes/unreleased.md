
As of ISIS 9.0.0...
These changes recorded in the dev changelog 
but not yet released when the dev team started 
using towncrier to manage the changelog. 

Each change summary should be manually added 
to the next release it is a part of.



## [Unreleased]


### Added
- Added Linux Arm support.
- Added support for Chandrayaan-2 TMC and OHRC cameras. [#5828](https://github.com/DOI-USGS/ISIS3/pull/5828)
- Added OFFBODY and OFFBODYTRIM parameters to noproj and cam2cam. Added tests and updated documentation. [#3602](https://github.com/DOI-USGS/ISIS3/issues/3602)
- Added use Bullet Anaconda feedstock library, which includes double precision support, and fix ISIS use of general Bullet configurations. [#5772](https://github.com/DOI-USGS/ISIS3/pull/5772)
- Added Mac Arm build support and adjusted the tests appropriately.
- Added adjusted XYZ point coordinate sigmas to the points.csv jigsaw output file. Modified
ctest FunctionalTestJigsawApollo to validate this output. [#5710](https://github.com/DOI-USGS/ISIS3/issues/5710)
- Added support for reading, writing, and viewing GeoTIFFs in ISIS. [#5618](https://github.com/DOI-USGS/ISIS3/pull/5618)
- Added GDAL SRS propagation for systems outside of ISIS to display projected GTiffs. [#5736](https://github.com/DOI-USGS/ISIS3/pull/5736)
- Added ale version to history blob. [#5207](https://github.com/DOI-USGS/ISIS3/issues/5207)
- Added CSM State output capability for jigsaw. [#5609](https://github.com/DOI-USGS/ISIS3/issues/5609)
- Added ShowDeprecated option to show or hide warnings in IsisPreferences. [#5611](https://github.com/DOI-USGS/ISIS3/issues/5611)
- Added SpiceQL integration to replace cspice calls. [#5545](https://github.com/DOI-USGS/ISIS3/pull/5545)
- Added initial `eisstitch` app [#5591](https://github.com/DOI-USGS/ISIS3/pull/5591)
- Added the ability to disable individual options for parameters in ISIS GUIs [#5849](https://github.com/DOI-USGS/ISIS3/pull/5849)
- Added PAD or SHRINK options to crop for crops that extend beyond the source image [#5843](https://github.com/DOI-USGS/ISIS3/pull/5843)
- Added std:: namespace for isinf, fixes build errors for some versions of c++
- Adds PROJ into ISIS, and exposes the capability with a new class called IProj. [#5317](https://github.com/DOI-USGS/ISIS3/pull/5317)
- Added `MATCHBANDBIN` option to himos and hicolormos. [#5859](https://github.com/DOI-USGS/ISIS3/issues/5859) and [#5860](https://github.com/DOI-USGS/ISIS3/issues/5860)
- Added serial number translation for ideal camera [#5662](https://github.com/DOI-USGS/ISIS3/issues/5662)

### Changed
- Removed Arm dependency on xalan-c, as it does not build for now on conda-forge. This requires turning off doc building on Arm. Also changed some variables to avoid name clashes on Arm with clang 16. [#5802](https://github.com/DOI-USGS/ISIS3/pull/5802)
- Update OSIRIS-REx OCams instrument (Map, Poly, & SamCam) support to current state in UofA code base [#5426](https://github.com/DOI-USGS/ISIS3/issues/5426)
- Enhanced csminit by removing the need to specify model and plugin [#5585](https://github.com/DOI-USGS/ISIS3/issues/5585)
- Changed file format to propagate from the input image format to the output image format [#5737](https://github.com/DOI-USGS/ISIS3/pull/5737)
- Changed `StripPolygonSeeder` and `GridPolygonSeeder` to seed individual polygons within each images multipolygon footprint [#5193](https://github.com/DOI-USGS/ISIS3/issues/5193)
- Pinned SpiceQL to 1.2.0 [#5852](https://github.com/DOI-USGS/ISIS3/pull/5852)
- Changed `ControlMeasure` object comparison to no longer factor in creation date for equality [#5862](https://github.com/DOI-USGS/ISIS3/pull/5862)
- In Application.cpp, converted initialization environmental variable shell commands and file read to c commands. [#5906](https://github.com/DOI-USGS/ISIS3/pull/5906)
- Improved the Conda Bullet CMAKE configuration in FindBulletFloat64.cmake. Adds the Bullet::Bullet_double target to ALLLIBS. [#5899](https://github.com/DOI-USGS/ISIS3/issues/5899)
- Changed 'jigsaw' attribute type to account for Linux compiler changes [#5904](https://github.com/DOI-USGS/ISIS3/pull/5904)
- Updated GDAL to 3.12 and QT to 6.X [#5909](https://github.com/DOI-USGS/ISIS3/pull/5909)
- Updated cmake configs to accommodate new cspice release [#5886](https://github.com/DOI-USGS/ISIS3/pull/5886)
- Changed `PVL` class to read data from GDAL metadata directly [#5824](https://github.com/DOI-USGS/ISIS3/pull/5824)

### Fixed
- Fixed Chandrayaan-2 TMC2 serial numbers by setting InstrumentId to CH2_TMC_FORE/NADIR/AFT based on the filename sensor [#5871](https://github.com/DOI-USGS/ISIS3/issues/5871)
- Fixed kaguyatc2isis invalid BandBin values [#5629](https://github.com/DOI-USGS/ISIS3/issues/5629)
- Fixed SpiceClient to handle redirect requests.
- Fixed jigsaw to default OUTADJUSTMENTH5 option to false and allow this feature to run on read-only images [#5700](https://github.com/DOI-USGS/ISIS3/issues/5700)
- Fixed Cube::fromIsd to add "LineScanTimes" table from HRSC isds [#5668](https://github.com/DOI-USGS/ISIS3/issues/5668)
- Fixed segfault in SpiceClient when an authentication error was encountered. [#5735](https://github.com/DOI-USGS/ISIS3/pull/5735)
- Fixed offset and scale not being applied to gdal 32 bit floating point pixels [#5753](https://github.com/DOI-USGS/ISIS3/pull/5753)
- Fixed `getLocalNormal` not reseting the sample and line [#5752](https://github.com/DOI-USGS/ISIS3/pull/5752)
- Fixed QView bug that would not allow manual editing of min/max values via the text fields and defaulted to the current min/max type dropdown selection [#5719](https://github.com/DOI-USGS/ISIS3/issues/5719)
- Fixed Juno Data Area SPKs causing Spiceinit to fail [#5724](https://github.com/DOI-USGS/ISIS3/issues/5724)
- Fixed embree shapemodel intersection calculation [#5592](https://github.com/DOI-USGS/ISIS3/issues/5592)
- Fixed findFeaturesSegment.py errors from issues [#5725](https://github.com/DOI-USGS/ISIS3/issues/5725) and [#5702](https://github.com/DOI-USGS/ISIS3/issues/5702)
- Fixed jigsaw save/apply bug by adding back missing metadata to Instrument Position/Pointing tables [#5701](https://github.com/DOI-USGS/ISIS3/issues/5701)
- Fixed `StripPolygonSeeder` and `GridPolygonSeeder` memory leaks [#5193](https://github.com/DOI-USGS/ISIS3/issues/5193)
- Fixed `pointreg` helper function to display deffile to application log [#5806](https://github.com/DOI-USGS/ISIS3/pull/5806)
- Fixed ISIS Q Applications crashing when opening more than one cube at once. [#5805](https://github.com/DOI-USGS/ISIS3/pull/5805)
- Fixed csminit docs with more details on the `ISD` and `STATE` pameters [#5790](https://github.com/DOI-USGS/ISIS3/issues/5790)
- Fixed SpiceQL 1.2.0 API changes [#5846](https://github.com/DOI-USGS/ISIS3/pull/5846)
- Fixed CSM State String blob not being propagated between input and output cubes [#5847](https://github.com/DOI-USGS/ISIS3/pull/5847)
- Fixed campt reporting when `ALLOWERROR` is set to true [#5845](https://github.com/DOI-USGS/ISIS3/pull/5845)
- Fixed order of observer and target in spiceql call in `ctxcal`. [#5823](https://github.com/DOI-USGS/ISIS3/pull/5823)
- Fixed bundle serialization on MacOS for IPCE [#5808](https://github.com/DOI-USGS/ISIS3/pull/5808)
- Fixed `noseam` to use correct temporary files when running [#5878](https://github.com/DOI-USGS/ISIS3/pull/5878)
- Fixed `CubeInfixToPostfix` to safely determine if a known symbol is a function symbol [#5822](https://github.com/DOI-USGS/ISIS3/pull/5822)
- Fixed `BundleObservationSolveSettings` to properly read empty solve setting xmls [#5822](https://github.com/DOI-USGS/ISIS3/pull/5822)
- Fixed `footprintinit` to find mapping group (no caps). [#5920](https://github.com/DOI-USGS/ISIS3/pull/5820)
- Fixed unnecessary un/distortion checks for pixels near the origin. [#5861](https://github.com/DOI-USGS/ISIS3/issues/5861)
- Fixed the isisStartup scripts for users outside of USGS systems. [#5478](https://github.com/DOI-USGS/ISIS3/issues/5478)
- Fixed all Q apps not consistently prioritized projction over camera in various tools [#5896](https://github.com/DOI-USGS/ISIS3/issue/5896)

### Removed
- QT 6 removed MySQL support so MySQL is no longer supported in `isisminer` [#5909](https://github.com/DOI-USGS/ISIS3/pull/5909)
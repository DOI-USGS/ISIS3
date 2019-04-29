# Overview

Running the [spiceinit](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) application with `attach=true` will attach all of the SPICE data required by the camera model directly to the Cube. There are 6 locations information is added to the Cube:

* The `Kernels` group
* The `NaifKeywords` group
* The `InstrumentPointing` Table
* The `InstrumentPosition` Table
* The `BodyRotation` Table
* The `SunPosition` Table

The following sections outline what is stored in each of these locations. The [spiceinit application documentation](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) shows what textual information is added to these locations.

# The Kernels Group

## Before spiceinit

The `Kernels` group is already a part of the Cube label after ingestion (it is nested under the `IsisCube` object). Usually, the only information it contains after ingestion is the `NaifFrameCode` keyword. The `NaifFrameCode` keyword contains the NAIF code for the instrument. So, information about the instrument will be stored as `INS{NaifFrameCode}_{Some property}` in the NAIF kernels. For example the `NaifFrameCode` for Kaguya Terrain Camera 2 is `-131371`. So, the focal length for Kaguya Terrain Camera 2 is stored as `INS-131371_FOCAL_LENGTH`. Usually, the NAIF ID for the instrument is also the NAIF ID for the sensor reference frame. If this is not the case, then there will be also be a `NaifCkCode` keyword in the `Kernels` group that contains the NAIF ID for the sensor reference frame.

## After spiceinit

All of the kernels used will be added to the `Kernels` group under the following keywords:

* `LeapSecond`
* `TargetAttitudeShape`
* `TargetPosition`
* `InstrumentPointing`
* `Instrument`
* `SpacecraftClock`
* `InstrumentPosition`
* `InstrumentAddendum`

If a shapemodel is is used, then that shapemodel will also be specified by the `ShapeModel` keyword. The `InstrumentPositionQuality` and `InstrumentPointingQuality` keywords will specify the quality of the kernels used for the `InstrumentPositon` and `InstrumentPointing` Tables.
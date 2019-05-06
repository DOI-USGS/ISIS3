# Overview

Running the [spiceinit](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) application with `attach=true` will attach all of the SPICE data required by the camera model directly to the Cube. There are 6 locations information is added to the Cube:

* [The Kernels Group](#The-Kernels-Group)
* [The NaifKeywords Object](#The-NaifKeywords-Object)
* [The InstrumentPointing Table](#The-InstrumentPointing-Table)
* [The InstrumentPosition Table](#The-InstrumentPosition-Table)
* [The BodyRotation Table](#The-BodyRotation-Table)
* [The SunPosition Table](#The-SunPosition-Table)

The following sections outline what is stored in each of these locations. The [spiceinit application documentation](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) shows what textual information is added to these locations.

# The Kernels Group

## Before spiceinit

The Kernels group is already a part of the Cube label after ingestion (it is nested under the `IsisCube` object). Usually, the only information it contains after ingestion is the `NaifFrameCode` keyword. The `NaifFrameCode` keyword contains the NAIF code for the instrument. So, information about the instrument will be stored as `INS{NaifFrameCode}_{Some property}` in the NAIF kernels. For example the `NaifFrameCode` for Kaguya Terrain Camera 2 is `-131371`. So, the focal length for Kaguya Terrain Camera 2 is stored as `INS-131371_FOCAL_LENGTH`. Usually, the NAIF ID for the instrument is also the NAIF ID for the sensor reference frame. If this is not the case, then there will be also be a `NaifCkCode` keyword in the `Kernels` group that contains the NAIF ID for the sensor reference frame. If using a DSK shapemodel, then the ray casting engine used will also be added to the Kernels group as the `RayTraceEngine` keyword. There are also `OnError` and `Tolerance` keywords added that determine behavior with the ray casting engine.

## After spiceinit

All of the kernels used will be added to the Kernels group under the following keywords:

* `LeapSecond`
* `TargetAttitudeShape`
* `TargetPosition`
* `InstrumentPointing`
* `Instrument`
* `SpacecraftClock`
* `InstrumentPosition`
* `InstrumentAddendum`

If a shapemodel is is used, then that shapemodel will also be specified by the `ShapeModel` keyword. The `InstrumentPositionQuality` and `InstrumentPointingQuality` keywords will specify the quality of the kernels used for the InstrumentPositon and InstrumentPointing Tables. Finally, the `CameraVersion` keyword specifies the version of the camera model that the Cube will now work with.

# The NaifKeywords Object

The NaifKeywords object is a PVL object that contains values queried from SPICE text kernels. It is not a part of the Cube prior to spiceinit being run. After spiceinit, the group is located at the very end of the Cube label.

This object contains all of the keywords and values collected from the [SPICE kernel pool](https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/kernel.html#Text%20Kernels%20and%20the%20Kernel%20Pool). Some examples what is usually contained in the NaifKeywords object are the target body radii, the instrument focal length, distortion model coefficients, and the transformations between image pixels and detector pixels.

The sensor model specifies what is contained in the `NaifKeywords` object. Keywords and values are added to the `NaifKeywords` object the following Spice class methods (methods in italics are protected):

* getDouble
* getInteger
* getString
* _readValue_
* _storeResult_
* _storeValue_

The sensor model and its maps use these methods to get any keyword values they need from the SPICE kernel pool. The keyword and its values are then stored in the `NaifKeywords` object as a keyword, value pair. For example, if getDouble was called with the NAIF keyword `INS-131371_FOCAL_LENGTH`, then `INS-131371_FOCAL_LENGTH` would be added to the NaifKeywords ovject as a PVL keyword whose values is the value returned by the NAIF SPICE ToolKit. Any future calls to access keyword values that have been stored use the `NaifKeywords` object instead of the SPICE kernel pool.

# ISIS Tables

The `InstrumentPointing` Table, `InstrumentPosition` Table, `BodyRotation` Table, and `SunPosition` Table are ISIS Tables. These are tables of binary information stored on the Cube file. Each Table consists of two parts; the label, and the binary data.

## ISIS Table Labels

The labels for all of the Tables in a Cube file are located in the Cube label after the `IsisCube` object. Each label is an object called `Table`, each Table is identified by the values of its `Name` keyword. So, the InstrumentPointing Table label is the `Table` object whose `Name` keyword has a value of `InstrumentPointing`. The Table label also contains information about where the binary data is stored in the `StartByte` and `Bytes` keywords. The format of the table is defined by the `Records`` and ByteOrder` keywords and the `Field` groups. The `Records` keyword specifies how many records, or rows, there are in the Table. Each `Field` group describes a field, or column, in the table. Each `Field` group contains a `Name` keyword that describes the field's name, a `Type` keyword that describes what type of binary data that field contains, and a `Size` keyword that describes how many pieces of binary data are stored in that field.

## ISIS Table Binary Data

The binary data for ISIS Tables are stored as contiguous blocks of binary data at the end of the Cube file. Each record, or row, in the Table is stored contiguously starting with the first record. Within a record, the values for each field, or column, are stored in the order that the `Field` groups are stored in the Table Label.

# The InstrumentPointing Table

The InstrumentPointing Table contains information needed to rotate from the J2000 reference frame to the sensor reference frame. It is not a part of the Cube prior to spiceinit being run. After spiceinit, the label for the Table is located after the `IsisCube` object in the Cube label and the binary table data is located at the end of the file.

## The InstrumentPointing Table Label

The InstrumentPointing Table's label contains all of the information described in [ISIS Table Labels](#ISIS-Table-Labels) along some additional keywords. The `TimeDependentFrames` and `ConstantFrames` keywords contain the NAIF codes of the reference frames that are rotated through. These keywords should be read right to left, so the start frame is last in the value array and the final frame is first in the value array. The `ConstantRotation` keyword contains a 3x3 rotation matrix that rotates from the last time dependent frame to the final frame, that is the frames in the `ConstantFrames` keyword. The rotations through the frames in the `TimeDependentFrames` are stored in the binary portion of the InstrumentPointing Table. The `CkTableStartTime` and `CkTableEndTime` keywords contain the valid time range for the rotation information in the InstrumentPointing Table. The `CkTableOriginalSize` keyword describes how many records were in the InstrumentPointing Table prior to any reduction steps. The `FrameTypeCode` keyword describes what type of NAIF SPICE C Kernel the rotation information came from. The `Kernels` keyword contains the SPICE Kernels the rotation information came from.

## The InstrumentPointing Table Binary Data

The InstrumentPointing Table's binary data contains the rotations from the starting frame (always J2000) to the last time dependent frame (usually the spacecraft). This data can be stored in two formats:

* A quaternion cache
* coefficients for Euler angles 

### Quaternion Cache

After spiceinit, the InstrumentPointing Table binary data is always a quaternion cache. Each record contains, a rotation quaternion, the time for that quaternion, and optionally the 3-element rotational velocity. The quaternions use [NAIF's format](ftp://naif.jpl.nasa.gov/pub/naif/misc/Quaternion_White_Paper/Quaternions_White_Paper.pdf) which is not the same as the standard quaternion format. NAIF stores the quaternion as (w, x, y, z). Most other tools and libraries store the quaternion as (x, y, z, w).

### Coefficients

During bundle adjustment (the jigsaw application mostly), the InstrumentPointing is converted to polynomials equations for Euler angles. After this point, the InstrumentPointing Table binary data contains coefficients, precisely stored time scaling vlaues, and the polynomial degree. The first record in the table contains the 0th degree coefficients, the second record in the table contains the 1st degree coefficients, and so on through the second to last record. The final record of the table contains the time scaling offset in the first field, the time scale in the second field, and the polynomial degree in the third field. ISIS uses the 3, 1, 3 axis order for Euler angles. The only exception to this is the appjit application uses 1, 2, 3 axis order.

# The InstrumentPosition Table

The InstrumentPosition Table contains the position and velocity of the sensor relative to the target body in the J2000 reference frame. It is not a part of the Cube prior to spiceinit being run. After spiceinit, the label for the Table is located after the `IsisCube` object in the Cube label and the binary table data is located at the end of the file.

## The InstrumentPosition Table Label

The InstrumentPosition Table's label contains all of the information described in [ISIS Table Labels](#ISIS-Table-Labels) along some additional keywords.The `SpkTableStartTime` and `SpkTableEndTime` keywords contain the valid time range for the information. The `SpkTableOriginalSize` keyword contains the original number of entries in the InstrumentPosition Table prior to any reduction techniques. The `CacheType` keyword describes what type of information is stored in the binary portion of the Table. The `Kernels` keyword contains the SPICE Kernels the information came from. The possible values of `CacheType` are `Linear`, `HermiteSpline`, and `PolyFunction`.


## The InstrumentPosition Table Binary Data

The InstrumentPosition Table's binary data contains the position and optionally the velocity of the sensor relative to the target body in the J2000 reference frame. The format of the Table depends on the value of the `CacheType` keyword in the Table label.

### Linear

Each record in the InstrumentPosition Table contains the sensor position and potentially velocity at a specific time. If there is a single record, that postion and velocity will be used for any time within the valid time range. If there are multiple records, then the position and velocity are linearly interpolated between times.

### HermiteSpline

Each record in the InstrumentPosition Table contains the sensor position and velocity at a specific time. Unlike the Linear case, velocity is required for every time. Position and velocity are interpolated from a hermite spline fit over all of the times.

### PolyFunction

During bundle adjustment (the jigsaw application mostly), the InstrumentPosition is converted to polynomials equations for the sensor position. After this point, the InstrumentPosition Table binary data contains coefficients, precisely stored time scaling vlaues, and the polynomial degree. The first record in the table contains the 0th degree coefficients, the second record in the table contains the 1st degree coefficients, and so on through the second to last record. The final record of the table contains the time scaling offset in the first field, the time scale in the second field, and the polynomial degree in the third field.

# The BodyRotation Table

The BodyRotation Table contains the rotation information between the J2000 reference frame and the target body reference frame. It is not a part of the Cube prior to spiceinit being run. After spiceinit, the label for the Table is located after the `IsisCube` object in the Cube label and the binary table data is located at the end of the file.

Depending on what type of PCK was used to create the BodyRotation table, it will have a different format.

## Binary PCK BodyRotation Table

When a binary PCK is used, the BodyRotation Table has the same format as the [InstrumentPointing Table](#The-InstrumentPointing-Table) except it has the rotation from J2000 to the target body reference frame and it always has a [quaternion cache](#Quaternion-Cache).


## Text PCK BodyRotation Table

When a text PCK is used, the BodyRotation Table contains coefficients for polynomials that define the rotation from the J2000 reference frame to the target body reference frame. Similar to the [InstrumentPointing Table Label](#The-InstrumentPointing-Table-Label), The BodyRotation Table label will have the `TimeDependentFrames`, `CkTableStartTime`, `CkTableEndTime`, `CkTableOriginalSize`, `FrameTypeCode`, and `Kernels` keywords. Unlike, the [InstrumentPointing Table Label](#The-InstrumentPointing-Table-Label), it will not have the `ConstantFrames` or `ConstantRotation` keywords because the rotation is handled by a single time dependent rotation, hence the `TimeDependentFrames` keyword will have only the target body reference frame ID and the J2000 reference frame ID (1) in it. The coefficients for the rotation polynomials are stored in the `PoleRa`, `PoleDec`, and `PrimeMeridian` keywords. These polynomials have a slighlty different format than other polynomials, see the [NAIF documentation](https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/pck.html#Models%20for%20the%20Sun,%20Planets,%20and%20some%20Minor%20Bodies%20in%20Text%20PCK%20Kernels) for a detailed description. There may also be `PoleRaNutPrec`, `PoleDecNutPrec`, `PmNutPrec`, `SysNutPrec0`, and/or `SysNutPrec1` keywords. These keywords contain the nutation precession angles, again see the [NAIF documentation](https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/pck.html#Models%20for%20Satellites%20in%20Text%20PCK%20Kernels) for how to use these values.

The polynomial coefficients for the rotation from J2000 to the target body reference frame are stored on the BodyrRotation Table label, but the binary data also contains a [quaternion cache](#Quaternion-Cache) derived from the rotation polynomials.

## Bundle Adjusted BodyRotation Table

TBD

# The SunPosition Table

The SunPosition Table contains the position of the Sun relative to the target body in the J2000 reference frame. The SunPosition Table has the same format as the [the InstrumentPosition Table](#The-InstrumentPosition-Table) except it always has a [linear cache](#Linear).
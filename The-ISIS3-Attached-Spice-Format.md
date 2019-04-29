# Overview

Running the [spiceinit](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) application with `attach=true` will attach all of the SPICE data required by the camera model directly to the Cube. There are 6 locations information is added to the Cube:

* The Kernels group
* The NaifKeywords group
* The InstrumentPointing Table
* The InstrumentPosition Table
* The BodyRotation Table
* The SunPosition Table

The following sections outline what is stored in each of these locations. The [spiceinit application documentation](https://isis.astrogeology.usgs.gov/Application/presentation/Tabbed/spiceinit/spiceinit.html) shows what textual information is added to these locations.

# The Kernels Group

## Before spiceinit

The Kernels group is already a part of the Cube label after ingestion (it is nested under the `IsisCube` object). Usually, the only information it contains after ingestion is the `NaifFrameCode` keyword. The `NaifFrameCode` keyword contains the NAIF code for the instrument. So, information about the instrument will be stored as `INS{NaifFrameCode}_{Some property}` in the NAIF kernels. For example the `NaifFrameCode` for Kaguya Terrain Camera 2 is `-131371`. So, the focal length for Kaguya Terrain Camera 2 is stored as `INS-131371_FOCAL_LENGTH`. Usually, the NAIF ID for the instrument is also the NAIF ID for the sensor reference frame. If this is not the case, then there will be also be a `NaifCkCode` keyword in the `Kernels` group that contains the NAIF ID for the sensor reference frame.

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

# The NaifKeywords Group

The NaifKeywords group is not a part of the Cube prior to spiceinit being run. After spiceinit, the group is located at the very end of the Cube label.

This group contains all of the keywords and values collected from the [SPICE kernel pool](https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/kernel.html#Text%20Kernels%20and%20the%20Kernel%20Pool). Some examples what is usually contained in the NaifKeywords group are the target body radii, the instrument focal length, distortion model coefficients, and the transformations between image pixels and detector pixels.

The sensor model specifies what is contained in the `NaifKeywords` group. Keywords and values are added to the `NaifKeywords` group by the following Spice class methods (methods in italics are protected):

* getDouble
* getInteger
* getString
* _readValue_
* _storeResult_
* _storeValue_

The sensor model and its maps use these methods to get any keyword values they need from the SPICE kernel pool. The keyword and its values are then stored in the `NaifKeywords` group as a keyword, value pair. For example, if getDouble was called with the NAIF keyword `INS-131371_FOCAL_LENGTH`, then `INS-131371_FOCAL_LENGTH` would be added to the NaifKeywords group as a PVL keyword whose values is the value returned by the NAIF SPICE ToolKit. Any future calls to access keyword values that have been stored use the `NaifKeywords` group instead of the SPICE kernel pool.

# ISIS Tables

The `InstrumentPointing` Table, `InstrumentPosition` Table, `BodyRotation` Table, and `SunPosition` Table are ISIS Tables. These are tables of binary information stored on the Cube file. Each Table consists of two parts; the label, and the binary data.

## ISIS Table Labels

The labels for all of the Tables in a Cube file are located in the Cube label after the `IsisCube` object. Each label is an object called `Table`, each Table is identified by the values of its `Name` keyword. So, the InstrumentPointing Table label is the `Table` object whose `Name` keyword has a value of `InstrumentPointing`. The Table label also contains information about where the binary data is stored in the `StartByte` and `Bytes` keywords. The format of the table is defined by the `Records`` and ByteOrder` keywords and the `Field` groups. The `Records` keyword specifies how many records, or rows, there are in the Table. Each `Field` group describes a field, or column, in the table. Each `Field` group contains a `Name` keyword that describes the field's name, a `Type` keyword that describes what type of binary data that field contains, and a `Size` keyword that describes how many pieces of binary data are stored in that field.

## ISIS Table Binary Data

The binary data for ISIS Tables are stored as contiguous blocks of binary data at the end of the Cube file. Each record, or row, in the Table is stored contiguously starting with the first record. Within a record, the values for each field, or column, are stored in the order that the `Field` groups are stored in the Table Label.

# The InstrumentPointing Table

The InstrumentPointing Table is not a part of the Cube prior to spiceinit being run. After spiceinit, the label for the Table is located after the `IsisCube` object in the Cube label and the binary table data is located at the end of the file.

The InstrumentPointing Table's label contains all of the information described in 
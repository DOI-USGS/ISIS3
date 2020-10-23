- Feature/Process Name: BLOB Redesign
- Start Date: October 23, 2020
- RFC PR: (empty until a PR is opened)
- Issue: (link or create and link and associated issue for discussion)
- Author: Jesse Mapel, Adam Paquette, & Austin Sanders

<!-- This is a comment block that is not visible. We provide some instructions in here. When submitting an RFC please copy this template into a new wiki page titled RFC#:Title, where the number is the next incrementing number. If you would like to submit an RFC, but are unable to edit the wiki, please open an issue and we will assist you in getting your RFC posted. Please fill in, to the largest extent possible, the template below describing your RFC. After that, be active on the associated issue and we can move the RFC through the process.-->

# Summary

Binary Large Objects (BLOBs) are how non-image binary data is attached to Cubes.
This includes ephemerides, footprints, processing history, and many other important
data sets. In the ISIS API, BLOBs are represented by a polymorphic class hierarchy
that inherits from the [Blob class](https://isis.astrogeology.usgs.gov/Object/Developer/class_isis_1_1_blob.html).
All Blob subclasses are responsible for two things: holding a specific type of data
in memory and reading that data from or writing it to a file. Some Blob subclasses
are also responsible for generating the data that they store. This RFC seeks to separate
these responsibilities. File I/O will be handled by the Blob class. Then, the classes
that currently inherit from the Blob class will no longer inherit from it and instead
pass their data to Blob object which can then perform file I/O. For the remainder
of this document, we will refer to these classes as the container classes. Finally,
the creation of data to be stored in the container classes will be moved into new
components.

The Blob class already neatly encapsulates performing file IO with generic binary
data. Unfortunately, accessing and storing data in a Blob object requires inheriting
from the Blob class and then only the code inside of that subclass can access it.
We propose changing this so that the Blob class allows direct access and
modification to the binary data that it stores. This way, Blob objects could be
used to generically perform binary data I/O on Cubes.

With the new Blob interface, the container classes can change from using inheritance
to using composition. This is a standard design paradigm described in the
[Composite Reuse Principle](http://www.cs.sjsu.edu/~pearce/cs251b/principles/crp.htm).
We propose that current container classes, and future container classes that need
to be serialized to Cubes, convert their internal data to and from raw binary, then
use Blob objects for writing and reading that data to and from files. This would
make the container classes just in memory contains for their data.

The ImagePolygon class is the most complex of the container classes. It is designed
to represent the footprint of an image, but it also contains the logic to calculate
image footprints. This logic adds nearly a thousand lines of code to the class and
makes it dependent upon several complex classes. We propose moving this code out
of the ImagePolygon class and into a separate class or function that uses the ImagePolygon
class. Currently, code that needs to read the image footprint from a Cube file becomes
transitively dependent upon the Camera class and the Projection class because ImagePolygon
is dependent upon them. By moving the code to generate image footprints out of ImagePolygon,
we could remove these transitive dependencies.

# Motivation

The redesign of how BLOBs are handled in ISIS is motivated by separation of responsibilities.
By more strictly enforcing the [single-responsibility principle](https://en.wikipedia.org/wiki/Single-responsibility_principle),
we can structurally decouple components into discrete modules. This new structure
will support components that can be independently maintained, distributed, and executed,
thereby imposing lower overhead on users who only require a subset of ISIS's functionality.
If this RFC is accepted, then users who need to work with BLOBs only have to depend
upon as much of ISIS as they require.

# Proposed Solution / Explanation

The Blob class is no longer used as a parent class, and is instead used within other
classes to read and write from and to Cube files. The Blob class also has methods
to replace its internal buffer with arbitrary binary data and directly access that
data. In the example below we have named these methods `Blob::SetData` and `Blob::Data`.

The following container classes have been modified:
  - GisBlob
  - History
  - ImagePolygon
  - OriginalLabel
  - OriginalXmlLabel
  - Stretch
  - Table

These container classes do not inherit from Blob and instead have a constructor that
takes a Blob and a method that returns a Blob containing all of the data required
to recreate themselves. In the example below we have named the method that returns
a Blob `toBlob`. Code that used to directly read these objects from a Cube file,
now instead reads a Blob from the Cube file and then creates the object from the Blob.
The example below demonstrates this for the OriginalLabel class.

```
// ImagePolygon.cpp

OriginalLabel::OriginalLabel(Blob blob) {
    stringstream os;
    char* dataBuffer = blob.Data();
    for(int i = 0; i < blob.Size(); i++) {
        os << dataBuffer [i];
    }
    os >> m_originalLabel;
}

Blob  OriginalLabel::toBlob() {
    Blob("IsisCube", "OriginalLabel");
    stringstream is;
    is << originalLabel;
    std::String dataString = is.str();
    Blob.SetData(dataString.data(), dataString.length());
    return blob;
}
```

```
// This is a short example program that copies the original label from one Cube
// to another using the new interface.

Cube inCube(“cube.cub”, "r");
Blob originalLabelBlob("IsisCube", "OriginalLabel");
inCube.read(originalLabelBlob);
inCube.close()
OriginalLabel originalLabel(originalLabelBlob);

Cube outCube(“new_cube.cub”, "w");
someCube.write(originalLabel.toBlob());
outCube.close();
```

The following classes have also been changed in more specific ways:
  - GisBlob
    - This class is removed and the ability to interact with image footprints via
    WKT strings is in the ImagePolygon class.
  - ImagePolygon
    - This class takes a geometry for its construction. The representation of that
    geometry is still being decided, but the current options are:
      - a vector of lat, lon pairs
      - a Geos MultiPolygon
      - a Geos CoordinateSequence
      - a WKT string
    - Creation of a Geos MultiPolygon for the footprint of an image is handled in
    a separate class or function.
  - Stretch
    - PR [#3972](https://github.com/USGS-Astrogeology/ISIS3/pull/3972) makes significant
    modifications to this class. Regardless of its outcome, Stretch and the clases
    that is uses will manage the conversion of a stretch into bytes and then pack
    that into a Blob similar to the other container classes.
  - Table
    - The Table class still has a label that holds additional metadata and is added
    to and read from the Blob label. The other container classes no longer have
    such a label.

# Drawbacks

The primary drawback to this work is the potential to introduce new bugs into the
existing software. If moving to a modular decoupled structure in the ISIS library
is not important to the project, then these changes are not worth the risk involved
in modifying highly coupled, stable software. We believe that this risk is worth
it, not only because the changes add new options for consumers who want to interact
with ISIS at the API level, but also because it reduces the risk for future modifications
by decoupling components of the software.

Additionally, the proposed changes reduce the responsibilities of the container
classes, so the existing regression tests will remain valid and sensitive to new
bugs. The ImagePolygon class is an exception, but the proposed constructors are
not complicated and will be easy to test.

These changes also add a small amount of extra code whenever the software writes
or reads a Blob from a Cube file. The current interface directly reads the container
classes from a Cube object. Under the proposed interface an extra line of code is
needed to construct the container object from the Blob object. This is a small additional
requirement and we do not believe it is a significant impact on the software.

# Alternatives

During the design process, we discussed two other options.

First, all of the container classes could still inherit from the Blob class. This
would require fewer changes in the code that uses them because the current interface
to read from and write to Cube files would be unchanged. We do not believe this
is the correct choice because the container classes are not inherently Blobs;
rather, they want the functionality provided by the Blob class to serialize and
deserialize themselves. This is fundamentally not an "is a" relationship, but rather
a "has a" or "uses a" relationship. Thus, it is more appropriate for the container
classes to use the Blob class in their interface.

Second, we could create intermediate subclasses of Blob that work with basic data
types. For example, we could have subclasses of Blob that work with strings, Pvl,
Geometries, and tabular data. These classes would encapsulate conversion to and from
binary for these data types and would be used by the container classes to convert
to and from binary in a reusable fashion. We decided not to do this and instead
create functions that can be reused. There is nothing inherently stateful about
these data conversions and thus they do not need to be bound to classes.

# Unresolved Questions

The largest remaining question is the status of the Stretch class and its associated
classes. The outstanding PR makes several changes and it is highly likely that we
will have to make further changes to those same files regardless of its outcome.
We do not believe that it poses a risk to this RFC and is already moving in the
direction of this work.

This RFC address the structure of the code, but not the structure of how the code
is built and consumed. There are many tools in CMake that help support a modular
architecture that would be beneficial. These changes are out of scope for this RFC,
we expect to submit another RFC dealing with them.

# Future Possibilities

This RFC is a first step towards defining a core library for ISIS. Right now,
the ISIS library consists of all of the classes in the base, database, control,
qisis, and system directories. This is everything from PVL objects to the bundle
adjustment. In order to disentangle all of these classes and form a modular structure,
we will need to do work like what is proposed in this RFC across the library. Currently,
the first major module that we would like to see is the Cube class and all of its
appropriate dependencies, of which Blob is a member. We are currently working on
another RFC to address the broader scope of what such a module could look like.

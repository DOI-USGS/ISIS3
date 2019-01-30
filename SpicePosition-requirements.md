# What is this document?
This document outlines the critical components of the SpicePosition class. This will help guide our refactor efforts by defining the limitations imposed by the rest of the ISIS code base.

# Compute Position and Velocity
This is the core functionality of SpicePosition. It is a part of the stateful ISIS camera model so when the position and velocity are computed at a time, all three are stored internally. Then, there are three accessors for the stored values.

## Data sources
SpicePosition needs to be able to use several different data sources.

### SPIKE Kernels
SpicePosition is the part of ISIS that reads from SPKs. 99% of the time this only happens during spiceinit because it is immediately cached and saved to the cube file.

### Position and Velocity Cache
This is the data source most often used by ISIS. The data is interpolated by either a linear method or a cubic hermite spline.

### Polynomials
This is the data source used during and after bundle adjustment.

### Combination of Cache and Polynomial
This is used for bundle adjustment of very jittery images. Mechanically, this is the sum of a low degree polynomial and a cubic hermite spline cache.
## Relevant public methods
* SetEphemerisTime(double et)
* EphemerisTime()
* Coordinate()
* Velocity()

# Create a Cache from SPICE Data for an Image
This is both a big part of regular ISIS usage and how the spice server works. The spice server creates the cache and then ships it back over the wire.

## Reducing Cache Size
For large push broom images, a data point for every line can be a huge amount of data to store and work with. So, SpicePosition must able to reduce the cache size based on a given tolerance.
## Relevant public methods
* LoadCache(double startTime, double endTime, int size)
* LoadCache(double time)
* ReloadCache()
* Memcache2HermiteCache(double tolerance)

# Write and Read Polynomial or Cached Data From/To a Cube
Currently this is done via an ISIS Table object, but it could be a generic BLOB.
## Relevant public methods
* LoadCache(Table &table)
* ReloadCache(Table &table)
* Cache(const QString &tableName)
* LineCache(const QString &tableName)
* LoadHermiteCache(const QString &tableName)

# Fit Polynomial Coefficients over Cached Data
This is how the polynomial coefficients are initialized at the beginning of bundle adjustment.
## Relevant public methods
* SetPolynomial(const Source type)

# Provide Access to Polynomial Coefficients
The bundle adjustment needs access to these values in order to properly construct the normal equations.
## Relevant public methods
* GetPolynomial(std::vector<double>& XC, std::vector<double>& YC, std::vector<double>& ZC)

# Take Adjustments to Polynomial Coefficients
This is how the BundleAdjust applies its corrects each iteration.
## Relevant public methods
* SetPolynomial(const std::vector<double>& XC, const std::vector<double>& YC, const std::vector<double>& ZC, const Source type)

# Compute the Partial Derivatives with Respect to a Polynomial Coefficient
Mathematically these are something like d/db(at^2 + bt + c). These are needed for the bundle adjustment.
## Relevant public methods
* CoordinatePartial(SpicePosition::PartialType partialVar, int coeffIndex)
* VelocityPartial(SpicePosition::PartialType partialVar, int coeffIndex)
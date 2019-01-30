# What is this?
This document outlines the critical components of the SpicePosition class. This doesn't necessarily need to be a 

# Computing position and velocity
This is the core functionality of SpicePosition. It is a part of the stateful ISIS camera model so when the position and velocity are computed at a time, all three are stored internally. Then, there are three accessors for the stored values. The relevant methods are:
* const std::vector<double> &SetEphemerisTime(double et);
* double EphemerisTime() const
* const std::vector<double> &Coordinate()
* const std::vector<double> &Velocity()

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

# Create a Cache from SPICE Data for an Image
This is both a big part of regular ISIS usage and how the spice server works. The spice server creates the cache and then ships it back over the wire.

## Reducing Cache Size
For large push broom images, a data point for every line can be a huge amount of data to store and work with. So, SpicePosition must able to reduce the cache size based on a given tolerance.

# Write and Read Polynomial or Cached Data From/To a Cube
Currently this is done via an ISIS Table object, but it could be a generic BLOB.

# Fit Polynomial Coefficients over Cached Data
This is how the polynomial coefficients are initialized at the beginning of bundle adjustment.

# Take Adjustments to Polynomial Coefficients
This is how the BundleAdjust applies its corrects each iteration.

# Compute the Partial Derivatives with Respect to a Polynomial Coefficient
Mathematically these are something like d/db(at^2 + bt + c). These are needed for the bundle adjustment.
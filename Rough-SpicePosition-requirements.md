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
This is the data source used during and after Bundle Adjustment.

### Combination of Cache and Polynomial
This is used for bundle adjustment of very jittery images. Mechanically, this is the sum of a low degree polynomial and a cubic hermite spline cache.
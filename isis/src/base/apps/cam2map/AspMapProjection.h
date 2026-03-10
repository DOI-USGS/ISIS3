#ifndef AspMapProjection_h
#define AspMapProjection_h

namespace Isis {

class Cube;
class UserInterface;

namespace asp {

// Run ASP-compatible per-pixel exact map projection. Computes camera
// footprint bbox and GSD from the DEM, creates the output cube, snaps
// the grid, and does the per-pixel projection. Only the camera model
// comes from ISIS. DEM and input image are read via GDAL.
// Interpolation uses VW bicubic.
void mapproject(Cube *inCube, const UserInterface &ui);

}} // namespace Isis::asp

#endif

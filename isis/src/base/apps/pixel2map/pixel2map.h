#ifndef pixel2map_h
#define pixel2map_h

#include <QDebug>
#include <QList>
#include <QPointF>
#include <QString>

#include "Camera.h"
#include "Cube.h"
#include "FileList.h"
#include "PixelFOV.h"
#include "ProcessByBrick.h"
#include "ProcessGroundPolygons.h"
#include "Pvl.h"
#include "Target.h"

#include "Transform.h"

#include "geos/geom/Geometry.h"
#include "geos/io/WKTWriter.h"

#include "UserInterface.h"



extern void pixel2map(UserInterface &ui, Pvl *log=nullptr);
  

/**
 * @author 2008-02-13 Stacy Alley
 *
 * @internal 
 *   @history 2013-07-30 Stuart C. Sides & Tracie Sucharski Renamed from vim2map
 */

  /**
class pixel2mapForward : public Isis::Transform {
  private:
    Isis::Camera *p_incam;
    Isis::Projection *p_outmap;

  public:
    // constructor
    pixel2mapForward(const int inputSamples, const int inputLines, Isis::Camera *incam,
             const int outputSamples, const int outputLines, Isis::Projection *outmap,
             bool trim);

    // destructor
    ~pixel2mapForward() {};


};
**/

} // namespace Isis

#endif

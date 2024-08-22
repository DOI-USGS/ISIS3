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
//#include "ProcessRubberSheet.h"
//#include "ProjectionFactory.h"
#include "Pvl.h"
//#include "PvlGroup.h"
#include "Target.h"

#include "Transform.h"

#include "geos/geom/Geometry.h"
#include "geos/io/WKTWriter.h"

#include "UserInterface.h"

void PrintMap();
//void rasterizePixel(Isis::Buffer &in);
//void vectorizePixel(Isis::Buffer &in);

//std::map <QString, void *> GuiHelpers() {
//  map <QString, void *> helper;
//  helper ["PrintMap"] = (void *) PrintMap;
//  return helper;
//}




// Global variables
Isis::ProcessGroundPolygons g_processGroundPolygons;
Isis::Camera *g_incam;
int g_numIFOVs = 0;
int g_vectorOut = 0;  // to be set bool in future

int csamples;
int clines;

QString vectOut;
QString outvect;

std::ofstream fout_csv;
QString ogc_SRS;


namespace Isis {
  extern void pixel2map(UserInterface &ui);	
  extern void pixel2map(Cube* incube, UserInterface &ui);
  //extern void pixel2map(UserInterface &ui);	
  //extern void pixel2map(UserInterface &ui, Pvl *log=nullptr);
  //extern void pixel2map(Cube *incube, Pvl &userMap);
  
  //static void rasterizePixel(Isis::Buffer &in);
  //static void vectorizePixel(Isis::Buffer &in);	



/**
 * @author 2008-02-13 Stacy Alley
 *
 * @internal 
 *   @history 2013-07-30 Stuart C. Sides & Tracie Sucharski Renamed from vim2map
 */
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

} // namespace Isis

#endif

#ifndef ProcessPolygons_h
#define ProcessPolygons_h

#include "ProjectionFactory.h"
#include "Process.h"
#include "Brick.h"
#include "Filename.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Polygon.h"

namespace Isis {
                   
  /**
   * 
   * @author 2007-12-07 Stacy Alley  
   * 
   * @internal
   *   @history 2010-02-26 Steven Lambright - Now using a geometry snapper on
   *             points before testing if they are inside a polygon inside
   *             DoWork(...).
   */
  class ProcessPolygons : public Isis::Process {

    public:
      ProcessPolygons();

      void SetOutputCube (const std::string &parameter, const int nsamps,
                          const int nlines, int nbands = 1);

      void SetOutputCube(const std::string &avgFilename, const std::string 
                         &countFilename, Isis::CubeAttributeOutput &atts, 
                         const int nsamps, const int nlines, int nbands = 1);
                            
      Isis::Cube* AppendOutputCube(const std::string &avgFilename, 
                            const std::string &countFilename="");

      void Rasterize (std::vector<double> &samples, 
                      std::vector<double> &lines, 
                      std::vector<double> &values);

      void Rasterize (std::vector<double> &samples, 
                      std::vector<double> &lines, 
                     int &band, double &value);


      void EndProcess();

  private:
    void DoWork(int Flag);
    void FillPolygon(int Flag);
    void GetPolygonCoords();
    std::vector<double> p_samples, p_lines, p_values;
    std::vector<geos::geom::Coordinate> p_polygonCoordinates;
    double p_value;
    int p_band;
    Brick *p_brick1;
    Brick *p_brick2;
    geos::geom::Polygon *p_imagePoly;


  };

};

#endif

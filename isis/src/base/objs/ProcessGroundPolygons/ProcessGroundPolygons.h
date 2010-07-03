#ifndef ProcessGroundPolygons_h
#define ProcessGroundPolygons_h

#include "ProjectionFactory.h"
#include "Process.h"
#include "Brick.h"
#include "Filename.h"
#include "ProcessPolygons.h"
#include "UniversalGroundMap.h"

namespace Isis {
  /**                                                                       
  * @brief Process cube polygons to map or camera projections     
  *                                                                        
  * This class allows a programmer to develop a program which              
  * @ingroup HighLevelCubeIO                                                  
  *                                                                        
  * @author  2008-12-14 Stacy Alley                               
  *                                                                                                                                                                                      
  * @internal
  *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo      
  *  @history 2008-08-18 Steven Lambright - Updated to work with geos3.0.0
  *           instead of geos2. Mostly namespace changes.     
  */                                                   
  class ProcessGroundPolygons : public ProcessPolygons {
    public:
      ProcessGroundPolygons();

      //Cube is an existing camera cube or projection cube
      void SetOutputCube (const std::string &parameter, std::string &cube);

      //Determine cube size from the projection map
      void SetOutputCube (const std::string &parameter,Isis::Pvl &map, int bands);

      void SetOutputCube(const std::string &avgFilename, const std::string 
                         &countFilename, Isis::CubeAttributeOutput &atts, 
                         std::string &cube);

      void SetOutputCube(const std::string &avgFilename, const std::string 
                         &countFilename, Isis::CubeAttributeOutput &atts, 
                         Isis::Pvl &map, int bands);
                            
      void AppendOutputCube(std::string &cube, const std::string &avgFilename, 
                            const std::string &countFilename="");

      void Rasterize (std::vector<double> &lat, 
                      std::vector<double> &lon, 
                      std::vector<double> &values);

      void Rasterize (std::vector<double> &lat, 
                      std::vector<double> &lon, 
                      int &band, double &value);

      void Rasterize (double &lat, double &lon,int &band, double &value);

      void EndProcess();
      UniversalGroundMap *GetUniversalGroundMap(){ return p_groundMap;};

    private:
      void Convert(std::vector<double> &lat, std::vector<double> &lon);
      void Convert(double &lat, double &lon);
      UniversalGroundMap *p_groundMap;
      std::vector<double> p_samples, p_lines;

  };

};

#endif

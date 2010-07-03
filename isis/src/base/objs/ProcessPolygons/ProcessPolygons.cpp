
#include "ProcessPolygons.h"
#include "PolygonTools.h"
#include "Application.h"
#include "SpecialPixel.h"

#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/Point.h"
#include "geos/util/IllegalArgumentException.h"
#include "geos/operation/overlay/snap/GeometrySnapper.h"

using namespace std;
namespace Isis {

  ProcessPolygons::ProcessPolygons () {

  }
 
  /** 
   * 
   * 
   * @param samples
   * @param lines
   * @param values
   */
  void ProcessPolygons::Rasterize (std::vector<double> &samples, 
                                   std::vector<double> &lines, 
                                   std::vector<double> &values) {
    p_samples = samples;
    p_lines = lines;
    p_values = values;
    DoWork(0);
    //FillPolygon(0);
   
  }

  /** 
   * 
   * 
   * @param samples
   * @param lines
   * @param band
   * @param value
   */
  void ProcessPolygons::Rasterize (std::vector<double> &samples, 
                                   std::vector<double> &lines, 
                                   int &band, double &value){

    p_samples = samples;
    p_lines = lines;
    p_band = band;
    p_value = value;

    /*Make sure we only loop thru one time since we only have one band.*/
    p_values.clear();
    p_values.push_back(1.0);
    DoWork(1);
    //FillPolygon(1);
    

  }

  

  void ProcessPolygons::FillPolygon(int Flag){

    geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();        
    for (unsigned int i = 0; i < p_samples.size(); i++ ) {
      pts->add(geos::geom::Coordinate(p_samples[i], p_lines[i]));
    }/*end for*/
    /*Add the first point again in order to make a closed line string*/
    pts->add(geos::geom::Coordinate(p_samples[0], p_lines[0]));

    try {
      geos::geom::Polygon *poly = Isis::globalFactory.createPolygon(
                                  globalFactory.createLinearRing(pts),NULL);

      /*If there is not an intersecting polygon, there is no reason to go on.*/
      if (!poly->intersects(p_imagePoly))return;

      geos::geom::Polygon *intersectPoly = ((geos::geom::Polygon*)p_imagePoly->intersection(poly));    
      const geos::geom::Envelope *envelope = intersectPoly->getEnvelopeInternal();

      for (double y = floor(envelope->getMinY()); y <= ceil(envelope->getMaxY()); y++ ) {
        /*create a horizontal line that runs across the entire evelope.*/
        geos::geom::CoordinateSequence *linePts = new geos::geom::CoordinateArraySequence();
        linePts->add(geos::geom::Coordinate(floor(envelope->getMinX()), y));
        linePts->add(geos::geom::Coordinate(floor(envelope->getMaxX()), y));

        geos::geom::LineString *line = Isis::globalFactory.createLineString(linePts);

        /*intersect the line with the polygon*/
        geos::geom::Geometry *intersects = poly->intersection(line);

        /*find out all the points were the line intersects the polygon*/
        geos::geom::CoordinateSequence *intersectCoords = intersects->getCoordinates();

        for (unsigned int l = 0; l < intersectCoords->getSize()-1; l++ ) {
          
          /*now i want to go from coord 1 - coord 2 and do work....*/
          for (int x = (int) intersectCoords->getAt(l*2).x; x < (int)intersectCoords->getAt(l*2+1).x; x++ ) {

            for (unsigned int i = 0; i < p_values.size(); i++ ) {  /* for each band */

               /*write the count file*/
              if (Flag == 0) {
                p_brick2->SetBasePosition((int)(x+0.5), (int) (y+0.5), i+1); 
              }
              if (Flag == 1) {
                p_brick2->SetBasePosition((int)(x+0.5), (int) (y+0.5), p_band);
              }

              this->OutputCubes[1]->Read(*p_brick2);
              double previousPixelCount = (*p_brick2)[0];


              if ((*p_brick2)[0] != Isis::Null) {
                (*p_brick2)[0] += 1;
              } else {
                (*p_brick2)[0] = 1;
              }

              this->OutputCubes[1]->Write(*p_brick2);
              double currentCount = (*p_brick2)[0];


              /*write the average file*/
              if (Flag == 0) {
                p_brick1->SetBasePosition((int)(x+0.5), (int)(y+0.5), i+1); 
              }
              if (Flag == 1) {
                p_brick1->SetBasePosition((int)(x+0.5), (int)(y+0.5), p_band);
              }
              //We need to think about how to handle special pixels in p_values also if
              //the read-in value is a special pixel.
              this->OutputCubes[0]->Read(*p_brick1);
              double previousPixelValue = (*p_brick1)[0];
              if ((*p_brick1)[0] == Isis::Null) {
                if (Flag == 0) {
                  (*p_brick1)[0] = p_values[i];
                }
                if (Flag == 1) {
                  (*p_brick1)[0] = p_value; 
                }
              } else {
                /*Calculate the running average.*/
                double avg = 0;
                if (Flag == 0) {
                  avg = (previousPixelCount * previousPixelValue + p_values[i])
                        /currentCount;
                }
                if (Flag == 1) {
                  avg = (previousPixelCount * previousPixelValue + p_value)
                        /currentCount;
                }
                (*p_brick1)[0] = avg;

              }

              /*The new average value is written to the output cube.*/
              this->OutputCubes[0]->Write(*p_brick1);
            }/*End for each band*/

          } /*End for x*/
        }
        delete linePts;
      }/*End for y*/

      delete poly;
      delete intersectPoly;
      

    } /*end try*/

    catch (geos::util::IllegalArgumentException *ill) {
      std::string msg = "ERROR! geos exception 1 [";
      msg += (iString)ill->what() + "]";
      delete ill;
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }/*end catch*/


  }

  /** 
   * This method does the actuall reading and writing to the cube 
   * file.  The Flag parameter is there to help out where the two 
   * Rasterize method need to behave differently during this 
   * operation.  Most notibly, when we set the position of the 
   * bricks and when we are calculating the average using the 
   * given value or values. 
   * 
   * @param Flag
   */
  void ProcessPolygons::DoWork(int Flag){

    geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();        
    for (unsigned int i = 0; i<p_samples.size(); i++) {
      pts->add(geos::geom::Coordinate(p_samples[i], p_lines[i]));
    }/*end for*/
    /*Add the first point again in order to make a closed line string*/
    pts->add(geos::geom::Coordinate(p_samples[0], p_lines[0]));

    try {
      geos::geom::Polygon *poly = Isis::globalFactory.createPolygon(
                                  globalFactory.createLinearRing(pts),NULL);

      /*If there is not an intersecting polygon, there is no reason to go on.*/
      if(!poly->intersects(p_imagePoly))return;

      geos::geom::Polygon *intersectPoly = ((geos::geom::Polygon*)p_imagePoly->intersection(poly));   
      const geos::geom::Envelope *envelope = intersectPoly->getEnvelopeInternal();

      geos::operation::overlay::snap::GeometrySnapper snap(*intersectPoly);

      /*go thru each coord. in the envelope and ask if it is within the polygon*/
      for (double x = floor(envelope->getMinX()); x <= ceil(envelope->getMaxX()); x++) {
        if(x == 0 ) continue;

        for (double y = floor(envelope->getMinY()); y <= ceil(envelope->getMaxY()); y++) {
          if(y == 0 ) continue;

          geos::geom::Coordinate c(x,y);
          geos::geom::Point *p = Isis::globalFactory.createPoint(c);
          geos::geom::Geometry *pSnapped = snap.snapTo(*p, 1.0e-10)->clone();

          bool contains = pSnapped->within(intersectPoly);

          delete p;
          delete pSnapped;

          if (contains) {
            /*write the count file*/
            for (unsigned int i = 0; i<p_values.size(); i++) {
              if(Flag == 0) {
                p_brick2->SetBasePosition((int)(x+0.5), (int) (y+0.5), i+1); 
                
              }
              if(Flag == 1) {
                p_brick2->SetBasePosition((int)(x+0.5), (int) (y+0.5), p_band);
              }
        
              this->OutputCubes[1]->Read(*p_brick2);
              double previousPixelCount = (*p_brick2)[0];  

              if ((*p_brick2)[0] != Isis::Null) {
                (*p_brick2)[0] += 1;
              } else {
                (*p_brick2)[0] = 1;
              }
              
              this->OutputCubes[1]->Write(*p_brick2);
              double currentCount = (*p_brick2)[0];


              /*write the average band*/
              if(Flag == 0) {
                p_brick1->SetBasePosition((int)(x+0.5), (int)(y+0.5), i+1); 
              }
              if(Flag == 1) {
                p_brick1->SetBasePosition((int)(x+0.5), (int)(y+0.5), p_band);
              }
              //We need to think about how to handle special pixels in p_values also if
              //the read-in value is a special pixel.
              this->OutputCubes[0]->Read(*p_brick1);
              double previousPixelValue = (*p_brick1)[0];
              if ((*p_brick1)[0] == Isis::Null) {
                if(Flag == 0) {
                  (*p_brick1)[0] = p_values[i];
                }
                if(Flag == 1) {
                  (*p_brick1)[0] = p_value; 
                }
              } else {
                /*Calculate the running average.*/
                double avg = 0;
                if(Flag == 0) {                  
                  avg = (previousPixelCount*previousPixelValue+p_values[i])
                               /currentCount;
                }
                if(Flag == 1) {
                  avg = (previousPixelCount*previousPixelValue+p_value)
                               /currentCount;
                }
                (*p_brick1)[0] = avg;
                
              }

              /*The new average value is written to the output cube.*/
              this->OutputCubes[0]->Write(*p_brick1);
            }/*End for each band*/

          }/*End if (contains)*/

        } /*End for y*/

      }/*End for x*/

      delete poly;
      delete intersectPoly;
      
    } /*end try*/

    catch (geos::util::IllegalArgumentException *ill) {
      std::string msg = "ERROR! geos exception 1 [";
      msg += (iString)ill->what() + "]";
      delete ill;
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }/*end catch*/
    
  }

  /** 
   * 
   * 
   */
  void ProcessPolygons::EndProcess () {
    delete p_imagePoly;
    delete p_brick1;
    delete p_brick2;
    Process::EndProcess();
  
  }

  /** 
   * This gives the option to append to the cube  
   * 
   * @param avgFilename
   * @param countFilename
   * 
   * @return Isis::Cube*
   */
  Isis::Cube* ProcessPolygons::AppendOutputCube(const std::string &avgFilename, 
                                         const std::string &countFilename){

    Filename *file = new Filename(avgFilename);
    std::string path = file->Path();
    std::string filename = file->Basename();
    std::string extension = file->Extension();

    /*Open the average file with read/write permission*/
    Cube *averageCube = new Cube();
    averageCube->Open(avgFilename,"rw");
    OutputCubes.push_back(averageCube);

    /*Now open the count file with read/write permission*/
    Cube *countCube = new Cube();

    if(countFilename == "") {
      /*if the countFilename was set to nothing, then we use the default count 
      file name.*/
      std::string openFile = path + "/" + filename + "-count-." + extension;
      countCube->Open(openFile, "rw");
      
    } else {
      countCube->Open(countFilename, "rw");
    }

    OutputCubes.push_back(countCube); 
    return countCube; 
  }
  
  /** 
   * 
   * 
   * @param avgFilename
   * @param countFilename
   * @param nsamps
   * @param nlines
   * @param nbands
   */
  void ProcessPolygons::SetOutputCube (const std::string &avgFilename, const 
                                       std::string &countFilename, 
                                       Isis::CubeAttributeOutput &atts, 
                                       const int nsamps,const int nlines, 
                                       const int nbands) {

    this->Process::SetOutputCube(avgFilename, atts, nsamps, nlines, nbands);
    this->Process::SetOutputCube(countFilename, atts, nsamps, nlines, nbands);

    geos::geom::CoordinateArraySequence imagePts;
  
    imagePts.add(geos::geom::Coordinate(0.0, 0.0));
    imagePts.add(geos::geom::Coordinate(0.0, this->OutputCubes[0]->Lines()));
    imagePts.add(geos::geom::Coordinate(this->OutputCubes[0]->Samples(), 
                                   this->OutputCubes[0]->Lines()));
    imagePts.add(geos::geom::Coordinate(this->OutputCubes[0]->Samples(), 0.0));
    imagePts.add(geos::geom::Coordinate(0.0, 0.0));

    p_imagePoly = Isis::globalFactory.createPolygon(
                                  globalFactory.createLinearRing(imagePts),NULL);

    p_brick1 = new Brick(*this->OutputCubes[0], 1, 1, nbands);
    p_brick2 = new Brick(*this->OutputCubes[1], 1, 1, nbands);
  }

  /** 
   * 
   * 
   * @param parameter
   * @param nsamps
   * @param nlines
   * @param nbands
   */
  void ProcessPolygons::SetOutputCube (const std::string &parameter, 
                                       const int nsamps,const int nlines, 
                                       const int nbands) {

    std::string avgString = 
    Application::GetUserInterface().GetFilename(parameter);

    Isis::CubeAttributeOutput atts = 
    Application::GetUserInterface().GetOutputAttribute(parameter);

    Filename *file = new Filename(avgString);
    std::string path = file->Path();
    std::string filename = file->Basename();
    std::string countString = path + "/" + filename + "-count";
    SetOutputCube(avgString, countString, atts, nsamps, nlines, nbands);

  }

 
} /* end namespace isis*/


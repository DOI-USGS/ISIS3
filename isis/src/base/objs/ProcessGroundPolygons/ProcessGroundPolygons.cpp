#include "ProcessGroundPolygons.h"

#include <geos/geosAlgorithm.h>

#include "Application.h"
#include "PolygonTools.h"
#include "Projection.h"

using namespace std;
namespace Isis {

  ProcessGroundPolygons::ProcessGroundPolygons() {
    p_groundMap = NULL;
  }

  /**
   * This method gets called from the application with the lat/lon
   * vertices of a polygon along with a vector of values.
   * The location of the values with in the verctor indicate which
   * band that value gets written to
   *
   * @param lat
   * @param lon
   * @param values
   */
  void ProcessGroundPolygons::Rasterize(std::vector<double> &lat,
                                        std::vector<double> &lon,
                                        std::vector<double> &values) {
    Convert(lat, lon);
    ProcessPolygons::Rasterize(p_samples, p_lines, values);

  }

  /**
   * This method gets called from the application with the lat/lon
   * verticies of a polygon along with the band number and the
   * value for the polygon.
   *
   * @param lat
   * @param lon
   * @param band
   * @param value
   */
  void ProcessGroundPolygons::Rasterize(std::vector<double> &lat,
                                        std::vector<double> &lon,
                                        int &band, double &value) {

    Convert(lat, lon);
    ProcessPolygons::Rasterize(p_samples, p_lines, band, value);

  }

  void ProcessGroundPolygons::Rasterize(double &lat, double &lon,
                                        int &band, double &value) {

    Convert(lat, lon);
    ProcessPolygons::Rasterize(p_samples, p_lines, band, value);

  }

  /**
   * Converts lat/long to line/sample using the universal ground
   * map object.
   *
   * @param lat
   * @param lon
   */
  void ProcessGroundPolygons::Convert(std::vector<double> &lat,
                                      std::vector<double> &lon) {

    p_samples.clear();
    p_lines.clear();
    double sample = 1;
    double line = 1;
    //double midSample, midLine;
    for(unsigned int i = 0; i < lat.size(); i++) {
      //std::cout << "ProcessGroundPolygons::Convert i = " << i << std::endl;
      if(p_groundMap->SetUniversalGround(lat[i], lon[i])) {

        sample = p_groundMap->Sample();
        line = p_groundMap->Line();
        p_samples.push_back(sample);
        p_lines.push_back(line);
        //std::cout << "sample = " << sample << " Line = " << line << std::endl;

        /*if(i>0) {
          std::cout << "Calculated:: midSample = " << (p_samples[i]+p_samples[i-1])/2 <<
                                    " midLine = " << (p_lines[i]+p_lines[i-1])/2<< std::endl;

          //get the actual numbers
          p_groundMap->SetUniversalGround(lat[i], (lon[i-1]+lon[i])/2);
          midSample = p_groundMap->Sample();
          p_groundMap->SetUniversalGround((lat[i-1]+lat[i])/2, lon[i]);
          midLine = p_groundMap->Line();

          std::cout << "Actual:: midSample = " << midSample << " midLine = " << midLine << "\n\n" <<std::endl;
        }*/

      }
      else {
        p_samples.push_back(sample);
        p_lines.push_back(line);

      }

    }/* end for loop*/

  }

  void ProcessGroundPolygons::Convert(double &lat, double &lon) {

    p_samples.clear();
    p_lines.clear();
    double sample = 1;
    double line = 1;
    //std::cout << "\n\nProcessGroundPolygons::Convert " << std::endl;
    //std::cout << "lat = " << lat << " Lon = " << lon << std::endl;
    //std::cout << "ProcessGroundPolygons::Convert i = " << i << std::endl;
    if(p_groundMap->SetUniversalGround(lat, lon)) {

      sample = p_groundMap->Sample();
      line = p_groundMap->Line();
      //std::cout << "sample = " << sample << " Line = " << line << std::endl;
      p_samples.push_back(sample);
      p_lines.push_back(line);

    }


  }

  /**
   * This method cleans up any open outputcube files and deletes
   * the pointer to the universal ground map if there was one
   * created.
   *
   * @deprecated Please use Finalize()
   */
  void ProcessGroundPolygons::EndProcess() {

    if(p_groundMap != NULL) {
      delete p_groundMap;
    }

    ProcessPolygons::EndProcess();
  }

  /**
   * This method cleans up any open outputcube files and deletes
   * the pointer to the universal ground map if there was one
   * created.
   *
   */
  void ProcessGroundPolygons::Finalize() {

    if(p_groundMap != NULL) {
      delete p_groundMap;
    }

    ProcessPolygons::Finalize();
  }

  /**
   * This gives the option to append to the cube
   *
   * @param cube
   * @param avgFileName
   * @param countFileName
   */
  void ProcessGroundPolygons::AppendOutputCube(QString &cube,
      const QString &avgFileName,
      const QString &countFileName) {
    /*We need a ground map for converting lat/long to line/sample  see Convert()*/
    Pvl pvl(cube);
    p_groundMap = new UniversalGroundMap(pvl);
    ProcessPolygons::AppendOutputCube(avgFileName, countFileName);

  }

  /**
   * This method creates two cubes and creates a universal ground
   * map using the pvl information of the 'cube of interest'
   *
   * @param avgFileName
   * @param countFileName
   * @param outAtts
   * @param cube
   */
  void ProcessGroundPolygons::SetOutputCube(const QString &avgFileName,
      const QString &countFileName,
      Isis::CubeAttributeOutput &outAtts,
      QString &cube) {
    /*We need a ground map for converting lat/long to line/sample  see Convert()*/
    Pvl pvl(cube);
    p_groundMap = new UniversalGroundMap(pvl);

    /*setup input cube to transfer projection or camera labels*/
    CubeAttributeInput inAtts;
    Isis::Process::SetInputCube(cube, inAtts, 0);
    int nBands = this->InputCubes[0]->bandCount();
    int nLines = this->InputCubes[0]->lineCount();
    int nSamples = this->InputCubes[0]->sampleCount();

    this->Process::SetOutputCube(avgFileName, outAtts, nSamples, nLines, nBands);
    this->Process::SetOutputCube(countFileName, outAtts, nSamples, nLines, nBands);

    ClearInputCubes();

  }


  /**
   * This is a method that is called directly from the
   * application.  Using the "TO" parameter we also create a
   * count cube name.  The we call the overloaded SetOutputCube
   * method above.
   *
   * @param parameter
   * @param cube
   */
  void ProcessGroundPolygons::SetOutputCube(const QString &parameter,
      QString &cube) {

    QString avgString =
      Application::GetUserInterface().GetFileName(parameter);
    CubeAttributeOutput atts =
      Application::GetUserInterface().GetOutputAttribute(parameter);

    FileName file(avgString);
    QString path = file.path();
    QString filename = file.baseName();
    QString countString = path + "/" + filename + "-count-";

    SetOutputCube(avgString, countString, atts, cube);

  }

  /**
   *
   *
   * @param parameter
   * @param map
   * @param bands
   */
  void ProcessGroundPolygons::SetOutputCube(const QString &parameter,
      Isis::Pvl &map, int bands) {

    QString avgString =
      Application::GetUserInterface().GetFileName(parameter);
    CubeAttributeOutput atts =
      Application::GetUserInterface().GetOutputAttribute(parameter);

    FileName file(avgString);
    QString path = file.path();
    QString filename = file.baseName();
    QString countString = path + "/" + filename + "-count-";

    SetOutputCube(avgString, countString, atts, map, bands);

  }


  /**
   *
   *
   * @param avgFileName
   * @param countFileName
   * @param atts
   * @param map
   * @param bands
   */
  void ProcessGroundPolygons::SetOutputCube(const QString &avgFileName,
      const QString &countFileName,
      Isis::CubeAttributeOutput &atts,
      Isis::Pvl &map, int bands) {
    int samples, lines;

    Projection *proj = ProjectionFactory::CreateForCube(map, samples, lines,
                       false);

    this->ProcessPolygons::SetOutputCube(avgFileName, countFileName, atts,
                                         samples, lines, bands);

    /*Write the pvl group to the cube files.*/

    PvlGroup group = map.FindGroup("Mapping", Pvl::Traverse);

    OutputCubes[0]->putGroup(group);
    OutputCubes[1]->putGroup(group);

    /*We need a ground map for converting lat/long to line/sample  see Convert()*/
    p_groundMap = new UniversalGroundMap(*OutputCubes[0]->label());

    delete proj;
  }


} /* end namespace isis*/


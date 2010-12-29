
#include "Isis.h"
#include "IsisDebug.h"
#include "Application.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNetFilter.h"
#include "iException.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SpecialPixel.h"
#include "TextFile.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void PrintControlNetInfo(Isis::ControlNet & pCNet);

void IsisMain()
{
  Isis::Preference::Preferences(true);
  cout << "UnitTest for ControlNetFilter ...." << endl << endl;

  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  
  Isis::ControlNet cnetOrig(ui.GetFilename("CNET"));
  Isis::ControlNet cnet = cnetOrig;
  
  //test filters
  std::string sSerialFile = ui.GetFilename("FROMLIST");
  Isis::ControlNetFilter cnetFilter(&cnet, sSerialFile);
  
  // PointError Filter
  Isis::PvlGroup filterGrp("Point_ErrorMagnitude");
  Isis::PvlKeyword keyword("LessThan", 1);
  filterGrp += keyword;
  cnetFilter.PointErrorFilter (filterGrp, false);
  filterGrp.Clear();
  cout << "****************** PointError Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // PointID Filter
  filterGrp = Isis::PvlGroup("Point_IdExpression");
  keyword = Isis::PvlKeyword("Expression", "P0*");
  filterGrp += keyword;
  cnetFilter.PointIDFilter (filterGrp, false);
  filterGrp.Clear();
  cout << "******************  PointID Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // PointNumMeasures Filter
  filterGrp = Isis::PvlGroup("Point_NumMeasures");
  keyword = Isis::PvlKeyword("GreaterThan", 2);
  filterGrp += keyword;
  cnetFilter.PointMeasuresFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** PointNumMeasures Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // PointsProperties Filter 
  filterGrp = Isis::PvlGroup("Point_Properties");
  keyword = Isis::PvlKeyword("Ignore", false);
  filterGrp += keyword;
  cnetFilter.PointPropertiesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** PointsProperties Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Point_LatLon  Filter
  filterGrp = Isis::PvlGroup("Point_LatLon");
  Isis::PvlKeyword keyword1("MinLat", -180);
  filterGrp += keyword1;
  
  Isis::PvlKeyword keyword2("MaxLat", 180);
  filterGrp += keyword2;
  
  Isis::PvlKeyword keyword3("MinLon", 0);
  filterGrp += keyword3;
  
  Isis::PvlKeyword keyword4("MaxLon", 240);
  filterGrp += keyword4;
  
  cnetFilter.PointLatLonFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_LatLon Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Point_Distance Filter
  filterGrp = Isis::PvlGroup("Point_Distance");
  keyword1 = Isis::PvlKeyword("MaxDistance", 100000);
  filterGrp += keyword1;
  keyword2 = Isis::PvlKeyword("Units", "meters");  
  filterGrp += keyword2;
  cnetFilter.PointDistanceFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_Distance Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Points_MeasureProperties Filter
  filterGrp = Isis::PvlGroup("Point_MeasureProperties");
  keyword = Isis::PvlKeyword("MeasureType", "Candidate");
  filterGrp += keyword;  
  cnetFilter.PointMeasurePropertiesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Points_MeasureProperties Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Filter Points by Goodness of Fit
  /*filterGrp = Isis::PvlGroup("Point_GoodnessOfFit");
  keyword = Isis::PvlKeyword("LessThan", 5);
  filterGrp += keyword;  
  cnetFilter.PointGoodnessOfFitFilter(filterGrp,  false);
  filterGrp.Clear();*/
  
  // Point_CubeNames Filter
  filterGrp = Isis::PvlGroup("Point_CubeNames");
  keyword1 = Isis::PvlKeyword("Cube1", "Clementine1/UVVIS/1994-04-05T12:17:21.337");
  filterGrp += keyword1;  
  keyword2 = Isis::PvlKeyword("Cube2", "Clementine1/UVVIS/1994-03-08T20:03:40.056");
  filterGrp += keyword2;
  keyword3 = Isis::PvlKeyword("Cube3", "Clementine1/UVVIS/1994-03-08T20:04:59.856");
  filterGrp += keyword3; 
  keyword4 = Isis::PvlKeyword("Cube4", "Clementine1/UVVIS/1994-04-05T12:18:07.957");
  filterGrp += keyword4;
  cnetFilter.PointCubeNamesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_CubeNames Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Cube Filters
  // Cube_NameExpression Filter
  filterGrp = Isis::PvlGroup("Cube_NameExpression");
  keyword = Isis::PvlKeyword("Expression", "Clementine1/UVVIS/1994-04*");
  filterGrp += keyword;  
  cnetFilter.CubeNameExpressionFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Cube_NameExpression Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";
  
  // Cube_NumPoints Filter 
  filterGrp = Isis::PvlGroup("Cube_NumPoints");
  keyword = Isis::PvlKeyword("GreaterThan", 2);
  filterGrp += keyword;
  cnetFilter.CubeNumPointsFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Cube_NumPoints Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n"; 
        
  // Cube_Distance Filter
  filterGrp = Isis::PvlGroup("Cube_Distance");
  keyword1 = Isis::PvlKeyword("MaxDistance", 100000);
  filterGrp += keyword1;
  keyword2 = Isis::PvlKeyword("Units=", "meters");
  filterGrp += keyword2;
  cnetFilter.CubeDistanceFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Cube_Distance Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

}

/**
 * Print Control Net info like Point ID and Measure SerialNum
 * 
 * @author Sharmila Prasad (12/28/2010)
 * 
 * @param pCNet - Control Net
 */
void PrintControlNetInfo(Isis::ControlNet & pCNet)
{
  for (int i=0; i<pCNet.Size(); i++) {
    cout << "Control Point ID  " << pCNet[i].GetId() << endl;
    for (int j=0; j<pCNet[i].Size(); j++) {
      cout << "   Measure SerialNum " << pCNet[i][j].GetCubeSerialNumber() << endl;
    }
    cout << endl;
  }
  
}

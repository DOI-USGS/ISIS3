
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

using namespace Isis;
using namespace std;

void PrintControlNetInfo(ControlNet &pCNet);

void IsisMain() {
  Preference::Preferences(true);
  cout << "UnitTest for ControlNetFilter ...." << endl << endl;

  UserInterface &ui = Application::GetUserInterface();

  ControlNet cnetOrig(ui.GetFilename("CNET"));
  ControlNet cnet = cnetOrig;

  //test filters
  std::string sSerialFile = ui.GetFilename("FROMLIST");
  ControlNetFilter cnetFilter(&cnet, sSerialFile);

  // PointError Filter
  PvlGroup filterGrp("Point_ErrorMagnitude");
  PvlKeyword keyword("LessThan", 1);
  filterGrp += keyword;
  cnetFilter.PointErrorFilter(filterGrp, false);
  filterGrp.Clear();
  cout << "****************** PointError Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // PointID Filter
  filterGrp = PvlGroup("Point_IdExpression");
  keyword = PvlKeyword("Expression", "P0*");
  filterGrp += keyword;
  cnetFilter.PointIDFilter(filterGrp, false);
  filterGrp.Clear();
  cout << "******************  PointID Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // PointNumMeasures Filter
  filterGrp = PvlGroup("Point_NumMeasures");
  keyword = PvlKeyword("GreaterThan", 2);
  filterGrp += keyword;
  cnetFilter.PointMeasuresFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** PointNumMeasures Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // PointsProperties Filter
  filterGrp = PvlGroup("Point_Properties");
  keyword = PvlKeyword("Ignore", false);
  filterGrp += keyword;
  cnetFilter.PointPropertiesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** PointsProperties Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Point_LatLon  Filter
  filterGrp = PvlGroup("Point_LatLon");
  PvlKeyword keyword1("MinLat", -180);
  filterGrp += keyword1;

  PvlKeyword keyword2("MaxLat", 180);
  filterGrp += keyword2;

  PvlKeyword keyword3("MinLon", 0);
  filterGrp += keyword3;

  PvlKeyword keyword4("MaxLon", 240);
  filterGrp += keyword4;

  cnetFilter.PointLatLonFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_LatLon Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Point_Distance Filter
  filterGrp = PvlGroup("Point_Distance");
  keyword1 = PvlKeyword("MaxDistance", 100000);
  filterGrp += keyword1;
  keyword2 = PvlKeyword("Units", "meters");
  filterGrp += keyword2;
  cnetFilter.PointDistanceFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_Distance Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Points_MeasureProperties Filter
  filterGrp = PvlGroup("Point_MeasureProperties");
  keyword = PvlKeyword("MeasureType", "Candidate");
  filterGrp += keyword;
  cnetFilter.PointMeasurePropertiesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Points_MeasureProperties Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Filter Points by Goodness of Fit
  /*filterGrp = PvlGroup("Point_GoodnessOfFit");
  keyword = PvlKeyword("LessThan", 5);
  filterGrp += keyword;
  cnetFilter.PointGoodnessOfFitFilter(filterGrp,  false);
  filterGrp.Clear();*/

  // Point_CubeNames Filter
  filterGrp = PvlGroup("Point_CubeNames");
  keyword1 = PvlKeyword("Cube1", "Clementine1/UVVIS/1994-04-05T12:17:21.337");
  filterGrp += keyword1;
  keyword2 = PvlKeyword("Cube2", "Clementine1/UVVIS/1994-03-08T20:03:40.056");
  filterGrp += keyword2;
  keyword3 = PvlKeyword("Cube3", "Clementine1/UVVIS/1994-03-08T20:04:59.856");
  filterGrp += keyword3;
  keyword4 = PvlKeyword("Cube4", "Clementine1/UVVIS/1994-04-05T12:18:07.957");
  filterGrp += keyword4;
  cnetFilter.PointCubeNamesFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Point_CubeNames Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Cube Filters
  // Cube_NameExpression Filter
  filterGrp = PvlGroup("Cube_NameExpression");
  keyword = PvlKeyword("Expression", "Clementine1/UVVIS/1994-04*");
  filterGrp += keyword;
  cnetFilter.CubeNameExpressionFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Cube_NameExpression Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Cube_NumPoints Filter
  filterGrp = PvlGroup("Cube_NumPoints");
  keyword = PvlKeyword("GreaterThan", 2);
  filterGrp += keyword;
  cnetFilter.CubeNumPointsFilter(filterGrp,  false);
  filterGrp.Clear();
  cout << "****************** Cube_NumPoints Filter ******************" << endl;
  PrintControlNetInfo(cnet);
  cout << "************************************************************************\n";

  // Cube_Distance Filter
  filterGrp = PvlGroup("Cube_Distance");
  keyword1 = PvlKeyword("MaxDistance", 100000);
  filterGrp += keyword1;
  keyword2 = PvlKeyword("Units=", "meters");
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
void PrintControlNetInfo(ControlNet &pCNet) {
  QList<QString> pointIds(pCNet.GetPointIds());
  qSort(pointIds);

  for (int i = 0; i < pointIds.size(); i++) {
    const QString &pointId = pointIds[i];
    ControlPoint *controlPoint = pCNet[pointId];

    cerr << "Control Point ID  " << pointId.toStdString() << endl;
    QList<QString> serialNums(controlPoint->GetCubeSerialNumbers());
    qSort(serialNums);
    for (int j = 0; j < serialNums.size(); j++) {
      const QString &serialNum = serialNums.at(j);
      cerr << "   Measure SerialNum "
          << serialNum.toStdString() << endl;
    }
    cerr << endl;
  }

}

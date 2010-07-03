#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "iException.h"
#include "Preference.h"
#include "ControlGraph.h"

#include <iostream>

#include <QVector>


using namespace std;
using namespace Isis;

int main()
{
  cerr << "\nUnit Test for ControlGraph!!!\n\n"
               "building the following ControlNet for testing...\n\n";
  Preference::Preferences(true);

  // control point 0
  ControlMeasure cp0cm1;
  cp0cm1.SetCubeSerialNumber("A");
  ControlMeasure cp0cm2;
  cp0cm2.SetCubeSerialNumber("B");

  ControlPoint cp0("0");
  cp0.Add(cp0cm1);
  cp0.Add(cp0cm2);


  // control point 1
  ControlMeasure cp1cm1;
  cp1cm1.SetCubeSerialNumber("A");
  ControlMeasure cp1cm2;
  cp1cm2.SetCubeSerialNumber("B");
  ControlMeasure cp1cm3;
  cp1cm3.SetCubeSerialNumber("C");

  ControlPoint cp1("1");
  cp1.Add(cp1cm1);
  cp1.Add(cp1cm2);
  cp1.Add(cp1cm3);
  
  
  // control point 2
  ControlMeasure cp2cm1;
  cp2cm1.SetCubeSerialNumber("A");
  ControlMeasure cp2cm2;
  cp2cm2.SetCubeSerialNumber("B");
  ControlMeasure cp2cm3;
  cp2cm3.SetCubeSerialNumber("C");

  ControlPoint cp2("2");
  cp2.Add(cp2cm1);
  cp2.Add(cp2cm2);
  cp2.Add(cp2cm3);


  // control point 3
  ControlMeasure cp3cm1;
  cp3cm1.SetCubeSerialNumber("B");
  ControlMeasure cp3cm2;
  cp3cm2.SetCubeSerialNumber("C");

  ControlPoint cp3("3");
  cp3.Add(cp3cm1);
  cp3.Add(cp3cm2);


  // control point 4
  ControlMeasure cp4cm1;
  cp4cm1.SetCubeSerialNumber("B");
  ControlMeasure cp4cm2;
  cp4cm2.SetCubeSerialNumber("C");

  ControlPoint cp4("4");
  cp4.Add(cp4cm1);
  cp4.Add(cp4cm2);


  // control point 5
  ControlMeasure cp5cm1;
  cp5cm1.SetCubeSerialNumber("D");
  ControlMeasure cp5cm2;
  cp5cm2.SetCubeSerialNumber("E");

  ControlPoint cp5("5");
  cp5.Add(cp5cm1);
  cp5.Add(cp5cm2);
//  cp5.SetIgnore(true);


  // now build controlnet
  ControlNet cnet;
  cnet.Add(cp0);
  cnet.Add(cp1); 
  cnet.Add(cp2);
  cnet.Add(cp3);
  cnet.Add(cp4); 
  cnet.Add(cp5);

  cerr << "  ControlPoint  |  Images\n"
               "----------------|--------------------------";
  for (int i = 0; i < cnet.Size(); i++)
  {
    cerr << "\n\t" << cnet[i].Id() << "\t|";
    for (int j = 0; j < cnet[i].Size(); j++)
    {
      cerr << "\t" << cnet[i][j].CubeSerialNumber();
    }
  }
  cerr << "\n\nControlNet built!\n\n"
               "constructing a ControlGraph...\n";

  ControlGraph cg(&cnet);
  cerr << "ControlGraph constructed!\n\n"
               "IsConnected() returns: ";
  
  if (cg.IsConnected())
    cerr << "true\n\n";
  else
    cerr << "false\n\n";

  cerr << "GetIslandCount returns: " << cg.GetIslandCount() << "\n\n"
               "GetCubesOnIsland(0) returns:";
  QVector< QString > cubesOnIsland = cg.GetCubesOnIsland(0);
  for (int i = 0; i < cubesOnIsland.size(); i++)
    cerr << "  " << cubesOnIsland[i].toStdString();
  
  cerr << "\n\nGetCubesOnIsland(1) returns:";
  cubesOnIsland = cg.GetCubesOnIsland(1);
  for (int i = 0; i < cubesOnIsland.size(); i++)
    cerr << "  " << cubesOnIsland[i].toStdString();

  try
  {
    cerr << "\n\nGetCubesOnIsland(42) returns:";
    cubesOnIsland = cg.GetCubesOnIsland(42);
  }
  catch (iException e)
  {
    e.Report();
  }
  
  cerr << "\nGetCubeList() returns:\n";
  QVector< QString > cubeList = cg.GetCubeList();
  for (int i = 0; i < cubeList.size(); i++)
  {
    cerr << "    " << cubeList[i].toStdString() << "\n";
  }
  
  cerr << "\n";
  
  return 0;
}

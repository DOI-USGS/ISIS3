#include <string>
#include <iostream>
#include <sstream>
#include <ctime>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasureLogData.h"
#include "SurfacePoint.h"
#include "SpecialPixel.h"
#include "TextFile.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main()
{
  Preference::Preferences(true);
  cout << "UnitTest for ControlNet ...." << endl << endl;
  ControlNet cn1;

  cn1.SetTarget("Mars");
  cn1.SetNetworkId("Test");
  cn1.SetUserName("TSucharski");
  cn1.SetCreatedDate("2010-07-10T12:50:15");
  cn1.SetModifiedDate("2010-07-10T12:50:55");
  cn1.SetDescription("UnitTest of ControlNetwork");

  std::string pointId = "T000";
  std::string id = "id";

  for (int i = 0; i < 5; i++)
  {
    std::stringstream oss1;
    oss1.flush();
    oss1 << pointId << i;
    ControlPoint * cp = new ControlPoint(oss1.str());

    if (i == 0)
    {
      cp->SetType(ControlPoint::Ground);
      cp->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
      cp->SetAprioriSurfacePointSourceFile("/work1/tsucharski/basemap.cub");
      cp->SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
      cp->SetAprioriRadiusSourceFile("$base/dems/molaMarsPlanetaryRadius0003.cub");

      SurfacePoint surfacePt(Displacement(-424.024048, Displacement::Meters),
          Displacement(734.4311949, Displacement::Meters),
          Displacement(529.919264, Displacement::Meters),
          Distance(10, Distance::Meters),
          Distance(50, Distance::Meters),
          Distance(20, Distance::Meters));

      cp->SetSurfacePoint(surfacePt);
      cp->SetAprioriSurfacePoint(surfacePt);
    }
    else
      if (i == 1)
      {
        cp->SetIgnored(true);
      }
      else
      {
        cp->SetType(ControlPoint::Tie);
      }

    for (int k = 0; k < 2; k++)
    {
      ControlMeasure * cm = new ControlMeasure;
      std::stringstream oss2;
      oss2.flush();
      oss2 << id << k;
      cm->SetCubeSerialNumber(oss2.str());
      cm->SetType(ControlMeasure::RegisteredSubPixel);
      cm->SetLogData(
        ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit,
            0.53523 * (k + 1)));
      cm->SetCoordinate(1.0, 2.0);
      cm->SetResidual(-3.0, 4.0);
      cm->SetDiameter(15.0);
      cm->SetAprioriSample(2.0);
      cm->SetAprioriLine(5.0);
      cm->SetSampleSigma(.01);
      cm->SetLineSigma(.21);
      cm->SetChooserName("pointreg");
      cm->SetDateTime("2010-08-27T17:10:06");

      cp->Add(cm);

      if (k == 0)
      {
        cp->SetRefMeasure(cm);
        cm->SetChooserName("cnetref");
        cm->SetDateTime("2010-08-27T17:10:06");
        cm->SetEditLock(true);
      }
      if (k == 1)
      {
        cm->SetType(ControlMeasure::Candidate);
        cm->SetIgnored(true);
        cm->SetChooserName("autoseed");
      }

      cm->SetDateTime("2010-08-27T17:10:06");
    }

    cp->SetChooserName("autoseed");
    cp->SetDateTime("2010-08-27T17:10:06");

    if (i == 0)
      cp->SetEditLock(true);

    cn1.AddPoint(cp);
  }

  cout << "Test adding control points with identical id numbers ..." << endl;
  try
  {
    cn1.AddPoint(cn1.GetPoint(3));
  }
  catch (iException & e)
  {
    e.Report(false);
  }
  cout << endl;

  //  Delete point with invalid point type, first save id for next test
  string id2 = cn1[2]->GetId();
  cn1.DeletePoint(2);

  cout << "Test deleting nonexistant control point id ..." << endl;
  try
  {
    cn1.DeletePoint(id2);
  }
  catch (iException & e)
  {
    e.Report(false);
  }
  cout << endl;


  cout << "Test deleting nonexistant control point index ..." << endl;
  try
  {
    cn1.DeletePoint(7);
  }
  catch (iException & e)
  {
    e.Report(false);
  }
  cout << endl;

  cout << "Writing ControlNet to temp.txt in Pvl format" << endl;
  cn1.Write("temp.txt", true);

  cout << "Reading ControlNet from temp.txt" << endl;
  ControlNet cn2("temp.txt");

  cout << "Writing ControlNet to temp2.txt in Pvl format" << endl;
  cn2.Write("temp2.txt", true);
  cout << "Diffing temp.txt and temp2.txt" << endl;
  if (system("cmp temp.txt temp2.txt"))
  {
    cout << "ERROR:  Text Files are not the same!" << endl;
  }

  cout << "Test read/write of binary control networks ..." << endl;

  //  Test read/write of binary
  cout << "Writing ControlNet to temp.bin in binary format" << endl;
  cn2.Write("temp.bin");
  ControlNet cn3;

  cout << "Reading ControlNet from temp.bin" << endl;
  cn3.ReadControl("temp.bin");

  cout << "Writing ControlNet to temp.txt in Pvl format" << endl;
  cn3.Write("temp.txt", true);

  cout << "Reading Pvl from temp.txt and then printing" << endl;
  Pvl p1("temp.txt");
  cout << endl << p1 << endl << endl;

  cout << "Writing ControlNet to temp2.bin in binary format" << endl;
  cn3.Write("temp2.bin");
  cout << "Reading ControlNet from temp2.bin" << endl;
  ControlNet cn4("temp2.bin");

  cout << "Diffing temp.bin and temp2.bin" << endl;
  if (system("cmp temp.bin temp2.bin"))
  {
    cout << "ERROR:  Binary files are not the same." << endl;
  }
  else
  {
    cout << "Read/Write of binary files OK." << endl;
  }
  remove("temp.txt");
  remove("temp2.txt");
  remove("temp.bin");
  remove("temp2.bin");

  //system("cat unitTest.output | grep -v DateTime > temp.output; mv temp.output unitTest.output");
  //system("cat unitTest.output | sed -r s/`date +%Y-%m-%dT`\\[0-9:\\]\\{8\\}/2010-08-27T17:10:06/g > temp.output; mv temp.output unitTest.output");

  return 0;
#if 0

  // -------------------------------------------------------------------------
  // Testing the google protocol buffer methods added to the ControlNet class
  // SLA 6/30/09
  // -------------------------------------------------------------------------

  cout << "Enter input cnet: ";
  string inNet;
  cin >> inNet;
  string outFile;
  cout << "Enter output file (directory & prefix, no extension): ";
  cin >> outFile;

  ControlNet * cn1 = new ControlNet;
  cout << "Speed Test for ControlNet ...." << endl << endl;
  cout << "\nReading from the ascii file....    " << inNet << endl;
  std::clock_t start = std::clock();
//  cn1.ReadControl("/work1/tsucharski/protobuf/isis/nets/cnet.net");
//  cn1->ReadControl("/work1/tsucharski/protobuf/isis/nets/pntreg2.net");
//  cn1->ReadControl("/work1/tsucharski/protobuf/isis/nets/pntreg_combinedparts.net");
  cn1->ReadControl(inNet);
  std::cout << ((std::clock() - start) / (double)CLOCKS_PER_SEC) << " seconds \n";

  cout << "\nWriting to the binary file...." << endl;
  start = std::clock();
//  cn1.WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.bin");
//  cn1->WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.bin");
//  cn1->WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.bin");
  cn1->WritePB(outFile + ".bin");
  std::cout << ((std::clock() - start) / (double)CLOCKS_PER_SEC) << " seconds \n";
  delete cn1;

//  ControlNet cn2;
  ControlNet * cn2 = new ControlNet;

  cout << "\nReading from the binary file...." << endl;
  std::clock_t start2 = std::clock();
//  cn2.ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.bin");
//  cn2->ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.bin");
//  cn2->ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.bin");
  cn2->ReadPBControl(outFile + ".bin");
  std::cout << ((std::clock() - start2) / (double)CLOCKS_PER_SEC) << " seconds \n";

//  apLat = (*cn2)[2].AprioriLatitude();
//  cout<<"binaryNet AprioriLatitude = "<<apLat<<endl;

//cout << "\nConverting the binary to Pvl...." << endl;
//std::clock_t start2 = std::clock();
//cn1.ConvertBinToPvl();
//std::cout<< ( ( std::clock() - start2 ) / (double)CLOCKS_PER_SEC ) <<" seconds \n";



  cout << "\nWriting to the Pvl file...." << endl;
  std::clock_t start3 = std::clock();
//  cn2.Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.pvl");
//  cn2->Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.pvl");
//  cn2->Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.pvl");
  cn2->Write(outFile + ".pvl");
  std::cout << ((std::clock() - start3) / (double)CLOCKS_PER_SEC) << " seconds \n";

#endif
}

#include "PBControlNetIO.pb.h"
#include "Preference.h"
#include "iException.h"

#include <string>
#include <iostream>

using namespace std;
using namespace Isis;

int main () {
  Preference::Preferences(true);
  /**
   * @brief Test PBControlNetIO object for accuracy and correct behavior.
   *
   * @author 2010-06-30 Tracie Sucharski
   *
  */
  cout << "PBControlNetIO unitTest" << endl;
  cout << "Temporary files created,compared,then deleted." << endl;

  PBControlNet pbNet;
  pbNet.mutable_pedigree()->set_version(1.0);
  pbNet.mutable_pedigree()->set_date("2010-06-29");
  pbNet.set_networkid("UnitTest");
  pbNet.set_targetname("Moon");
  pbNet.set_created("2010-06-29T17:06:31");
  pbNet.set_lastmodified("2010-06-30T13:51:45");
  pbNet.set_description("UnitTest");
  pbNet.set_username("autoseed");
  
  PBControlNet_PBControlPoint *pbPoint = pbNet.add_points();
  pbPoint->set_id("Point_1");
  pbPoint->set_type(PBControlNet_PBControlPoint::Ground);
  pbPoint->set_choosername("autoseed");
  pbPoint->set_editlock(true);
  pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint::Basemap);
  pbPoint->set_apriorixyzsourcefile("basemap.cub");
  pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint::DEM);
  pbPoint->set_aprioriradiussourcefile("$base/dems/ulcn2005_clean.cub");
  pbPoint->set_apriorix(-424.024048);
  pbPoint->set_aprioriy(734.4311949);
  pbPoint->set_aprioriz(529.919264);
 
  pbPoint->add_aprioricovar(100.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(2500.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(0.);
  pbPoint->add_aprioricovar(400.);

  pbPoint->set_x(-424.024048);
  pbPoint->set_y(734.4311949);
  pbPoint->set_z(529.919264);

  pbPoint->add_apostcovar(100.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(2500.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(0.);
  pbPoint->add_apostcovar(400.);

  PBControlNet_PBControlPoint_PBControlMeasure *pbMeas = pbPoint->add_measures();
  pbMeas->set_serialnumber("APOLLO15/METRIC/1971-07-30T02:17:20.758");
  pbMeas->set_type(PBControlNet_PBControlPoint_PBControlMeasure::Candidate);
  pbMeas->set_choosername("cnetref");
  pbMeas->set_datetime("2010-06-30T13:51:45");

  PBControlNet_PBControlPoint_PBControlMeasure::PBMeasure *m = pbMeas->mutable_measurement();
  m->set_sample(16641.066683342);
  m->set_line(18441.299208028);
  m->set_sampleresidual(-17.716400775222);
  m->set_lineresidual(1.9004653407428);

  pbMeas->set_diameter(23.2);
  pbMeas->set_apriorisample(166394.1668);
  pbMeas->set_aprioriline(18442.1238988);
  pbMeas->set_samplesigma(1.0113);
  pbMeas->set_linesigma(1.0215);

  pbMeas = pbPoint->add_measures();
  pbMeas->set_serialnumber("APOLLO15/METRIC/1971-07-30T02:17:41.097");
  pbMeas->set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel);
  pbMeas->set_choosername("pointreg");
  pbMeas->set_datetime("2010-06-30T15:02:21");
  pbMeas->set_ignore(true);
  pbMeas->set_editlock(true);

  m = pbMeas->mutable_measurement();
  m->set_sample(21165.339298552);
  m->set_line(18441.299208028);
  m->set_sampleresidual(18388.043343993);
  m->set_lineresidual(1.9004653407428);

  pbMeas->set_diameter(23.2);
  pbMeas->set_apriorisample(21164.028939);
  pbMeas->set_aprioriline(18440.8182934);
  pbMeas->set_samplesigma(1.0113);
  pbMeas->set_linesigma(1.0215);

  //  Add a 2nd point
  pbPoint = pbNet.add_points();
  pbPoint->set_id("Point_2");
  pbPoint->set_type(PBControlNet_PBControlPoint::Tie);
  pbPoint->set_choosername("autoseed");
  pbPoint->set_ignore(true);

  pbMeas = pbPoint->add_measures();
  pbMeas->set_serialnumber("APOLLO15/METRIC/1971-07-30T02:17:41.097");
  pbMeas->set_type(PBControlNet_PBControlPoint_PBControlMeasure::Candidate);
  pbMeas->set_choosername("cnetref");
  pbMeas->set_datetime("2010-07-15T09:42:55");

  m = pbMeas->mutable_measurement();
  m->set_sample(21165.339298552);
  m->set_line(18441.299208028);
  m->set_sampleresidual(18388.043343993);
  m->set_lineresidual(1.9004653407428);

  pbMeas->set_diameter(23.2);
  pbMeas->set_apriorisample(21164.028939);
  pbMeas->set_aprioriline(18440.8182934);
  pbMeas->set_samplesigma(1.0113);
  pbMeas->set_linesigma(1.0215);

  fstream output("temp.bin", ios::out | ios::trunc | ios::binary);
  if (!pbNet.SerializeToOstream(&output)) {
    cout<<"Error writing binary file."<<endl;
    return -1;
  }
  output.close();

  fstream input("temp.bin", ios::in | ios::binary);
  if (!pbNet.ParseFromIstream(&input)) {
    cout<<"Error reading binary file."<<endl;
    return -1;
  }
  input.close();

  //  ********************  DEBUG PRINT  **************************
//  std::cout<<std::endl<<std::endl<<pbNet.DebugString()<<std::endl;

  fstream output2("temp2.bin", ios::out | ios::trunc | ios::binary);
  if (!pbNet.SerializeToOstream(&output2)) {
    cout<<"Error writing binary file."<<endl;
    return -1;
  }
  output2.close();

  if (system("cmp temp.bin temp2.bin")) {
    cout<<"ERROR:  Binary files are not the same."<<endl;
  }
  system("rm temp.bin");
  system("rm temp2.bin");


}


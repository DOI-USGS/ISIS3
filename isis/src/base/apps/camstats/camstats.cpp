#include "Isis.h"

#include "Camera.h"
#include "CameraStatistics.h"
#include "Cube.h"
#include "Distance.h"
#include "IString.h"
#include "UserInterface.h"
#include "Process.h"
#include "Progress.h"
#include "Statistics.h"


using namespace std;
using namespace Isis;

//function to build stats data
void buildStats(Camera *cam, int &sample, int &line);
void writeFlat(ofstream &os, const Statistics *s);
string valueToString(const double &value);


void IsisMain() {
  Process p;
  Cube *icube = p.SetInputCube("FROM");
  Camera *cam = icube->getCamera();

  UserInterface &ui = Application::GetUserInterface();

  QString from = ui.GetFileName("FROM");
  int sinc = ui.GetInteger("SINC");
  int linc = ui.GetInteger("LINC");
  CameraStatistics camStats(cam, sinc, linc, from);

  // Send the Output to the log area
  Pvl statsPvl = camStats.toPvl();
  for (int i = 0; i < statsPvl.Groups(); i++) {
    Application::Log(statsPvl.Group(i));
  }

  if(ui.WasEntered("TO")) {
    QString outfile = FileName(ui.GetFileName("TO")).expanded();
    bool exists = FileName(outfile).fileExists();
    bool append = ui.GetBoolean("APPEND");

    // If the user chose a format of PVL, then write to the output file ("TO")
    if(ui.GetString("FORMAT") == "PVL") {
      (append) ? statsPvl.Append(outfile) : statsPvl.Write(outfile);
    }
    else {
      // Create a flatfile of the data with columhn headings the flatfile is
      // comma-delimited and can be imported in to spreadsheets
      ofstream os;
      bool writeHeader = true;
      if(append) {
        os.open(outfile.toAscii().data(), ios::app);
        if(exists) {
          writeHeader = false;
        }
      }
      else {
        os.open(outfile.toAscii().data(), ios::out);
      }

      // if new file or append and no file exists then write header
      if(writeHeader) {
        os << "Filename," <<
           "LatitudeMinimum," <<
           "LatitudeMaximum," <<
           "LatitudeAverage," <<
           "LatitudeStandardDeviation," <<
           "LongitudeMinimum," <<
           "LongitudeMaximum," <<
           "LongitudeAverage," <<
           "LongitudeStandardDeviation," <<
           "SampleResolutionMinimum," <<
           "SampleResolutionMaximum," <<
           "SampleResolutionAverage," <<
           "SampleResolutionStandardDeviation," <<
           "LineResolutionMinimum," <<
           "LineResolutionMaximum," <<
           "LineResolutionAverage," <<
           "LineResolutionStandardDeviation," <<
           "ResolutionMinimum," <<
           "ResolutionMaximum," <<
           "ResolutionAverage," <<
           "ResolutionStandardDeviation," <<
           "AspectRatioMinimum," <<
           "AspectRatioMaximum," <<
           "AspectRatioAverage," <<
           "AspectRatioStandardDeviation," <<
           "PhaseMinimum," <<
           "PhaseMaximum," <<
           "PhaseAverage," <<
           "PhaseStandardDeviation," <<
           "EmissionMinimum," <<
           "EmissionMaximum," <<
           "EmissionAverage," <<
           "EmissionStandardDeviation," <<
           "IncidenceMinimum," <<
           "IncidenceMaximum," <<
           "IncidenceAverage," <<
           "IncidenceStandardDeviation," <<
           "LocalSolarTimeMinimum," <<
           "LocalSolarTimeMaximum," <<
           "LocalSolarTimeAverage," <<
           "LocalSolarTimeStandardDeviation," <<
           "LocalRadiusMaximum," <<
           "LocalRadiusMaximum," <<
           "LocalRadiusAverage," <<
           "LocalRadiusStandardDeviation," <<
           "NorthAzimuthMinimum," <<
           "NorthAzimuthMaximum," <<
           "NorthAzimuthAverage," <<
           "NorthAzimuthStandardDeviation," << endl;
      }
      os << FileName(from).expanded() << ",";
      //call the function to write out the values for each group
      writeFlat(os, camStats.getLatStat());
      writeFlat(os, camStats.getLonStat());
      writeFlat(os, camStats.getSampleResStat());
      writeFlat(os, camStats.getLineResStat());
      writeFlat(os, camStats.getResStat());
      writeFlat(os, camStats.getAspectRatioStat());
      writeFlat(os, camStats.getPhaseStat());
      writeFlat(os, camStats.getEmissionStat());
      writeFlat(os, camStats.getIncidenceStat());
      writeFlat(os, camStats.getLocalSolarTimeStat());
      writeFlat(os, camStats.getLocalRaduisStat());
      writeFlat(os, camStats.getNorthAzimuthStat());
      os << endl;
    }
  }

  if(ui.GetBoolean("ATTACH")) {

    QString cam_name = "CameraStatistics";

    //Creates new CameraStatistics Table
    TableField fname("Name", Isis::TableField::Text, 20);
    TableField fmin("Minimum", Isis::TableField::Double);
    TableField fmax("Maximum", Isis::TableField::Double);
    TableField favg("Average", Isis::TableField::Double);
    TableField fstd("StandardDeviation", Isis::TableField::Double);

    TableRecord record;
    record += fname;
    record += fmin;
    record += fmax;
    record += favg;
    record += fstd;

    Table table(cam_name, record);

    // Place all the gathered camera statistics in a table and attach it to the
    // cube. Skip "User Parameters" group.
    for (int i = 1; i < statsPvl.Groups(); i++) {
      PvlGroup &group = statsPvl.Group(i);

      int entry = 0;
      record[entry] = group.Name();
      entry++;
      for (int j = 0; j < group.Keywords(); j++) {
        record[entry] = toDouble(group[j][0]);
        entry++;
      }
      table += record;
    }

    icube->reopen("rw");
    icube->write(table);
    p.WriteHistory(*icube);
    icube->close();
  }
}


//function to write the stats values to flat file
void writeFlat(ofstream &os, const Statistics *s) {
  os << valueToString(s->Minimum()) << "," <<
     valueToString(s->Maximum()) << "," <<
     valueToString(s->Average()) << "," <<
     valueToString(s->StandardDeviation()) << ",";
}


string valueToString(const double &value) {
  if(IsSpecial(value)) {
    return (string("NULL"));
  }
  else {
    return ((string) IString(value));
  }
}

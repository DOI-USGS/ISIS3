#include "Isis.h"

#include <sstream>
#include <set>

#include "SerialNumberList.h"
#include "ImageOverlapSet.h"
#include "ImageOverlap.h"
#include "Statistics.h"
#include "PvlGroup.h"
#include "Pvl.h"
#include "Target.h"
#include "TProjection.h"
#include "ProjectionFactory.h"
#include "PolygonTools.h"
#include "Progress.h"

using namespace std;
using namespace Isis;

bool full = false;
bool tab = false;

QString FormatString(double input, int head, int tail);

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  SerialNumberList serialNumbers(ui.GetFileName("FROMLIST"));

  // Find all the overlaps between the images in the FROMLIST
  // The overlap polygon coordinates are in Lon/Lat order
  ImageOverlapSet overlaps;
  overlaps.ReadImageOverlaps(ui.GetFileName("OVERLAPLIST"));

  // Progress
  Progress progress;
  progress.SetMaximumSteps(overlaps.Size());
  progress.CheckStatus();

  // Perform some sanity checking on our inputs to make sure that the serial
  // numbers in the overlap list are in the provided cube list. We don't want
  // users to provide a different cube list from the one that was used in
  // findimageoverlaps.
  for (int i = 0; i < overlaps.Size(); i++) {
    ImageOverlap const * currOverlap = overlaps[i];
    for (int j = 0; j < currOverlap->Size(); j++) {
      QString currSerialNum = (*currOverlap)[j];
      if (!serialNumbers.hasSerialNumber(currSerialNum)) {
        IString msg = "Found serial number [" + currSerialNum + "] in overlap "
            "list that was not in the provided cube list. Please ensure that "
            "the cube list is the same one used to generate your overlap list "
            "file.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }

  // Sets up the no overlap list
  set<QString> nooverlap;
  for (int i = 0; i < serialNumbers.size(); i ++) {
    nooverlap.insert(serialNumbers.serialNumber(i));
  }

  // Create the output
  stringstream output(stringstream::in | stringstream::out);
  output.precision(16);
  output.setf(ios::showpoint);

  QString delim = "";
  QString pretty = ""; // Makes tab tables look pretty, ignored in CSV
  bool singleLine = false;
  if (ui.WasEntered("DETAIL")) {
    if (ui.GetString("TABLETYPE") == "CSV") {
      delim = ",";
      singleLine = ui.GetBoolean("SINGLELINE");
      // This line was removed because readability (ios::showpoint) was more
      // important than an extra decimal place of precision.
      //output.setf(ios::scientific,ios::floatfield);
    }
    else if (ui.GetString("TABLETYPE") == "TAB") {
      delim = "\t";
      pretty = "\t";
      tab = true;
    }

    full = (ui.GetString("DETAIL") == "FULL");
  }

  bool firstFullOutput = true;

  stringstream errors(stringstream::in | stringstream::out);
  int errorNum = 0;

  // Extracts the stats of each overlap and adds to the table
  Statistics thickness;
  Statistics area;
  Statistics sncount;
  int overlapnum = 0;//Makes sure there are overlaps
  for (int index = 0; index < overlaps.Size(); index ++) {

    if (overlaps[index]->Size() > 1) {
      overlapnum++;

      // Removes the overlapping Serial Numbers for the nooverlap set
      for (int i = 0; i < overlaps[index]->Size(); i ++) {
        nooverlap.erase((*overlaps[index])[i]);
      }

      // Sets up the serial number stats
      sncount.AddData(overlaps[index]->Size());

      // Sets up the thickness stats by doing A over E
      const geos::geom::MultiPolygon *mpLatLon = overlaps[index]->Polygon();

      // Construct a Projection for converting between Lon/Lat and X/Y
      Pvl cubeLab(serialNumbers.fileName(0));

      // Get empty mapping label
      Pvl maplab;
      maplab.addGroup(PvlGroup("Mapping"));
      PvlGroup &mapGroup = maplab.findGroup("Mapping");

      // This call adds TargetName, EquatorialRadius and PolarRadius to mapGroup
      mapGroup = Target::radiiGroup(cubeLab, mapGroup);

      // mapGroup started as empty, so no need to replace here, just add keywords 
      mapGroup += PvlKeyword("LatitudeType", "Planetocentric");
      mapGroup += PvlKeyword("LongitudeDirection", "PositiveEast");
      mapGroup += PvlKeyword("LongitudeDomain", "360");
      mapGroup += PvlKeyword("CenterLatitude", "0.0");
      mapGroup += PvlKeyword("CenterLongitude", "0.0");
      mapGroup += PvlKeyword("ProjectionName", "Sinusoidal");
      TProjection *proj = (TProjection *) ProjectionFactory::Create(maplab);

      // Sets up the thickness and area stats
      try {
        geos::geom::MultiPolygon *mpXY = PolygonTools::LatLonToXY(*mpLatLon, proj);

        double thicknessValue = PolygonTools::Thickness(mpXY);
        thickness.AddData(thicknessValue);

        double areaValue = mpXY->getArea();
        area.AddData(areaValue);

        if (full) {
          if (firstFullOutput) {
            output << "Overlap ID";
            output << delim << "Thickness";
            output << delim << pretty << "Area";
            output << delim << pretty << pretty << "Image Count";
            output << delim << "Serial Numbers in Overlap";
            output << delim << "Image Files in Overlap";
            output << endl;
            firstFullOutput = false;
          }
          output << index << pretty;
          output << delim << thicknessValue;
          if (tab) {
            output << delim << FormatString(areaValue, 18, 4);
          }
          else {
            output << delim << areaValue;
          }
          output << delim << overlaps[index]->Size() << pretty;
          output << delim << (*overlaps[index])[0];
          output << delim << serialNumbers.fileName((*overlaps[index])[0]);
          for (int sn = 1; sn < overlaps[index]->Size(); sn ++) {
            if (!singleLine) {
              output << endl << pretty << delim << pretty << delim << pretty << delim;
              output << pretty << pretty;
            }
            output << delim << pretty << (*overlaps[index])[sn];
            output << delim << serialNumbers.fileName((*overlaps[index])[sn]);
          }
          output << endl;
        }

        delete mpXY;
        mpXY = NULL;

      }
      catch (IException &e) {
        errorNum++;

        if (ui.WasEntered("ERRORS")) {
          errors << e.toPvl().group(0).findKeyword("Message")[0];

          for (int serNum = 0; serNum < overlaps[index]->Size(); serNum++) {
            if (serNum == 0) {
              errors << ": ";
            }
            else {
              errors << ", ";
            }
            errors << (*overlaps[index])[serNum];
          }

          errors << endl;
        }

        progress.CheckStatus();
        continue;
      }
    }

    progress.CheckStatus();
  }

  // Checks if there were overlaps to output results from
  if (overlapnum == 0) {
    QString msg = "The overlap file [";
    msg += FileName(ui.GetFileName("OVERLAPLIST")).name();
    msg += "] does not contain any overlaps across the provided cubes [";
    msg += FileName(ui.GetFileName("FROMLIST")).name() + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }


  //Create and Log the BRIEF description
  PvlGroup brief("Results");

  brief += PvlKeyword("ThicknessMinimum", toString(thickness.Minimum()));
  brief += PvlKeyword("ThicknessMaximum", toString(thickness.Maximum()));
  brief += PvlKeyword("ThicknessAverage", toString(thickness.Average()));
  brief += PvlKeyword("ThicknessStandardDeviation", toString(thickness.StandardDeviation()));
  brief += PvlKeyword("ThicknessVariance", toString(thickness.Variance()));

  brief += PvlKeyword("AreaMinimum", toString(area.Minimum()));
  brief += PvlKeyword("AreaMaximum", toString(area.Maximum()));
  brief += PvlKeyword("AreaAverage", toString(area.Average()));
  brief += PvlKeyword("AreaStandardDeviation", toString(area.StandardDeviation()));
  brief += PvlKeyword("AreaVariance", toString(area.Variance()));

  brief += PvlKeyword("ImageStackMinimum", toString(sncount.Minimum()));
  brief += PvlKeyword("ImageStackMaximum", toString(sncount.Maximum()));
  brief += PvlKeyword("ImageStackAverage", toString(sncount.Average()));
  brief += PvlKeyword("ImageStackStandardDeviation", toString(sncount.StandardDeviation()));
  brief += PvlKeyword("ImageStackVariance", toString(sncount.Variance()));

  brief += PvlKeyword("PolygonCount", toString(overlaps.Size()));

  // Add non-overlapping cubes to the output
  if (!nooverlap.empty()) {
    for (set<QString>::iterator itt = nooverlap.begin(); itt != nooverlap.end(); itt ++) {
      brief += PvlKeyword("NoOverlap", serialNumbers.fileName(*itt));
    }
  }

  Application::Log(brief);


  //Log the ERRORS file
  if (ui.WasEntered("ERRORS")) {
    QString errorname = ui.GetFileName("ERRORS");
    std::ofstream errorsfile;
    errorsfile.open(errorname.toLatin1().data());
    errorsfile << errors.str();
    errorsfile.close();
  }

  //Log error num in print.prt if there were errors
  if (errorNum > 0) {
    PvlGroup grp("OverlapStats");
    PvlKeyword key("ErrorNumber", toString(errorNum));
    grp.addKeyword(key);
    Application::Log(grp);
  }

  // Display FULL output
  if (full) {
    QString outname = ui.GetFileName("TO");
    std::ofstream outfile;
    outfile.open(outname.toLatin1().data());
    outfile << output.str();
    outfile.close();
    if (outfile.fail()) {
      IString msg = "Unable to write the statistics to [" + ui.GetFileName("TO") + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }

}


/**
 * Takes a QString and formats the length of that QString around the decimal
 * point.
 *
 * @param input The input double to be formatted
 * @param head  The desired character length for the double prior to the
 *              decimal point. It will be filled with " " (white space)
 * @param tail  The desired character length for the double after the decimal
 *              point. It will be filled with "0".
 *
 * @return QString The formatted double for display
 */
QString FormatString(double input, int head, int tail) {

  QString result(toString(input));

  int point = result.indexOf(".");
  QString resultHead(result.mid(0, point));
  QString resultTail(result.mid(point + 1, result.size() - point - 1));

  for (int place = resultHead.size(); place < head; place ++) {
    resultHead = " " + resultHead;
  }

  for (int place = resultTail.size(); place < tail; place ++) {
    resultTail = resultTail + "0";
  }

  return resultHead + "." + resultTail;
}

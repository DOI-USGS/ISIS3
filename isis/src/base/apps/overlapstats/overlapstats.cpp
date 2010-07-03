#include "Isis.h"

#include <sstream>
#include <set>

#include "SerialNumberList.h"
#include "ImageOverlapSet.h"
#include "ImageOverlap.h"
#include "Statistics.h"
#include "PvlGroup.h"
#include "Pvl.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "PolygonTools.h"
#include "Progress.h"

using namespace std;
using namespace Isis;

bool full = false;
bool tab = false;

std::string FormatString( double input, int head, int tail );

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  SerialNumberList serialNumbers(ui.GetFilename("FROMLIST"));

  // Find all the overlaps between the images in the FROMLIST
  // The overlap polygon coordinates are in Lon/Lat order
  ImageOverlapSet overlaps;
  overlaps.ReadImageOverlaps(ui.GetFilename("OVERLAPLIST"));

  // Progress
  Progress progress;
  progress.SetMaximumSteps( overlaps.Size() );
  progress.CheckStatus();

  // Sets up the no overlap list
  set<string> nooverlap;
  for ( int i = 0; i < serialNumbers.Size(); i ++ ) {
    nooverlap.insert( serialNumbers.SerialNumber(i) );
  }

  // Create the output

  stringstream output (stringstream::in | stringstream::out);
  output.precision(16);
  output.setf(ios::showpoint);

  string delim = "";
  string pretty = ""; // Makes tab tables look pretty, ignored in CSV
  bool singleLine = false;
  if ( ui.WasEntered("DETAIL") ) {
    if ( ui.GetString("TABLETYPE") == "CSV" ) {
      delim = ",";
      singleLine = ui.GetBoolean("SINGLELINE");
      // This line was removed because reability (ios::showpoint) was more
      // important than an extra decimal place of precision.
      //output.setf(ios::scientific,ios::floatfield);
    }
    else if ( ui.GetString("TABLETYPE") == "TAB" ) {
      delim = "\t";
      pretty = "\t";

      tab = true;
    }
  
    full = ( ui.GetString("DETAIL") == "FULL" );
  }

  bool firstFullOutput = true;

  stringstream errors (stringstream::in | stringstream::out);
  int errorNum = 0;

  // Extracts the stats of each overlap and adds to the table
  Statistics thickness;
  Statistics area;
  Statistics sncount;
  int overlapnum = 0;//Makes sure there are overlaps
  for ( int index = 0; index < overlaps.Size(); index ++ ) {

    if ( overlaps[index]->Size() > 1 ) {
      overlapnum++;

      // Removes the overlapping Serial Numbers for the nooverlap set
      for ( int i = 0; i < overlaps[index]->Size(); i ++ ) {
        nooverlap.erase( (*overlaps[index])[i] );
      }

      // Sets up the serial number stats
      sncount.AddData( overlaps[index]->Size() );

      // Sets up the thickness stats by doing A over E
      const geos::geom::MultiPolygon *mpLatLon = overlaps[index]->Polygon();

      // Construct a Projection for converting between Lon/Lat and X/Y
      Pvl cubeLab(serialNumbers.Filename(0));
      PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
      string target = inst["TargetName"];
      PvlGroup radii = Projection::TargetRadii(target);
      Isis::Pvl maplab;
      maplab.AddGroup(Isis::PvlGroup("Mapping"));
      Isis::PvlGroup &mapGroup = maplab.FindGroup("Mapping");
      mapGroup += Isis::PvlKeyword("EquatorialRadius",(string)radii["EquatorialRadius"]);
      mapGroup += Isis::PvlKeyword("PolarRadius",(string)radii["PolarRadius"]);
      mapGroup += Isis::PvlKeyword("LatitudeType","Planetocentric");
      mapGroup += Isis::PvlKeyword("LongitudeDirection","PositiveEast");
      mapGroup += Isis::PvlKeyword("LongitudeDomain",360);
      mapGroup += Isis::PvlKeyword("CenterLatitude",0);
      mapGroup += Isis::PvlKeyword("CenterLongitude",0);
      mapGroup += Isis::PvlKeyword("ProjectionName","Sinusoidal");
      Projection *proj = Isis::ProjectionFactory::Create(maplab);

      // Sets up the thickness and area stats
      try {
        geos::geom::MultiPolygon *mpXY = PolygonTools::LatLonToXY( *mpLatLon, proj );

        double thicknessValue = PolygonTools::Thickness( mpXY );
        thickness.AddData( thicknessValue );

        double areaValue = mpXY->getArea();
        area.AddData( areaValue );

        if( full ) {
          if( firstFullOutput ) {
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
          if( tab ) {
            output << delim << FormatString( areaValue, 18, 4 );
          } else {
            output << delim << areaValue;
          }
          output << delim << overlaps[index]->Size() << pretty;
          output << delim << (*overlaps[index])[0];
          output << delim << serialNumbers.Filename( (*overlaps[index])[0] );
          for( int sn=1; sn < overlaps[index]->Size(); sn ++ ) {
            if( !singleLine ) {
              output << endl << pretty << delim << pretty << delim << pretty << delim;
              output << pretty << pretty;
            }
            output << delim << pretty << (*overlaps[index])[sn];
            output << delim << serialNumbers.Filename( (*overlaps[index])[sn] );
          }
          output << endl;
        }

        delete mpXY;
        mpXY = NULL;

      } catch ( iException &e ) {

        if( ui.WasEntered("ERRORS") ) {

          if( errorNum > 0 ) {
            errors << endl;
          }
          errorNum ++;

          errors << e.PvlErrors().Group(0).FindKeyword("Message")[0];
          for (int serNum = 0; serNum < overlaps[index]->Size(); serNum++) {
            if( serNum == 0 ) {
              errors << ": ";
            } else {
              errors << ", ";
            }
            errors << (*overlaps[index])[serNum];
          }
        }

        e.Clear();

        progress.CheckStatus();
        continue;
      }
      
    }

    progress.CheckStatus();

  }

  // Checks if there were overlaps to output results from
  if ( overlapnum == 0 ) {
    std::string msg = "The overlap file [";
    msg += Filename(ui.GetFilename("OVERLAPLIST")).Name();
    msg += "] does not contain any overlaps across the provided cubes [";
    msg += Filename(ui.GetFilename("FROMLIST")).Name() + "]";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }


  //Create and Log the BRIEF description
  PvlGroup brief("Results");

  brief += PvlKeyword( "ThicknessMinimum", thickness.Minimum() );
  brief += PvlKeyword( "ThicknessMaximum", thickness.Maximum() );
  brief += PvlKeyword( "ThicknessAverage", thickness.Average() );
  brief += PvlKeyword( "ThicknessStandardDeviation", thickness.StandardDeviation() );
  brief += PvlKeyword( "ThicknessVariance", thickness.Variance() );

  brief += PvlKeyword( "AreaMinimum", area.Minimum() );
  brief += PvlKeyword( "AreaMaximum", area.Maximum() );
  brief += PvlKeyword( "AreaAverage", area.Average() );
  brief += PvlKeyword( "AreaStandardDeviation", area.StandardDeviation() );
  brief += PvlKeyword( "AreaVariance", area.Variance() );

  brief += PvlKeyword( "ImageStackMinimum", sncount.Minimum() );
  brief += PvlKeyword( "ImageStackMaximum", sncount.Maximum() );
  brief += PvlKeyword( "ImageStackAverage", sncount.Average() );
  brief += PvlKeyword( "ImageStackStandardDeviation", sncount.StandardDeviation() );
  brief += PvlKeyword( "ImageStackVariance", sncount.Variance() );

  brief += PvlKeyword( "PolygonCount", overlaps.Size() );

  // Add non-overlapping cubes to the output
  if ( !nooverlap.empty() ) {
    for ( set<string>::iterator itt = nooverlap.begin(); itt != nooverlap.end(); itt ++ ) {
      brief += PvlKeyword( "NoOverlap", serialNumbers.Filename(*itt) );
    }
  }

  Application::Log( brief );


  //Log the ERRORS file
  if( ui.WasEntered("ERRORS") ) {
    string errorname = ui.GetFilename("ERRORS");
    std::ofstream errorsfile;
    errorsfile.open( errorname.c_str() );
    errorsfile << errors.str();
    errorsfile.close();
  }

  //Log error num in print.prt if there were errors
  if( errorNum > 0 ) {
    PvlGroup grp( "OverlapStats" );
    PvlKeyword key( "ErrorNumber", iString(errorNum) );
    grp.AddKeyword( key );
    Application::Log( grp );
  }

  // Display FULL output
  if( full ) {
    string outname = ui.GetFilename("TO");
    std::ofstream outfile;
    outfile.open( outname.c_str() );
    outfile << output.str();
    outfile.close();
    if(outfile.fail()) {
      iString msg = "Unable to write the statistics to [" + ui.GetFilename("TO") + "]";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }
  }

}


/**
 * Takes a string and formats the length of that string around the decimal 
 * point. 
 * 
 * @param input The input double to be formatted
 * @param head  The desired character length for the double prior to the 
 *              decimal point. It will be filled with " " (white space)
 * @param tail  The desired character length for the double after the decimal 
 *              point. It will be filled with "0".
 * 
 * @return std::string The formatted double for display
 */
std::string FormatString( double input, int head, int tail ) {

  iString result( input );

  int point = result.find_first_of(".");
  iString resultHead( result.substr(0,point) );
  iString resultTail( result.substr(point+1,result.size()-point-1) );

  for( int place = resultHead.size(); place < head; place ++ ) {
    resultHead = " " + resultHead;
  }

  for( int place = resultTail.size(); place < tail; place ++ ) {
    resultTail = resultTail + "0";
  }

  return resultHead + "." + resultTail;
}

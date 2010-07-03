#include "Isis.h"

#include <sstream>

#include "ControlNet.h"
#include "FileList.h"
#include "iException.h"
#include "iTime.h"
#include "Progress.h"

using namespace std;
using namespace Isis;

ControlPoint MergePoints( ControlPoint master, ControlPoint mergee, bool allowReferenceOverride, bool & needsReport );

// Main program
void IsisMain() {
  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList filelist;
  if( ui.GetString("INPUTTYPE") == "LIST" ) {
    filelist = ui.GetFilename("FROMLIST");
  }
  else if ( ui.GetString("INPUTTYPE") == "CNETS" ) {
    filelist.push_back( ui.GetFilename("FROM1") );
    filelist.push_back( ui.GetFilename("FROM2") );
  }
  Filename outfile( ui.GetFilename("TO") );

  bool allowPointOverride = false;
  bool allowMeasureOverride = false;
  if ( ui.GetString("DUPLICATEPOINTS") == "MERGE" ) {
    allowPointOverride = ui.GetBoolean("OVERWRITEPOINTS");
    allowMeasureOverride = ui.GetBoolean("OVERWRITEMEASURES");
  }

  // Creates a Progress
  Progress progress;
  progress.SetMaximumSteps( filelist.size() );
  progress.CheckStatus();

  // Set up the output ControlNet with the first Control Net in the list
  ControlNet cnet( Filename(filelist[0]).Expanded() );
  cnet.SetNetworkId( ui.GetString("ID") );
  cnet.SetUserName( Isis::Application::UserName() );
  cnet.SetCreatedDate( Isis::Application::DateTime() );
  cnet.SetModifiedDate( Isis::iTime::CurrentLocalTime() );
  cnet.SetDescription( ui.GetString("DESCRIPTION") );

  progress.CheckStatus();

  ofstream ss;
  bool report = false;
  if( ui.WasEntered("REPORT") ) {
    report = true;
    string report = ui.GetFilename("REPORT");
    ss.open(report.c_str(),ios::out);
  }

  for ( int f = 1; f < (int)filelist.size(); f ++ ) {

    ControlNet currentnet( Filename(filelist[f]).Expanded() );

    // Checks to make sure the ControlNets are valid to merge
    if ( cnet.Target() != currentnet.Target() ) {
      string msg = "Input [" + currentnet.NetworkId() + "] does not target the ";
      msg += "same target as other Control Nets.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // ERROR
    if ( ui.GetString("DUPLICATEPOINTS") == "ERROR" ) {

      // Throws an error if there is a duplicate Control Point
      for ( int cp=0; cp<currentnet.Size(); cp++ ) {
        if ( cnet.Exists(currentnet[cp]) ) {
          string msg = "Inputs contain the same ControlPoint. [Id=";
          msg += currentnet[cp].Id() + "] Set DUPLICATEPOINTS=MERGE to";
          msg += " merge duplicate Control Points.";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
        // Adds the point to the output ControlNet
        cnet.Add( currentnet[cp] );
      }

    }
    // MERGE
    else if ( ui.GetString("DUPLICATEPOINTS") == "MERGE" ) {

      // Adds currentnet to the ControlNet if it does not exist in cnet
      for ( int cp=0; cp<currentnet.Size(); cp++ ) {
        try {
          // Find if there is a duplicate point
          ControlPoint * dupPoint = cnet.Find( currentnet[cp].Id() ); //This is the failing line

          if( report ) {
            ss << "Control Point " << currentnet[cp].Id() << " was merged from ";
            ss << currentnet.NetworkId() << endl;
          }

          bool needsReport = false;

          // Merge the Control Points correctly
          if ( (dupPoint->Type() == ControlPoint::Ground  &&  currentnet[cp].Type() == ControlPoint::Tie)
               || (dupPoint->Held()  &&  !currentnet[cp].Held()) ) {
            ControlPoint mergedPoint = MergePoints( *dupPoint, currentnet[cp], !allowMeasureOverride, needsReport );

            if( report && needsReport ) {
              ss << "    Control Measures from " << currentnet[cp].Id() << " were not merged due to conflicts." << endl;
            }

            cnet.Delete( currentnet[cp].Id() );
            cnet.Add( mergedPoint );
          }

          else if ( (dupPoint->Type() == ControlPoint::Ground  &&  currentnet[cp].Type() == ControlPoint::Ground)
                    || (dupPoint->Held()  &&  currentnet[cp].Held()) ) {

            // See if there are conflicts in merging the 2 points
            bool hasPointConflict = false;
            if ( dupPoint->UniversalLatitude() > DBL_MIN &&
                 dupPoint->UniversalLongitude() > DBL_MIN &&
                 dupPoint->UniversalLatitude() == currentnet[cp].UniversalLatitude() &&
                 dupPoint->UniversalLongitude() == currentnet[cp].UniversalLongitude() ) {
              hasPointConflict = true;
            }

            // Merge the Control Points correctly
            if ( hasPointConflict ) {
              if ( allowPointOverride ) {
                ControlPoint mergedPoint = MergePoints( currentnet[cp], *dupPoint, allowMeasureOverride, needsReport  );

                if( report && needsReport ) {
                  ss << "    Control Measures from " << currentnet[cp].Id() << " were not merged due to conflicts." << endl;
                }

                cnet.Delete( currentnet[cp].Id() );
                cnet.Add( mergedPoint );
              }
              else {
                if( report ) {
                  ss << "    The merge of Control Point " << currentnet[cp].Id() << " was canceled due to conflicts." << endl;
                }
                // These 3 lines keep cnet's points in order with an "unnecessary" delete
                ControlPoint copyPoint = (*dupPoint);
                cnet.Delete( currentnet[cp].Id() );
                cnet.Add( copyPoint );
              }
            }
            else {
              ControlPoint mergedPoint = MergePoints( currentnet[cp], *dupPoint, allowMeasureOverride, needsReport  );

              if( report && needsReport ) {
                ss << "    Control Measures from " << currentnet[cp].Id() << " were not merged due to conflicts." << endl;
              }

              cnet.Delete( currentnet[cp].Id() );
              cnet.Add( mergedPoint );
            }
          }

          else {
            ControlPoint mergedPoint = MergePoints( currentnet[cp], *dupPoint, allowMeasureOverride, needsReport  );

            if( report && needsReport ) {
              ss << "    Control Measures from " << currentnet[cp].Id() << " were not merged due to conflicts." << endl;
            }

            cnet.Delete( currentnet[cp].Id() );
            cnet.Add( mergedPoint );
          }

          dupPoint = NULL;
        } catch ( iException &e ) {
          e.Clear();
          //then currentnet[i] was not found and was not deleted so:
          //Add the point to the output ControlNet
          cnet.Add( currentnet[cp] );
        }
      }

    }

    progress.CheckStatus();
  }

  // Writes out the final Control Net
  cnet.Write( outfile.Expanded() );

}


ControlPoint MergePoints( ControlPoint master, ControlPoint mergee, bool allowReferenceOverride, bool & needsReport ) {
  ControlPoint newPoint = master;

  // Merge mergee measures into newPoint
  for ( int cm = 0; cm < mergee.Size(); cm ++ ) {
    bool merged = false;

    // Check for duplicate measures to know when to keep "older" measures
    for ( int newcm = 0; newcm < newPoint.Size() && !merged; newcm ++ ) {
      if ( mergee[cm].CubeSerialNumber() == newPoint[newcm].CubeSerialNumber() ) {

        if ( (mergee.Type() == ControlPoint::Ground ||  newPoint.Held()) ) {
          if ( !allowReferenceOverride ) {
            // Remove new measure, pull old measure, and Report that the new wasn't merged
            if (mergee[cm].IsReference() && !newPoint[newcm].IsReference() && newPoint.HasReference() ) {
              newPoint[newPoint.ReferenceIndex()].SetReference( false );
            }
            newPoint[newcm] = mergee[cm];
            needsReport |= true;
          }
        }

        merged = true;
      }
    }

    // If no duplicate measure was found
    if( !merged ) {
      if ( newPoint.HasReference()  &&  mergee[cm].IsReference() ) {
        if ( allowReferenceOverride ) {
          // Remove reference to old Measure and pull it over
          mergee[cm].SetReference( false );
          newPoint.Add( mergee[cm] );
        }
        else {
          // Remove Reference from new measure and Report that it wasn't allowed
          newPoint[newPoint.ReferenceIndex()].SetReference( false );
          newPoint.Add( mergee[cm] );
          needsReport |= true;
        }
      }
      else {
        newPoint.Add( mergee[cm] );
      }
    }
  }

  return newPoint;
}

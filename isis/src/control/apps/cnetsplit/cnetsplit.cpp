#include "Isis.h"
#include "IsisDebug.h"
#include "iException.h"

#include "ID.h"
#include "ControlNet.h"
#include "ControlNetStatistics.h"
#include "ControlPoint.h"
#include "Progress.h"

#include <string.h>

using namespace std;
using namespace Isis;

void IsisMain() {

  try {
    UserInterface &ui = Application::GetUserInterface();
    
    // To determine the progress of the application
    Progress progress;
    
    // Get the input Control Net
    ControlNet cNet(ui.GetFilename("CNET"), &progress);
    
    // Get the output file pattern string
    // Set up an automatic id generator for the point ids
    ID outFileID = ID(ui.GetString("ONET_PREFIX"));
    
    int numOutputFiles = ui.GetInteger("NUM_OUTPUT_FILES");
    
    int numPoints = cNet.GetNumPoints();
    
    if (numOutputFiles > numPoints) {
      string msg = "The number of output files is greater than total number of Control Points";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    
    // Display input ControlNet Stats
    ControlNetStatistics cnetStats(&cNet);
    PvlGroup statsGrp;
    cnetStats.GenerateControlNetStats(statsGrp);
    Application::Log(statsGrp);
    
    // Set the progress
    progress.SetText("Splitting the ControlNet...");
    progress.SetMaximumSteps(numOutputFiles);
    
    int numPointsInOutFile = numPoints / numOutputFiles;
    int numPointsDiff = (numPoints - (numPointsInOutFile * numOutputFiles));
    int startIndex=0 ,endIndex=0;
    for (int i=0; i<numOutputFiles; i++) {
      
      // Check the progress
      progress.CheckStatus();
      
      ControlNet oNet;
      oNet.SetCreatedDate(Application::DateTime());
      oNet.SetDescription(cNet.Description());
      oNet.SetNetworkId(cNet.GetNetworkId());
      oNet.SetTarget(cNet.GetTarget());
      oNet.SetUserName(Application::UserName());
        
      startIndex = endIndex;
      endIndex  += numPointsInOutFile;
      if (numPointsDiff > 0 && i < numPointsDiff)
        endIndex++;
        
      if (endIndex > numPoints) {
        endIndex = numPoints;
      }
      
      for (int j=startIndex; j<endIndex; j++) {
        oNet.AddPoint(new ControlPoint(*cNet[j]));
      }
        
      oNet.Write(outFileID.Next() + ".net");

      if (endIndex >= numPoints) {
        break;
      }
    }
  } // REFORMAT THESE ERRORS INTO ISIS TYPES AND RETHROW
  catch(Isis::iException &e) {
    throw;
  }
}

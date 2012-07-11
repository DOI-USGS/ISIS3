#include "ApolloPanoramicDetectorMap.h"
#include "Table.h"
#include "iTime.h"

using namespace std;
namespace Isis {
  namespace Apollo {

    bool ApolloPanoramicDetectorMap::SetDetector(const double sample, const double line)
    {
      //given a detector coordinate and a time (read from the 'parent' camera class) set the image coordinates
      //save the detector coordinates
      p_detectorLine = line;  //(p_camera->time().Et() - p_etMiddle)/p_lineRate + line;
      p_detectorSample = sample;

      //convert from detector to fiducial coordinants
      double fidL = (p_camera->time().Et() - p_etMiddle)/p_lineRate + line;
      //double fidS = sample;  //conversion is identity so it is skiped

      //convert from fiducial coordinates to parent image coordinates
      io.image2machine(p_detectorSample,fidL,&p_parentSample,&p_parentLine);

      return true;
    }

    bool ApolloPanoramicDetectorMap::SetParent(const double sample,const double line)
    {
      //Given an image (aka 'Parent', aka encoder, aka machine) coordinate set the detector coordiante and the time (time is set in the 'parent' camera class)
      //save the parent data
      p_parentLine = line;
      p_parentSample = sample;
      //convert from image to fiducial coordinates
      io.machine2image(sample,line,&p_detectorSample,&p_detectorLine);
      //convert from fiducial coordinates to detector/time coordinates
      iTime isisTime(p_etMiddle + p_detectorLine*p_lineRate);
      p_camera->setTime(isisTime);
      p_detectorLine = 0.0;  //This declaration may cause some debate.  Regardless it seems to that that since we model the motion of the camera as continuous smooth motion (not some discretely defined series of 1 pixel or 1 mm wide 'push-frames'), and we calculate the positions, pointings, etc at the specific time implied by the sub-pixel/mm line then the line in the dector will always be the same (in this case zero)

      return true;
    }


    int ApolloPanoramicDetectorMap::Initialize_Interior_Orientation()
    {
      int i,nrec;
      //read the fidicial measurements from the attached table in the camera labels to define a series of affine transformation between image (aka encoder aka machine) coordinates and fiducial coordinates
      Table tableFid("Fiducial Measurement",p_lab->FileName());

      nrec = tableFid.Records();  //get the number of records found in the cube blobs

      if( nrec <= 0)
      {
        printf("No FID_MEASURES table found in cube blobs.\n");
        throw IException(IException::User,"No FID_MEASURES table found in cube blobs.\n",_FILEINFO_);
        return -1;
      }

      if( tableFid.Records() < 4)
      {
        printf("Less than four FID_MEASURES found in cube blobs.\n");
        throw IException(IException::User,"Less than four FID_MEASURES found in cube blobs.\n",_FILEINFO_);
        return -1;
      }

      io.initialize();

      for(i=0;i<nrec;i++)  //input all the observations to the the Interior Orientation Class
        io.fiducial_observation(tableFid[i][0],tableFid[i][1],tableFid[i][2]);

      i = io.compute_interior_orienation();

      if( i != 1)  //unsuccess computation of the interior orienation
      {
        printf("Insufficient Fiducial Observations for computation of the interior orientation. At least one vertical pair must be measured, many more is recomented.\n");
        throw IException(IException::User,"Insufficient Fiducial Observations for computation of the interior orientation.\nAt least one vertical pair must be measured, many more is recomented.\n",_FILEINFO_);
        return -1;
      }
      else  //print some summary statistics to the screan
      {
        printf("\nMaximum Residual: %lf (pixels)\n",io.get_maxR());
        printf("Mean of Residuals: %lf (pixels)\n",io.get_meanR());
        printf("Standard deviation of Residuals: %lf (pixels)\n\n",io.get_stdevR());
      }

      return 1;
    }
  }
}

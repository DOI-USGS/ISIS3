#ifndef ApolloPanImage_h
#define ApolloPanImage_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"

#include "ApolloPanTile.h"
#include "DetectedContour.h"
#include "IException.h"
#include "iTime.h"
#include "FileList.h"
#include "Pvl.h"
#include "Statistics.h"

#include <QString>

using namespace cv;
namespace Isis {

  /**
   * This class encapsulates an image from an Apollo panoramic camera image.  Because of their size,
   * Apollo panoramic camera images are separated into 8 tiles numbered 1-8 from right to left.
   * 
   * @author 2016-09-06 Jesse Mapel
   * 
   * @internal
   *   @histroy 2016-09-06 Jesse Mapel - Original version.
   */
  class ApolloPanImage {

  public:
    ApolloPanImage();
    ApolloPanImage(QString imageNumber, int lastTile = 8);
    ~ApolloPanImage();

    void detectTiles();
    void detectTiles(QString imageFileList);
    void decodeTimingMarks();
    void writeToPvl(QString filePrefix);
    void readFromPvl(QString filePrefix, int lastTile = 8);
    void readFromPvl(FileList inputList, int lastTile = 8);
    void matchTiles();
    void numberTimingMarks();
    void numberFiducialMarks(int firstFiducialIndex = 0);
    void computeAffines(QString csvFilename);
    void flagOutliers(double threshold);
    void checkTimeCode();
    void readTimeCode();
    void fillExteriorTimingMarks();
    void computeStartStop();

  private:
    vector<double> computeJenksBreaks(vector<double> inData, size_t classCount);
    vector<int> matchByFiducials(ApolloPanTile tileA, ApolloPanTile tileB, int threshold);
    vector<int> matchByTiming(ApolloPanTile tileA, ApolloPanTile tileB);
    bool checkMatch(ApolloPanTile tileA, ApolloPanTile tileB, int offset);
    vector<int> createTimeCode();

    QString m_imageNumber; //!< The four-digit image number.
    vector<ApolloPanTile> m_tiles; //!< The 8 tiles that make up the image.
  };
};
#endif
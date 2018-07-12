#ifndef ApolloPanTile_h
#define ApolloPanTile_h
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

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "Affine.h"
#include "CSVReader.h"
#include "DetectedContour.h"
#include "IException.h"
#include "iTime.h"
#include "FileName.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "Statistics.h"

#include <QString>

using namespace cv;
namespace Isis {

  /**
   * This class encapsulates a tile from an Apollo panoramic camera image.  Because of their size,
   * Apollo panoramic camera images are separated into 8 tiles numbered 1-8 from right to left.
   * The first tile contains the guages on the right edge and the last tile contains a black region
   * on the left edge.
   * 
   * @author 2016-09-06 Jesse Mapel
   * 
   * @internal
   *   @histroy 2016-09-06 Jesse Mapel - Original version.
   */
  class ApolloPanTile {

  public:
    ApolloPanTile();
    ApolloPanTile(QString imageNumber, int tileNumber);
    ~ApolloPanTile();

    void detectTile();
    void detectTile(FileName imageFileName);
    PvlObject toPvl();
    PvlObject toPvlNew();
    void fromPvl(QString filename);
    void fromPvlNew(QString filename);
    vector<double> jenksData();
    void classifyTimingMarks(vector<double> breaks);
    void numberTimingMarks();
    void numberFiducialMarks(int firstNumber);
    void loadCalibratedFiducials(QString csvFilename);
    void computeAffine();
    void computeResiduals();
    void flagOutliers(double threshold);
    void computeTiming(int flippedSecondStart, double firstSecond);
    void computeStartStop();

    // Accessors
    vector<int> codeSegment();
    int numberOfTimingMarks();
    int numberOfFiducialMarks();
    QString imageNumber();
    TimingMark &timingMark(int index);
    FiducialMark &fiducialMark(int index);
    TimingMark &timingMarkByNumber(int number);
    int timingOffset();
    int sampleOffset();
    int samples();
    int lines();

    // Mutators
    void setTimingOffset(int offset);
    void setSampleOffset(int offset);
    void clearFiducialMarks();
    void clearTimingMarks();
    void addFiducialMark(FiducialMark mark);
    void addTimingMark(TimingMark mark);

  private:
    void detectTimingMarks(Mat &tileData);
    void detectFiducialMarks(Mat &tileData);
    void detectEdges(Mat &tileData);
    void checkMissingTimingMarks();
    void checkMissingFiducialMarks();

    QString m_imageNumber; //!< The four-digit image number.
    int m_tileNumber; //!< The tile number, 1-8 numbered right to left.
    int m_rows; //!< The number of lines in the tile.
    int m_columns; //!< The number of samples in the tile.
    int m_topTrim; //!< How far the image data is from the top of the tile.
    int m_bottomTrim; //!< How far the image data is from the bottom of the tile.
    int m_leftTrim; /**< How far the image data is from the left of the tile.
                         This will be 0 except for tile 8. On tile 8, this will be the start
                         of the last timing mark, as after that we do not know what the exposure
                         time is.**/
    int m_rightTrim; /**< How far the image data is from the right of the tile.
                          This will be 0 except for tile 1. On tile 1, this will be the start of
                          the second timing mark, as before that we do not know what the exposure
                          time is.**/
    int m_timingOffset; /** The offset for the timing marks. The number for the
                            first valid timing mark.**/
    int m_sampleOffset; /** The sample offset for the match to the tile to the left.**/
    double m_startTime; /** The start time for the tile. The time at the right edge of the tile.**/
    double m_stopTime; /** The stop time for the tile. The time at the left edge of the tile.**/
    vector<double> m_transX; /** Coefficients for the x-component of the affine transformation.**/
    vector<double> m_transY; /** Coefficients for the y-component of the affine transformation.**/
    vector<TimingMark> m_timingMarks; /**< The timing marks located on the bottom of the
                                           tile.  These are used to determine the exposure
                                           time for each sample in the image.**/
    vector<FiducialMark> m_fiducialMarks; /**< The fiducial marks located at the top and bottom
                                               of the tile.  These are used to compute and affine
                                               transformation to account for warping of the film
                                               and imperfections in the scanned data.**/
    vector<TimingMark> m_rejectedTimingMarks; /**< Potential timing marks that were rejected
                                                   during the detection process.**/
    vector<FiducialMark> m_rejectedFiducialMarks; /**< Potential fiducial marks that were rejected
                                                       during the detection process.**/
                                                       
    vector<double> m_etimes;
    vector<double> m_exptimes;
    vector<int> m_expSampleTimes;
    PvlKeyword m_leftClockCount;
    PvlKeyword m_rightClockCount;
    PvlKeyword m_leftTime;
    PvlKeyword m_rightTime;                                                       
  };
};
#endif

#ifndef DetectedContour_h
#define DetectedContour_h
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

using namespace cv;
using namespace std;
namespace Isis {

  /**
   * This class is a container used to store detected contours from an Apollo 15 panoramic camera
   * image.
   * 
   * @author 2016-09-05 Jesse Mapel
   * 
   * @internal
   *   @histroy 2016-09-05 Jesse Mapel - Original version.
   */
  class DetectedContour {

  public:
    DetectedContour();
    DetectedContour(Point2f massCenter, Rect boundingRectangle);
    ~DetectedContour();

    bool operator<(const DetectedContour &other) const;

    Point2f massCenter() const;
    Rect boundingRect() const;
    double sample() const;
    double line() const;
    int leftSample() const;
    int rightSample() const;
    int topLine() const;
    int bottomLine() const;
    int length() const;
    int height() const ;
    bool valid() const;
    void setValid(bool validity);
    void setLineOffset(int offset);

  protected:
    Point2f m_massCenter;     /**< The center of mass of the contour.  The sample and line
                                   coordinates for the mass center are stored relative to the
                                   subsection of the image that the marks were detected in.**/
    Rect m_boundingRectangle; /**< The bounding rectangle of the contour.  The sample and line
                                   coordinates of the rectangle are stored relative to the
                                   subsection of the image that the marks were detected in.**/
    int m_lineOffset;         /**< Contours are extracted from subsections of the entire image.
                                   This stores the line offset for that subsection.**/
    bool m_valid;             /**< If the contour is valid.  Contours will be flagged invalid if
                                   they are cut off by the edge of a tile or the edge of an
                                   image.**/
  };

  /**
   * @brief This class contians a timing mark from an Apollo 15 panoramic camera image.
   * 
   * This class allows for storing what number the timing mark is, what value it has,
   * what time it starts, and its exposure time.
   * 
   * @author 2016-09-05 Jesse Mapel
   * 
   * @internal
   *   @histroy 2016-09-05 Jesse Mapel - Original version.
   */
  class TimingMark: public DetectedContour {

  public:
    TimingMark();
    TimingMark(Point2f massCenter, Rect boundingRectangle);
    TimingMark(DetectedContour contour);
    int number() const;
    int value() const;
    double time() const;
    double exposureTime() const;
    void setNumber(int number);
    void setValue(int value);
    void setTime(double time);
    void setExposureTime(double exposureTime);
    void adjustLocation(int startSample, int stopSample, int line);

  private:
    int m_number;          /**< The TimingMark's number in the image,
                                ordered from left to right on the image.**/
    int m_value;           /**< The TimingMark's value in the time code for the image:
                                0 is a short mark, 1 is a medium mark, and 2 is a long mark.**/
    double m_time;         /**< The ephemeris time at which this TimingMark starts in J2000.**/
    double m_exposureTime; /**< The exposure time for samples between the start of this TimingMark
                                and the start of the next TimingMark.  Units are seconds per
                                sample.**/
  };

  /**
  * @brief Container class for a fiducial mark from an Apollo 15 panoramic camera image.
  * 
  * This class allows for storing what number the timing mark is, the expected image space
  * location of the fiducial mark, and the fiducial mark's residual from the affine transformation.
  * 
  * @author 2016-09-05 Jesse Mapel
   * 
   * @internal
   *   @histroy 2016-09-05 Jesse Mapel - Original version.
  */
  class FiducialMark: public DetectedContour {

  public:
    FiducialMark();
    FiducialMark(Point2f massCenter, Rect boundingRectangle);
    FiducialMark(DetectedContour contour);

    bool operator<(const FiducialMark &other) const;

    int number() const;
    double calibratedX() const;
    double calibratedY() const;
    double residualX() const;
    double residualY() const;
    double residualMagnitude() const;
    void setNumber(int number);
    void setCalibratedX(double calValue);
    void setCalibratedY(double calValue);
    void setResidualX(double residual);
    void setResidualY(double residual);
    void computeResidualMagnitude();
    void adjustLocation(double sample, double line);

  private:
    int m_number;               /**< The fiducial mark's number in the image.  Fiducial marks are
                                     numbered from 0 to 89 starting with the top left fiducial
                                     mark being 0, the bottom left fiducial mark being 1, and so
                                     on.  The center fiducial mark pair are numbered 44 for the
                                     top mark and 45 for the bottom mark.**/
    double m_calibratedX;       /**< The expected Y location of the fiducial mark in the entire
                                     image.  Units are mm.**/
    double m_calibratedY;       /**< The expected X location of the fiducial mark in the entire
                                     image.  Units are mm.**/
    double m_residualX;         /**< The X component of the fiducial mark's resicuial vector.
                                     Units are mm.**/
    double m_residualY;         /**< The Y component of the fiducial mark's resicuial vector.
                                     Units are mm.**/
    double m_residualMagnitude; /**< The magnitude of the fiducial mark's residual vector.**/
  };
};
#endif
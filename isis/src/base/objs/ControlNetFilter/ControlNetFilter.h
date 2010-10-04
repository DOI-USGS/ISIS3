#ifndef _CONTROLNETFILTER_H_
#define _CONTROLNETFILTER_H_

#include "ControlNetStatistics.h"
#include <fstream>

using namespace std;

/**
 * @file
 * $Revision: $
 * $Date: $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
 
namespace Isis {
  class ControlNet;
  class ControlPoint;
  class ControlMeasure;

  #define TOTAL_FILTERS 12

  /**
   * @brief Filter Control Network
   *
   * This class is used to filter Control Network based on 
   * different options 
   *
   * @ingroup ControlNetwork
   *
   * @author 2010-08-10 Sharmila Prasad 
   *
   * @see ControlNetwork ControlPoint ControlMeasure
   *
   * @internal
   *  @history 2010-08-10 Sharmila Prasad - Original version
   *  @history 2010-09-16 Sharmila Prasad - Modified prototype for GetImageStatsBySerialNum API
   *                                        in sync with the ControlNetStatistics class
   *  @history 2010-09-27 Sharmila Prasad - Moved ParseExpression functionality to iString class
   *                                        Verify the DefFile in the PVL Class
   *  @history 2010-10-04 Sharmila Prasad - Use iString's Token method instead of ParseExpression(..) 
   */
  class ControlNetFilter : public ControlNetStatistics {
    public:
      //! Constructor
      ControlNetFilter(ControlNet *pCNet, string & psSerialNumFile, Progress *pProgress=0);
      
      //! Destructor
      ~ControlNetFilter();

      // Point Filters
      //! Filter Points by Error Magnitude
      void PointErrorFilter            (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Point ID Expression
      void PointIDFilter               (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Number of measures
      void PointMeasuresFilter         (const PvlGroup & pvlGrp, bool pbLastFilter); 
      
      //! Filter Points by properties
      void PointPropertiesFilter       (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Lat Lon Range
      void PointLatLonFilter           (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by distance between points
      void PointDistanceFilter         (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Measure properties
      void PointMeasurePropertiesFilter(const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Goodness of Fit
      void PointGoodnessOfFitFilter    (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Points by Cube names
      void PointCubeNamesFilter        (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Standard Point stats Header
      void PointStatsHeader            (void);
      
      //! Standard Point Stats
      void PointStats                  (ControlPoint & pcPoint);
      
      // Cube Filters
      //! Filter Cubes by Cube name expression
      void CubeNameExpressionFilter (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Cubes by number of points in the cube
      void CubeNumPointsFilter      (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Filter Cubes by Distance between points in a Cube
      void CubeDistanceFilter       (const PvlGroup & pvlGrp, bool pbLastFilter);
      
      //! Print the standard cube stats Header
      void CubeStatsHeader          (void);
      
      //! Set the output print file 
      void SetOutputFile   (string psPrintFile);
      
      void PrintCubeFileSerialNum(ControlMeasure & pcMeasure);
      
    private:
      ofstream mOstm;                     //!< output stream for printing to output file
      SerialNumberList mSerialNumFilter;  //!< Serial Number List file
  };
}
#endif

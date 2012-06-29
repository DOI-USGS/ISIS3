#ifndef Selection_h
#define Selection_h
/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2011/10/12 12:52:30 $
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

#include "Chip.h"
#include "Statistics.h"

typedef struct Ellipse
{
  double A[3];
  double semiMajor;
  double semiMinor;
  double majorAxis[2];
  double minorAxis[2];
  double cen[2];
  double area;
}Ellipse;

namespace Isis {
  /**                                                                       
   * @brief Pure Virtual Parent Class for all Selection classes
   *                                                                        
   * Create Selection object.  Because this is a pure virtual class you can
   * not create a Selection class directly.  
   *
   * @ingroup Selection
   *                                                                        
   * @author 2011-10-12 Orrin Thomas
   *                                                                        
   * @internal                                                              
   *   @history 2011-10-12 Orrin Thomas - Original version
   *   @history 2012-02-14 Orrin Thomas - add centerOfMassWeighted routine to support using Selection/Centroid for subpixel registration
   */        
  class Selection {
  public:
    Selection();
    virtual ~Selection();

    virtual int select(Chip *inputChip,Chip *selectionChip)=0;  //pure virtual function to be redefined in each child class

    //Reduction methods--methods used to trim off outliers in the selection based on aprior knowledge of the expected shape
    virtual int elipticalReduction(Chip *selectionChip, double percent_selected, double play, int patience_limit);    //a general, but slow elliptical trimming is provided, it is virtual so it can redefined to take advantage of differing levels of apriori knowlege of the characteristics of the ellipse

    //Observation Methods--methods used to reduce a selection to a single sub-pixel observation
    int centerOfMass(Chip *selectionChip, double *sample, double *line);
    int centerOfMassWeighted(Chip *inputChip, Chip *selectionChip, double *sample, double *line);

    //Sub-tasks for elliptical trimming and ellipse fitting
    bool ellipseFrom5Pts(Ellipse *ell,double pts[5][2]);
    double elipsePercentSelected(Chip *selectionChip,Ellipse *ell);
    std::vector<double> minimumBoundingElipse( std::vector< std::vector<int> > pts,Ellipse *ell);  
    bool ellipseFromCubic(Ellipse *ell, double cubic[6]);            
    bool ellipseAxesAreaFromMatrix(Ellipse *ell);
    bool bestFitEllipse(Ellipse *ell, std::vector < std::vector<int> > *pts,double play,unsigned int max_iter);
    bool pointInEllipse(Ellipse *ell, double pt[2],double play);
    bool ellipseInChip(Ellipse *ell, Chip *chip);
    bool ellipseFromCenterAxesAngle(Ellipse *ell, double centerSample, double centerLine, double semiMajor, double semiMinor, double theta);

    void selectionEdge(Chip *selectionChip, std::vector < std::vector <int> > *pts);    
  };
};

#endif

#ifndef SubArea_h
#define SubArea_h
/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/10/15 22:21:19 $
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

#include <string>
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
/**                                                                       
 * @brief  Apply corrections to a cube label for subarea extraction
 *                                                                        
 * This class is used to apply corrections to a cube label when a subarea
 * has been extracted. It is a base class which will take the cube label from
 * the original cube file along with subarea information and generate a
 * corrected cube label for the output cube file. 
 *                                                                        
 * If you would like to see SubArea being used in implementation, see
 * crop, reduce, enlarge, cropspecial, or pad.
 * 
 * @ingroup HighLevelCubeIO                                                  
 *                                                                        
 * @author 2009-10-02 Janet Barrett
 *                                                                        
 */                                                                       

class SubArea {
  public:
    SubArea () {};

    ~SubArea () {};

    // Define the subarea
    void SetSubArea (const int orignl, const int origns, const int sl,
             const int ss, const int el, const int es, 
             const double linc, const double sinc);

    // Create an updated label for a subarea file
    void UpdateLabel(Cube *icube, Cube *ocube, PvlGroup &results);

  private:
    int p_sl;            //!<Starting line of subarea
    int p_ss;            //!<Starting sample of subarea
    int p_el;            //!<Ending line of subarea
    int p_es;            //!<Ending sample of subarea
    int p_nl;            //!<Number of lines in original file
    int p_ns;            //!<Number of samples in original file
    double p_linc;       //!<Line increment for subarea
    double p_sinc;       //!<Sample increment for subarea
  };
};
#endif

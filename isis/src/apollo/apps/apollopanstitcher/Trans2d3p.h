#include "Cube.h"
#include "Transform.h"
#include <math.h>

/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2005/10/03 22:43:39 $                                                                 
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


using namespace Isis;

/**                                                                       
 * @brief Brief coming soon
 *                                                                        
 * Description coming soon
 *                                                                        
 * @author 2011-09-19 Orrin Thomas
 *                                                                        
 * @internal                                                              
 *   @history 2011-09-19 Orrin Thomas - Original version
 */        
class Trans2d3p : public Transform {
public: 
  Trans2d3p(double theta, double sampOffset, double lineOffset,int samples, int lines) {
    m_lines = lines;
    m_samples = samples;
    m_ct = cos(theta);
    m_st = sin(theta);
    m_lineOffset = lineOffset;
    m_sampOffset = sampOffset;
  }

  ~Trans2d3p() {}

  bool Xform(double &inSample, double &inLine, const double outSample, const double outLine) {
    inSample  = outSample*m_ct - outLine*m_st + m_sampOffset;
    inLine    = outSample*m_st + outLine*m_ct + m_lineOffset;

    return true;
  }

  int OutputSamples() const {
    return m_samples;
  }

  int OutputLines() const {
    return m_lines;
  }

private:
  double m_sampOffset;
  double m_lineOffset;
  double m_ct;
  double m_st;
  int m_lines;
  int m_samples;
};

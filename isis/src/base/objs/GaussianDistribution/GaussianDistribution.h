/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/07/09 16:58:08 $                                                                 
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
#ifndef GaussianDistribution_h
#define GaussianDistribution_h

#include "Statistics.h"
#include "iException.h"
#include "Constants.h"

namespace Isis {
/**                                                                       
 * @brief gaussian distribution class
 *                                                                        
 * This class is used to calculate the probability distribution
 * function, the cumulative distribution function, and the
 * inverse cumulative distribution function of a gaussian (or
 * normal) distribution.
 *                                                                        
 * @ingroup Statistics                                                  
 *                                                                        
 * @author 2006-05-25 Jacob Danton
 *                                                                        
 * @internal                                                              
 *   @history 2006-05-25 Jacob Danton Original Version
 *   @history 2007-07-09 Janet Barrett - Removed invalid declaration of
 *                                       "static const double" variables
 *                                       p_lowCutoff and p_highCutoff from
 *                                       this file and moved them to the
 *                                       GaussianDistribution.cpp file. The
 *                                       variables were also renamed to
 *                                       lowCutoff and highCutoff.
 */                                                                       
  class GaussianDistribution : public Isis::Statistics {
  public:
    GaussianDistribution (const double mean = 0.0, const double standardDeviation = 1.0) ;
    ~GaussianDistribution () {};

    double Probability (const double value);
    double CumulativeDistribution (const double value);
    double InverseCumulativeDistribution (const double percent);

    /**
* Returns the mean.
* 
* @returns The mean
*/
    inline double Mean () const { return p_mean;};

    /**
 * Returns the standard deviation.
 * 
 * @returns The standard deviation
 */
    inline double StandardDeviation () const { return p_stdev;};

  private:
    //! Value of the mean
    double p_mean;
    //! Value of the standard deviation
    double p_stdev;

    // functions used for computing the ICDF
    double A (const double x);
    double B (const double x);
    double C (const double x);
    double D (const double x);
  }; 
};

#endif

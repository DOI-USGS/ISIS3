#ifndef QuickFilter_h
#define QuickFilter_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:09 $
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

#include "SpecialPixel.h"

namespace Isis {
/**                                                                       
 * @brief Container for boxcar statistics                
 *                                                                        
 * This class is used to compute statisics for NxM boxcars, where N and M are 
 * positive odd integers. In general, this object will be loaded by another 
 * derived class such as FilterLoader or FilterProcess. The programmer can then 
 * use the methods in this class to compute statistics such as the boxcar 
 * average, variance, and number of valid pixels in the boxcar.
 * 
 * If you would like to see QuickFilter being used in implementation, 
 * see FilterLoader or FilterProcess                                     
 *                                                                        
 * @ingroup Statistics                                                  
 *                                                                        
 * @author 2002-06-20 Jeff Anderson                                    
 *                                                                                                                                               
 * @internal      
 *  @history 2002-07-10 Jeff Anderson - Added the Compute private method and 
 *                                      made it unfold data at the edges as 
 *                                      opposed to using NULL values.
 *  @history 2002-07-15 Jeff Anderson - Added Low and High methods
 *  @history 2002-08-01 Jeff Anderson - Added MinimumPixels method
 *  @history 2003-02-14 Jeff Anderson - Changed unfolding technique to use 
 *                                      symmetry as opposed to mirroring 
 *                                      (i.e., 32123 not 21123)
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2003-08-27 Jeff Anderson - Modified variance method to divide by  
 *                                      (n-1)*n instead of n*n
 *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
 *                                          documentation
 *                                                                        
 *  @todo 2005-02-15  Jeff Anderson - add coded example to class documentation                                                    
 *  @todo 2005-05-23  Jeff Anderson - Unlikely but we may have issues with 2GB+ 
 *                    cubes.  It's unlikely because the boxcar size would
 *                    have to have more than 2^32-1 pixels.
 */                                                                       

  class QuickFilter {
    private:
      double *p_sums;    /**< Sum accumulator for each column/sample. This array  
                              is allocated to the size of p_ns. For each column 
                              in the cube, this array will contain the sum of M 
                              lines at the particular sample position. For 
                              example, if M=3 and we had loaded lines 3,4,5 
                              using AddLine then 
                              p_sum[0] = cube(1,3) + cube(1,4) + cube(1,5), 
                              where cube(sample,line) is the value at the 
                              position in the cube. The sums are used to 
                              compute the average.*/
      double *p_sumsqrs; /**< This is identical to p_sums with the exception 
                              that the pixel values are squared before summing
                              occurs. This of course is needed to compute the
                              variance.*/
      int *p_counts;     /**< This is identical to p_sums with the exception 
                              that it keeps count of the number of valid pixels
                              in p_sums and p_sumsqrs. A valid pixel is 
                              considered to be not special (NULL, LIS, HIS, etc) 
                              and within the range of p_minimum and p_maximum. 
                              Therefore, p_counts[i] for any i is a number
                              between 0 and M (number of lines in the boxcar) 
                              depending on the number of valid pixels.*/
      int p_ns;          /**< This value is initialized in the constructor. It  
                              is set using the value passed into the constructor. 
                              It represents the number of samples across the 
                              image. It will be used to allocate internal 
                              buffers.*/
  
      double p_minimum;    /**< Minimum valid pixel value. It is set to DBL_MIN  
                                in the constructor and can be changed using the 
                                protected method SetMinMax. It is used to 
                                evaluate input pixels to determine if they are  
                                valid. A pixel will be accumulated in p_sums, 
                                p_sumsqrs, and p_counts, if it is within the
                                range of p_minimum and p_maximum.*/
      double p_maximum;    /**< Maximum valid pixel value. It is set to DBL_MAX 
                                in the constructor and can be changed using the 
                                protected method SetMinMax. It is used to 
                                evaluate input pixels to determine if they are  
                                valid. A pixel will be accumulated in p_sums, 
                                p_sumsqrs, and p_counts, if it is within the 
                                range of p_minimum and p_maximum.*/
      int p_minimumPixels; /**< The minimum number of pixels in the boxcar which  
                                must be valid (not special and inside
                                p_minimum/p_maximum) in order for Average and 
                                Variance to compute a value. If there are not 
                                enough valid pixels then those methods will
                                return Isis::NULL8;*/
      int p_width;         /**< This is the width of the boxcar. It is set in 
                                the constructor using a parameter passed into
                                the constructor. It must be positive and odd and 
                                is used in the Average, Variance, and Count 
                                methods.*/
      int p_halfWidth;     /**< This is half the width of the boxcar rounded  
                                down to the next integer since the boxcar is 
                                always odd. For example, p_width=5 implies
                                p_halfWidth=2 (5/2 = 2.5 = 2). It is used for  
                                internal computations within Average, Variance, 
                                and Count methods.*/
      int p_height;        /**< This is the height of the boxcar. It is set in  
                                the constructor using a parameter passed into 
                                the constructor. It must be positive and odd and
                                is used in the Average, Variance, and Count 
                                methods.*/
      int p_halfHeight;    /**< This is half the height of the boxcar rounded  
                                down to the next integer since the boxcar is 
                                always odd. For example, p_height=5 implies
                                p_halfHeight=2 (5/2 = 2.5 = 2). It is used for  
                                internal computations within Average, Variance, 
                                and Count methods.*/
  
      double p_lastSum;     /**< The last sum of a full boxcar. That is using 
                                 the width of the boxcar (assume 3)
                                 p_lastSum = p_sums[0] + p_sums[1] + p_sums[2] 
                                 at sample 2. At sample 3,
                                 p_lastSum = p_sums[1] + p_sums[2] + p_sums[3]. 
                                 If the programmer makes ordered calls to the 
                                 Average, Variance, and/or Count methods we can
                                 speed those routines. For example, 
                                 p_lastSum += p_sums[4] - p_sums[1] would be 
                                 correct for sample 4. This will make a
                                 significant difference in speed if p_width is 
                                 large.*/
      double p_lastSumsqr;  //!< See p_lastSum.
      int p_lastCount;      //!< See p_lastSum.
      int p_lastIndex;      /**< This is used to keep track of the last index  
                                 passed into the Average, Variance and/or count 
                                 method. If the index is the same as p_lastIndex
                                 then computations are trival. If the index has 
                                 increased by one then the next accumulator will  
                                 be added and the first accumulator will be 
                                 removed from p_lastSum, p_lastSumsqr,and  
                                 p_latCount. This speed processing. Others the  
                                 entire boxcar needs to be recomputed. This 
                                 generally happens when a new line is added or
                                 the programmer bounces around with the index.*/
      int p_linesAdded;     /**< This is used to keep track of the number of 
                                 lines added. If the programmer adds more lines 
                                 than the height of the boxcar, an error will be
                                 thrown.*/
  
      void Compute (const int index);
  
    public:
      QuickFilter (const int ns, const int width, const int height);
      ~QuickFilter ();
      
      double Average (const int index);
      double Variance (const int index);
      int Count (const int index);
  
      int Width () const;
      int HalfWidth () const;
      int Height () const;
      int HalfHeight () const;
      int Samples() const;
  
      double Low() const;
      double High() const;
      int MinimumPixels () const;
  
      void AddLine (const double *buf);
      void RemoveLine (const double *buf);
      void Reset ();
      
      void SetMinMax (const double minimum, const double maximum);
      void SetMinimumPixels (const int minimumValid);
  
  };
};

#endif

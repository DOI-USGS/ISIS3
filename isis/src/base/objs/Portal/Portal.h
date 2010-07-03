#if !defined(Portal_h)
#define Portal_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:08 $                                                                 
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

#include <cmath>
#include "PixelType.h"
#include "Buffer.h"

namespace Isis {
/**                                                                       
 * @brief  Buffer for containing a two dimensional section of an image                
 *                                                                        
 * This class is a Buffer. The shape of the buffer is two dimensional in the 
 * line and sample directions only. The band dimension is always one. This 
 * class provides a random access window into a cube. The position can be set 
 * to any line, sample and band, including outside the image.
 *                                                                                                                                      
 * If you would like to see Portal being used in implementation, 
 * see the ProcessRubberSheet class                                     
 *                                                                        
 * @ingroup LowLevelCubeIO                                                  
 *                                                                        
 * @author 2002-10-09 Stuart Sides                                                                            
 *                                                                        
 * @internal                                                              
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2005-02-28 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation                                                      
 */                                                                       
  class Portal : public Isis::Buffer {
  
    private:
      double p_hotSample;  /**<The sample position within the buffer which is 
                               the point of interest. This position is zero 
                               based and has no default.*/
      double p_hotLine;    /**<The line position within the buffer which is the 
                               point of interest. This position is zero based 
                               and has no default.*/
  
    public:
      // Constructors and Destructors

     /** 
      * Constructs a Portal object. The hotspot defaults of (-0.5,-0.5) cause 
      * the nearest neighbor to the requested pixel to be returned in the top 
      * left corner of the portal buffer
      * 
      * @param bufSamps The number of samples in the portal.
      * 
      * @param bufLines The number of lines in the portal.
      * 
      * @param type Type of pixel in raw buffer
      * 
      * @param hotSamp The point of interest within the buffer in the sample 
      *                direction. When a buffer is being setup, the hotSamp 
      *                will be subtracted from the point of interest to give a 
      *                buffer of the requested size around the hot spot. This 
      *                number is zero based. Defaults to 0.5
      * 
      * @param hotLine The point of interest within the buffer in the line 
      *                direction. When a buffer is being setup, the hotLine 
      *                will be subtracted from the point of interest to give a
      *                buffer of the requested size around the hot spot. This 
      *                number is zero based. Defaults to 0.5
      */
      Portal(const int bufSamps, const int bufLines, const Isis::PixelType type,
                 const double hotSamp=-0.5, const double hotLine=-0.5) :
          Isis::Buffer (bufSamps, bufLines, 1, type) {
        p_hotSample = hotSamp;
        p_hotLine = hotLine;
      };
  
      //! Destroys the Portal object
      ~Portal() {};
  
     /** 
      * Sets the line and sample position of the buffer. The hotspot location 
      * is subtracted from this position to set the upper left corner of the 
      * buffer.
      * 
      * @param sample The sample position of the buffer.
      * 
      * @param line The line position of the buffer.
      * 
      * @param band The band position of the buffer.
      */
      inline void SetPosition(const double sample, const double line,
                              const int band) {
        Isis::Buffer::SetBasePosition ((int)floor(sample-p_hotSample),
                                     (int)floor(line-p_hotLine), band);
      };

     /** 
      * Sets the line and sample offsets for the buffer. The defaults of 
      * (-0.5,-0.5) cause the nearest neighbor to the requested pixel to be 
      * returned in the top left corner of the portal buffer
      * 
      * @param sample The sample offset for the buffer. A zero for this 
      *               parameter will cause the buffer to be alligned with the 
      *               hot spot at the left edge of the buffer. Defaults to 0.5
      * 
      * @param line The line offset for the buffer. A zero for this parameter 
      *             will cause the buffer to be alligned with the hot spot at 
      *             the top edge of the buffer. Defaults to 0.5
      */
      inline void SetHotSpot (const double sample=-0.5, const double line=-0.5) {
        p_hotSample = sample;
        p_hotLine = line;
      };
  };
};

#endif


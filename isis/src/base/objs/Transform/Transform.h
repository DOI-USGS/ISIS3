/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:10 $                                                                 
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

#ifndef Transform_h
#define Transform_h

namespace Isis {
/**                                                                       
 * @brief Pixel transformation                
 *                                                                        
 * This class is used as a virtual base class for rubbersheeting (geometric 
 * warping) of image data. In Isis, rubbersheeting is performed by a transform 
 * from output space to input space, where a space could be a disk cube or an 
 * internalized memory cube. In particular, the transform must provide a method 
 * for converting output pixel locations (sample,line) to input pixel locations. 
 * A very simple example of a transform is a translation (shifting the cube 
 * left/right and up/down). More complex transforms can be created such as 
 * rotations, scaling, and converting to a map projection. The Transform class 
 * is "pure virtual" and should never be instantiated but instead used in an 
 * inheriting class. Using the translation example:                              
 * @code                                                                  
 *   class Translate : public Transform {
 *     public: Translate (IsisCube &cube, double sampOffset, double lineOffset) 
 *     {
 *       p_lines = cube.Lines(); 
 *       p_samples = cube.Samples();
 *       p_lineOffset = lineOffset; 
 *       p_sampOffset = sampOffset; 
 *     } 
 *     ~Translate () {}; 
 *     int OutputSamples () { return p_samples; };
 *     int OutputLines() { return p_lines; }; 
 *     void Transform (double &insamp, double &inline, const double outsamp, 
 *                      const double outline) 
 *     {
 *      insamp = outsamp + p_sampOffset; inline = outline + p_lineOffset; 
 *     } 
 *     private: 
 *        double p_sampOffset; 
 *        double p_lineOffset;
 *        int p_lines; 
 *        int p_samples; 
 *   };
 * @endcode 
 * If you are developing an application program whose intent is to geometrically 
 * manipulate the pixels in a cube, you should investigate the RubberSheet 
 * object which will deals nicely with a significant amount of the cube access 
 * and user input. Also, check out other applications which use the RubberSheet 
 * object such as "rotate" or "translate".         
 * 
 * If you would like to see Transform                                     
 * being used in implementation, see transform.cpp                                      
 *                                                                        
 * @ingroup Geometry                                                  
 *                                                                        
 * @author 2002-10-14 Stuart Sides                                                                            
 *                                                                        
 * @internal                                                              
 *  @history 2002-11-14 Stuart Sides - Modified documentation after review 
 *                                     by Jeff Anderson
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2004-06-22 Jeff Anderson - Modified Transform method so that it 
 *                                      was not const
 *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation  
 * 
 *  @todo 2005-02-22 Stuart Sides - finish documentation
 */                                                                       
  class Transform {
    private:
  
    protected:
  
    public:

      //! Constructs a Transform object
      Transform () {};
  
      //! Destroy the Transform object
      virtual ~Transform () {};
  
      // Pure virtual members

     /** 
      * Allows the retrieval of the calculated number of samples in the output 
      * image.
      * 
      * @return int The number of samples in the output image.
      */
      virtual int OutputSamples () const = 0;

     /** 
      * Allows the retrieval of the calculated number of lines in the output 
      * image.
      * 
      * @return int The number of lines in the output image.
      */
      virtual int OutputLines () const = 0;

     /** 
      * Transforms the given output line and sample to the corresponding output 
      * line and sample.
      * 
      * @param inSample The calculated input sample where the output pixel came 
      *                 from.
      * 
      * @param inLine The calculated input line where the output pixel came 
      *               from.
      * 
      * @param outSample The output sample for which an input line and sample is 
      *                  being sought
      *   
      * @param outLine The output line for which an input line and sample is 
      *                being sought
      */
      virtual bool Xform (double &inSample, double &inLine,
                              const double outSample, 
                              const double outLine) =0;
  
  };
};

#endif


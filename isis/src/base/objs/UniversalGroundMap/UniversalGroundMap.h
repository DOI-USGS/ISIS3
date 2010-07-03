#ifndef UniversalGroundMap_h
#define UniversalGroundMap_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.6 $                                                             
 * $Date: 2010/04/29 00:54:15 $                                                                 
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

namespace Isis {
/**                                                                       
 * @brief Universal Ground Map
 *                                                                        
 * ???
 * 
 * @ingroup Geometry
 *                                                                        
 * @author  2005-08-09 Jeff Anderson                                    
 *                                                                        
 * @internal                               
 *  @todo Jeff Anderson - Finish Class Documentation
 *  @history 2005-11-09 Tracie Sucharski Added HasProjection method.
 *  @history 2006-03-20 Elizabeth Miller Added camera, projection, and 
 *                                         Resolution methods and documentation
 *  @history 2006-03-31 Elizabeth Miller Added unitTest
 *  @history 2006-05-17 Elizabeth Miller Depricated CameraManager to 
 *                                         CameraFactory
 *  @history 2007-02-06 Tracie Sucharski Added SetBand method.
 *  @history 2007-02-06 Steven Lambright Fixed documentation
 *  @history 2009-04-24 Steven Koechle Added a check to SetUniversalGround that
 *  		 makes sure the result is on the cube before returning true.
 *  @history 2010-04-09 Sharmila Prasad Added an API to check for camera in an image
 *  @history 2010-04-28 Mackenzie Boyd Fixed dereferencing issue in constructor
 *                                     that takes a cube.
 */                                                                       
  class Camera;
  class Projection;
  class Pvl;
  class Cube;

  class UniversalGroundMap {
    public:
      UniversalGroundMap(Pvl &pvl);
      UniversalGroundMap(Cube &cube);
      ~UniversalGroundMap();

      void SetBand (const int band);
      bool SetUniversalGround (double lat, double lon);
      double Sample () const;
      double Line() const;

      bool SetImage (double sample, double line);
      double UniversalLatitude () const;
      double UniversalLongitude() const;
      double Resolution() const;

     /**
      * Returns whether the ground map has a projection or not
      * 
      * @return bool Returns true if the ground map has a projection, and false
      *              if it does not
      */
      bool HasProjection () { return p_projection != 0; };

	  bool HasCamera() { return p_camera != 0; };

      //! Return the projection associated with the ground map (NULL implies none)
      Isis::Projection *Projection () const { return p_projection; };

      //! Return the camera associated with the ground map (NULL implies none)
      Isis::Camera *Camera () const { return p_camera; };


    private:
      void Init(Pvl &pvl);

      Isis::Camera *p_camera; //!<The camera (if the image has a camera)
      Isis::Projection *p_projection; //!<The projection (if the image is projected)
  };
};

#endif


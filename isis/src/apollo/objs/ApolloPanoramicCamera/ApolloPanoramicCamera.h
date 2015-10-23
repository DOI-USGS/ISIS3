#ifndef ApolloPanoramicCamera_h
#define ApolloPanoramicCamera_h

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

#include "LineScanCamera.h"

namespace Isis {
  class PvlGroup;
    /**                                                                       
     * @brief Apollo Panoramic Camera
     *                                                                        
     * Description: Geometric camera model for the Apollo Panoramic Camera
     *
     * @ingroup Apollo
     *                                                                        
     * @author 2011-09-19 Orrin Thomas
     *                                                                        
     * @internal                                                              
     *   @history 2011-09-19 Orrin Thomas - Original version
     *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
     *                           coding standards. References #972.
     *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
     *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Added new data members and
     *                           methods to get spacecraft and instrument names.
     *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument 
     *                           members and methods and removed implementation of these methods
     *                           since Camera now handles this. References #2335. Fixed 
     *                           indentation.
     */        
    class ApolloPanoramicCamera : public LineScanCamera {
    public:
      ApolloPanoramicCamera(Cube &lab);

      //! Destorys the ApolloPanoramicCamera object
      ~ApolloPanoramicCamera() {};

      /**
      * CK frame ID -  - Instrument Code 
      *  
      * @return @b int The appropriate instrument code for the "Camera-matrix" 
      *         Kernel Frame ID
      */
      //this sensor was used on multiple missions so it is necessary to check which Apollo 
      virtual int CkFrameId() const {return m_CkFrameId; }  

      /** 
      * CK Reference ID - J2000
      * 
      * @return @b int The appropriate instrument code for the "Camera-matrix"
      *         Kernel Reference ID
      */
      virtual int CkReferenceId() const { return (1); }

      /** 
      *  SPK Reference ID - J2000
      *  
      * @return @b int The appropriate instrument code for the Spacecraft 
      *         Kernel Reference ID
      */
      virtual int SpkReferenceId() const { return (1); }

      /** Returns residual summary statistics from interior orientation as a PvlGroup
       *
       */
      PvlGroup intOriResidualsReport();

      //! Max interior orientation residual vector length, accesor
      double intOriResidualMax()   const { return m_residualMax; }

      //! Mean (average) of interior orientation residual vector length, accesor
      double intOriResidualMean()  const { return m_residualMean; }

      //! Standard deviation of interior orientation residual vector length, accesor
      double intOriResidualStdev() const { return m_residualStdev; }

    private:
      //! Max interior orientation residual vector length
      double m_residualMean;

      //! Mean (average) of interior orientation residual vector length
      double m_residualMax;

      //! Standard deviation of interior orientation residual vector length
      double m_residualStdev;

      //! CK "Camera Matrix" kernel frame ID
      int m_CkFrameId;
    };
};

#endif

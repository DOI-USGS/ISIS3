#ifndef ApolloPanoramicDetectorMap_h
#define ApolloPanoramicDetectorMap_h

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


#include "ApolloPanIO.h"
#include "LineScanCameraDetectorMap.h"
#include "Pvl.h"
#include "PvlGroup.h"

namespace Isis {
    /**                                                                       
     * @brief Convert between parent image (aka encoder aka machine)  coordinates and detector 
     *         coordinates
     *                                                                        
     * This class is used to convert between parent image (aka encoder aka machine) coordinates
     * (sample/line) and detector coordinates for a the Apollo Panoramic Image. 
     *                                                                        
     * @author 2011-11-21 Orrin Thomas
     *                                                                        
     * @internal                                                              
     *   @history 2011-11-21 Orrin Thomas - Original version
     *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
     */        
    class ApolloPanoramicDetectorMap : public CameraDetectorMap {
    public:
      /**
       * Construct a detector map for line scan cameras
       *
       * @param parent    The parent Camera Model
       * @param etMiddle  Time of the center line, line 0 after interior orientation
       * @param lineRate  the time in seconds between lines (msec)
       * @param lab The labels to use for the camera creation
       *
       */
      ApolloPanoramicDetectorMap(Camera *parent, double etMiddle,
                                double lineRate, Pvl *lab) : CameraDetectorMap(parent) {
        m_lineRate = lineRate;
        m_etMiddle = etMiddle;
        m_lab = lab;

        this->initializeInteriorOrientation();
      }

      //! Destroys ApolloPanoramicDetectorMap object
      virtual ~ApolloPanoramicDetectorMap() {};

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

      /** Reset the line rate
       *
       * Use this method to reset the time between lines.  Usually this
       * will not need to be done unless the rate changes between bands.
       *
       * @param lineRate the time in seconds between lines
       *
       */
      void SetLineRate(const double lineRate) {
        m_lineRate = lineRate;
      };

      //! Return the time in seconds between scan lines
      double LineRate() const {
        return m_lineRate;
      };

      //! Mean (average) of interior orientation residual vector lengths, accesor
      double  meanResidual() { return p_intOri.meanResiduals(); };

      //! Max interior orientation residual vector length, accesor
      double   maxResidual() { return p_intOri.maxResiduals(); };

      //! Standard deviation of interior orientation residual vector lengths, accesor
      double stdevResidual() { return p_intOri.stdevResiduals(); };

      private:
        //! Ephemeris time at the middle line
        double m_etMiddle;  

        //!line exposure duration
        double m_lineRate;

        //! Image label used to construct camera object    
        Pvl *m_lab;

        //! Class to preform transformations from cube coordiantes to image coordinates
        ApolloPanIO p_intOri;

        int initializeInteriorOrientation();
    };
};
#endif


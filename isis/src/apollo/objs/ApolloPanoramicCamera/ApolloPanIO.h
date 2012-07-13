#ifndef ApolloPanIO_h
#define ApolloPanIO_h

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

namespace Isis {
 /* Numbering scheme for the fiducial marks
  * (gages on the right)
  * :--------------------------------------------------------------------------------------------:0
  * :0  2  4 ....                                                                  ... 84  86  88:0
  * :                                                                                            :0
  * :1  3  5 ....                                                                  ... 85  87  89:0
  * ----------------------------------------------------------------------------------------------0
 */


 /**                                                                       
  * @brief Calculates a series of affine transformations from the measured coordinates of the
  *   up to 90 fiducial marks on a complete (stiched) apollo panoramic cube
  *                                                                        
  * Description: The 90 Fiducial marks effectively divide a complete (stitched) apollo panormaic
  *   cube into 44 rectangular regions.  42 of these represent the same period of time, T, the 
  *   center two are 0.5T.  This class uses up to 44 2 dimensional affine transformations linked
  *   together with first order continuity conditions at the boundaries to enforce consistent
  *   line scan durations and correct warping in the film.
  *   
  * @ingroup Apollo
  *                                                                        
  * @author 2011-09-19 Orrin Thomas
  *                                                                        
  * @internal                                                              
  *   @history 2011-09-19 Orrin Thomas - Original version
  *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
  */        
  class ApolloPanIO
  {
  private:
    /*  C style structure for storing the calculation optimized parameters of a foreward and
     *  reverse 2D affine transformation
     */
    typedef struct Affine2D {
      int flag;      //solve flag
      double A2I[6];    //transformation coefficients to go to image coordinates
      double A2M[6];    //transformation coefficients to go to machine coordinates
      double rotM[2];    //coefficients to rotate machine coordinates so that the right edge of the region is vertical    
      double rotI[2];     //coefficients to rotate Image coordinates so that the right edge of the region is vertical
      double mI;    //max rotated image y coordinate in the region
      double mM;    //max rotate machine y coordinate in the region
      int indeces[2];  //indeces of the first and last fiducial marks included in this region
    }Affine2D;

    //! C Style structure for storing measured fiducial coorinates
    typedef struct FidObs {
      int flag;  //flag to tell if the observation has been made
      double mach[2];  //machine coordinates of the fiducial mark
      double image[2]; //theoretically perfect image coordinates
      double residuals[2];  //residuals of the fiducial mark measurement in machine coordinates
    }FidObs;

    Affine2D affines[44];  //an array of discreet affine transformations pertaining to regions of the image (there can never be more than 44)

    FidObs obs[90];  //array to hold all possible observations of fiducial marks

    int n;   //the number of affines used to model the image

    //! maximum residual vector length
    double maxR;
    //! mean of residual vector lenghts
    double meanR;     
    //! standard deviation of residual vector lengths
    double stdevR;    

    void calc_residual_stats();

  public:
    ApolloPanIO();

    ~ApolloPanIO();

    void initialize();

    int fiducialObservation(int fiducialNumber, double machine_x, double machine_y);

    int clearFiducialObservation(int fiducialNumber);

    int computeInteriorOrienation();

    int machine2Image(double *machineX, double *machineY);

    int machine2Image(double machineX, double machineY, double *imageX, double *imageY);

    int image2Machine(double *imageX, double *imageY);

    int image2Machine(double imageX, double imageY, double *machineX, double *machineY);

    double stdevResiduals();
    double meanResiduals();
    double maxResiduals();
  };
} //end namespace Isis
#endif


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

/* Numbering scheme for the fiducial marks
(gages on the right)
:--------------------------------------------------------------------------------------------:
:0  2  4 ....                                                                  ... 84  86  88:
:                                                                                            :
:1  3  5 ....                                                                  ... 85  87  89:
----------------------------------------------------------------------------------------------
*/

typedef struct affine2d
{
  int flag;      //solve flag

  double A2I[6];    //transformation coefficients to go to image coordinates

  double A2M[6];    //transformation coefficients to go to machine coordinates

  double rotM[2];    //coefficients to rotate machine coordinates so that the right edge of the region is vertical
              
  double rotI[2];     //coefficients to rotate Image coordinates so that the right edge of the region is vertical

  double mI;    //max rotated image y coordinate in the region

  double mM;    //max rotate machine y coordinate in the region

  int indeces[2];  //indeces of the first and last fiducial marks included in this region
}affine2d;


typedef struct fid_obs
{
  int flag;  //flag to tell if the observation has been made

  double mach[2];  //machine coordinates of the fiducial mark

  double image[2]; //theoretically perfect image coordinates

  double residuals[2];  //residuals of the fiducial mark measurement in machine coordinates
}fid_obs;


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
class Apollo_Pan_IO
{
private:

  affine2d affines[44];  //an array of discreet affine transformations pertaining to regions of the image (there can never be more than 44)

  fid_obs obs[90];  //array to hold all possible observations of fiducial marks

   int n;   //the number of affines used to model the image

   double maxR;      //maximum residual in R^2 (residual vecter length for a fiducial point)
   double meanR;     //mean residual in R^2
   double stdevR;    //standard deviation residuals in R^2

   void calc_residual_stats();

public:
  Apollo_Pan_IO();

  ~Apollo_Pan_IO();

  void initialize();

  int fiducial_observation(int fiducial_number, double machine_x, double machine_y);

  int clear_fiducial_observation(int fiducial_number);

  int compute_interior_orienation();

  int machine2image(double *machine_x, double *machine_y);

  int machine2image(double machine_x, double machine_y, double *image_x, double *image_y);

  int image2machine(double *image_x, double *image_y);

  int image2machine(double image_x, double image_y, double *machine_x, double *machine_y);

   double get_stdevR();
   double get_meanR();
   double get_maxR();


};

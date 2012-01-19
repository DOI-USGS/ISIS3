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

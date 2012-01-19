#include "apollo_pan_IO.h"
#include "math.h"
#include "Ransac.h"
#include "stdio.h"
#define FIDL 26.72093  //nominal line spread of fiducials  in mm
#define FIDS 114.9    //nominal sample spread of fiducials in mm


/* Numbering scheme for the fiducial marks
(gages on the right)
:--------------------------------------------------------------------------------------------: O
:0  2  4 ....                                                                  ... 84  86  88: O
:                                                                                            : O
:1  3  5 ....                                                                  ... 85  87  89: O
---------------------------------------------------------------------------------------------- O

Vector from pt 88 to 0 is the positive line (aka positive y) image direction
Vector from pt 1 to 0 is the positive sample direction (aka positive x) image direction
the origin is at the center of the two middle fiducial marks
*/

Apollo_Pan_IO::Apollo_Pan_IO()
{
  this->initialize();
}

Apollo_Pan_IO::~Apollo_Pan_IO()
{
  //empty destructor....
}

void Apollo_Pan_IO::initialize()
{
  int i;

  //zero solved flags in discrete affines
  for(i=0;i<44;i++)
    affines[i].flag=0;

   //initial residuals statistic variable to nonsense values
   maxR  = -1.0;
   meanR = -1.0;
   stdevR= -1.0;

  //zero observed flags in fiducial observations
  for(i=0;i<90;i++)
  {
    obs[i].flag=0;

    //also calculate theoretical image observation of the fiducial marks
    if( i%2 == 0)
      obs[i].image[0] =  FIDS/2;
    else
      obs[i].image[0] = -FIDS/2;

    obs[i].image[1] = (-21.5 + double(int(i/2)))*FIDL;

    //adjustment to account for the half spacing among the 22nd, 23rd, and 24th fiducials on each side
    if(int(i/2)==22)
      obs[i].image[1] -= FIDL/2;
    else if( int(i/2) > 22)
      obs[i].image[1] -= FIDL;
    
    obs[i].image[1] = -obs[i].image[1];  //sign reversal to match camera layout
  }

}

int Apollo_Pan_IO::fiducial_observation(int fiducial_number, double machine_x, double machine_y)
{
  /*
  used to enter/overwrite observations of fiducial marks

  fiducial_number  -> number indicating which fiducial mark, see diagram at the top of the file
  machine_x    -> x machine coordinate
  machine_y    -> y machine coordinate

  returns 1 if the input data is reasonable otherwise -1
  */


  //first check the range of the fiducial number
  if( fiducial_number < 0 || fiducial_number > 89)
    return -1;

  //next check the range of the machine coordinates (this is essentially just a nonsense number check)
  if( fabs(machine_x) > 1e20 )
    return -1;

  if( fabs(machine_y) > 1e20 )
    return -1;

  //if all looks OK record the data
  obs[fiducial_number].mach[0] = machine_x;
  obs[fiducial_number].mach[1] = machine_y;

  //and mark the point observed
  obs[fiducial_number].flag = 1;

  return 1;
}

int Apollo_Pan_IO::clear_fiducial_observation(int fiducial_number)
{
  /* used to null out the observation of a particular fiducial mark

  fiducial_number  -> number indicating which fiducial mark, see diagram at the top of the file
  
  
  returns 1 if the input data is reasonable otherwise -1
  */

  //first check the range of the fiducial number
  if( fiducial_number < 0 || fiducial_number > 89)
    return -1;

  //and mark the point unobserved
  obs[fiducial_number].flag = 0;

  return 1;

}

int Apollo_Pan_IO::compute_interior_orienation()
{
  /*
  after sufficient fiducial observations have been entered this function linearly computes 1-44 sets of affine transformation parameters

  sufficient data is defined as at least the four corner fiducials (numbers 0,1,88, and 89)

  if there is sufficient data the function returns 1 otherwise -1 (it's linear so there are no other failure modes)

  It should be noted that while the only failure mode is insufficient data, that does not mean that a solutions is a good solution.

  Residual summary statistics can be used to assess the quality of a solution
      see   get_maxR()
            get_meanR()
            get_stdevR()
  */

  //check for sufficient data  I had to abandon this requirement becuase some of the Pan images simply didn't have all four corner fiducials
  //if( !obs[0].flag || !obs[1].flag || !obs[88].flag || !obs[89].flag)
  //  return -1;

  int    nu,   //number of unknowns
       i,j,k,l,m;  //indeces-counters for linear algebra computations

  double  cdot[43][4][6],
      angle,              //angle of rotation to vertical for determining rotation coefficients
      det,                //determinate of 2x2 affine matrices used for matrix inversions
      //wcdot[43][4],    //all conditions are identically zero.. no need to store that..
      ndot[44][21],
      cndot[43][4][12],
      cxstar[172],     //cX*  matrix, Uotila course notes page 122
      cnct[14878],     //CNCt matrix, Uotila course notes page 122
      atw[44][6];          //right side of normal equation (see Uotila course notes) NOTE:  I store w with reversed signs compared to Uotila's notes

  double adot[2][6],wdot[2];

      //adot is a sub matrix of A (see Uotila Course notes page 122).  It is composed of partials WRT to unknowns.
  //some affine partials are the same for every point so might as well hard code them now...
  adot[0][2] = 0.0;
  adot[0][3] = 0.0;

  adot[0][4] = 1.0;
  adot[0][5] = 0.0;

  adot[1][0] = 0.0;
  adot[1][1] = 0.0;

  adot[1][4] = 0.0;
  adot[1][5] = 1.0;

  //first order of business... figure out how many affines there will be and which fiducial belong to each

  //the first affine always begins with point zero
  affines[0].indeces[0] =0;
  affines[0].flag = 1;

  for(i=2,n=0;i<90;i+=2)
  {
    // i is counting the fiducials
    // n is counting the affines
    
    // every time both fiducials in a horizontal (equal x coordinate) pair have been measured start a new affine
    if( obs[i].flag && obs[i+1].flag)
    {
      affines[n  ].indeces[1] = i+1;
      affines[n+1].indeces[0] = i  ;
      affines[n+1].flag=1;
      n++;
    }
  }

  //check for sufficient data
  if (n ==0) return -1;

  //now that we know how many affines will be needed we can get started
  nu = 6*n;    //code change 8-19-2010 eroneously 4*n (probably confusion with 2D conformal transformation)

  //solve each affine individually
  
  //for each affine
  for(i =0;i<n;i++)
  {
    //initialize the appropriate ndot and atw matrices
    for(j=0;j<21;j++)
      ndot[i][j] =0.0;

    for(j=0;j<6;j++)
      atw[i][j]=0.0;

    //for each point pertaining to each affine
    for(j=affines[i].indeces[0];j<=affines[i].indeces[1];j++)
    {
      //if the point has been observed use the observation to build the normal
      if(obs[j].flag)
      {
            //store the partials that change...
        adot[0][0] = obs[j].mach[0];
        adot[0][1] = obs[j].mach[1];

        adot[1][2] = obs[j].mach[0];
        adot[1][3] = obs[j].mach[1];

        wdot[0] = obs[j].image[0];
        wdot[1] = obs[j].image[1];


        //build up the appropriate normal equations
        for(k=0;k<6;k++)
          for(l=0;l<=k;l++)
            for(m=0;m<2;m++)
              ndot[i][Isis::isymp(k,l)] += adot[m][k]*adot[m][l];

        for(k=0;k<6;k++)
          atw[i][k] += adot[0][k]*wdot[0] + adot[1][k]*wdot[1];
      }
    }
    
      //copy the atw matrix into the affine class structure so that it isn't lost when the solution is done
    for(j=0;j<6;j++)
      affines[i].A2I[j] = atw[i][j];

      //solve this affine
    if(!Isis::choleski_solve(ndot[i],affines[i].A2I,6,3))  //it must also be inverted for later calculations, hence three instead of two for the last argument
      j=0;
  }

  
   //DEBUG output for testing.  
 //  FILE *OUT;

//  OUT = fopen("out.txt","w");

  /*fprintf(OUT,"diagonal blocks of the AtwA matrix\n");
  for(i=0;i<n;i++)
  {
    for(j=0;j<6;j++)
    {
      for(k=0;k<6;k++)
        fprintf(OUT,"%25.20lf\t",ndot[i][isymp(j,k)]);
      fprintf(OUT,"\n");
    }
    fprintf(OUT,"\n\n\n");
  }

  fprintf(OUT,"Atw Matrix\n");

  for(i=0;i<n;i++)
    for(j=0;j<6;j++)
      fprintf(OUT,"%25.20lf\n",affines[i].A2I[j]);*/

  //for(i=0;i<affines[this->n-1].indeces[1];i++)
  //  fprintf(OUT,"%lf %lf %lf %lf\n",obs[i].image[0],obs[i].image[1],obs[i].mach[0],obs[i].mach[1]);

  //end debug
   

  //now the contributions of the continuity conditions must be added....

   //cdot[i] are sub matrices of C matrix (Uotila page 122)  Note that the cdot matrices are 4x12 (4 continuity conditions between 2 six paramter affines)
      //however the left 4x6 portion of the cdot matrix and right 4x6 portion are equal but opposite so it's only necessary to calc/store one of them
  for(i=0;i<n-1;i++)
  {
    //build cdot matrix for continuity condition between affine i and i+1
    cdot[i][0][0 ] =  obs[affines[i].indeces[1]-1].mach[0];
    cdot[i][0][1 ] =  obs[affines[i].indeces[1]-1].mach[1];
    cdot[i][0][2 ] =  0.0;
    cdot[i][0][3 ] =  0.0;
    cdot[i][0][4 ] =  1.0;
    cdot[i][0][5 ] =  0.0;

    cdot[i][1][0 ] =  0.0;
    cdot[i][1][1 ] =  0.0;
    cdot[i][1][2 ] =  obs[affines[i].indeces[1]-1].mach[0];
    cdot[i][1][3 ] =  obs[affines[i].indeces[1]-1].mach[1];
    cdot[i][1][4 ] =  0.0;
    cdot[i][1][5 ] =  1.0;

    cdot[i][2][0 ] =  obs[affines[i].indeces[1]  ].mach[0];
    cdot[i][2][1 ] =  obs[affines[i].indeces[1]  ].mach[1];
    cdot[i][2][2 ] =  0.0;
    cdot[i][2][3 ] =  0.0;
    cdot[i][2][4 ] =  1.0;
    cdot[i][2][5 ] =  0.0;

    cdot[i][3][0 ] =  0.0;
    cdot[i][3][1 ] =  0.0;
    cdot[i][3][2 ] =  obs[affines[i].indeces[1]  ].mach[0];
    cdot[i][3][3 ] =  obs[affines[i].indeces[1]  ].mach[1];
    cdot[i][3][4 ] =  0.0;
    cdot[i][3][5 ] =  1.0;
  }  

  //calculate cndot sub matrices of Uotila's C(AtM-A)- page122
  for(i=0;i<n-1;i++)
  {
    //initialize the matrix
    for(j=0;j<4;j++)
      for(k=0;k<12;k++)
        cndot[i][j][k] = 0.0;

    for(j=0;j<4;j++)
      for(k=0;k<6;k++)
        for(l=0;l<6;l++)
        {
          cndot[i][j][k  ] += cdot[i][j][l] * ndot[i  ][Isis::isymp(l,k)];
          cndot[i][j][k+6] -= cdot[i][j][l] * ndot[i+1][Isis::isymp(l,k)];          
        }
  }

  //calculate the CX* matrix -page 122
  for(i=0;i<(n-1)*4;i++)
  {
    j= int(i/4);  //which c sub matrix
    k= i%4;      //row of the c sub matrix

    cxstar[i] = 0;

    for(l=0;l<6;l++)
    {
      cxstar[i] -= cdot[j][k][l]*affines[j  ].A2I[l];
      cxstar[i] += cdot[j][k][l]*affines[j+1].A2I[l];
    }
  }

   //initialize CNCt matrix
   for(i=0;i<14878;i++)
      cnct[i] = 0.0;

  //calculate all of the diagonal portions
  for(i=0;i<n-1;i++)
  {
      for(j=0;j<4;j++)
         for(k=0;k<=j;k++) //remembering that this matrix is a memory optimized symmetric matrix
            for(l=0;l<12;l++)
            {
               if(l<6)
                  cnct[Isis::isymp(j+4*i,k+4*i)] += cndot[i][j][l] * cdot[i][k][l];
               else
                  cnct[Isis::isymp(j+4*i,k+4*i)] -= cndot[i][j][l] * cdot[i][k][l-6];
            }
  }

   //calculate all of the diagonal portions
   for(i=0;i<n-2;i++)
   {
      for(j=0;j<4;j++)
         for(k=0;k<4;k++)
            for(l=0;l<6;l++)
               cnct[Isis::isymp(j+i*4,k+4+i*4)] += cndot[i][j][l+6]*cdot[i+1][k][l];

   }


   Isis::choleski_solve(cnct,cxstar,4*(n-1),2);

   //cxstar are now Kc Lagrange multipliers

   //now (CN)tKc = NCtKc are the secondary corrections to the unknowns and they can be added directly to the previously calculated values

   //corrections to the first affine
   for(i=0;i<6;i++)
   {  
      for(j=0;j<4;j++)
         affines[0].A2I[i] += cxstar[j]*cndot[0][j][i];
   }

   //for all the middle affines
   for(i=1;i<n-1;i++)
   {
      for(j=0;j<6;j++)
      {
         for(k=0;k<4;k++)
            affines[i].A2I[j] += cxstar[(i-1)*4+k]*cndot[i-1][k][j+6];
            //temp += cxstar[(i-1)*4+k]*cndot[i-1][k][j+6];
         for(k=0;k<4;k++)
            affines[i].A2I[j] += cxstar[(i  )*4+k]*cndot[i  ][k][j  ];
            //temp += cxstar[(i  )*4+k]*cndot[i  ][k][j  ];

      }

   }

   //corrections for the last affine
   for(i=0;i<6;i++)
   {  
      for(j=0;j<4;j++)
         affines[n-1].A2I[i] += cxstar[4*(n-2)+j]*cndot[n-2][j][i+6];
   }

   
   //now compute the reverse affines
   for(i=0;i<n;i++)
   {
      det = affines[i].A2I[0]*affines[i].A2I[3] - affines[i].A2I[1]*affines[i].A2I[2];

      affines[i].A2M[0] =  affines[i].A2I[3]/det;
      affines[i].A2M[3] =  affines[i].A2I[0]/det;

      affines[i].A2M[1] = -affines[i].A2I[1]/det;
      affines[i].A2M[2] = -affines[i].A2I[2]/det;

      affines[i].A2M[4] = - affines[i].A2M[0]*affines[i].A2I[4] - affines[i].A2M[1]*affines[i].A2I[5];
      affines[i].A2M[5] = - affines[i].A2M[2]*affines[i].A2I[4] - affines[i].A2M[3]*affines[i].A2I[5];

   }

   //now define the rotation coefficients that will transform any image/machine y coordinate into a reference frame 
      //where the two right most fiducials are have the exact same y value and it is thus easy to determine where a
      //point lies on the left side of the line

   double pt1[2], pt2[2],v[2];
   
   for(i=0;i<n;i++)
   {
      //rotations in machine space,

      //the two points obs[affines[i].indeces[1]-1] and obs[affines[i].indeces[1]] define a line

      pt1[0] = obs[affines[i].indeces[1]-1].mach[0];
      pt1[1] = obs[affines[i].indeces[1]-1].mach[1];

      pt2[0] = obs[affines[i].indeces[1]  ].mach[0];
      pt2[1] = obs[affines[i].indeces[1]  ].mach[1];

      
      //find the point on the line closest to the origin

      //vector in the direction of a line
      v[0] = pt1[0] - pt2[0];
      v[1] = pt1[1] - pt2[1];
      
      //using det as the scale factor along the line from pt1 to the point nearest the origin
      det = -(pt1[0]*v[0] + pt1[1]*v[1])/(v[0]*v[0]+v[1]*v[1]);

      //pt1 + det*v is the point closest to the origin and the angle needed to rotate that point so that it lies on the y axis is...
      angle = atan2((pt1[0]+det*v[0]),(pt1[1]+det*v[1]));

      //and since we're only ever going to be concerned with rotating the y coordinates and z's are alwyas zero there are only two values we need to save to elements of the rotation matrix
      affines[i].rotM[0] =  sin(angle);
      affines[i].rotM[1] =  cos(angle);

      //any machine coordinate [u,v] can be rotated into a system where the greatest y values of the fiducial marks are the same,
         //and therefore the greatest y value in a particular affine is

      affines[i].mM = pt1[0]*affines[i].rotM[0] + pt1[1]*affines[i].rotM[1];

      //or equivalently...
      //affines[i].mM = pt2[0]*affines[i].rotM[0] + pt2[1]*affines[i].rotM[1];

     

      //rotations in image space
      
      //for this calculation the transformed Machine (measured coordinates) are used rather than the
         //the theoretical/expected coordinates of the fiducial marks.  They should be close to identical, but it is the affines and therefore the
         //the transformed points that define the actual work space..

      pt1[0] = affines[i].A2I[0] * obs[affines[i].indeces[1]-1].mach[0] + 
               affines[i].A2I[1] * obs[affines[i].indeces[1]-1].mach[1] + 
               affines[i].A2I[4];

      pt1[1] = affines[i].A2I[2] * obs[affines[i].indeces[1]-1].mach[0] + 
               affines[i].A2I[3] * obs[affines[i].indeces[1]-1].mach[1] + 
               affines[i].A2I[5];

      pt2[0] = affines[i].A2I[0] * obs[affines[i].indeces[1]  ].mach[0] + 
               affines[i].A2I[1] * obs[affines[i].indeces[1]  ].mach[1] + 
               affines[i].A2I[4];

      pt2[1] = affines[i].A2I[2] * obs[affines[i].indeces[1]  ].mach[0] + 
               affines[i].A2I[3] * obs[affines[i].indeces[1]  ].mach[1] + 
               affines[i].A2I[5];


      //find the point on the line closest to the origin

      //vector in the direction of a line
      v[0] = pt1[0] - pt2[0];
      v[1] = pt1[1] - pt2[1];

      //using det as the scale factor along the line from pt1 to the point nearest the origin
      det = -(pt1[0]*v[0] + pt1[1]*v[1])/(v[0]*v[0]+v[1]*v[1]);

      //pt1 + det*v is the point closest to the origin and the angle needed to rotate that point so that it lies on the y axis is...
      angle = atan((pt1[0]+det*v[0])/(pt1[1]+det*v[1]));

      //and since we're only ever going to be concerned with rotating the y coordinates and z's are alwyas zero there are only two values we need to save to elements of the rotation matrix
      affines[i].rotI[0] =  sin(angle);
      affines[i].rotI[1] =  cos(angle);

      //any machine coordinate [u,v] can be rotated into a system where greatest y values of the affine border are the same
      //that is the border is a line parrallel to the image x axis, and therefore the greatest y value in a particular affine is

      affines[i].mI = pt1[0]*affines[i].rotI[0] + pt1[1]*affines[i].rotI[1];

      //or equivalently...
      ///affines[i].mI = pt2[0]*affines[i].rotI[0] + pt2[1]*affines[i].rotI[1];

   }






   // all the parameters necessary for the machine2image and image2machin coordinate transformations are now computed and stored






   //calculate and store residuals
   for(i=0;i<90;i++)
   {
      if(obs[i].flag)   //if the point has been observed...
      {
         //convert the theoretical image coordinate to machine space and compare it to the measured coordinate--this way residuals are in pixels
         image2machine(obs[i].image[0],obs[i].image[1],&pt1[0],&pt1[1]);

         obs[i].residuals[0] = pt1[0] - obs[i].mach[0];
         obs[i].residuals[1] = pt1[1] - obs[i].mach[1];
      }
   }

   calc_residual_stats();  //DEBUG

   return 1;
}



int Apollo_Pan_IO::machine2image(double *machine_x, double *machine_y)
{
  /* This class method converts a machine coordinate to an image coordinate (overwriting the input arguments)
      
    machine_x <-> machine x input, overwritten as image_x
    machine_y <-> machine y input, overwritten as image_y

    
  */

  double temp,npt[2];
  int i;

  //first do a domain check to be sure this point is in the last affine
  temp = *machine_x * affines[n-2].rotM[0] + *machine_y * affines[n-2].rotM[1];

  if( temp > affines[n-2].mM )
  {
    i=n-1;
  }
  else //now step through the affines to determine in the domain of which one this point lies
  {
    for(i=0;i<n-1;i++)
    {
      temp = *machine_x * affines[i].rotM[0] + *machine_y * affines[i].rotM[1];

      if( temp <= affines[i].mM)  //if the point is in the domain of this affine do the conversion...
        break;
    }
  }

  npt[0] = affines[i].A2I[0] * *machine_x + affines[i].A2I[1] * *machine_y + affines[i].A2I[4];
  npt[1] = affines[i].A2I[2] * *machine_x + affines[i].A2I[3] * *machine_y + affines[i].A2I[5];

  *machine_x = npt[0];  //overwrite the input
  *machine_y = npt[1];  //overwrite the input

  return 1;    //point successfully converted
}

int Apollo_Pan_IO::machine2image(double machine_x, double machine_y, double *image_x, double *image_y)
{
   /* This class method converts a machine coordinate to an image coordinate
      
      machine_x -> machine x input, overwritten as image_x
      machine_y -> machine y input, overwritten as image_y
      image_x   <- image x coordinate, only changed if conversion successful
      image_y   <- image y coordinate, only changed if conversion successful

      return values  -1  <- y coordinate to be converted is too small, it is not within the domain of the image machine space
                     -2  <- y coordinate to be converted is too large, it is not within the domain of the image machine space
                      0  <- coordinate not within domain of the affines
                      1  <- coordinate successfully converted
   */

   int i;

   *image_x = machine_x;
   *image_y = machine_y;
   
   i = machine2image(image_x,image_y);

   return i;
}

int Apollo_Pan_IO::image2machine(double *image_x, double *image_y)
{
  /* This class method converts a machine coordinate to an image coordinate (overwriting the input arguments)
      
    image_x <-> machine x input, overwritten as machine_x
    image_y <-> machine y input, overwritten as machine_y
  */

  double temp,npt[2];
  int i;

  //first do a domain check if this point is in the last affine
  temp = *image_x * affines[n-2].rotI[0] + *image_y * affines[n-2].rotI[1];

  if( temp < affines[n-2].mI)
    i = n-1;
  else
  {
    //now step through the affines to determine in the domain of which one this point lies
    for(i=0;i<n-1;i++)
    {
      temp = *image_x * affines[i].rotI[0] + *image_y * affines[i].rotI[1];

      if( temp >= affines[i].mI)  //if the point is in the domain of this affine do the conversion...
        break;
    }
  }
  npt[0] = affines[i].A2M[0] * *image_x + affines[i].A2M[1] * *image_y + affines[i].A2M[4];
  npt[1] = affines[i].A2M[2] * *image_x + affines[i].A2M[3] * *image_y + affines[i].A2M[5];

  *image_x = npt[0];  //overwrite the input
  *image_y = npt[1];  //overwrite the input

  return 1;    //point successfully converted
}


int Apollo_Pan_IO::image2machine(double image_x, double image_y, double *machine_x, double *machine_y)
{
   /* This class method converts a machine coordinate to an image coordinate (overwriting the input arguments)
      
      image_x -> machine x input
      image_y -> machine y input
      machine_x   <- to be overwritten as the converted coordinate
      machine_y   <- to be overwritten as the converted coordinate
   */

   *machine_x = image_x;
   *machine_y = image_y;

   return image2machine(machine_x,machine_y);
}


void Apollo_Pan_IO::calc_residual_stats()
{
   // method calculates a few summary statistics for the residuals of the interior orientation calculation
   
    maxR = 0.0;

    meanR = 0.0;

    stdevR = 0.0;

    int i,n=0;

    double l;

    for(i=0;i<90;i++)
    {
      if(obs[i].flag)
      {
         l = sqrt(obs[i].residuals[0]*obs[i].residuals[0] + obs[i].residuals[1]*obs[i].residuals[1]);

         if( l > maxR)
            maxR = l;

         meanR += l;

         n++;
      }
    }

    meanR /=n;

    for(i=0;i<90;i++)
    {
       if(obs[i].flag)
       {
          l = sqrt(obs[i].residuals[0]*obs[i].residuals[0] + obs[i].residuals[1]*obs[i].residuals[1]);

          stdevR += (l-meanR)*(l-meanR);
       }
    }

    stdevR = sqrt(stdevR/(n-1));

   return;
}

double Apollo_Pan_IO::get_stdevR()
{
   //returns the standard deviation of the length of the 2D residual vectors
      //if it hasn't been calucated a nonsense number (-1.0) will be returned
   return stdevR;
}

double Apollo_Pan_IO::get_meanR()
{
   //return the mean of the length of the 2D residual vectors
      //if it hasn't been calucated a nonsense number (-1.0) will be returned
   return meanR;
}

double Apollo_Pan_IO::get_maxR()
{
   //return the maximum of the length of the 2D residual vectors
      //if it hasn't been calucated a nonsense number (-1.0) will be returned
   return maxR;
}

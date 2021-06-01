/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ApolloPanIO.h"

#include <math.h>
#include <iostream>

#include "Ransac.h"
#include "stdio.h"


#define FIDL 5344.186 //nominal line spread of fiducials  in 5 micro pixels
#define FIDS 22980    //nominal sample spread of fiducials in 5 micron pixels

using namespace std;

namespace Isis {

  /**
   * Constructs an ApolloPanIO object
   */
  ApolloPanIO::ApolloPanIO() {
    this->initialize();
  }

  /**
   * Destroys the ApolloPanObject
   */
  ApolloPanIO::~ApolloPanIO() {
    //empty destructor....
  }

  /**
   * Initilizes member variables in preparation for solving for the interior orientation affines
   */
  void ApolloPanIO::initialize() {
    int i;

    //the number of affines to be calculated
    n=0;

    //zero solved flags in discrete affines
    for (i=0; i<44; i++)
      affines[i].flag=0;

     //initial residuals statistic variable to nonsense values
     maxR  = -1.0;
     meanR = -1.0;
     stdevR= -1.0;

    //zero observed flags in fiducial observations
    for (i=0; i<90; i++) {
      obs[i].flag=0;

      //also calculate theoretical image observation of the fiducial marks
      if (i%2 == 0)
        obs[i].image[0] =  FIDS/2;
      else
        obs[i].image[0] = -FIDS/2;

      obs[i].image[1] = (-21.5 + double(int(i/2)))*FIDL;

      //adjustment to account for the half spacing among the 22nd, 23rd, and 24th
      //  fiducials on each side
      if (int(i/2)==22)
        obs[i].image[1] -= FIDL/2;
      else if ( int(i/2) > 22)
        obs[i].image[1] -= FIDL;

      obs[i].image[1] = -obs[i].image[1];  //sign reversal to match camera layout
    }

  }

  /**
   * This method adds a measurement of the center of an apollo panoramic image fiducial mark
   * for consideration in calculation of the image interior orienation.
   *
   * @param fiducialNumber  the index of the fiducial mark [0,89], see ApolloPanIO.h documnetation
   *                        for the numbering sequence
   *
   * @param machineX The x coordinate (sample) of the fiducial mark in cube space
   *
   * @param machineY The y coordinate (line) of the fiducial mark in cube space
   *
   * @return 1  success
   * @return -1 failure, fiducial index invalid, or coordinates too large
   *
   */
  int ApolloPanIO::fiducialObservation(int fiducialNumber, double machineX, double machineY) {
    /*
    used to enter/overwrite observations of fiducial marks

    fiducial_number  -> number indicating which fiducial mark, see diagram at the top of the file
    machineX    -> x machine coordinate
    machineY    -> y machine coordinate

    returns 1 if the input data is reasonable otherwise -1
    */


    //first check the range of the fiducial number
    if (fiducialNumber < 0 || fiducialNumber > 89)
      return -1;

    //next check the range of the machine coordinates
    // (this is just a nonsense number check)
    if (fabs(machineX) > 1e20)
      return -1;

    if (fabs(machineY) > 1e20)
      return -1;

    //if all looks OK record the data
    obs[fiducialNumber].mach[0] = machineX;
    obs[fiducialNumber].mach[1] = machineY;

    //and mark the point observed
    obs[fiducialNumber].flag = 1;

    return 1;
  }

  /**
   * This method removes a measurement of the center of an apollo panoramic image fiducial mark
   * from consideration in calculation of the image interior orienation.
   *
   * @param fiducialNumber  the index of the fiducial mark [0,89], see ApolloPanIO.h documnetation
   *                        for the numbering sequence
   *
   * @return 1  success
   * @return -1 failure, fiducial index invalid
   */
  int ApolloPanIO::clearFiducialObservation(int fiducialNumber) {
    //first check the range of the fiducial number
    if (fiducialNumber < 0 || fiducialNumber > 89)
      return -1;

    //and mark the point unobserved
    obs[fiducialNumber].flag = 0;

    return 1;

  }

  /**
   * This method leverages all the fiducial obersatvions to caculate a series of affine
   * transformations for cube (machine) space into image space.  The quality of solution
   * can be accessed using the residual stats accessors.
   *
   * @return 1  success
   * @return -1 failure, insufficient fiducial measurements
   */
  int ApolloPanIO::computeInteriorOrienation()
  {
    int     i,j,k,l,m;  //indeces-counters for linear algebra computations

    double  cdot[43][4][6],
        angle,              //angle of rotation to vertical for determining rotation coefficients
        det,                //determinate of 2x2 affine matrices used for matrix inversions
        //wcdot[43][4],    //all conditions are identically zero.. no need to store that..
        ndot[44][21],
        cndot[43][4][12],
        cxstar[172],     //cX*  matrix, Uotila course notes page 122
        cnct[14878],     //CNCt matrix, Uotila course notes page 122
        atw[44][6];      //right side of normal equation (see Uotila course notes)
                         // NOTE:  I store w with reversed signs compared to Uotila's notes

    double adot[2][6],wdot[2];

    //adot is a sub matrix of A (see Uotila Course notes page 122).
    //  It is composed of partials WRT to unknowns.
    //some affine partials are the same for every point so might as well hard code them now...
    adot[0][2] = 0.0;
    adot[0][3] = 0.0;

    adot[0][4] = 1.0;
    adot[0][5] = 0.0;

    adot[1][0] = 0.0;
    adot[1][1] = 0.0;

    adot[1][4] = 0.0;
    adot[1][5] = 1.0;

    //first order of business...
    // figure out how many affines there will be and which fiducial measures belong to each

    //the first affine always begins with point zero
    affines[0].indeces[0] =0;
    affines[0].flag = 1;

    for (i=2, n=0;  i<90;  i+=2) {
      // i is counting the fiducials
      // n is counting the affines

      //every time both fiducials in a horizontal (equal x coordinate) pair have been measured start
      //  a new affine
      if (obs[i].flag && obs[i+1].flag) {
        affines[n  ].indeces[1] = i+1;
        affines[n+1].indeces[0] = i  ;
        affines[n+1].flag=1;
        n++;
      }
    }

    //check for sufficient data
    if (n ==0) return -1;

    //now that we know how many affines will be needed we can get started

    //solve each affine individually

    //for each affine
    for (i =0; i<n; i++) {
      //initialize the appropriate ndot and atw matrices
      for (j=0; j<21;j++)
        ndot[i][j] =0.0;

      for (j=0; j<6; j++)
        atw[i][j]=0.0;

      //for each point pertaining to each affine
      for (j=affines[i].indeces[0]; j<=affines[i].indeces[1]; j++) {
        //if the point has been observed use the observation to build the normal
        if(obs[j].flag) {
          //store the partials that change...
          adot[0][0] = obs[j].mach[0];
          adot[0][1] = obs[j].mach[1];

          adot[1][2] = obs[j].mach[0];
          adot[1][3] = obs[j].mach[1];

          wdot[0] = obs[j].image[0];
          wdot[1] = obs[j].image[1];


          //build up the appropriate normal equations
          for (k=0; k<6; k++)
            for (l=0; l<=k; l++)
              for (m=0; m<2; m++)
                ndot[i][Isis::isymp(k,l)] += adot[m][k]*adot[m][l];

          for (k=0; k<6; k++)
            atw[i][k] += adot[0][k]*wdot[0] + adot[1][k]*wdot[1];
        }
      }

      //copy the atw matrix into the affine class structure so that it isn't lost when the solution
      // is done
      for (j=0; j<6; j++)
        affines[i].A2I[j] = atw[i][j];

      //solve this affine
      //NOTE: it must also be inverted for later calculations, hence three instead of two for the
      //  last argument
      if (!Isis::choleski_solve(ndot[i],affines[i].A2I,6,3))
        j=0;
    }


    //now the contributions of the continuity conditions must be added....

     //cdot[i] are sub matrices of C matrix (Uotila page 122)
     //Note: the cdot matrices are 4x12 (4 continuity conditions between 2 six paramter affines)
     //  however the left 4x6 portion of the cdot matrix and right 4x6 portion are equal but
     //  opposite so it's only necessary to calc/store one of them
    for (i=0; i<n-1; i++) {
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
    for (i=0; i<n-1; i++) {
      //initialize the matrix
      for (j=0; j<4; j++)
        for (k=0; k<12; k++)
          cndot[i][j][k] = 0.0;

      for (j=0; j<4; j++)
        for (k=0; k<6; k++)
          for (l=0; l<6; l++) {
            cndot[i][j][k  ] += cdot[i][j][l] * ndot[i  ][Isis::isymp(l,k)];
            cndot[i][j][k+6] -= cdot[i][j][l] * ndot[i+1][Isis::isymp(l,k)];
          }
    }

    //calculate the CX* matrix -page 122
    for (i=0; i<(n-1)*4; i++) {
      j= int(i/4);  //which c sub matrix
      k= i%4;      //row of the c sub matrix

      cxstar[i] = 0;

      for (l=0; l<6; l++) {
        cxstar[i] -= cdot[j][k][l]*affines[j  ].A2I[l];
        cxstar[i] += cdot[j][k][l]*affines[j+1].A2I[l];
      }
    }

     //initialize CNCt matrix
     for (i=0; i<14878; i++)
        cnct[i] = 0.0;

    //calculate all of the diagonal portions
    for (i=0; i<n-1; i++) {
        for (j=0; j<4; j++)
           for (k=0; k<=j; k++) //remember that this matrix is a memory optimized symmetric matrix
              for (l=0; l<12; l++) {
                 if(l<6)
                    cnct[Isis::isymp(j+4*i,k+4*i)] += cndot[i][j][l] * cdot[i][k][l];
                 else
                    cnct[Isis::isymp(j+4*i,k+4*i)] -= cndot[i][j][l] * cdot[i][k][l-6];
              }
    }

     //calculate all of the diagonal portions
     for (i=0; i<n-2; i++) {
        for (j=0; j<4; j++)
           for (k=0; k<4; k++)
              for (l=0; l<6; l++)
                 cnct[Isis::isymp(j+i*4,k+4+i*4)] += cndot[i][j][l+6]*cdot[i+1][k][l];
     }


     Isis::choleski_solve(cnct,cxstar,4*(n-1),2);

     //cxstar are now Kc Lagrange multipliers
     //now (CN)tKc = NCtKc are the secondary corrections to the unknowns and they can be added
     //  directly to the previously calculated values

     //corrections to the first affine
     for (i=0; i<6; i++) {
        for (j=0; j<4;j ++)
           affines[0].A2I[i] += cxstar[j]*cndot[0][j][i];
     }

     //for all the middle affines
     for (i=1; i<n-1; i++) {
        for (j=0; j<6; j++) {
           for (k=0; k<4; k++)
              affines[i].A2I[j] += cxstar[(i-1)*4+k]*cndot[i-1][k][j+6];
           for (k=0; k<4; k++)
              affines[i].A2I[j] += cxstar[(i  )*4+k]*cndot[i  ][k][j  ];
        }
     }

     //corrections for the last affine
     for (i=0; i<6; i++) {
        for (j=0; j<4; j++)
           affines[n-1].A2I[i] += cxstar[4*(n-2)+j]*cndot[n-2][j][i+6];
     }


     //now compute the reverse affines
     for (i=0; i<n; i++) {
        det = affines[i].A2I[0]*affines[i].A2I[3] - affines[i].A2I[1]*affines[i].A2I[2];

        affines[i].A2M[0] =  affines[i].A2I[3]/det;
        affines[i].A2M[3] =  affines[i].A2I[0]/det;

        affines[i].A2M[1] = -affines[i].A2I[1]/det;
        affines[i].A2M[2] = -affines[i].A2I[2]/det;

        affines[i].A2M[4] = - affines[i].A2M[0]*affines[i].A2I[4]
                            - affines[i].A2M[1]*affines[i].A2I[5];
        affines[i].A2M[5] = - affines[i].A2M[2]*affines[i].A2I[4]
                            - affines[i].A2M[3]*affines[i].A2I[5];
     }

     //now define the rotation coefficients that will transform any image/machine y coordinate into
     //  a reference frame where the two right most fiducials are have the exact same y value and
     //  it is thus easy to determine where a point lies on the left side of the line

     double pt1[2], pt2[2],v[2];

     for (i=0; i<n; i++) {
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

        //pt1 + det*v is the point closest to the origin and the angle needed to rotate that
        //  point so that it lies on the y axis is...
        angle = atan2((pt1[0]+det*v[0]),(pt1[1]+det*v[1]));

        //and since we're only ever going to be concerned with rotating the y coordinates and z's
        //  are alwyas zero there are only two values we need to save to elements of the rotation
        //  matrix
        affines[i].rotM[0] =  sin(angle);
        affines[i].rotM[1] =  cos(angle);

        //any machine coordinate [u,v] can be rotated into a system where the greatest y values of
        // the fiducial marks are the same, and therefore the greatest y value in a particular
        // affine is:
        affines[i].mM = pt1[0]*affines[i].rotM[0] + pt1[1]*affines[i].rotM[1];

        //or equivalently...
        //affines[i].mM = pt2[0]*affines[i].rotM[0] + pt2[1]*affines[i].rotM[1];



        //rotations in image space

        //for this calculation the transformed Machine (measured coordinates) are used rather than
        //  the theoretical/expected coordinates of the fiducial marks.  They should be close to
        //  identical, but it is the affines and therefore the transformed points that define the
        //  actual work space.
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

        //pt1 + det*v is the point closest to the origin and the angle needed to rotate that point
        // so that it lies on the y axis is...
        angle = atan((pt1[0]+det*v[0])/(pt1[1]+det*v[1]));

        //and since we're only ever going to be concerned with rotating the y coordinates and z's
        // are alwyas zero there are only two values we need to save to elements of the rotation
        // matrix
        affines[i].rotI[0] =  sin(angle);
        affines[i].rotI[1] =  cos(angle);

        //any machine coordinate [u,v] can be rotated into a system where greatest y values of the
        // affine border are the same that is the border is a line parrallel to the image x axis,
        // and therefore the greatest y value in a particular affine is
        affines[i].mI = pt1[0]*affines[i].rotI[0] + pt1[1]*affines[i].rotI[1];

        //or equivalently...
        ///affines[i].mI = pt2[0]*affines[i].rotI[0] + pt2[1]*affines[i].rotI[1];
     }






     //all the parameters necessary for the machine2image and image2machin coordinate
     //  transformations are now computed and stored






     //calculate and store residuals
     for (i=0; i<90; i++) {
        if (obs[i].flag) {   //if the point has been observed...
           //convert the theoretical image coordinate to machine space and compare it to the
           // measured coordinate--this way residuals are in pixels
           image2Machine(obs[i].image[0],obs[i].image[1],&pt1[0],&pt1[1]);

           obs[i].residuals[0] = pt1[0] - obs[i].mach[0];
           obs[i].residuals[1] = pt1[1] - obs[i].mach[1];
        }
     }

     calc_residual_stats();  //DEBUG

     return 1;
  }

  /**
   * This method does an in place (overwriting input) conversion of a cube coordinate
   * (sample, line) into image coordinates
   * Call after computeInteriorOrienation()
   *
   * @param machineX The x coordinate (sample) in cube space
   *                 overwriten with the image coordinate
   *
   * @param machineY The y coordinate (line) in cube space
   *                 overwriten with the image coordinate
   *
   * @return 1  success
   * @return -1 failure, machine coordinate not within the image
   *
   */
  int ApolloPanIO::machine2Image(double *machineX, double *machineY) {

    double temp,npt[2];
    int i;

    //first do a domain check to be sure this point is in the last affine
    temp = *machineX * affines[n-2].rotM[0] + *machineY * affines[n-2].rotM[1];

    if (temp > affines[n-2].mM) {
      i=n-1;
    }
    else {//now step through the affines to determine in the domain of which one this point lies
      for (i=0; i<n-1; i++) {
        temp = *machineX * affines[i].rotM[0] + *machineY * affines[i].rotM[1];

        //if the point is in the domain of this affine do the conversion...
        if (temp <= affines[i].mM)
          break;
      }
    }
    //cout << "DEBUG: affine index: " << i << endl;
    //cout << "affine " << affines[i].A2I[0] << " " << affines[i].A2I[1] << " "<< affines[i].A2I[2] << " "<< affines[i].A2I[3] << " "<< affines[i].A2I[4] << " "<< affines[i].A2I[5];
    npt[0] = affines[i].A2I[0] * *machineX + affines[i].A2I[1] * *machineY + affines[i].A2I[4];
    npt[1] = affines[i].A2I[2] * *machineX + affines[i].A2I[3] * *machineY + affines[i].A2I[5];

    *machineX = npt[0];  //overwrite the input
    *machineY = npt[1];  //overwrite the input

    return 1;    //point successfully converted
  }

 /**
   * This method does a conversion of a cube coordinates
   * (sample, line) into image coordinates
   * Call after computeInteriorOrienation()
   *
   * @param machineX Input, the x coordinate (sample) in cube space
   *
   * @param machineY Input, the y coordinate (line) in cube space
   *
   * @param imageX Output, the x coordinate (sample) in cube space
   *
   * @param imageY Output, the y coordinate (line) in cube space
   *
   * @return 1  success
   * @return -1 failure, y coordinate to be converted is too small, it is not within the
   *            domain of the image machine space
   * @return -2 failure, y coordinate to be converted is too large, it is not within the
   *            domain of the image machine space
   * @return 0  failure, coordinate not within domain of the affines
   * @return 1  successful conversion
   *
   */
  int ApolloPanIO::machine2Image(double machineX,
                                 double machineY,
                                 double *imageX,
                                 double *imageY) {

     int i;

     *imageX = machineX;
     *imageY = machineY;

     i = machine2Image(imageX,imageY);

     return i;
  }

 /**
   * This method does an in place (overwriting input) conversion of an image coordinate
   * into cube (machine) coordinates (sample, line)
   * Call after computeInteriorOrienation()
   *
   * @param imageX The x coordinate (sample) of in image space cube space
   *                 overwriten with the cube (machine) sample coordinate
   *
   * @param imageY The y coordinate of in image space
   *                 overwriten with the cube machine sample coordinate
   *
   * @return 1  success
   * @return -1 failure, machine coordinate not within the image
   *
   */
  int ApolloPanIO::image2Machine(double *imageX, double *imageY) {

    double temp, npt[2];
    int i;

    //first do a domain check if this point is in the last affine
    temp = *imageX * affines[n-2].rotI[0] + *imageY * affines[n-2].rotI[1];

    if( temp < affines[n-2].mI)
      i = n-1;
    else {
      //now step through the affines to determine in the domain of which one this point lies
      for (i=0; i<n-1; i++) {
        temp = *imageX * affines[i].rotI[0] + *imageY * affines[i].rotI[1];

        //if the point is in the domain of this affine do the conversion...
        if( temp >= affines[i].mI)
          break;
      }
    }
    //cout << "DEBUG: affine index: " << i << endl;
    //cout << "affine " << affines[i].A2M[0] << " " << affines[i].A2M[1] << " "<< affines[i].A2M[2] << " "<< affines[i].A2M[3] << " "<< affines[i].A2M[4] << " "<< affines[i].A2M[5];
    npt[0] = affines[i].A2M[0] * *imageX + affines[i].A2M[1] * *imageY + affines[i].A2M[4];
    npt[1] = affines[i].A2M[2] * *imageX + affines[i].A2M[3] * *imageY + affines[i].A2M[5];

    *imageX = npt[0];  //overwrite the input
    *imageY = npt[1];  //overwrite the input

    return 1;    //point successfully converted
  }

 /**
   * This method does a conversion of an image coordinates
   * to a cube (sample, line) coordinates.
   * Call after computeInteriorOrienation()
   *
   * @param imageX Input, the x coordinate (sample) in cube space
   *
   * @param imageY INput, the y coordinate (line) in cube space
   *
   * @param machineX Output, the x coordinate (sample) in cube space
   *
   * @param machineY Output, the y coordinate (line) in cube space
   *
   * @return 1  success
   * @return -1 failure, y coordinate to be converted is too small, it is not within the
   *            domain of the image machine space
   * @return -2 failure, y coordinate to be converted is too large, it is not within the
   *            domain of the image machine space
   * @return 0  failure, coordinate not within domain of the affines
   * @return 1  successful conversion
   *
   */
  int ApolloPanIO::image2Machine(double imageX,
                                 double imageY,
                                 double *machineX,
                                 double *machineY) {
     /* This class method converts a machine coordinate to an image coordinate
        (overwriting the input arguments)

        imageX -> machine x input
        imageY -> machine y input
        machineX   <- to be overwritten as the converted coordinate
        machineY   <- to be overwritten as the converted coordinate
     */

     *machineX = imageX;
     *machineY = imageY;

     return image2Machine(machineX,machineY);
  }

 /**
   * This method calculates summary statistics for the residual vector lenths for all
   * measured fiducial marks.
   * Call after computeInteriorOrienation()
   *
   */
  void ApolloPanIO::calc_residual_stats() {
      maxR = 0.0;

      meanR = 0.0;

      stdevR = 0.0;

      int i,n=0;

      double l;

      for (i=0; i<90; i++) {
        if (obs[i].flag) {
           l = sqrt(obs[i].residuals[0]*obs[i].residuals[0] +
                    obs[i].residuals[1]*obs[i].residuals[1]);

           if( l > maxR)
              maxR = l;

           meanR += l;

           n++;
        }
      }

      meanR /=n;

      for (i=0; i<90; i++) {
         if(obs[i].flag) {
            l = sqrt(obs[i].residuals[0]*obs[i].residuals[0] +
                     obs[i].residuals[1]*obs[i].residuals[1]);

            stdevR += (l-meanR)*(l-meanR);
         }
      }

      stdevR = sqrt(stdevR/(n-1));

     return;
  }

  /**
   * Accessor for the standard deviation of the residual vector lenghts
   * Call after computeInteriorOrienation() and calc_residual_stats()
   *
   * @return -1 value not yet calculated
   */
  double ApolloPanIO::stdevResiduals() {
    //returns the standard deviation of the length of the 2D residual vectors
       //if it hasn't been calucated a nonsense number (-1.0) will be returned
    return stdevR;
  }

  /**
   * Accessor for the mean (average) of the residual vector lenghts
   * Call after computeInteriorOrienation() and calc_residual_stats()
   *
   * @return -1 value not yet calculated
   */
  double ApolloPanIO::meanResiduals()
  {
    return meanR;
  }

  /**
   * Accessor for the mean (average) of the residual vector lenghts
   * Call after computeInteriorOrienation() and calc_residual_stats()
   *
   * @return -1 value not yet calculated
   */
  double ApolloPanIO::maxResiduals()
  {
    return maxR;
  }
} //end namespace Isis

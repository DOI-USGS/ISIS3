/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

//C++ standard libraries if needed
#include <math.h>
#include <time.h>

//QT libraries if needed if needed

//third party libraries if needed
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_rng.h>
#include <GSLUtility.h>


//Isis Headers if needed
#include "Ransac.h"
#include "Selection.h"


using namespace std;
namespace Isis {
  Selection::Selection() {};
  Selection::~Selection() {};
    

  //Reduction methods--methods used to trim off outliers in the selection based on aprior knowledge of the expected shape
  int Selection::elipticalReduction(Chip *selectionChip, double percent_selected, double play, int patience_limit) {
    /* This method searchs for the largest elipsoid that has at least percent_selected of the pixels within it selected, and lies entirely within the given chip
    *  The purpose of this is to trim off 'hairs' and 'danglies' and thus reduce the data to just what is close to being within the range of the ellipse
    *  The method is general (meaning it works for any orientation of any ellipse within the chip) provided enough of the edge of the ellipse is preserved to define it.
    *
    *  Algorithim:
    *    1 Compile an array of all points on the border of the selected area
    *    2 Pick Five points at random from the border points and use them to define a general conic
    *    3 if the general conics is an elipse do a least squares generization.  Any points within a distance of play pixels to the edge of the ellipse are included in the gernalization. the distance check is repeated for every iteration so the ellipse can effective grow to include more points.
    *    4 If the generization is successfully check to see if the area is at least as great as the current best.
    *    5 If the area is great enough check that the percent sellected is at least percent_selected.
    *    6 If all above tests are passed then we have a new Best ellipse and the number of consecutive emptySets is zeroed.  Otherwise emptySets++
    *    7 repeat steps 2 through 6 until pacience_limit consecquitive failures to find a better (larger area) elipse have occured
    */
    
    int i, j, //=0,k,l,
        samples,  //samples in selectionChip
        lines;    //lines in selectionChip

    std::vector < std::vector<int> > pts;  //vector of pixels on the border of the slection

    double dpt[2], random5[5][2];

    unsigned long int randPt;

    int   emptySets;  //consecutive sets of five points that haven't given a new best ellipse

    gsl_rng *rand_gen = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set( rand_gen, time(NULL) );  //using the current time as a seed value

    
    samples = selectionChip->Samples();
    lines  = selectionChip->Lines();
      
    //STEP 1: finding points along the boundary of the selection
    selectionEdge(selectionChip,&pts);

    Ellipse ellNew,ellBest;  //new and best ellipses
    emptySets =0;
    ellBest.area = 0.0;  //initialize the current best ellipse to zero area  

    while (emptySets < patience_limit) {
      //STEP 2: selecting edge points at random to define ellipses to define a general conic
      for (i=0;i<5;i++) {
        randPt = gsl_rng_uniform_int( rand_gen  ,pts.size());
        random5[i][0] = pts[randPt][0];
        random5[i][1] = pts[randPt][1];
      }

      //STEP 3: Testing to see if it's an ellipse
      if (!ellipseFrom5Pts(&ellNew,random5)) { //if it's not a real ellipse
        emptySets++;
        continue;
      }
      
      //if the center isn't within the chip continue
      if ( ellNew.cen[0] < 1 || ellNew.cen[0] > samples || ellNew.cen[1] < 1 || ellNew.cen[1] > lines ) {
        emptySets++;
        continue;
      }
      
      //STEP 3 continued: attempt to generalize the ellipse to a least squares fit of all the edge points near it
      if (!bestFitEllipse(&ellNew, &pts, play, 50)) {
        emptySets++;
        continue;  //if optimization fails go on to the next five point set
      }

      //STEP 4 If the generization is successfully check to see if the area is at least as great as the current best.
      if (ellNew.area < ellBest.area) {    //if the new elipse isn't at least as big as the current best ellipse there's no point in continueing
        emptySets++;
        continue;  
      }
          

      if (!ellipseInChip(&ellNew,selectionChip)) {  //if the ellipse isn't entirely contained in the chip
        emptySets++;
        continue;  
      }


      //STEP 5 is there a sufficient potion of the ellipse selected?
      if (elipsePercentSelected(selectionChip,&ellNew) < percent_selected) {
        emptySets++;
        continue;  
      }

      //STEP 6 saving the Best ellipse so far
      //printf("Debug:  Best area: %lf  emptySets: %d  \n",ellNew.area,emptySets);
      //we have a new best
      emptySets=0;
      ellBest.area = ellNew.area;
      ellBest.A[0] = ellNew.A[0];
      ellBest.A[1] = ellNew.A[1];
      ellBest.A[2] = ellNew.A[2];

      ellBest.cen[0] = ellNew.cen[0];
      ellBest.cen[1] = ellNew.cen[1];

      ellBest.semiMajor = ellNew.semiMajor;
      ellBest.semiMinor = ellNew.semiMinor;
      ellBest.majorAxis[0] = ellNew.majorAxis[0];
      ellBest.majorAxis[1] = ellNew.majorAxis[1];
      ellBest.minorAxis[0] = ellNew.minorAxis[0];
      ellBest.minorAxis[1] = ellNew.minorAxis[1];
    
    }//end emptySets while loop

    if (ellBest.area == 0)
      return 0;  //no ellipse meeting the selection criteria was found

    //go through and unselect the points outside the trimming ellipse
    for (i=1;i<=samples;i++) {
      for (j=1;j<=lines;j++) {
        dpt[0] = double(i) ;
        dpt[1] = double(j) ;
              
        if (!pointInEllipse(&ellBest,dpt,play)) {  //if the point isn't within play pixles of being within the elipse
          //selectionChip->setValue(i,j,0.0);
          selectionChip->SetValue(i,j,0.0);
        }
        
      }
    }

    return 1;
  }

  


  //Observation Methods--methods used to reduce a selection to a single sub-pixel observation

  int Selection::centerOfMass(Chip *selectionChip, double *sample, double *line) {
    //calculates the unweighted center of mass of all the selected pixels
    unsigned int samples, lines, i, j, n=0;
    *sample=0.0;
    *line=0.0;
    samples = selectionChip->Samples();
    lines =   selectionChip->Lines();

    for (i=1;i<=samples;i++) {
      for (j=1;j<=lines;j++) {
        if (selectionChip->GetValue(i,j) == 1) {
          *sample += i;
          *line   += j;
          n++;
        }
      }
    }

    *sample /= double(n);
    *line   /= double(n);
    return 1;
  }

  int Selection::centerOfMassWeighted(Chip *inputChip, Chip *selectionChip, double *sample, double *line) {
    //this function computes a center of mass, as the average of the coordiantes of the selected pixels in the selectionChip weighted by the DN in the inputChip

    int samples, lines,i, j;
    //make sure the two chips are the same size
    samples = selectionChip->Samples();
    if (inputChip->Samples() != samples) {
      //todo messege
      return 0;
    }

    lines =   selectionChip->Lines();
    if (inputChip->Lines() != lines) {
      //todo messege
      return 0;
    }
       
    
    *sample=0.0;
    *line=0.0;
    double temp,sumDN=0;

    for (i=1;i<=samples;i++) {
      for (j=1;j<=lines;j++) {
        if (selectionChip->GetValue(i,j) == 1) {
          temp = inputChip->GetValue(i,j);
          *sample += double(i)*temp;
          *line   += double(j)*temp;
          sumDN += temp;
        }
      }
    }

    *sample /= sumDN;
    *line   /= sumDN;
    return 1;
  }



  //basic math methods
  std::vector<double> Selection::minimumBoundingElipse(std::vector< std::vector<int> > pts,Ellipse *ell) {
  /*Function finds the minimum bounding elipsoid for pts
    input:  pts  vector of points
    output: definition of elipse (pt-center)'A(pt-center) = 2 for all points on the elipse ( <2 for all points within the elipse )
    Algorithim taken from "Estimation of Correlation Coefficients by Ellipsoidal Trimming".  D. M. Titterington.  Journal of the Royal Statistical Society, Series C (Applied Statistics), Vol. 27, No. 3
  */
  
    std::vector<double> lamda(pts.size(),1.0/double(pts.size()));  //vector of weights for the individual pts

    
    double delta,temp,ptc[2];

    unsigned int niter=0;
    
    unsigned int i;

    do {
      //find the center
      ell->cen[0] = ell->cen[1] = 0.0;
      for (i=0; i<pts.size(); i++) {
        ell->cen[0] += pts[i][0]*lamda[i];
        ell->cen[1] += pts[i][1]*lamda[i];
      }
      
      //calc A sum( transpose(pt)*pt*lamda )
      ell->A[0]=ell->A[1]=ell->A[2]=0.0;
      for (i=0;i<pts.size();i++) {
        ell->A[0] += pts[i][0] * pts[i][0] * lamda[i];
        ell->A[1] += pts[i][0] * pts[i][1] * lamda[i];
        ell->A[2] += pts[i][1] * pts[i][1] * lamda[i];
      }
      //symetric 2x2 inverse
      temp = ell->A[0];
      ell->A[0] = ell->A[2];
      ell->A[2] = temp;
      temp = ell->A[0]*ell->A[2] - ell->A[1]*ell->A[1];
      ell->A[0] /= temp;
      ell->A[2] /= temp;
      ell->A[1] /= -temp;


      //find the updated weights
      delta=0;
      for (i=0; i<pts.size(); i++) {
        temp = lamda[i];
        ptc[0] = pts[i][0] - ell->cen[0];
        ptc[1] = pts[i][1] - ell->cen[1];
        lamda[i] = ( (ptc[0]*ell->A[0] + ptc[1]*ell->A[1] )*ptc[0] + ( ptc[0]*ell->A[1] + ptc[1]*ell->A[2] )*ptc[1]  )*lamda[i]/2.0;  // transpose(pt - center)*A*(pt-center)*lamda/2.0
        delta += ( lamda[i] - temp )*( lamda[i] - temp );
      }

      niter++;
    } while (delta > 1e-10 && niter < 500);

    ell->A[0] /= 2.0;
    ell->A[1] /= 2.0;
    ell->A[1] /= 2.0;
    ell->A[2] /= 2.0;

    ellipseAxesAreaFromMatrix(ell);

    return lamda;  //return the relative weights--this is the relative importance of each point in determining the center of the MBE
  }

  double Selection::elipsePercentSelected(Chip *selectionChip,Ellipse *ell) {
    //Given and elipse definition and a selectionChip find the percentage of the elipse that is selected assuming the whole ellipse is within the chip
    unsigned int lines, samples, i,j, 
                 ellipsePixels=0,
                 ellipsePixelsSelected=0,
                 outsideEllipsePixelsSelected=0;
                 
    double ptc[2];
    lines = selectionChip->Lines();
    samples = selectionChip->Samples();

    //Chip temp(*selectionChip);//debug

    for (i=1; i<=samples; i++) {
      for (j=1; j<=lines; j++) {
        ptc[0] =i;
        ptc[1] =j;
        if (pointInEllipse(ell, ptc, 0.0)) {  //if the point is within the elipse
          ellipsePixels++;  //increment the number of points
          if (selectionChip->GetValue(i,j) == 1) ellipsePixelsSelected++;  //increment the number selected within the ellipse;
          //else  temp.setValue(i,j,3.0);//debug
        }  
        else
          if (selectionChip->GetValue(i,j) == 1) outsideEllipsePixelsSelected++;  //increment the number selected outside the ellipse;
      }
    }
   
    //temp.Write("solutionTemp.cub");//debug

    
    //printf("DEBUG ellipse pixels: %d  selected inside: %d  selected outside: %d  ratio of outside selected %lf\n",ellipsePixels,ellipsePixelsSelected,outsideEllipsePixelsSelected,double(outsideEllipsePixelsSelected)/( double(samples*lines) - double(ellipsePixels) ));
    //if (double(outsideEllipsePixelsSelected)/( double(samples*lines) - double(ellipsePixels) ) > 0.2) return 0;  //if more than 20% of the pixels outside the ellipse are selected return 0 (this is an unexceptable solution)
    if (double(outsideEllipsePixelsSelected)/double(outsideEllipsePixelsSelected+ellipsePixelsSelected) > 0.33) return 0;//if more than a third of the total selected pixels are outside the ellipse return 0 (this avoids returning questionable solutions)'
    //printf("DEBUG: outsideEllipsePixelsSelected: %d   ellipsePixelsSelected: %d\n",outsideEllipsePixelsSelected,ellipsePixelsSelected);
    //printf("DEBUG Percent of ellipse selected: %lf   percent of selected pixels outside ellipse: %lf\n",double(ellipsePixelsSelected)/double(ellipsePixels)*100.0,double(outsideEllipsePixelsSelected)/double(outsideEllipsePixelsSelected+ellipsePixelsSelected))*100.0;getchar();
    return double(ellipsePixelsSelected)/double(ellipsePixels)*100.0;  //return the percent selected
  }


  bool Selection::ellipseFrom5Pts(Ellipse *ell,double pts[5][2]) {
    //this function fits a general conic to five points using a singular value decompisition
    double svdu[6][6];
    svdu[5][0]=svdu[5][1]=svdu[5][2]=svdu[5][3]=svdu[5][4]=svdu[5][5]=0.0;  //extra line to make gsl library happy

    double svdv[6][6],cubic[6],discriminant,delta, svdd[6];

    int i;

    gsl_matrix SVDVT,SVDU;
    gsl_vector SVDD;

    SVDU.size1 = 6;
    SVDU.size2 = 6;
    SVDU.data = &svdu[0][0];
    SVDU.tda = 6;
    SVDU.owner = 0;

    SVDVT.size1 = 6;
    SVDVT.size2 = 6;
    SVDVT.data = &svdv[0][0];
    SVDVT.tda = 6;
    SVDVT.owner = 0;

    SVDD.size = 6;
    SVDD.stride = 1;
    SVDD.data = svdd;
    SVDD.owner =0;

    for (i=0;i<5;i++) {
      svdu[i][3] = pts[i][0];
      svdu[i][4] = pts[i][1];
      svdu[i][5] = 1.0;
      svdu[i][0] = svdu[i][3]*svdu[i][3];
      svdu[i][1] = svdu[i][3]*svdu[i][4];
      svdu[i][2] = svdu[i][4]*svdu[i][4];
    }

    gsl_linalg_SV_decomp_jacobi(&SVDU,&SVDVT,&SVDD);

    //save the general cubic coefficients
    for (i=0;i<6;i++) cubic[i] = svdv[i][5];
    
    //check to see if it is a real ellipse see: http://www.geom.uiuc.edu/docs/reference/CRC-formulas/node28.html     and: http://en.wikipedia.org/wiki/Ellipse
    discriminant = cubic[1]*cubic[1]-4*cubic[0]*cubic[2];
    delta = (cubic[0]*cubic[2]-cubic[1]*cubic[1]/4.0)*cubic[5] + cubic[1]*cubic[4]*cubic[3]/4.0 - cubic[2]*cubic[3]*cubic[3]/4.0 - cubic[0]*cubic[4]*cubic[4]/4.0;
    delta *= cubic[2];
    if( discriminant < 0 && delta < 0) //then it's a real ellipse
      return this->ellipseFromCubic(ell,cubic);

    else return false;  //not a real ellipse

  }

  bool Selection::ellipseFromCubic(Ellipse *ell, double cubic[6]) {
    //This function tests a general cubic to see if it's an ellipse and if it is caculates all members of Ellipse structure
    //check to see if it is a real ellipse see: http://www.geom.uiuc.edu/docs/reference/CRC-formulas/node28.html     and: http://en.wikipedia.org/wiki/Ellipse
    double discriminant,delta;
    discriminant = cubic[1]*cubic[1]-4*cubic[0]*cubic[2];
    delta = (cubic[0]*cubic[2]-cubic[1]*cubic[1]/4.0)*cubic[5] + cubic[1]*cubic[4]*cubic[3]/4.0 - cubic[2]*cubic[3]*cubic[3]/4.0 - cubic[0]*cubic[4]*cubic[4]/4.0;
    delta *= cubic[2];
    if (discriminant > 0 || delta > 0) return false;  //it's not a real ellipse
    double temp;

    ell->cen[0] = (-cubic[4]*cubic[1]+2*cubic[2]*cubic[3])/(cubic[1]*cubic[1] - 4*cubic[0]*cubic[2]);
    ell->cen[1] = (cubic[4] + cubic[1]*ell->cen[0])/(-2*cubic[2]);

    //finish converting new ellipse to general matrix form
    ell->A[0] = cubic[0];
    ell->A[1] = cubic[1]/2.0;
    ell->A[2] = cubic[2];

    //it sounds weird but ell->cen will always be correctly scaled after this calculation but the ellements of A will not... so to correct the scale of A
    temp = (ell->A[0]*ell->cen[0]*ell->cen[0] + 2.0*ell->A[1]*ell->cen[0]*ell->cen[1] + ell->A[2]*ell->cen[1]*ell->cen[1] - cubic[5]);
        
    ell->A[0] /= temp;
    ell->A[1] /= temp;
    ell->A[2] /= temp;
    
    ellipseAxesAreaFromMatrix(ell);

    return true;
  }


  bool Selection::bestFitEllipse(Ellipse *ell, std::vector < std::vector<int> > *pts,double play,unsigned int max_iter) {
    /*Given an initial Ellipse ell (A and cen must be defined) and an array of pts (integers because this was desinged for imagery)
      find an outlier resistent best fit ellipse
    
      Input:
        ell    initial ellipse defintion
        pts    points to be fit--gernerally edge points of a conitinuous selection see Selection::SelectionEdge
        play    any point further than play pixels from the ellipse are ignored.  This is rechecked for every iteration so the ellipse can grow to include more points during the processing
        max_iter  maximum number of iterations before the software gives up

      Output:
        ell    refined ellipse values. NOTE: all members of Ellipse will be recalcuate whether the optimization succeeds or not so check the return value


      Return:
        1  -> success
        0  -> failure

      This is a Gauss-Helemert non-linear least squares adjustment

      Matrices
  a  partials wrt unknowns
  b  partials wrt to measured quantities
  w  constant portion of linearized equations evaluate using estimates of unknown parameters
  p  p covariance matrix of measured quantities
  m  b*p*transpose(b)  which is the propogated covariance of the design equations
  delta  is vector of corrections to estimated unknowns
  v  residual vector

      linearized math model:
  a*delta + b*v = w

      normal equation:
  transpose(a)*inverse(m)*a*delta = transpose(a)*inverse(m)*w
  
      Solution:
  delta = minverse(transpose(a)*inverse(m)*a)*transpose(a)*inverse(m)*w    iterate until corrections in delta are insignificant

      In this case to keep all resdiuals in pixel units and weight each observation identically p is modeled as the identity matrix thus the solution can be built using a, b, m, and w submatrices (indicated with a dot suffix) as follows

      Normal equation:
  sum(transpose(adot)*inverse(mdot)*adot)*delta = sum(transpose(adot)*inverse(mdot)*wdot)  or  ata*delta = atw

      solution:
  delta = inverse(ata)*atw  iterate until corrections in delta are insignificant
    */

    double adot[5],bdot[2],mdot,wdot,ata[15],atf[5],dpt[2];  //variable definitions given in introductory comments above
    unsigned int ni;
    int iterFlag,i,j,k,l;
    //attempt to generalize the ellipse to a least squares fit of all the edge points near it
    ni=0;
    iterFlag=1;
    l=0;
    while (iterFlag && ni<max_iter) {
      //intialize the normal equation
      for(i=0;i<5;i++)
        atf[i] =0.0;
      for(i=0;i<15;i++)
        ata[i] =0.0;
      //printf("Debug a: %lf b: %lf theta: %lf center: %lf %lf\n",ell->semiMajor,ell->semiMinor,ell->majorAxis[0],ell->cen[0],ell->cen[1]);getchar();
      for (i=0,l=0; i<int((*pts).size()); i++) {
        dpt[0] = (*pts)[i][0];  //integer pixel locations coverted to double for calcualations
        dpt[1] = (*pts)[i][1];
        if (dpt[0]>150) continue;  //debug

  //partials wrt measured quantities
        bdot[0] = 2*ell->A[0]*dpt[0] + 2*ell->A[1]*dpt[1] + -2.0*(ell->A[0]*ell->cen[0] + ell->A[1]*ell->cen[1]);  //wrt sample
        bdot[1] = 2*ell->A[2]*dpt[1] + 2*ell->A[1]*dpt[0] + -2.0*(ell->A[1]*ell->cen[0] + ell->A[2]*ell->cen[1]);  //wrt line

        mdot = 1.0 / (bdot[0]*bdot[0] + bdot[1]*bdot[1]); //note this is actually mdot invserse

        wdot = -(     ell->A[0]*dpt[0]*dpt[0] + 
                  2.0*ell->A[1]*dpt[0]*dpt[1] + 
                      ell->A[2]*dpt[1]*dpt[1] + 
                -2.0*(ell->A[0]*ell->cen[0] + ell->A[1]*ell->cen[1])*dpt[0] + 
                -2.0*(ell->A[1]*ell->cen[0] + ell->A[2]*ell->cen[1])*dpt[1] +
                     (ell->A[0]*ell->cen[0]*ell->cen[0] + 2.0*ell->A[1]*ell->cen[0]*ell->cen[1] + ell->A[2]*ell->cen[1]*ell->cen[1] -1.0) );  //linearized objective equation evaluated with estimates of unknown parameters
              
        if (fabs(wdot*sqrt(mdot)) > play) continue; //if the point is more than play pixels (approximately) away from the ellipse then don't include it in the best fit ellipse calculation
        l++;  //counter for number of points included in the best fit ellipse
  
  //partials wrt unknowns
        adot[0] = dpt[0]*dpt[0] + -2.0*ell->cen[0]*dpt[0] + ell->cen[0]*ell->cen[0];          //wrt A[0] ellipse matrix ellement
        adot[1] = 2.0*dpt[0]*dpt[1] - 2.0*ell->cen[1]*dpt[0] -2.0*ell->cen[0]*dpt[1] + 2.0*ell->cen[0]*ell->cen[1];  //wrt A[1] ellipse matrix ellement
        adot[2] = dpt[1]*dpt[1] + -2.0*ell->cen[1]*dpt[1] + ell->cen[1]*ell->cen[1];          //wrt A[2] ellipse matrix ellement
        adot[3] = -bdot[0];  //wrt to center sample coordinate, ellipse.cen[0]
        adot[4] = -bdot[1];  //wrt to center line coordinate, ellipse.cen[1]

  //summing sum(transpose(adot)*inverse(mdot)*adot)
        for (j=0;j<5;j++)
          for (k=0;k<=j;k++)  //because ata is a memory optimized symetric matrix it is stored as a one deminsional array and only 1 of each pair of symetric ellements is calculated, hence k<=j rather than k<= 5
            ata[isymp(j,k)] += adot[j]*mdot*adot[k];  //isymp(j,k) converst the 2d matrix location j,k to the memory optimized 1d location

  //summing sum(transpose(adot)*inverse(mdot)*wdot)
        for (j=0;j<5;j++)
          atf[j] += mdot*adot[j]*wdot;                
      }
      //printf("Debug pts in caculation: %d\n",l);getchar();
      if (l<5)return false;

      //solve for delat
      if (choleski_solve(ata,atf,5,2) != 1) return false;  //calculation is done in place as delta is returned in atf

      //add corections
      ell->A[0] += atf[0];
      ell->A[1] += atf[1];
      ell->A[2] += atf[2];

      ell->cen[0]  += atf[3];
      ell->cen[1]  += atf[4];

      iterFlag = 0;  //assume iterations are complete and then verify
      for (i=0;i<5;i++) {
        if (fabs(atf[i]) > 0.001) {
          iterFlag = 1;
          break;
        }
      }
      ni++;
            

    }   //end of iteration loop for best fit ellipse*/

    //if( fabs( double(l) / double((*pts).size()) ) < .33 ) return false;  //insisting that at least a third of the edge points are involved in the solution

    for (i=0;i<5;i++)
      if (atf[i] != atf[i]) return false;  //nan check, I've never had to do this check before... linux?


    if (ni > max_iter)
      return false;

    return ellipseAxesAreaFromMatrix(ell);  //if the matrix form can't be decomposed as an ellipse return false

  }

  void Selection::selectionEdge(Chip *selectionChip, std::vector < std::vector <int> > *pts) {
    //Populates a vector of 2D points that are on the edge of the selection
      //any selected point with at least one unsellected edge pixel is included
      //It only makes since to use this method for a continuous selection eg a centroid
      //algorithim:  If the center pixel is selected and at least 1 neighboring pixel is not add it to the array.
    std::vector <int> pt(2);
    int i,j,k,l,nUS,lines,samples;
    (*pts).clear();
    samples = selectionChip->Samples();
    lines   = selectionChip->Lines();
    for (i=2;i<samples;i++) {
      for (j=2;j<lines;j++) {
        //calculate nUS (number of unselected) for the square
        for (k=i-1,nUS=0;k<=i+1;k++)
          for (l=j-1;l<=j+1;l++)
            if (selectionChip->GetValue(k,l) == 0 && k != l) nUS++;
  
        if (nUS > 0 && selectionChip->GetValue(i,j) == 1) {   //add it to the array of border pixels
          pt[0] = i;
          pt[1] = j;
          (*pts).push_back(pt);
        }
      }
    }

  }


  bool Selection::ellipseAxesAreaFromMatrix(Ellipse *ell) {
    //several methods solve directly for the ellipse in center matrix form--this function populates the rest of the ellipse structure, axes, area, etc
    //  if the matrix form given isn't actually an ellipse it will return false
    double temp, Ai[3];

    //invert A
    temp = ell->A[0]*ell->A[2] - ell->A[1]*ell->A[1];

    Ai[0] =  ell->A[2]/temp;
    Ai[1] = -ell->A[1]/temp;
    Ai[2] =  ell->A[0]/temp;

    //find the eigen values of the Ai matrix-this can be done simply using the quadratic formula becuase A is 2x2--the sqaure roots of these eigen values are the lengths of the semi axes
    //the numerically stable quadratic equation formula proposed in Numerical Recipies will be used
    temp = -(Ai[0] + Ai[2]);
    temp = -0.5*(temp + ((temp>=0) - (temp<0))*sqrt(temp*temp - 4.0*(Ai[0]*Ai[2]-Ai[1]*Ai[1])));

    ell->semiMajor = temp;  //sqrt delayed until the end.. so these are currently the actual eigen values
    ell->semiMinor = (Ai[0]*Ai[2]-Ai[1]*Ai[1])/temp;

    if (ell->semiMajor < 0 || ell->semiMinor < 0) return false;  //if the matrix equation is actually and ellipse, ell->A and el->A inverse will be positive definite, and positive definite matrices have postive eigen values thus if one is negative this isn't an ellipse

    if (ell->semiMajor < ell->semiMinor) {  //if need be, switch them
      temp= ell->semiMajor;
      ell->semiMajor = ell->semiMinor;
      ell->semiMinor = temp;
    }

    //now find the eigen vectors associated with these values for the axis directions
      //the idea used to solve for these vectors quickly is as follows: Ai*Vector = eigen_value*vector
      // Vector can be any length, and for the system above the length must be held for it to be solvable
      // in 2D this is easily accomplished--without loss of generality--by letting vector = transpose(cos(theta) sin(theta)) where theta is the righthanded angle of the eigen vector wrt to the positve x axis
      // understanding this substitution reduces the eigen vector computation to simply...
    temp = atan2(Ai[0]-ell->semiMajor,-Ai[1]);
    ell->majorAxis[0] = cos(temp);
    ell->majorAxis[1] = sin(temp);

    temp = atan2(Ai[0]-ell->semiMinor, -Ai[1]);
    ell->minorAxis[0] = cos(temp);
    ell->minorAxis[1] = sin(temp);

    ell->semiMajor = sqrt(ell->semiMajor);  //sqrt reduces the eigen values to semi axis lengths
    ell->semiMinor = sqrt(ell->semiMinor);

    //the area of the ellipse is proportional to the product of the semi axes-as
    ell->area = ell->semiMajor*ell->semiMinor*acos(-1.0);
    
    return true;

  }

  bool Selection::ellipseInChip(Ellipse *ell, Chip *chip) {
    //Function determines whether the ellipse is entirely contained within the chip
    //  The axes of the ellipse-not just the Matrix and center-must be defined to use this method  see Selection::ellipseAxesAreaFromMatrix
    double pt[4][2],vec[2],temp;
    int i,samples,lines;


    samples = chip->Samples();
    lines   = chip->Lines();

    if (ell->cen[0] < 1 || ell->cen[0] > samples || ell->cen[1] < 1 || ell->cen[1] > lines) return false;
    
    //four corner points of the chip-translated so the the center of the ellipse is (0,0)
    pt[0][0] = 1-ell->cen[0];
    pt[0][1] = 1-ell->cen[1];

    pt[1][0] = 1-ell->cen[0];
    pt[1][1] = double(lines)-ell->cen[1];

    pt[2][0] = double(samples)-ell->cen[0];
    pt[2][1] = double(lines)-ell->cen[1];

    pt[3][0] = double(samples)-ell->cen[0];
    pt[3][1] = 1-ell->cen[1];


    //four corners rotated into a system where the ellipse major axis is parrallel to the x axis
    // note: theta -> the right handed angle from the positive x axis to the semi major axis
    //    cos(theta) = ell->semiMajor[0]
    //    sin(theta) = ell->semiMajor[1]
    //  thus rotations are-
    temp = pt[0][0];
    pt[0][0] =  ell->majorAxis[0] * temp + ell->majorAxis[1] * pt[0][1];
    pt[0][1] = -ell->majorAxis[1] * temp + ell->majorAxis[0] * pt[0][1];
    temp = pt[1][0];
    pt[1][0] =  ell->majorAxis[0] * temp + ell->majorAxis[1] * pt[1][1];
    pt[1][1] = -ell->majorAxis[1] * temp + ell->majorAxis[0] * pt[1][1];

    temp = pt[2][0];
    pt[2][0] =  ell->majorAxis[0] * temp + ell->majorAxis[1] * pt[2][1];
    pt[2][1] = -ell->majorAxis[1] * temp + ell->majorAxis[0] * pt[2][1];

    temp = pt[3][0];
    pt[3][0] =  ell->majorAxis[0] * temp + ell->majorAxis[1] * pt[3][1];
    pt[3][1] = -ell->majorAxis[1] * temp + ell->majorAxis[0] * pt[3][1];

    //coordinates scaled so as to be in a system where the ellipse is a unit circle
    for (i=0;i<4;i++) {
      pt[i][0] /= ell->semiMajor;
      pt[i][1] /= ell->semiMinor;
    }

    //now check the distance between the four lines around the edge of the chip and the center of the ellipse
    //line from pt[0] to pt[1]
    vec[0] = pt[1][0] - pt[0][0];
    vec[1] = pt[1][1] - pt[0][1];
    temp = sqrt(vec[0]*vec[0] + vec[1]*vec[1]);
    vec[0] /= temp;
    vec[1] /= temp;

    temp = fabs(vec[0]*pt[0][1] - vec[1]*pt[0][0]);  //length of vec cross pt
    if (temp < 1.0) return false;

    //line from pt[1 to pt[2]
    vec[0] = pt[2][0] - pt[1][0];
    vec[1] = pt[2][1] - pt[1][1];
    temp = sqrt(vec[0]*vec[0] + vec[1]*vec[1]);
    vec[0] /= temp;
    vec[1] /= temp;

    temp = fabs(vec[0]*pt[1][1] - vec[1]*pt[1][0]);  //length of vec cross pt

    if (temp < 1.0) return false;

    //line from pt[2] to pt[3]
    vec[0] = pt[3][0] - pt[2][0];  
    vec[1] = pt[3][1] - pt[2][1];
    temp = sqrt(vec[0]*vec[0] + vec[1]*vec[1]);
    vec[0] /= temp;
    vec[1] /= temp;

    temp = fabs(vec[0]*pt[2][1] - vec[1]*pt[2][0]);  //length of vec cross pt

    if (temp < 1.0) return false;

    //line from pt[3] to pt[0]
    vec[0] = pt[0][0] - pt[3][0];
    vec[1] = pt[0][1] - pt[3][1];
    temp = sqrt(vec[0]*vec[0] + vec[1]*vec[1]);
    vec[0] /= temp;
    vec[1] /= temp;

    temp = fabs(vec[0]*pt[3][1] - vec[1]*pt[3][0]);  //length of vec cross pt
    if (temp < 1.0) return false;

    return true;
  }

  bool Selection::pointInEllipse(Ellipse *ell, double pt[2], double play) {
    //is a point withing a distance of play of being inside an ellipse
    //  ellipse matrix (A) and center (cen) must be defined
    //  not linear approximates of the ellipse function are used... thus the play is approximate
    double bdot[2],mdot,wdot;
        
    bdot[0] = 2*ell->A[0]*pt[0] + 2*ell->A[1]*pt[1] + -2.0*(ell->A[0]*ell->cen[0] + ell->A[1]*ell->cen[1]);
    bdot[1] = 2*ell->A[2]*pt[1] + 2*ell->A[1]*pt[0] + -2.0*(ell->A[1]*ell->cen[0] + ell->A[2]*ell->cen[1]);
    mdot = 1.0 / (bdot[0]*bdot[0] + bdot[1]*bdot[1]); //note this is actually mdot invserse
        
    wdot = (       ell->A[0]*pt[0]*pt[0] + 
               2.0*ell->A[1]*pt[0]*pt[1] + 
                   ell->A[2]*pt[1]*pt[1] + 
             -2.0*(ell->A[0]*ell->cen[0] + ell->A[1]*ell->cen[1])*pt[0] + 
             -2.0*(ell->A[1]*ell->cen[0] + ell->A[2]*ell->cen[1])*pt[1] +
                  (ell->A[0]*ell->cen[0]*ell->cen[0] + 2.0*ell->A[1]*ell->cen[0]*ell->cen[1] + ell->A[2]*ell->cen[1]*ell->cen[1] -1.0) );
              
    if (wdot*sqrt(mdot) > play) return false;  //if the point isn't within play pixles of being within the elipse  
    
    return true;

  }

  bool Selection::ellipseFromCenterAxesAngle(Ellipse *ell, double centerSample, double centerLine, double semiMajor, double semiMinor, double theta) {
    /*defines an ellipse from basic descritors
      
      Input
        centerSample  center sample (x) coordinate
        centerLine  center line (y) coordinate
        semiMajor  length of semiMajor axis
        semiMinor  length of semiMinor axis
        theta    right handed angle between the positive x-axis and the semiMajor axis    
    */

    if (semiMajor < semiMinor) return false;

    double Ai[3],temp; //inverse of ell->A

    ell->semiMajor = semiMajor;
    ell->semiMinor = semiMinor;
    ell->majorAxis[0] = cos(theta);
    ell->majorAxis[1] = sin(theta);
    ell->minorAxis[0] = -ell->majorAxis[1];
    ell->minorAxis[1] = -ell->majorAxis[0];
    ell->cen[0] = centerSample;
    ell->cen[1] = centerLine;

    Ai[0] = ell->majorAxis[0]*ell->majorAxis[0]*ell->semiMajor*ell->semiMajor + ell->majorAxis[1]*ell->majorAxis[1]*ell->semiMinor*ell->semiMinor;
    Ai[2] = ell->majorAxis[1]*ell->majorAxis[1]*ell->semiMajor*ell->semiMajor + ell->majorAxis[0]*ell->majorAxis[0]*ell->semiMinor*ell->semiMinor;
    
    Ai[1] = ell->majorAxis[0]*ell->majorAxis[1]*(ell->semiMajor*ell->semiMajor - ell->semiMinor*ell->semiMinor);

    temp = Ai[0]*Ai[2] + Ai[1]*Ai[1];

    ell->A[0] =  Ai[2]/temp;
    ell->A[1] = -Ai[1]/temp;
    ell->A[2] =  Ai[0]/temp;

    ell->area = semiMajor*semiMinor*acos(-1.0);

    return true;    
  }

}

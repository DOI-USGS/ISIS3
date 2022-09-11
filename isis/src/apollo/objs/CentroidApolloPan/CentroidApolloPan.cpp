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
#include <gsl/gsl_rng.h>
#include "GSLUtility.h"

//Isis Headers if needed
#include "CentroidApolloPan.h"


using namespace std;

namespace Isis {

  /**
   * Constructs a CentroidApolloPan object.
   *
   * @param pixelSizeMicrons The pixel size in microns
   */
  CentroidApolloPan::CentroidApolloPan(double pixelSizeMicrons)
  {
    if( pixelSizeMicrons > 0)
      this->m_pixelSize = pixelSizeMicrons;
    else
      this->m_pixelSize = 5.0;  //if a negative or zero value is passed set the default
  }


  /**
   * Destroys the CentroidApolloPan object
   */
  CentroidApolloPan::~CentroidApolloPan(){};

  /**
   * Set the pixel size in microns
   *
   * @param microns The pixel size value to set
   *
   * @return bool Returns false if microns is <= 0, else true.
   */
  bool CentroidApolloPan::setPixelSize(double microns)
  {
    if( microns <= 0)return false;
      this->m_pixelSize = microns;
    return true;
  }

  /**
   * Given a range of DN this function creates a biniary chip for all continuous pixels that have
   * the DN within the specified range using the center pixel of the chip as the seed value
   *
   * @param inputChip Chip centered around some centroidable feature (dark or light continous
   *                  block of pixels)
   * @param[out] selectionChip Binary chip of selected and unselected pixels
   *
   * @return int Returns 0 if the input chips aren't 2D, else return 1
   */
  int CentroidApolloPan::selectAdaptive(Chip *inputChip,Chip *selectionChip) {
    /*
    Given a range of DN this function creates a biniary chip for all continuous pixels that have the
    DN within the specified range using the center pixel of the chip as the seed value
    input:
      m_minDN,m_maxDN  set using the this->SetDNRange(..)
      inputChip  chip centered around some centroidable feature (dark or light continous block of
      pixels)

    output:
      selectionChip  binary chip of selected and unselected pixels

    */

    //check the sizes of the chips and make the selectionChip match the inputChip
    int lines, samples;

    int i,j,k,l,step;

    double minDN,maxDN,temp;

    vector< double> dn;

    lines = inputChip->Lines();
    samples = inputChip->Samples();
    //printf("DEBUG: samples: %d  lines: %d\n",samples,lines);

    if (lines <= 0 || samples <= 0)
      return 0;  //abort if the input chips isn't 2D

    //check DN's around the edge, we want this centroid to stop before the edge of the chip
    minDN = this->getMinDN();
    maxDN = this->getMaxDN();

    //build an array of all the DN around the border of the chip
    for (i=1;i<=lines;i++) {
      if (i == 1 || i == lines) {
        step = 1;
      }
      else {
        step = samples-1;
      }
      for(j=1;j<=samples;j+=step) {
        dn.push_back(inputChip->GetValue(j,i));
      }
    }

    //find the 90 percentile of dn
    j = dn.size();
    for (i=0;;i++) {
      //find the largest ellement of dn
      l=0;
      temp = dn[0];
      for (k=1;k<int(dn.size());k++) {
        if (dn[k] > temp) {
          temp = dn[k];
          l =k;
        }
      }
      //erase the largest ellement of dn or quit
      if (i < int(j/10)) dn.erase(dn.begin()+l);
      else break;
    }

    if( temp > minDN) this->setDNRange(temp,maxDN);  //set updated DN range

    //printf("DEBUG: minDN: %lf\n",temp); getchar();

    this->select(inputChip,selectionChip);

    this->setDNRange(minDN,maxDN); //restore original DN range

    return 1;
  }


  /**
   * This method will take advantage of all the apriori knowlege we have of the size and orientation
   * of the ellipse to speed up elliptical reduction.
   *
   * The general elliptical reduction tool provided in the selection class is slow for the very
   * large and potentially noisy ellipses of the apollo pan data.
   *
   * Specifically we know:
   *    semiMajor Axis,a, is approximately parrallel to the sample chip axis and is
   *      approximately 60 5-micron pixels long
   *    semiMinor Axis,b, is approximately parrallel to the   line chip axis and is
   *      approximately 60 5-micron pixels long
   *
   * Hence we know the center of the ellipse is on the range [a+1,Samples-a],[b+1,lines-b]
   * (because the entire ellipse must be within the chip)
   *
   * this->pixel_size will be used to do any scaling neccessary
   *
   * Algorithim:
   *    Step 1: Compile an array of all points on the border of the selected area
   *    Step 2: Choose a previously unused hypothesis center point from the range
   *      [a+1,samples-a],[b+1,lines-b]
   *    Step 3: For a given hypothesized ellipse center in the search set, define the Ellipse.
   *    Step 4: Do a least squares generization.  Any points within a distance of play pixels to
   *      the edge of the ellipse are included in the gernalization. The distance check is repeated
   *      for every iteration so the ellipse can effective grow to include more points.
   *    Step 5: If the generization is successfully check to see if the area is at least as great as
   *      the current best, and that the ellipse is contained in the chip
   *    Step 6: If the area is great enough check that the percent selected is at least
   *      percent_selected.
   *    Step 7: If all above tests are passed then we have a new Best ellipse and the number of
   *      consecutive emptySets is zeroed.  Otherwise emptySets++
   *    Step 8: repeat steps 2 through 7 until pacience_limit consecquitive failures to find a
   *      better (larger area) elipse have occured
   *
   * @param selectionChip Binary chip of selected and unselected pixels
   * @param percent_selected Minimum percent of selected points
   * @param play Distance allowed in the least squares generization
   * @param patience_limit Maximum number of empty sets allowed
   *
   * @return int Return 0 if there isn't an ellipse that meets the selection criteria, else return 1
   */
  int CentroidApolloPan::elipticalReduction(Chip *selectionChip, double percent_selected,
                                            double play, int patience_limit)
  {
    int i,j,k,l,
        emptySets,  //number of consecutive failures to find a better ellipse
        randPt,    //random potential center point selection
        samples,  //samples in selectionChip
        lines;    //lines in selectionChip

    std::vector< std::vector<int> > pts;      //vector of pixels on the border of the slection
    std::vector< std::vector<double> >  cen_pts;    //vector of possible center pixels

    std::vector<double> pt(2);  //individual pixel coordinate

    Ellipse ellNew,ellBest;  //new and best ellipses

    double  a = 60.0*5.0/m_pixelSize,  //approximate semiMajor axis length
            b = 60.0*5.0/m_pixelSize,  //approximate semiMinor axis length
            dpt[2];        //double 2D point

    //printf("DEBUG: a: %lf b: %lf pixel_size: %lf\n",a,b,m_pixelSize);
    gsl_rng *randGen = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(randGen, time(NULL));  //using the current time as a seed value

    samples = selectionChip->Samples();
    lines   = selectionChip->Lines();
    //printf("DEBUG selectionchip samples: %d lines: %d\n",samples,lines);

    //populating a vector of all possible integral center points
    k = int(ceil(double(samples) - a));
    l = int(ceil(double(lines  ) - b));
    j=int(floor(b+1.0));
    i=int(floor(a+1.0));

    cen_pts.clear();

    /*
     * inorder to concentrate the search near the center of the chip we'll varry the step size for
     * the center search nodes quadratically from 1*5.0/m_pixelSize at the center to
     * 5.0*5.0/m_pixelSize at the edge
     */
    double searchStep = 5.0*5.0/m_pixelSize;
    double aSearch,centerSample,centerLine,delta,cSearch;
    cSearch = searchStep/5.0;
    centerSample = double((k+i)/2.0);
    centerLine   = double((l+j)/2.0);
    delta = (centerSample-double(i))*(centerSample-double(i)) + (centerLine  -double(l))*(centerLine  -double(l));  //square of the maximum distance from center of the search space

    aSearch = (searchStep-cSearch)/delta;

    for (pt[0]=floor(a+1.0); pt[0]<k; pt[0]+=searchStep) {
      for (pt[1]=floor(b+1.0); pt[1]<l; pt[1]+=searchStep) {
        cen_pts.push_back(pt);
        delta = (centerSample-double(pt[0]))*(centerSample-double(pt[0])) + (centerLine-double(pt[1]))*(centerLine-double(pt[1]));  //square of the distance from center of the search space
        searchStep = aSearch*delta + cSearch;
        if (searchStep > l - pt[1] && l - pt[1] > 1e-4)  //don't jump outside the search area
          searchStep = l - pt[1];
      }
      if (searchStep > k - pt[0] && k - pt[0] > 1e-4)  //don't jump outside the search area
        searchStep = k - pt[0];
    }

    //STEP 1: finding points along the boundary of the selection
    selectionEdge(selectionChip,&pts);
    //printf("DEBUG: edgePoints: %d\n",pts.size());

    if (pts.size() ==0) return 0;

    emptySets = 0;
    ellBest.area = 0.0;  //initialize the current best ellipse to zero area

    while (emptySets < patience_limit && cen_pts.size()>0) {
      //printf("DEBUGA\n");
      //STEP 2: choosing a new random hypothesis point
      randPt = gsl_rng_uniform_int( randGen  ,cen_pts.size() );
      dpt[0] = cen_pts[randPt][0];
      dpt[1] = cen_pts[randPt][1];

      cen_pts.erase( cen_pts.begin() + randPt);  //erasing this ellement ensures it will never be chosen again
      //printf("DEBUGB\n");
      //STEP 3: Now define the ellipse
      ellipseFromCenterAxesAngle(&ellNew, dpt[0], dpt[1], a, b, 0.0);
      //printf("Debug a: %lf b: %lf theta: %lf center: %lf %lf\n",ellNew.semiMajor,ellNew.semiMinor,ellNew.majorAxis[0],ellNew.cen[0],ellNew.cen[1]);getchar();
      //printf("DEBUGC\n");
      //STEP 4: Do a least squares generization.  Any points within a distance of play pixels to the edge of the ellipse are included in the gernalization. The distance check is repeated for every iteration so the ellipse can effectively grow to include more points.
      if (!bestFitEllipse(&ellNew, &pts, play, 50)) {
        emptySets++;
        continue;  //if optimization fails go on to the next hypothesised ellipse
      }
      //printf("DEBUGD\n");
      //STEP 5: If the generization is successfully check to see if the area is at least as great as the current best, and that the ellipse is contained in the chip
      if (ellNew.area < ellBest.area) {
        emptySets++;
        continue;
      }
      //printf("DEBUGE\n");
      if (!ellipseInChip(&ellNew,selectionChip)) {  //if the ellipse is entirely contained in the chip
        emptySets++;
        continue;
      }
      //printf("DEBUGF\n");
      //STEP 6: if the area is great enough check that the percent sellected is at least percent_selected.
      if (elipsePercentSelected(selectionChip,&ellNew) < percent_selected) {
        emptySets++;
        continue;
      }

      //STEP 7 saving the new Best ellipse so far
      //printf("DEBUG: new best area: %lf\n",ellNew.area);
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
    }

    if( ellBest.area == 0)
      return 0;  //no ellipse meeting the selection criteria was found

    ///printf("Debug: a: %lf  b: %lf  majorAxis: %lf  %lf\n",ellBest.semiMajor,ellBest.semiMinor,ellBest.majorAxis[0],ellBest.majorAxis[1]);

    //go through and unselect the points outside the trimming ellipse
    for (i=1;i<=samples;i++) {
      for (j=1;j<=lines;j++) {
        dpt[0] = double(i) ;
        dpt[1] = double(j) ;

        if (!pointInEllipse(&ellBest,dpt,play)) {  //if the point isn't within play pixles of being within the elipse
          if( selectionChip->GetValue(i,j) == 1)
            selectionChip->SetValue(i,j,3.0);
        }
      }
    }
    return 1;

  }

}

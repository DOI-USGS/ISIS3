/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Centroid.h"


using namespace std;
namespace Isis {
  Centroid::Centroid(){
    m_maxDN = 0;    //range of dynamic numbers (DN's) to be included in the selection
    m_minDN = 0;
  };

  Centroid::~Centroid(){};

  
  /**
   * Given a range of DN this function creates a biniary chip for all continuous pixels 
   * that have the DN within the specified range using the center pixel of the chip as 
   * the seed value 
   * 
   * @param inputChip Pointer to the input chip
   * @param selectionChip Pointer to the binary chip of selected and unselected pixels
   * 
   * @returns the number 1 if successful
   */
  int Centroid::select(Chip *inputChip,Chip *selectionChip) {
    //check the sizes of the chips and make the selectionChip match the inputChip
    int lines, samples,line,sample;

    unsigned int i;

    double dn;
    
    lines = inputChip->Lines();
    samples = inputChip->Samples();

    if (lines <= 0 || samples <= 0)
      return 0;  //abort if the input chips isn't 2D

    selectionChip->SetSize(samples,lines);

    sample = int(inputChip->ChipSample());//(samples-1)/2;
    line = int(inputChip->ChipLine());//(lines-1)/2;
    //printf("DEBUG: centroid starting sample line: %d %d\n",sample,line);

    //set all values to unselected
    selectionChip->SetAllValues(0.0);  

    std::vector < std::vector<int> > Q;  //queue of nodes to check
    std::vector <int> pt(2);  

    dn = inputChip->GetValue(sample,line);
    if (dn < m_minDN || dn > m_maxDN) {
      // If the seed value doesn't meet the criteria then the selection is the empty set and the 
      // work is done
      return 0;
    }
    else {
      pt[0] = sample;
      pt[1] = line;
      Q.push_back(pt);  //otherwise it's the first point in the queue
    }
    for (i=0;i<Q.size();i++) {
      dn = inputChip->GetValue(Q[i][0], Q[i][1]);
      //if the point is in the dynamic range and not yet selected
      sample = Q[i][0];
      if (dn >= m_minDN && dn <= m_maxDN && selectionChip->GetValue(Q[i][0], Q[i][1]) == 0.0) {
        line   = Q[i][1];
        selectionChip->SetValue(sample, line, 1.0);   //set the cell to selected
        //test the four direction pixels and add them the queue if they meet the criteria
        if (sample < samples) {
          dn = inputChip->GetValue(sample + 1, line);
          //if the point is in the dynamic range and not yet selected
          if (dn >= m_minDN && dn <= m_maxDN && selectionChip->GetValue(sample+1, line) == 0.0) {
            pt[0] = sample + 1;
            pt[1] = line;
            Q.push_back(pt);
          }
        }
        if (sample > 1) {
          dn = inputChip->GetValue(sample-1, line);
          //if the point is in the dynamic range and not yet selected
          if (dn >= m_minDN && dn <= m_maxDN && selectionChip->GetValue(sample-1, line) == 0.0) {
            pt[0] = sample-1;
            pt[1] = line;
            Q.push_back(pt);
          }
        }
        if (line < lines) {
          dn = inputChip->GetValue(sample, line +1);
          //if the point is in the dynamic range and not yet selected
          if( dn >= m_minDN && dn <= m_maxDN && selectionChip->GetValue(sample, line+1) == 0.0 ) {
            pt[0] = sample;
            pt[1] = line + 1;
            Q.push_back(pt);
          }
        }
        if (line > 1) {
          dn = inputChip->GetValue(sample, line-1);
          //if the point is in the dynamic range and not yet selected
          if( dn >= m_minDN && dn <= m_maxDN && selectionChip->GetValue(sample, line-1) == 0.0 ) {
            pt[0] = sample;
            pt[1] = line-1;
            Q.push_back(pt);
          }
        }
      }
    }

    return 1;
  }


  /**
   * Set the range of the DNs
   * 
   * @param minimumDN The min DN value of the range
   * @param maximumDN The max DN value of the range
   *
   * @returns (int) The number 1 on success
   */
  int Centroid::setDNRange( double minimumDN,double maximumDN ) {
    //method to set the dynamic range of the pixels to be selected
    if (maximumDN < minimumDN)
      return 0;  //max must be >= min
    m_minDN = minimumDN;
    m_maxDN = maximumDN;
    return 1;
  }


  /**
   * @returns (double) The minimum DN value of the range
   */
  double Centroid::getMinDN() {
    return m_minDN;
  }


  /**
   * @returns (double) The maximum DN value of the range
   */
  double Centroid::getMaxDN() {
    return m_maxDN;
  }
}

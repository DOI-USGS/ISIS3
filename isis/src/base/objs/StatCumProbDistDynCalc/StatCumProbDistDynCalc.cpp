/**
 * @file
 * $Date: 2010/03/19 20:34:55 $
 * $Revision: 1.6 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <stdio.h>
#include "StatCumProbDistDynCalc.h"
#include <math.h>
#include <float.h>
#include "IException.h"
#include "IString.h"

namespace Isis {


  /**  Construtor sets up the class to start recieving data
   *
   *     @param[in] unsigned int nodes -- this is the number of specific evenly spaced quantiles that will be dynamically tracked
   */
  StatCumProbDistDynCalc::StatCumProbDistDynCalc(unsigned int nodes) {
    this->initialize(nodes);
  }


  /**  Inializer, resets the class to start its dynamic calculation anew
   *
   *     @param[in] unsigned int nodes -- this is the number of specific evenly spaced quantiles that will be dynamically tracked
   */
  void StatCumProbDistDynCalc::initialize(unsigned int nodes) {
    if (nodes<3) {
      m_nCells = 2;
      m_nQuan = 3; //there is one more border value then there are cells
    }
    else {
      m_nCells = nodes-1;
      m_nQuan = nodes; //there is one more border value then there are cells
    }

    //resize and initial data vectors
    m_quan.resize(m_nQuan);
    m_n.resize(m_nQuan);
    m_nIdeal.resize(m_nQuan);


    m_quan[0] = 0.0;
    m_quan[m_nQuan-1]=1.0;
    for (unsigned int i=1;i<m_nQuan-1;i++) m_quan[i] = m_quan[i-1] + 1/double(m_nCells);
    m_nObs = 0;  
    
    for (unsigned int i=0;i<m_nQuan;i++) m_nIdeal[i]=m_n[i]=i+1;
  
    m_q.resize(m_nQuan);  //these will be the first m_nQuan observations sorted from smallest to largest
  }


  /**  Returns the maximum observation so far included in the dynamic calculation 
   *
   *     @return double -- the maximum observation so far included in the dynamic calculation
   *     @throw  IsisProgrammerError -- StatCumDistDynCalc will return no data until there has been at least m_nQuan
   *       observations added
   */
  double StatCumProbDistDynCalc::max() {
     //if there isn't even as much data as there are quantiles to track return DBL_MAX
     if (m_nObs<m_nQuan) {
       IString msg = "StatCumDistDynCalc will return no data until there has been at least m_nQuan observations added";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     } 
     //return the largest value so far
     return m_q[m_nCells];
   }


  /**  Returns the maximum observation so far included in the dynamic calculation 
   *
   *     @return double -- the maximum observation so far included in the dynamic calculation
   *     @throw  IsisProgrammerError -- StatCumDistDynCalc will return no data until there has been at least m_nQuan
   *       observations added
   */
   double StatCumProbDistDynCalc::min() { 
     //if there isn't even as much data as there are quantiles to track return -DBL_MAX
     if (m_nObs<m_nQuan) {
       IString msg = "StatCumDistDynCalc will return no data until there has been at least m_nQuan observations added";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return -DBL_MAX;
     }
     //return the smallest value so far
     return m_q[0];
   }


  /**  Provides the value of the variable that has the given cumulative probility (according the current estimate of cumulative probility function)
   *
   *     @param[in] double -- cumlative probability domain [0.1]
   *     @return double -- the vaule of the variable that has the cumulative probility (according the current estimate of cumulative probility function)
   *     @throw  IsisProgrammerError -- StatCumDistDynCalc will return no data until there has been at least m_nQuan
   *       observations added
   *     @throw  IsisProgrammerError -- Argument to StatCumProbDistDynCalc::value(double cumProb) must be on the domain [0.1]
   */
   double StatCumProbDistDynCalc::value(double cumProb) {
     //given a cumProbability return the associate value

     int i;

     //if there isn't even as much data as there are quantiles to track return DBL_MAX
     if (m_nObs<m_nQuan) {
       IString msg = "StatCumDistDynCalc will return no data until there has been at least m_nQuan observations added";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     //if the requested cumProb is outside [0,1] return DBL_MAX
     if (cumProb < 0.0 || cumProb > 1.0) {
       IString msg = "Argument to StatCumProbDistDynCalc::value(double cumProb) must be on the domain [0,1]";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     //if the requested quantile is 0.0 return the lowest value
     if (cumProb == 0.0) return m_q[0];
   
     //if the requested quantile is 1.0 return the largest value
     if (cumProb == 1.0) return m_q[m_nCells];

     //otherwise find the the node nearest the value
     double temp = fabs(m_quan[0]-cumProb);
     unsigned int index=0;
     for (i=1;i<int(m_nQuan);i++) {
       if ( fabs(m_quan[i]-cumProb) < temp) {
         temp = fabs(m_quan[i]-cumProb);
         index=i;
       }
     }

     //get the coordinates of the three closest nodes, value as a function of cumProb
     double x[3]; //m_q values
     double y[3]; //cumlative probilities
      
     if (index ==0) {
       y[0] = m_q[0];
       y[1] = m_q[1];
       y[2] = m_q[2];

       x[0] = m_quan[0];
       x[1] = m_quan[1];
       x[2] = m_quan[2];
     }
     else if (index == m_nCells) {  
       y[0] = m_q[index-2];
       y[1] = m_q[index-1];
       y[2] = m_q[index  ];
 
       x[0] = m_quan[index-2];
       x[1] = m_quan[index-1];
       x[2] = m_quan[index  ];
     }
     else {
       y[0] = m_q[index-1];
       y[1] = m_q[index  ];
       y[2] = m_q[index+1];

       x[0] = m_quan[index-1];
       x[1] = m_quan[index  ];
       x[2] = m_quan[index+1];
     }

     if ( x[0] == x[1] || x[0] == x[2] || x[1] == x[2]) return m_q[index];  //this should never happen, but just in case return the value of the nearest node

     //try quadratic interpolation
     temp = (cumProb-x[1])*(cumProb-x[2])/(x[0]-x[1])/(x[0]-x[2])*y[0] + (cumProb-x[0])*(cumProb-x[2])/(x[1]-x[0])/(x[1]-x[2])*y[1] + (cumProb-x[0])*(cumProb-x[1])/(x[2]-x[0])/(x[2]-x[1])*y[2];

     //check for reasonability of the quadratice interpolation
     for (i=0;i<3;i++) {
       if ( x[i] <= cumProb && x[i+1] >= cumProb) //find the nodes on both sides of cumProb
         break;
     }

     if (y[i] <= temp && y[i+1] >= temp) //make sure things are strickly increasing
       return temp;

     //if the quadratice wasn't reasonable return the linear interpolation
     else 
       return ((x[i] - cumProb)*y[i+1] + (x[i+1]-cumProb)*y[i])/(x[i]-x[i+1]);

   }



  /**  Provides the cumulative probility, that is, the proportion of the distribution that is less than or equal to the value given (according the current estimate of cumulative probility function).
   *
   *     @param[in] double -- value, the upper bound of values considered in the cumlative probility calculation 
   *     @return double -- the cumulative probility, that is, the proportion of the distribution that is less than or equal to the value given (according the current estimate of cumulative probility function).
   *     @throw  IsisProgrammerError -- StatCumDistDynCalc will return no data until there has been at least m_nQuan
   *       observations added
   */
   double StatCumProbDistDynCalc::cumProb(double value) {
     //given a value return the cumulative probility

     if (m_nObs<m_nQuan) {
       IString msg = "StatCumDistDynCalc will return no data until there has been at least m_nQuan observations added";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     
     if (value <= m_q[0]) return 0.0; //if the value is less than or equal to the the current min, the best estimate is 0.0
     else if (value >= m_q[m_nCells]) return 1.0;  //if the value is greater than or equal to the current max, the best estimate is 1.0

     int i;

     //otherwise find the the node nearest the value
     double temp = fabs(m_q[0]-value);
     unsigned int index=0;
     for (i=1;i<int(m_nQuan);i++) {
       if ( fabs(m_q[i]-value) < temp) {
         temp = fabs(m_q[i]-value);
         index=i;
       }
     }

     //get the coordinates of the three closest nodes cumProb as a function of value
     double x[3]; //m_q values
     double y[3]; //cumlative probilities
      
     if (index ==0) {
       x[0] = m_q[0];
       x[1] = m_q[1];
       x[2] = m_q[2];

       y[0] = m_quan[0];
       y[1] = m_quan[1];
       y[2] = m_quan[2];
     }
     else if (index == m_nCells) {
       x[0] = m_q[index  ];
       x[1] = m_q[index-1];
       x[2] = m_q[index-2];

       y[0] = m_quan[index  ];
       y[1] = m_quan[index-1];
       y[2] = m_quan[index-2];
     }
     else {
       x[0] = m_q[index-1];
       x[1] = m_q[index  ];
       x[2] = m_q[index+1];

       y[0] = m_quan[index-1];
       y[1] = m_quan[index  ];
       y[2] = m_quan[index+1];
     }

     if ( x[0] == x[1] || x[0] == x[2] || x[1] == x[2]) return m_quan[index];  //this should never happen, but just in case return the cumProb of the nearest node

     //do a parabolic interpolation to find the probability at value
     temp = (value-x[1])*(value-x[2])/(x[0]-x[1])/(x[0]-x[2])*y[0] + (value-x[0])*(value-x[2])/(x[1]-x[0])/(x[1]-x[2])*y[1] + (value-x[0])*(value-x[1])/(x[2]-x[0])/(x[2]-x[1])*y[2];

     //check for reasonability of the quadratice interpolation
     for (i=0;i<3;i++) {
       if ( x[i] <= value && x[i+1] >= value) //find the nodes on both sides of cumProb
         break;
     }

     if (y[i] <= temp && y[i+1] >= temp) //make sure things are strickly increasing
       return temp;
     //if the quadratice wasn't reasonable return the linear interpolation
     else 
       return ((x[i] - value)*y[i+1] + (x[i+1]-value)*y[i])/(x[i]-x[i+1]);

   }



  /**  Values for the estimated quantile positions are update as observations are added
   *
   *   @param[in] double obs -- the individual observation to be used to dynamically readjust the cumulative probility distribution
   */
  void StatCumProbDistDynCalc::addObs(double obs) {
    unsigned int i,j;
    double temp;

    if (m_nObs<m_nQuan) {  //in this phase the algorithm is getting initial values
      m_q[m_nObs] = obs;
      m_nObs++;
      if (m_nObs==m_nQuan) {  //if there are now m_nQuan observations sort them from smallest to greatest
        for (i=0;i<m_nQuan-1;i++)  {
          for (j=i+1;j<m_nQuan;j++) {
            if (m_q[j] < m_q[i]) {
              temp = m_q[i];
              m_q[i] = m_q[j];
              m_q[j] = temp;
            }
          }
        }
      }
      return;
    }

    //incrementing the number of observations
    m_nObs++;
    m_n[m_nQuan-1] = m_nObs;
 
    //keep the min and max up to date
    if (obs > m_q[m_nQuan-1]) m_q[m_nQuan-1] = obs;
    if (obs < m_q[0]) {
      m_q[0] = obs;
      temp=1;
    }

    //estimated quantile positions are increased if obs <= estimated quantile value
    for (i=1;i<m_nQuan-1;i++)
      if (obs <= m_q[i]) for (;i<m_nQuan-1;i++) m_n[i]++; //all m_n's above the first >= obs get incremented

    for (i=1;i<m_nQuan;i++) m_nIdeal[i] += m_quan[i];
  
    //calculate corrections to node positions (m_n) and values (m_q)
    int d;
    for (i=1;i<m_nCells;i++) {  //note that the loop does not edit the first or last positions
      //calculation of d[i] it is either 1, -1, or 0
      temp = m_nIdeal[i] - m_n[i];
      if (fabs(temp)>1 && temp > 0.0)
        d = 1;
      else if (fabs(temp)>1 && temp < 0.0)
        d = -1;
      else
        continue;
    
      if ( (d ==1 && m_n[i+1]-m_n[i] > 1) || (d ==-1 && m_n[i-1]-m_n[i] < -1) ) { //if the node can be moved by d without stepping on another node
        //calculate a new quantile value for the node from the parabolic formula
        temp = m_q[i] + double(d)/(m_n[i+1] - m_n[i-1])*( (m_n[i]-m_n[i-1]+d)*(m_q[i+1]-m_q[i])/(m_n[i+1]-m_n[i]) + (m_n[i+1]-m_n[i]-d)*(m_q[i]-m_q[i-1])/(m_n[i]-m_n[i-1]));
        if ( m_q[i-1] < temp && temp < m_q[i+1]) //it is necessary that the values of the quantiles be strickly increasing, if the parabolic formula perserves this so be ti
          m_q[i] = temp;
        else 
          m_q[i] = m_q[i] + double(d)*(m_q[i+d]-m_q[i])/(m_n[i+d]-m_n[i]); //if the parabolic formula does not maintain that the values of the quantiles be strickly increasing then use linear interpolation
 
        m_n[i] += d; //the position of the quantile (in terms of the number of observations <= quantile) is also adjusted
      } 
    }
  }
}// end namespace Isis

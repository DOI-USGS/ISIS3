#ifndef StatCumProbDistDynCalc_h
#define StatCumProbDistDynCalc_h

/**
 * @file
 * $Date: 2012/03/23 20:34:55 $
 * $Revision: 1.0 $
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

#include <vector>

namespace Isis {
  /**
  * @brief This class is used to approximate cumulative probibility distributions of a stream of observations without storing the observations or having any apriori knowlege of the range of the data.  
  *
  * This class is used to approximate cumulative probibility distributions of a stream of observations without storing the observations or having any apriori knowlege of the range of the data.
  * "The P^2 algorithim for dynamic calculation of Quantiles and Histograms without storing Obswervations" Raj Jain and Imrich Chlamtac, Communication of the ACM Oct 1985, is used.
  * A finite set of evenly spaced qunatiles are dynamically updated as more observations are added.  The number of quantiles is set in the construtor, and has a defualt of 20.
  * After sufficient data points (number of observations >> number of quantiles to track) the class provides cumulative probility as a function of value or vice versa.
  * Thus it can be used to build histograms or find any number of discrete quantiles.  Specific points on the function are evaluated by fiting piece wise parabolic functions to the three nearest adjacent nodes.
  * Preformance of algorithim is within a few percent error for most of the distribution (given sufficient data), however care should be taken if the points to be querried are within 200/(numberOfQuantiles-1)% of the edges of the distributions.
  * Near the edges the individual quantiles are still well calculated, but the piece wise parabolic function doesn't always fit the tails well, so interpolated points are more unrealiable.
  * Developement note: Two possible ways to improve the fitting of the tails: caculate more densely place quantiles near the edges, use exponential regression (or some other alternative--perhaps adaptively selected).
  *
  *
  *
  * @ingroup Statistics
  *
  * @author 2012-03-23 Orrin Thomas
  *
  * @internal
  * @history 2012-03-23 Orrin Thomas - Original Version
  *
  */
  class StatCumProbDistDynCalc{
    //class uses the P^2 Algorithim to calculate equiprobability cell histograms from a stream of data without storing the data
    //  see "The p^2 Algorithim for Dynamic Calculations of Quantiles and Histograms Without Storing Observations"
  public:
    StatCumProbDistDynCalc(unsigned int nodes=20);  //individual qunatile value to be calculated
    ~StatCumProbDistDynCalc() { }; //empty destructor

    void addObs(double obs); //

    double cumProb(double value); //given a value return the cumulative probility
    double value(double cumProb); //given a cumulative probibility return a value
    double max(); //return the largest value so far
    double min(); //return the smallest values so far
    void initialize(unsigned int nodes=20); //resets the class to start a new dynamic calculation

  private:

    /** The number of cells or bins that being used to model the probility density function
     */
    unsigned int m_nCells;     //the number of cells in the histogram


    /** The number of quantiles being used to model the probility density function
     *    This is one more than the number of cells.
     */
    unsigned int m_nQuan;     //the number of quantiles being calculated (m_cells+1)



    /** the quantiles being modeled begining at 0 and going to 1
     */
    std::vector<double> m_quan;        //the target quantile



    /** The ideal number of observations that should be less than or equal to the value of the corresponding quantiles, note this is dynamically changing as observations are added
     */
    std::vector<double> m_nIdeal;      //ideal positions of quantiles



    /** The actual number of observations that are less than or equal to the value of the corresponding quantiles, note this is dynamically changing as observations are added
     */
    std::vector<int> m_n;



    /**  The calculated values of the quantiles, note this is dynamically changing as observations are added
     */
    std::vector<double> m_q;



    /**  The number of observations, note this is dynamically changing as observations are added
     */
    unsigned int m_nObs;  //the number of observations
  };

} //end namespace Isis

#endif

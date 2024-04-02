#ifndef StatCumProbDistDynCalc_h
#define StatCumProbDistDynCalc_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QObject>
#include <QVector>

#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;
class QXmlStreamReader;

namespace Isis {
  class Project;// ??? does xml stuff need project???

 /**
  * @brief This class is used to approximate cumulative probibility distributions of a stream of
  *        observations without storing the observations or having any apriori knowlege of the range
  *        of the data.  
  *
  * This class is used to approximate cumulative probibility distributions of a stream of
  * observations without storing the observations or having any apriori knowlege of the range of the 
  * data. "The P^2 algorithim for dynamic calculation of Quantiles and Histograms without storing 
  * Observations" Raj Jain and Imrich Chlamtac, Communication of the ACM Oct 1985, is used. A finite 
  * set of evenly spaced qunatiles are dynamically updated as more observations are added.  The 
  * number of quantiles is set in the construtor, and has a defualt of 20. After sufficient data 
  * points (number of observations >> number of quantiles to track) the class provides cumulative 
  * probility as a function of value or vice versa. Thus it can be used to build histograms or find 
  * any number of discrete quantiles.  Specific points on the function are evaluated by fiting piece 
  * wise parabolic functions to the three nearest adjacent nodes. Preformance of algorithim is 
  * within a few percent error for most of the distribution (given sufficient data), however care 
  * should be taken if the points to be querried are within 200/(numberOfQuantiles-1)% of the edges 
  * of the distributions. Near the edges the individual quantiles are still well calculated, but the 
  * piece wise parabolic function doesn't always fit the tails well, so interpolated points are more 
  * unrealiable. Developement note: Two possible ways to improve the fitting of the tails: caculate 
  * more densely place quantiles near the edges, use exponential regression (or some other 
  * alternative--perhaps adaptively selected).
  *
  *
  *
  * @ingroup Math
  * @ingroup Statistics
  *
  * @author 2012-03-23 Orrin Thomas
  *
  * @internal
  *   @history 2012-03-23 Orrin Thomas - Original Version
  *   @history 2014-07-19 Jeannie Backer - Added QDataStream >> and << operator methods. Brought
  *                           code closer to ISIS standards. Updated unitTest to include these
  *                           methods.
  *   @history 2014-09-11 Jeannie Backer - Added xml write/read capabilities. Fixed bug in cumPro()
  *                           method for previously untested lines (case where the given value is
  *                           closest to the last quantile value). Renamed member variables for
  *                           clarity.
  *
  */
  class StatCumProbDistDynCalc : public QObject {
    Q_OBJECT
    // class uses the P^2 Algorithim to calculate equiprobability cell histograms from a stream of 
    // data without storing the data 
    //  see "The p^2 Algorithim for Dynamic Calculations of Quantiles and Histograms Without Storing
    //  Observations"
    public:
      StatCumProbDistDynCalc(unsigned int nodes=20, QObject *parent = 0);  //individual qunatile value to be calculated
      StatCumProbDistDynCalc(QXmlStreamReader *xmlReader, QObject *parent = 0);
      void readStatistics(QXmlStreamReader *xmlReader);
      StatCumProbDistDynCalc(const StatCumProbDistDynCalc &other);
      ~StatCumProbDistDynCalc();
      StatCumProbDistDynCalc &operator=(const StatCumProbDistDynCalc &other);
    
      void initialize(); // clears the member lists and initializes the rest of the member data to 0 
      void setQuantiles(unsigned int nodes); // initializes/resets the class to start new calculation

      void validate();
      void addObs(double obs);
    
      double cumProb(double value); //given a value return the cumulative probility
      double value(double cumProb); //given a cumulative probibility return a value
      double max(); //return the largest value so far
      double min(); //return the smallest values so far
    
      void save(QXmlStreamWriter &stream, const Project *project) const;   // TODO: does xml stuff need project???
    
      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);

      unsigned int m_numberCells; /**< The number of cells or histogram bins that are being used to
                                      model the probility density function.*/
      
      unsigned int m_numberQuantiles; /**< The number of quantiles being used to model the probility
                                      density function. This value is one more than the number of
                                      cells, (i.e. m_numberQuantiles=m_cells+1).*/
      
      unsigned int m_numberObservations; /**< The number of observations, note this is dynamically
                                      changing as observations are added.*/

      QList<double> m_quantiles; /**< The target quantiles being modeled, between 0 and 1.*/
      
      QList<double> m_observationValues; /**< The calculated values of the quantiles, note this is
                                      dynamically changing as observations are added.*/

      QList<double> m_idealNumObsBelowQuantile; /**< The ideal number of observations that
                                      should be less than or equal to the value of the corresponding
                                      quantiles, note this is dynamically changing as observations
                                      are added.*/
      
      QList<int> m_numObsBelowQuantile; /**< The actual number of observations that are less
                                      than or equal to the value of the corresponding quantiles,
                                      note this is dynamically changing as observations are added.*/
            
  };

  // operators to read/write StatCumProbDistDynCalc to/from binary data
  QDataStream &operator<<(QDataStream &stream, const StatCumProbDistDynCalc &scpddc);
  QDataStream &operator>>(QDataStream &stream, StatCumProbDistDynCalc &scpddc);

} //end namespace Isis

#endif

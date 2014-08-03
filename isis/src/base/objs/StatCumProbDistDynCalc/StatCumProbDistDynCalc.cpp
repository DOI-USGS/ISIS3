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

#include "StatCumProbDistDynCalc.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QXmlStreamWriter>

#include <float.h>
#include <math.h>
#include <stdio.h>

#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {


  /** 
   * Construtor sets up the class to start recieving data
   *
   * @param [in] unsigned int nodes -- this is the number of specific evenly spaced quantiles that
   *                 will be dynamically tracked
   */
  StatCumProbDistDynCalc::StatCumProbDistDynCalc(unsigned int nodes, QObject *parent) : QObject(parent) {

    initialize(nodes);
  }



  StatCumProbDistDynCalc::StatCumProbDistDynCalc(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent) {   // TODO: does xml stuff need project???
    m_id = NULL;
    // ??? initializations ???
    xmlReader->pushContentHandler(new XmlHandler(this, project));   // TODO: does xml stuff need project???
  }



  StatCumProbDistDynCalc::StatCumProbDistDynCalc(const StatCumProbDistDynCalc &other)
    : m_id(new QUuid(other.m_id->toString())),
      m_numberCells(other.m_numberCells),      
      m_numberQuantiles(other.m_numberQuantiles),  
      m_quantiles(other.m_quantiles),        
      m_idealNum(other.m_idealNum),         
      m_n(other.m_n),                
      m_quantileValues(other.m_quantileValues),   
      m_numberObservations(other.m_numberObservations) {
  }



  /** 
   * Destroys StatCumProbDistDynCalc object.
   */ 
  StatCumProbDistDynCalc::~StatCumProbDistDynCalc() { 
    delete m_id;
    m_id = NULL;
  }



  StatCumProbDistDynCalc &StatCumProbDistDynCalc::operator=(const StatCumProbDistDynCalc &other) {

    if (&other != this) {
      delete m_id;
      m_id = NULL;
      m_id = new QUuid(m_id->toString());

      m_numberCells        = other.m_numberCells;
      m_numberQuantiles    = other.m_numberQuantiles;
      m_quantiles          = other.m_quantiles;
      m_idealNum           = other.m_idealNum;
      m_n                  = other.m_n;
      m_quantileValues     = other.m_quantileValues;
      m_numberObservations = other.m_numberObservations;
    }
    return *this;

  }



  /** 
   * Inializer, resets the class to start its dynamic calculation anew
   *
   * @param [in] unsigned int nodes -- this is the number of specific evenly spaced quantiles that 
   *                 will be dynamically tracked
   */
  void StatCumProbDistDynCalc::initialize(unsigned int nodes) {
    m_quantiles.clear();
    m_idealNum.clear();
    m_n.clear();
    m_quantileValues.clear();
    m_numberObservations = 0;

    if (nodes < 3) {
      m_numberQuantiles = 3; //there is one more border value then there are cells
    }
    else {
      m_numberQuantiles = nodes; //there is one more border value then there are cells
    }

    m_numberCells = m_numberQuantiles - 1;

    double stepSize = 1.0 / (double)m_numberCells;
    for (unsigned int i = 0; i < m_numberQuantiles; i++) {
      if (i == 0) {
        m_quantiles.append( 0.0 );
      }
      else {
        m_quantiles.append( m_quantiles[i-1] + stepSize );
      } // essentially the same as i/numCells without having to cast i every time...
      m_idealNum.append(i+1);
      m_n.append(i+1);
    }

    m_quantileValues.resize(m_numberQuantiles);  //these will be the first m_numberQuantiles observations sorted from smallest to largest
  }



  /** 
   * Returns the maximum observation so far included in the dynamic calculation
   *
   * @return double -- the maximum observation so far included in the dynamic calculation
   * @throw  IsisProgrammerError -- StatCumProbDistDynCalc will return 
   *             no data until the number of observations added matches
   *             the number of quantiles (i.e. number of nodes)
   *             selected.
   */
  double StatCumProbDistDynCalc::max() {
     //if there isn't even as much data as there are quantiles to track return DBL_MAX
     if (m_numberObservations < m_numberQuantiles) {
       IString msg = "StatCumProbDistDynCalc will return no data until the number of observations added"
                     " [" + toString(m_numberObservations) + "] matches the number of quantiles"
                     " [" + toString(m_numberQuantiles) + "] (i.e. number of nodes) selected.";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     } 
     //return the largest value so far
     return m_quantileValues[m_numberCells];
   }



  /** 
   * Returns the maximum observation so far included in the dynamic calculation
   *
   * @return double -- the maximum observation so far included in the dynamic calculation
   * @throw  IsisProgrammerError -- StatCumProbDistDynCalc will return 
   *             no data until the number of observations added matches
   *             the number of quantiles (i.e. number of nodes)
   *             selected.
   */
   double StatCumProbDistDynCalc::min() { 
     //if there isn't even as much data as there are quantiles to track return -DBL_MAX
     if (m_numberObservations < m_numberQuantiles) {
       IString msg = "StatCumProbDistDynCalc will return no data until the number of observations added"
                     " [" + toString(m_numberObservations) + "] matches the number of quantiles"
                     " [" + toString(m_numberQuantiles) + "] (i.e. number of nodes) selected.";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return -DBL_MAX;
     }
     //return the smallest value so far
     return m_quantileValues[0];
   }



  /** 
   * Provides the value of the variable that has the given cumulative probility
   * (according the current estimate of cumulative probility function)
   *
   * @param [in] double -- cumlative probability domain [0.1]
   * @return double -- the vaule of the variable that has the cumulative probility (according the 
   *             current estimate of cumulative probility function)
   * @throw  IsisProgrammerError -- StatCumProbDistDynCalc will return no data until 
   *             the number of observations added matches the number of
   *             quantiles (i.e. number of nodes) selected.
   * @throw  IsisProgrammerError -- Invalid cumulative probability passed in to 
   *             StatCumProbDistDynCalc::value(double cumProb). Must be on
   *             the domain [0, 1].
   */
   double StatCumProbDistDynCalc::value(double cumProb) {
     //given a cumProbability return the associate value

     int i;

     //if there isn't even as much data as there are quantiles to track return DBL_MAX
     if (m_numberObservations < m_numberQuantiles) {
       IString msg = "StatCumProbDistDynCalc will return no data until the number of observations added"
                     " [" + toString(m_numberObservations) + "] matches the number of quantiles"
                     " [" + toString(m_numberQuantiles) + "] (i.e. number of nodes) selected.";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     //if the requested cumProb is outside [0,1] return DBL_MAX
     if (cumProb < 0.0 || cumProb > 1.0) {
       IString msg = "Invalid cumulative probability [" + toString(cumProb) + "] passed in to "
                     "StatCumProbDistDynCalc::value(double cumProb). Must be on the domain [0, 1].";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     //if the requested quantile is 0.0 return the lowest value
     if (cumProb == 0.0) {
       return m_quantileValues[0];
     }

     //if the requested quantile is 1.0 return the largest value
     if (cumProb == 1.0) {
       return m_quantileValues[m_numberCells];
     }

     //otherwise find the the node nearest the value
     double temp = fabs(m_quantiles[0] - cumProb);
     unsigned int index = 0;
     for (i = 1; i < int(m_numberQuantiles); i++) {
       if ( fabs(m_quantiles[i] - cumProb) < temp) {
         temp = fabs(m_quantiles[i] - cumProb);
         index = i;
       }
     }

     //get the coordinates of the three closest nodes, value as a function of cumProb
     double x[3]; //m_quantileValues values
     double y[3]; //cumlative probilities

     if (index ==0) {
       y[0] = m_quantileValues[0];
       y[1] = m_quantileValues[1];
       y[2] = m_quantileValues[2];

       x[0] = m_quantiles[0];
       x[1] = m_quantiles[1];
       x[2] = m_quantiles[2];
     }
     else if (index == m_numberCells) {
       y[0] = m_quantileValues[index-2];
       y[1] = m_quantileValues[index-1];
       y[2] = m_quantileValues[index  ];

       x[0] = m_quantiles[index-2];
       x[1] = m_quantiles[index-1];
       x[2] = m_quantiles[index  ];
     }
     else {
       y[0] = m_quantileValues[index-1];
       y[1] = m_quantileValues[index  ];
       y[2] = m_quantileValues[index+1];

       x[0] = m_quantiles[index-1];
       x[1] = m_quantiles[index  ];
       x[2] = m_quantiles[index+1];
     }

     if ( x[0] == x[1] || x[0] == x[2] || x[1] == x[2]) {
       return m_quantileValues[index];  //this should never happen, but just in case return the value of the nearest node
     }

     //try quadratic interpolation
     temp = (cumProb-x[1]) * (cumProb-x[2]) / (x[0]-x[1]) / (x[0]-x[2]) * y[0]
            + (cumProb-x[0]) * (cumProb-x[2]) / (x[1]-x[0]) / (x[1]-x[2]) * y[1]
            + (cumProb-x[0]) * (cumProb-x[1]) / (x[2]-x[0]) / (x[2]-x[1]) * y[2];

     //check for reasonability of the quadratice interpolation
     for (i = 0; i < 3; i++) {
       if ( x[i] <= cumProb && x[i+1] >= cumProb) { // find the nodes on both sides of cumProb
         break;
       }
     }

     if (y[i] <= temp && y[i+1] >= temp) {//make sure things are strickly increasing
       return temp;
     }

     //if the quadratice wasn't reasonable return the linear interpolation
     else {
       return ((x[i] - cumProb)*y[i+1] + (x[i+1]-cumProb)*y[i])/(x[i]-x[i+1]);
     }

   }



  /** 
   * Provides the cumulative probility, that is, the proportion of the
   * distribution that is less than or equal to the value given (according the
   * current estimate of cumulative probility function).
   *
   * @param [in] double -- value, the upper bound of values considered in the cumlative 
   *                probility calculation
   * @return    double -- the cumulative probility, that is, the proportion of the distribution 
   *                that is less than or equal to the value given (according the current
   *                estimate of cumulative probility function).
   * @throw     IsisProgrammerError -- StatCumProbDistDynCalc will return no data until there has been 
   *                at least m_numberQuantiles observations added
   */
   double StatCumProbDistDynCalc::cumProb(double value) {
     //given a value return the cumulative probility

     if (m_numberObservations < m_numberQuantiles) {
       IString msg = "StatCumProbDistDynCalc will return no data until the number of observations added"
                     " [" + toString(m_numberObservations) + "] matches the number of quantiles"
                     " [" + toString(m_numberQuantiles) + "] (i.e. number of nodes) selected.";
       throw IException(IException::Programmer, msg, _FILEINFO_);
       return DBL_MAX;
     }

     if (value <= m_quantileValues[0]) {
       return 0.0; //if the value is less than or equal to the the current min, the best estimate is 0.0
     }
     else if (value >= m_quantileValues[m_numberCells]) {
       return 1.0;  //if the value is greater than or equal to the current max, the best estimate is 1.0
     }

     int i;

     //otherwise find the the node nearest the value
     double temp = fabs(m_quantileValues[0]-value);
     unsigned int index = 0;
     for (i = 1; i < int(m_numberQuantiles); i++) {
       if ( fabs(m_quantileValues[i]-value) < temp) {
         temp = fabs(m_quantileValues[i]-value);
         index = i;
       }
     }

     //get the coordinates of the three closest nodes cumProb as a function of value
     double x[3]; //m_quantileValues values
     double y[3]; //cumlative probilities

     if (index ==0) {
       x[0] = m_quantileValues[0];
       x[1] = m_quantileValues[1];
       x[2] = m_quantileValues[2];

       y[0] = m_quantiles[0];
       y[1] = m_quantiles[1];
       y[2] = m_quantiles[2];
     }
     else if (index == m_numberCells) {
       x[0] = m_quantileValues[index  ];
       x[1] = m_quantileValues[index-1];
       x[2] = m_quantileValues[index-2];

       y[0] = m_quantiles[index  ];
       y[1] = m_quantiles[index-1];
       y[2] = m_quantiles[index-2];
     }
     else {
       x[0] = m_quantileValues[index-1];
       x[1] = m_quantileValues[index  ];
       x[2] = m_quantileValues[index+1];

       y[0] = m_quantiles[index-1];
       y[1] = m_quantiles[index  ];
       y[2] = m_quantiles[index+1];
     }

     if ( x[0] == x[1] || x[0] == x[2] || x[1] == x[2]) {
       return m_quantiles[index];  //this should never happen, but just in case return the cumProb of the nearest node
     }

     //do a parabolic interpolation to find the probability at value
     temp = (value-x[1]) * (value-x[2]) / (x[0]-x[1]) / (x[0]-x[2]) * y[0]
            + (value-x[0]) * (value-x[2]) / (x[1]-x[0]) / (x[1]-x[2]) * y[1]
            + (value-x[0]) * (value-x[1]) / (x[2]-x[0]) / (x[2]-x[1]) * y[2];

     //check for reasonability of the quadratice interpolation
     for (i = 0; i < 3; i++) {
       if ( x[i] <= value && x[i+1] >= value) //find the nodes on both sides of cumProb
         break;
     }

     if (y[i] <= temp && y[i+1] >= temp) //make sure things are strickly increasing
       return temp;
     //if the quadratice wasn't reasonable return the linear interpolation
     else {
       return ((x[i] - value)*y[i+1] + (x[i+1]-value)*y[i])/(x[i]-x[i+1]);
     }

   }



  /** 
   * Values for the estimated quantile positions are update as observations are
   * added
   *
   * @param [in] double obs -- the individual observation to be used to 
   *                dynamically readjust the cumulative probility distribution
   */
  void StatCumProbDistDynCalc::addObs(double obs) {
    unsigned int i,j = 0;
    double temp = 0.0;

    if (m_numberObservations < m_numberQuantiles) {  //in this phase the algorithm is getting initial values
      m_quantileValues[m_numberObservations] = obs;
      m_numberObservations++;
      if (m_numberObservations == m_numberQuantiles) {  //if there are now m_numberQuantiles observations sort them from smallest to greatest
        for (i = 0; i < m_numberQuantiles-1; i++)  {
          for (j = i+1; j < m_numberQuantiles; j++) {
            if (m_quantileValues[j] < m_quantileValues[i]) {
              temp = m_quantileValues[i];
              m_quantileValues[i] = m_quantileValues[j];
              m_quantileValues[j] = temp;
            }
          }
        }
      }
      return;
    }

    //incrementing the number of observations
    m_numberObservations++;
    m_n[m_numberQuantiles-1] = m_numberObservations;

    //keep the min and max up to date
    if (obs > m_quantileValues[m_numberQuantiles-1]) {
      m_quantileValues[m_numberQuantiles-1] = obs;
    }
    if (obs < m_quantileValues[0]) {
      m_quantileValues[0] = obs;
      temp = 1;
    }

    //estimated quantile positions are increased if obs <= estimated quantile value
    for (i = 1; i < m_numberQuantiles-1; i++) {
      if (obs <= m_quantileValues[i]) for (; i < m_numberQuantiles-1; i++) m_n[i]++; //all m_n's above the first >= obs get incremented
    }

    for (i = 1; i < m_numberQuantiles; i++) {
      m_idealNum[i] += m_quantiles[i];
    }

    //calculate corrections to node positions (m_n) and values (m_quantileValues)
    int d;
    for (i = 1; i < m_numberCells; i++) {  //note that the loop does not edit the first or last positions
      //calculation of d[i] it is either 1, -1, or 0
      temp = m_idealNum[i] - m_n[i];
      if (fabs(temp)>1 && temp > 0.0) {
        d = 1;
      }
      else if (fabs(temp)>1 && temp < 0.0) {
        d = -1;
      } 
      else {
        continue;
      }

      if ( (d ==1 && m_n[i+1]-m_n[i] > 1) || (d ==-1 && m_n[i-1]-m_n[i] < -1) ) { //if the node can be moved by d without stepping on another node
        //calculate a new quantile value for the node from the parabolic formula
        temp = m_quantileValues[i] + double(d)/(m_n[i+1] - m_n[i-1])
                        *( (m_n[i]-m_n[i-1]+d)*(m_quantileValues[i+1]-m_quantileValues[i])/(m_n[i+1]-m_n[i])
                            + (m_n[i+1]-m_n[i]-d)*(m_quantileValues[i]-m_quantileValues[i-1])/(m_n[i]-m_n[i-1]) );

        if ( m_quantileValues[i-1] < temp && temp < m_quantileValues[i+1]) { // it is necessary that the values of the quantiles be strickly increasing, if the parabolic formula perserves this so be ti
          m_quantileValues[i] = temp;
        }
        else {
          m_quantileValues[i] = m_quantileValues[i] + double(d)*(m_quantileValues[i+d]-m_quantileValues[i])/(m_n[i+d]-m_n[i]); //if the parabolic formula does not maintain that the values of the quantiles be strickly increasing then use linear interpolation
        }

        m_n[i] += d; //the position of the quantile (in terms of the number of observations <= quantile) is also adjusted
      }
    }
  }



  void StatCumProbDistDynCalc::save(QXmlStreamWriter &stream, const Project *project) const {   // TODO: does xml stuff need project???

    stream.writeStartElement("statCumProbDistDynCalc");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("numberCells", toString(m_numberCells));
    stream.writeTextElement("numberObservations", toString(m_numberObservations));

    stream.writeStartElement("quantiles");
    for (int i = 0; i < m_quantiles.size(); i++) {
      stream.writeTextElement("value", toString(m_quantiles[i]));
    }
    stream.writeEndElement();

    stream.writeStartElement("idealNum");
    for (int i = 0; i < m_idealNum.size(); i++) {
      stream.writeTextElement("value", toString(m_idealNum[i]));
    }
    stream.writeEndElement();
    
    stream.writeStartElement("n");
    for (int i = 0; i < m_n.size(); i++) {
      stream.writeTextElement("value", toString(m_n[i]));
    }
    stream.writeEndElement();

    stream.writeStartElement("quantileValues");
    for (int i = 0; i < m_quantileValues.size(); i++) {
      stream.writeTextElement("value", toString(m_quantileValues[i]));
    }
    stream.writeEndElement();
    stream.writeEndElement();

  }



  StatCumProbDistDynCalc::XmlHandler::XmlHandler(StatCumProbDistDynCalc *probabilityCalc, Project *project) {   // TODO: does xml stuff need project???
    m_probabilityCalc = probabilityCalc;
    m_project = project;   // TODO: does xml stuff need project???
    m_characters = "";
  }



  StatCumProbDistDynCalc::XmlHandler::~XmlHandler() {
    // ??? compile error ??? delete m_project;    // TODO: does xml stuff need project???
    m_project = NULL;
  }



  bool StatCumProbDistDynCalc::XmlHandler::startElement(const QString &namespaceURI, 
                                                                const QString &localName,
                                                                const QString &qName,
                                                                const QXmlAttributes &atts) {
    m_characters = "";
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      // no element attibutes to evaluate
    }
    return true;
  }



  bool StatCumProbDistDynCalc::XmlHandler::characters(const QString &ch) {
    m_characters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool StatCumProbDistDynCalc::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName) {
    if (!m_characters.isEmpty()) {
      if (localName == "id") {
        delete m_probabilityCalc->m_id;
        m_probabilityCalc->m_id = NULL;
        m_probabilityCalc->m_id = new QUuid(m_characters);
      }
      if (localName == "numberCells") {
        m_probabilityCalc->m_numberCells = toInt(m_characters);
      }
      if (localName == "numberObservations") {
        m_probabilityCalc->m_numberObservations = toInt(m_characters);
      }
//
//    stream.writeStartElement("quantileValue");
//    for (int i = 0; i < m_quantiles.size(); i++) {
//      stream.writeTextElement("value", toString(m_quantiles[i]));
//    }
//    stream.writeEndElement();
//
//    stream.writeStartElement("idealNum");
//    for (int i = 0; i < m_idealNum.size(); i++) {
//      stream.writeTextElement("value", toString(m_idealNum[i]));
//    }
//    stream.writeEndElement();
//    
//    stream.writeStartElement("n");
//    for (int i = 0; i < m_n.size(); i++) {
//      stream.writeTextElement("value", toString(m_n[i]));
//    }
//    stream.writeEndElement();
//
//    stream.writeStartElement("quantileValues");
//    for (int i = 0; i < m_quantileValues.size(); i++) {
//      stream.writeTextElement("value", toString(m_quantileValues[i]));

      m_characters = "";
    }
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }



  QDataStream &StatCumProbDistDynCalc::write(QDataStream &stream) const {
    stream << (qint32)m_numberCells
           << (qint32)m_numberQuantiles 
           << m_quantiles
           << m_idealNum
           << m_n
           << m_quantileValues
           << (qint32)m_numberObservations;
    return stream;
  }



  QDataStream &StatCumProbDistDynCalc::read(QDataStream &stream) {
    qint32 numCells, numQuantiles, numObservations;
    stream >> numCells
           >> numQuantiles
           >> m_quantiles
           >> m_idealNum
           >> m_n
           >> m_quantileValues
           >> numObservations;
    m_numberCells = (unsigned int)numCells;
    m_numberQuantiles  = (unsigned int)numQuantiles;
    m_numberObservations   = (unsigned int)numObservations;
    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const StatCumProbDistDynCalc &scpddc) {
    return scpddc.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, StatCumProbDistDynCalc &scpddc) {
    return scpddc.read(stream);
  }

}// end namespace Isis

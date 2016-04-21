/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */ 
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <QtGlobal>
#include <QCoreApplication>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include <boost/foreach.hpp>

#include "DatumFunctoid.h"
#include "FileName.h"
#include "IString.h"
#include "IException.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

using namespace std;

namespace Isis {

DatumFunctoidFactory *DatumFunctoidFactory::m_maker = 0;

DatumFunctoidFactory::DatumFunctoidFactory() {
//  This ensures this singleton is shut down when the application exists
   qAddPostRoutine(DieAtExit);
   init();
   return;
}

DatumFunctoidFactory::~DatumFunctoidFactory() { } 

DatumFunctoidFactory *DatumFunctoidFactory::getInstance() { 
  if (!m_maker) {
    m_maker = new DatumFunctoidFactory();
  }
  return (m_maker);
}



QStringList DatumFunctoidFactory::algorithms() const {
  QStringList functoids;

  BOOST_FOREACH ( SharedDatumFunctoid d, m_functoids ) {
    functoids.push_back( d->name() );
  }
  return (functoids);
}

DatumFunctoidList DatumFunctoidFactory::create(const QString &specs,
                                               const bool &errorIfEmpty) 
                                               const {
  DatumFunctoidList functoids; 

  // Individual algorithm specifications are separated by vertical bars
  QStringList algorithms = specs.split(",", QString::SkipEmptyParts);
  // cout << "create: got " << algorithms.size() << " algorithms...\n";
  if ( algorithms.size() == 0 ) {
    if ( errorIfEmpty ) {
      QString mess = "No functoid algorithm specifictions found!"; 
      throw IException(IException::User, mess, _FILEINFO_);
    }
    return ( functoids );
  }

  IException ie;
  int nerrors(0);
  BOOST_FOREACH ( QString functoid, algorithms ) {
    SharedDatumFunctoid f( make(functoid) );
    if ( f.isNull() ) {
      QString mess = "Algorithm \"" + functoid + "\" was not found!";
      ie.append(IException(IException::User, mess, _FILEINFO_));
      nerrors++;
    }
    else {
      functoids.append(f); 
    }
  }

  // Toss errors if they occurred
  if ( nerrors > 0 ) { throw ie; }

  // return the list
  return ( functoids );
}


DatumFunctoid *DatumFunctoidFactory::make(const QString &funcspec) const {
  PvlFlatMap parms = parseParameters(funcspec);
  QString algorithm = parms.get("Functoid","").toLower();
  BOOST_FOREACH ( SharedDatumFunctoid d, m_functoids ) {
    if ( d->name().toLower() == algorithm ) {
      return ( d->clone(parms) );
    }
  }

  // Did not find a functoid
  return ( 0 );
}

/**
 * @brief Parse a parameter string for values and return in parameter map
 * 
 * @author 2015-09-08 Kris Becker
 * 
 * @param parameters 
 * 
 * @return PvlFlatMap 
 */
PvlFlatMap DatumFunctoidFactory::parseParameters(const QString &parameters) 
                                                    const {
  PvlFlatMap pmap;

  // If the string is empty, return an empty paramter list
  QStringList parts = parameters.split("@");
  if ( parts.size() == 0 ) { return (pmap); }

  // Pull the first value and name it the Functoid
  QString parmtag = parts.takeFirst();
  pmap.add("Functoid", parmtag);

  // All good so far, parse each parameter string
  BOOST_FOREACH ( QString specs, parts ) {

    // Is it valid?
    QStringList parmvaltag = specs.split(":");
    if ( 2 != parmvaltag.size() ) {
      QString mess = "Invalid parameter at or near [" + specs + "] in \"" +
                     parameters + "\" - must be of the form "
                     "\"name@parm1:value1@parm2:value2...\"";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Looks good, set the parameters
    pmap.add(parmvaltag[0], parmvaltag[1]);
  }

  // All done...
  return ( pmap );
}


 /**
   * @brief Exit termination routine
   *
   * This (static) method ensure this object is destroyed when Qt exits.  
   *
   * Note that it is error to add this to the system _atexit() routine because
   * this object utilizes Qt classes.  At the time the atexit call stack is
   * executed, Qt is long gone resulting in Very Bad Things.  Fortunately, Qt has
   * an exit stack function as well.  This method is added to the Qt exit call
   * stack.
   */
  void DatumFunctoidFactory::DieAtExit() {
    delete  m_maker;
    m_maker = 0;
    return;
  }


void DatumFunctoidFactory::init() {
  m_functoids.clear();

  m_functoids.append( SharedDatumFunctoid(new AverageRadius()) );
  m_functoids.append( SharedDatumFunctoid(new MedianRadius()) );
  m_functoids.append( SharedDatumFunctoid(new StandardDeviationRadius()) );
  m_functoids.append( SharedDatumFunctoid(new MaximumRadius()) );
  m_functoids.append( SharedDatumFunctoid(new MinimumRadius()) );
  m_functoids.append( SharedDatumFunctoid(new AverageDistance()) );
  m_functoids.append( SharedDatumFunctoid(new MedianDistance()) );
  m_functoids.append( SharedDatumFunctoid(new StandardDeviationDistance()) );
  m_functoids.append( SharedDatumFunctoid(new MaximumDistance()) );
  m_functoids.append( SharedDatumFunctoid(new MinimumDistance()) );
  m_functoids.append( SharedDatumFunctoid(new TypeCount()) );
  m_functoids.append( SharedDatumFunctoid(new ShepardsRadius()) );
  m_functoids.append( SharedDatumFunctoid(new FrankeNelsonRadius()) );
  m_functoids.append( SharedDatumFunctoid(new NaturalNeighborRadius()) );
  return;
}

} // namespace Isis

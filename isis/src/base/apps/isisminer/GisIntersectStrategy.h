#ifndef GisIntersectStrategy_h
#define GisIntersectStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: GisIntersectStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $ 
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

// parent class
#include "Strategy.h"

// Qt library
#include <QString>

// SharedResource, SharedGisGeometry, ResourceList typedef
#include "GisGeometry.h"
#include "Resource.h"

namespace Isis {

  class PvlFlatMap; 
  class PvlObject;

  /**
   * @brief GisIntersectStrategy - provides basic GIS Strategy others can derive from
   *  
   * Object = Strategy 
   *   Type = Gis* 
   *   Name = GisBasic*
   * EndObject
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-03 Kristin Berry - Merged with GisBasic class, added
   *                           documentation and tests.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class GisIntersectStrategy : public Strategy {
  
    public:
      GisIntersectStrategy();
      GisIntersectStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~GisIntersectStrategy();
  
      int apply(ResourceList &resources,  const ResourceList &globals);
      int apply(SharedResource &resource,  const ResourceList &globals);
  
  protected: 
      GisGeometry *geomFromPvl(const PvlFlatMap &parms, 
                               const ResourceList &globals) const; 
  
      SharedGisGeometry m_geom; //!< Shared geometry object for this class
      
    private:
      bool    m_computeOverlap; //!< If true, compute the overlap ratio and add it to the Resource.
      QString m_ratioKey; //!< Value of the PVL keyword "RatioRef".
  
  };

} // Namespace Isis

#endif

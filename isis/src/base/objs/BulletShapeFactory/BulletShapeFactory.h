#ifndef BulletShapeFactory_h
#define BulletShapeFactory_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/25 01:37:55 $
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

#include <QMap>
#include <QSharedPointer>

#include "BulletWorldManager.h"
#include "Pvl.h"

namespace Isis {

class Target;
class BulletShapeModel;

/**
 * @brief Class for managing the construction and destruction of 
 *        BulletWorldManagers
 * 
 * This class is a singleton that constructs and holds BulletWorldMangers. Its 
 * intent is to provide a single instance of a BulletShapeModel to many cubes 
 * that refer to the same model. These models typically are large and may 
 * require significant memory/resources. Unique instances of these models for
 * applications (e.g., qview, qmos, jigsaw, etc...) that open many cubes with 
 * Bullet shapes at a time will quickly consume resources and lead to failures. 
 * 
 * When an BulletWorldManager is requested the cache will first be checked if 
 * one for that file already exists. If it exists in the cache, a shared 
 * reference to the existing BulletWorldManger is returned. If one does not 
 * exist yet, a new BulletWorldManger is created on the heap. Reference counting 
 * is presumed to be managed in BulletWorldManager. 
 *  
 * It follows that when creating a BulletShapeModel, a BulletWorldManager is 
 * also created and added to the cache. The pointers returned by the create
 * methods are owned by the caller and can safely be deleted without affecting 
 * the cache. 
 *  
 * When creating BulletWordManagers, you can force the creation of a new one in 
 * the API. This may become necessary as geometry operations can slow down if 
 * all are using the same instance of the a Bullet shape. Creating a new world 
 * with the same shape file will replace the existing one with a new instance. 
 * All existing instances are unaffected. Subsequent requests for the same shape 
 * file will use the newly create one providing better control over balancing 
 * the load. 
 *  
 * One can explicitly remove a Bullet world object at any time wihthout 
 * affecting existing instances. However, subsequent requests will create new 
 * instance if they don't exist. Upon application exit, they are removed if not 
 * explicity removed. 
 * 
 * @author 2018-07-21 UA/OSIRIS-REx IPWG Team 
 * @internal 
 *   @history 2018-07-21 UA/OSIRIS-REx IPWG Team  - Original Version.
 *   @history 2018-09-14 UA/OSIRIS-REx IPWG Team - Improved implementation;
 *                          return a BulletShapeModel as well as a
 *                          BulletWorldManager which is the fundamental shared
 *                          component
 */
  class BulletShapeFactory {
    public:
      static BulletShapeFactory *getInstance();

      BulletWorldManager *createWorld(const QString &shapeFile, const bool forceNew = false);
      BulletShapeModel   *createShape(const QString &shapeFile, Target *target, Pvl &pvl,
                                      const bool forceNew = false);
      int remove(const QString &shapeFile);

      int size() const;
      bool exists(const QString &shapeFile) const;

    private:
      // This factory is a singleton, so the constructor and destructor are private.
      // To get an instance of this class use the static getInstance() method.
      BulletShapeFactory();
      ~BulletShapeFactory();

      // This method deletes the singleton factory when QT exits
      static void DieAtExit();

      QString fullFilePath(const QString &filePath) const;

      static BulletShapeFactory         *m_maker; /**!< Pointer to the singleton factory. */

      typedef QSharedPointer<BulletWorldManager> SharedBulletWorld;
      QHash<QString, SharedBulletWorld>  m_cache; /**!< The cache of created
                                                        target shapes. */
  };

};

#endif
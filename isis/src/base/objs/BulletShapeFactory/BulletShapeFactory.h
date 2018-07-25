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

/**
 * @brief Class for managing the construction and destruction of 
 *        BulletWorldManagers
 * 
 * This class is a singleton that constructs and holds BulletWorldMangers. Due 
 * to the time required to construct a new BulletWorldManger and the large 
 * memory requirements, this class limits their creation and the number that can 
 * be held in memory at one time. 
 * 
 * When an BulletWorldManger is requested the cache will first be checked if one
 * for that file already exists. If one does, a shared pointer to the existing 
 * BulletWorldManger is returned. If one does not exist yet, a new
 * BulletWorldManger is created on the heap. Because a shared pointer is 
 * returned to the object, reference counting is inhererently maintined. Once 
 * can explicitly remove a Bullet instance at any time but subsequent requests 
 * will create new instances if they dont exist. Upon application exit, they are 
 * removed if not explicity requested. 
 * 
 * @author 2018-07-21 Kris Becker
 * @internal 
 *   @history 2018-07-21 Kris Becker - Original Version.
 */
  class BulletShapeFactory {
    public:
      typedef QSharedPointer<BulletWorldManager> SharedBulletWorld;
      static BulletShapeFactory *getInstance();

      BulletWorldManager *create(const QString &shapeFile);
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
      QHash<QString, SharedBulletWorld>  m_cache; /**!< The cache of created
                                                        target shapes. */
  };

};

#endif
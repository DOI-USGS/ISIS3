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

#include <QtGlobal>
#include <QCoreApplication>
#include <QScopedPointer>

#include "FileName.h"
#include "IException.h"

#include "BulletShapeFactory.h"
#include "BulletShapeModel.h"
#include "Target.h"

using namespace std;

namespace Isis {

  /**
   * Initialize the singleton factory pointer 
   */
  BulletShapeFactory *BulletShapeFactory::m_maker = 0;


  /**
   * This constructor will initialize the BulletShapeFactory object to default 
   * values. The default maximum number of shape models held in memory is 10
   */
  BulletShapeFactory::BulletShapeFactory()
    : m_cache() {
    // This ensures this singleton is shut down when the application exists
    qAddPostRoutine(DieAtExit);
    return;
  }


  /**
   * Destructor that frees all of the BulletWorldManager managed by this object
   */
  BulletShapeFactory::~BulletShapeFactory() {
      m_cache.clear();
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
  void BulletShapeFactory::DieAtExit() {
    delete  m_maker;
    m_maker = 0;
    return;
  }


  /**
   * @brief Retrieve reference to Singleton instance of this object 
   *  
   * The only access provided for Singleton instance of this object. All method 
   * access is made through the pointer returned by this method. The object is 
   * created upon the first call to this method. The object is deleted when Qt 
   * shuts down. 
   * 
   * @return @b BulletShapeFactory* Pointer to Singleton instance of this object.
   */
  BulletShapeFactory *BulletShapeFactory::getInstance() {
    if (!m_maker) {
      m_maker = new BulletShapeFactory();
    }
    return (m_maker);
  }


  /**
   * Helper function that takes a file path and returns the full file path.
   * 
   * @param filePath The file path to expand.
   * 
   * @return @b QString The full file path.
   * 
   * @see FileName::Expanded()
   */
  QString BulletShapeFactory::fullFilePath(const QString &filePath) const {
    FileName fileName(filePath);
    return ( fileName.expanded() );
  }


  /**
   * Return a BulletWorldManager object for a shape model file 
   *  
   * This method returns a requested BulletWorldManager which may already exit. If 
   * it doesn't exist or the caller wishes to force a new one to be created (via 
   * forceNew), then it is created with a BulletTargetShape added for the file 
   * provided. 
   *  
   * If the caller sets forceNew = true, then if a model exists in the cache by 
   * the same filename, it will be replaced and any future request for the same 
   * file will return this instance. Note that all other instances will still be 
   * valid and the memory resource for each will be properly managed. 
   *  
   * @param shapeFile The path to the file to create a BulletWorldManager from 
   * @param forceNew  Force the creation of a new instance even if one already 
   *                  exists. This may be needed if too many uses impedes
   *                  performance, which can happen with Bullet worlds
   * 
   * @return @b BulletWorldManager* A pointer to the loaded target shape. This 
   *                                pointer is owned by the caller and must be
   *                                deleted when no longer in use.
   * 
   * @see BulletShapeFactory::remove
   * 
   * @throws IException::Programmer Unable to create the Bullet world
   */
  BulletWorldManager *BulletShapeFactory::createWorld(const QString &shapeFile, 
                                                      const bool forceNew) {
    // Get the full path to the file
    QString fullPath = fullFilePath(shapeFile);

    // Check existance of Bullet model in the cache or create a new one if forced
    if ( !exists(fullPath) || forceNew ) {
      // Otherwise, make a new BulletWorldManager
      QScopedPointer<BulletTargetShape> shape(BulletTargetShape::load(shapeFile));
      if ( shape.isNull() ) {
          QString msg = "Unable to create Bullet shape from file " + shapeFile;
          throw IException(IException::User, msg, _FILEINFO_);
      }
        
      // Add new model to cache
      SharedBulletWorld world( new BulletWorldManager( shape->name() ));
      world->addTarget( shape.take() );
      m_cache.insert( fullPath, world );
    }

    // Now ensure there is a model in the cache
    return ( new BulletWorldManager( *m_cache[fullPath] ) );
  }

/**
 * Return a BulletShapeModel containing a BulletWorldManager 
 *  
 * This method will return a complete BulletShapeModel initialized from a shape 
 * file. If a previous model has been created from a previous call using the 
 * same file a shared instance of this model will be returned. 
 * 
 * @param shapeFile Name of shape file
 * @param target    A Target object associated with the shape
 * @param pvl       A Pvl lable file accompanying the files 
 * @param forceNew  Force the creation of a new BulletWorldManager instance even
 *                  if one already exists. This may be needed if too many uses
 *                  impedes performance, which can happen with Bullet worlds
 * 
 * @return BulletShapeModel* A pointer to the newly created shape model. This 
 *                           pointer is owned by the caller
 */
  BulletShapeModel *BulletShapeFactory::createShape(const QString &shapeFile, 
                                                    Target *target, Pvl &pvl,
                                                    const bool forceNew) {
    return ( new BulletShapeModel(createWorld(shapeFile, forceNew), target, pvl) );
  }

  
  /**
   * Notify the manager that an BulletWorldManager is no longer in use. This
   * method will decrease the internal reference count for the target shape.
   * Then if there are not more references. the EmbreeTargetShape is destroyed
   * to free up memory.
   * 
   * @param shapeFile The path to the file used to create the BulletWorldManager.
   *                  This should be the same as the parameter used with
   *                  BulletShapeFactory::create to get a pointer to the
   *                  BulletWorldManager.
   * 
   * @throws IException::Programmer
   */
  int BulletShapeFactory::remove(const QString &shapeFile) {
    // Get the full path to the file
    QString fullPath = fullFilePath(shapeFile);
    return ( m_cache.remove(fullPath) );
  }

  /**
   * Return the number of currently stored BulletWorldManager.
   * 
   * @return @b int The number of currently stored BulletWorldManager.
   */
  int BulletShapeFactory::size() const {
    return ( m_cache.size() );
  }

  /**
   * Check if there is an already created BulletWorldManager for a file.
   * 
   * @param shapeFile The path to the file to check for
   */
  bool BulletShapeFactory::exists(const QString &shapeFile) const{
    return ( m_cache.contains( fullFilePath(shapeFile) ) );
  }
}
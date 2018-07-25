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
   * Destructor that frees all of the EmbreeTargetShapes managed by this object
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
   * @param shapeFile The path to the file to create an EmbreeTargetShape from
   * 
   * @return @b BulletWorldManager* A pointer to the loaded target shape. This 
   *                                pointer is owned by the caller and must be
   *                                deleted when no longer in use.
   * 
   * @see BulletShapeFactory::remove
   * 
   * @throws IException::Programmer
   */
  BulletWorldManager *BulletShapeFactory::create(const QString &shapeFile) {
    // Get the full path to the file
    QString fullPath = fullFilePath(shapeFile);

    // Check existance of Bullet model in the cache
    if ( !exists(fullPath) ) {
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
   * Notify the manager that an EmbreeTargetShape is no longer in use. This
   * method will decrease the internal reference count for the target shape.
   * Then if there are not more references. the EmbreeTargetShape is destroyed
   * to free up memory.
   * 
   * @param shapeFile The path to the file used to create the EmbreeTargetShape.
   *                  This should be the same as the parameter used with
   *                  BulletShapeFactory::create to get a pointer to the
   *                  EmbreeTargetShape.
   * 
   * @throws IException::Programmer
   */
  int BulletShapeFactory::remove(const QString &shapeFile) {
    // Get the full path to the file
    QString fullPath = fullFilePath(shapeFile);
    return ( m_cache.remove(fullPath) );
  }

  /**
   * Return the number of currently stored EmbreeTargetShapes.
   * 
   * @return @b int The number of currently stored EmbreeTargetShapes.
   */
  int BulletShapeFactory::size() const {
    return ( m_cache.size() );
  }

  /**
   * Check if there is an already created EmbreeTargetShape for a file.
   * 
   * @param shapeFile The path to the file to check for
   */
  bool BulletShapeFactory::exists(const QString &shapeFile) const{
    return ( m_cache.contains( fullFilePath(shapeFile) ) );
  }
}
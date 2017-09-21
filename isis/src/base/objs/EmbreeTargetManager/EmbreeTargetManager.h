#ifndef EmbreeTargetManager_h
#define EmbreeTargetManager_h
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

#include "EmbreeTargetShape.h"

namespace Isis {

/**
 * @brief Class for managing the construction and destruction of EmbreeTargetShapes.
 * 
 * This class is a singleton that constructs and holds EmbreeTargetShape.
 * Due to the time required to construct a new EmbreeTargetShape and the
 * large memory requirements, this class limits their creation and the number
 * that can be held in memory at one time.
 * 
 * When an EmbreeTargetShape is requested the EmbreeTargetManager will first
 * check if an EmbreeTargetShape for that file already exists. If one does,
 * a pointer to the existing EmbreeTargetShape is given and the reference count
 * is incremented. If one does not exist yet, a new EmbreeTargetShape is
 * created on the heap. When an EmbreeTargetShape is no longer used, the
 * EmbreeTargetManager should be notified and it will decrement the reference
 * count on that EmbreeTargetShape. If the EmbreeTargetShape is no longer used
 * by anything, then it is deleted.
 * 
 * @author 2017-05-08 Jesse mapel
 * @internal 
 *   @history 2017-05-08  Jesse Mapel - Original Version.
 */
  class EmbreeTargetManager {
    public:
      static EmbreeTargetManager *getInstance();

      EmbreeTargetShape *create(const QString &shapeFile);
      void free(const QString &shapeFile);

      int currentCacheSize() const;
      int maxCacheSize() const;
      bool inCache(const QString &shapeFile) const;
      void setMaxCacheSize(const int &numShapes);

    private:
      // This factory is a singleton, so the constructor and destructor are private.
      // To get an instance of this class use the static getInstance() method.
      EmbreeTargetManager();
      ~EmbreeTargetManager();

      // This method deletes the singleton factory when QT exits
      static void DieAtExit();

      /**
       * Reference counting container for EmbreeTargetShapes
       * 
       * @author 2017-05-08 Jesse mapel
       * @internal 
       *   @history 2017-05-08  Jesse Mapel - Original Version.
       */
      struct EmbreeTargetShapeContainer {
        /**
         * Default constructor
         */
        EmbreeTargetShapeContainer()
          : m_fullFilePath(""),
            m_targetShape(0),
            m_referenceCount(0) { }

        /**
          * Constructs a container for a target shape and path
          * 
          * @param fullPath The full path to the file used to create the target shape
          * @param targetShape The EmbreeTargetShape to contain
          */
        EmbreeTargetShapeContainer(const QString &fullPath,
                                   EmbreeTargetShape *targetShape)
          : m_fullFilePath(fullPath),
            m_targetShape(targetShape),
            m_referenceCount(0) { }

        QString            m_fullFilePath;   /**!< The full path to the file used to
                                                   construct the EmbreeTargetShape. */
        EmbreeTargetShape *m_targetShape;    /**!< The EmbreeTargetShape. */
        int                m_referenceCount; /**!< The number of objects using the
                                                   EmbreeTargetShape. */
      };

      QString fullFilePath(const QString &filePath) const;
      void removeTargetShape(const QString &shapeFile);

      static EmbreeTargetManager *m_maker; /**!< Pointer to the singleton factory. */
      QMap<QString, EmbreeTargetShapeContainer> m_targeCache; /**!< The cache of created
                                                                    target shapes. */
      int m_maxCacheSize; /**!< The maximum number of target shapes kept at once. */
  };

};

#endif
#ifndef EmbreeTargetManager_h
#define EmbreeTargetManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
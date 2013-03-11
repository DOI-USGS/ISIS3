#ifndef ImportImagesWorkOrder_H
#define ImportImagesWorkOrder_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "WorkOrder.h"

#include <functional>

#include <QDir>
#include <QMutex>

#include "IException.h"

class QString;

namespace Isis {
  class Cube;
  class FileName;

  /**
   * @brief Add cubes to a project
   *
   * Asks the user for a list of cube file names and whether they should be copied into
   * the project. The cubes are then converted to external cube label files inside the project (and
   * cube files if the user said to copy the DN data). These files are then handed off to the
   * project.
   *
   * @author 2012-??-?? Steven Lambright and Stuart Sides
   *
   * @internal
   *   @history 2012-08-24 Steven Lambright and Stuart Sides - Removed GUI-thread slowdown of
   *                           image verification
   *   @history 2012-08-29 Steven Lambright - This work order should now work correctly even when
   *                           just 1 QtConcurrent thread is enabled.
   *   @history 2012-10-02 Steven Lambright - Image ID's are now guaranteed to be the same after
   *                           undo/redo. This is a beginning step for making work orders whose
   *                           images are freed from memory undo/redo correctly.
   *   @history 2012-10-29 Steven Lambright - Added a prompt to save the project if importing a lot
   *                           of images to a temporary project.
   */
  class ImportImagesWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ImportImagesWorkOrder(Project *project);
      ImportImagesWorkOrder(const ImportImagesWorkOrder &other);
      ~ImportImagesWorkOrder();

      virtual ImportImagesWorkOrder *clone() const;

      bool execute();

      void asyncRedo();
      void postSyncRedo();
      void asyncUndo();
      void postSyncUndo();

    private:
      ImportImagesWorkOrder &operator=(const ImportImagesWorkOrder &rhs);

      /**
       * This copies the given cube(s) into the project. This is designed to work with
       *   QtConcurrentMap.
       *
       * @author 2012-??-?? ???
       *
       * @internal
       */
      class OriginalFileToProjectCubeFunctor :
          public std::unary_function<const FileName &, Cube *> {
        public:
          OriginalFileToProjectCubeFunctor(QThread *guiThread,
                                           QDir destinationFolder, bool copyDnData);
          OriginalFileToProjectCubeFunctor(const OriginalFileToProjectCubeFunctor &other);
          ~OriginalFileToProjectCubeFunctor();

          Cube *operator()(const FileName &original);

          IException errors() const;

        private:
          //! Not implemented
          OriginalFileToProjectCubeFunctor &operator=(const OriginalFileToProjectCubeFunctor &rhs);

          QDir m_destinationFolder;
          bool m_copyDnData;
          QThread *m_guiThread;

          QMutex m_errorsLock;
          QSharedPointer<IException> m_errors;
          QSharedPointer<int> m_numErrors;
      };

    private:
      void importConfirmedImages(QStringList confirmedImages, bool copyDnData);

    private:
      ImageList *m_newImages;
      QString m_warning;
  };
}
#endif // ImportImagesWorkOrder_H

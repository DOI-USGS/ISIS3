#ifndef Project_H
#define Project_H
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
#include <QDir>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QUndoStack>
#include <QXmlDefaultHandler>

class QMutex;
class QProgressBar;
class QXmlAttributes;
class QXmlStreamWriter;

#include "ControlList.h"
#include "Directory.h"
#include "GuiCameraList.h"
#include "ImageList.h"
#include "ShapeList.h"
#include "TargetBody.h"
#include "XmlStackedHandler.h"

namespace Isis {
  class BundleSolutionInfo;
  class BundleSettings;
  class Control;
  class ControlList;
  class CorrelationMatrix;
  class FileName;
  class Shape;
  class ImageReader;
  class ProgressBar;
  class ShapeReader;
  class WorkOrder;

  /**
   *
   * @brief The main project for cnetsuite
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2012-07-27 Kimberly Oyama - Added comments to some of the methods.
   *   @history 2012-09-04 Tracie Sucharski - Renamed addCNets to addCnets, controlNetRoot to
   *                           cnetRoot, networkLoaded to cnetLoaded.  Added new method,
   *                           addCnetFolder.
   *   @history 2012-09-11 Tracie Sucharski - Added mutex accessor method.
   *   @history 2012-09-12 Tracie Sucharski - Implemented ControlList instead of QList<Control *>,
   *                          re-ordered some methods to match header order.
   *   @history 2012-09-12 Steven Lambright - Renamed imageList() to createOrRetrieveImageList(),
   *                           added imageList() and image().
   *   @history 2012-09-17 Steven Lambright - Reduced the time complexity of image() to log(n) from
   *                           n/2. This method is often called n times.
   *   @history 2012-09-17 Steven Lambright - Added crash detection/cleanup. Prompt is coded but
   *                           disabled (we'll find a good wording or handle recovery better when
   *                           we don't expect so many crashes during development).
   *   @history 2012-10-29 Steven Lambright and Stuart Sides - Added isTemporaryProject(). This is
   *                           useful for the import images to know if it should prompt the user to
   *                           save their project.
   *   @history 2013-05-14 Jeannie Backer - Used return status of c++ system() in the constructor
   *                           to verify that the call was successful.
   *   @history 2014-07-14 Kimberly Oyama - Updated to better meet programming standards. Added
   *                           support for correlation matrix.
   *   @history 2015-02-20 Jeannie Backer - Replaced BundleResults references with
   *                           BundleSolutionInfo and BundleStatistics references with BundleResults
   *                           due to class name changes.
   *   @history 2015-09-03 Jeannie Backer - Removed svn merge conflict comment lines. Removed call
   *                           to save BundleSolutionInfo as an xml file. Added hdf5 preliminary
   *                           serialization calls. Some ISIS coding standards improvements.
   *   @history 2015-10-14 Jeffrey Covington - Declared Project * as a Qt
   *                           metatype for use with QVariant.
   *   @history 2016-06-23 Tracie Sucharski - Added a member variable for active control network
   *                           and active image list, along with accessor methods.
   *   @history 2016-07-06 Tracie Sucharski - Changed the ImageReader to require footprints, because
   *                           ImageReader class was changed so that footprints are no longer
   *                           created if not required.
   *   @history 2016-07-06 Tracie Sucharski - Add import shape models to project. 
   *   @history 2016-11-09 Tyler Wilson - Added try-catch blocks around reader.parse calls in
   *                           the open function, so that warnings/errors are output to the
   *                           warnings tab of the GUI instead of causing the application 
   *                           to exit.  Fixes #4488.
   */
  class Project : public QObject {
    Q_OBJECT
    public:
      Project(Directory &directory, QObject *parent = 0);
      ~Project();

      static QStringList images(QStringList);
      static QStringList shapes(QStringList);
//      static QStringList verifyCNets(QStringList);

      QList<QAction *> userPreferenceActions();
      QDir addBundleSolutionInfoFolder(QString folder);
      QDir addCnetFolder(QString prefix);
      void addControl(Control *control);
      QDir addImageFolder(QString prefix);
      void addImages(QStringList imageFiles);
      void addImages(ImageList newImages);
      QDir addShapeFolder(QString prefix);
      void addShapes(QStringList shapeFiles);
      void addShapes(ShapeList newShapes);
      void addBundleSolutionInfo(BundleSolutionInfo *bundleSolutionInfo);
      void loadBundleSolutionInfo(BundleSolutionInfo *bundleSolutionInfo);
      Control *control(QString id);
      Directory *directory() const;
      Image *image(QString id);
      ImageList *imageList(QString name);
      Shape *shape(QString id);
      ShapeList *shapeList(QString name);
//       CorrelationMatrix *correlationMatrix();
      bool isTemporaryProject() const;
      WorkOrder *lastNotUndoneWorkOrder();
      const WorkOrder *lastNotUndoneWorkOrder() const;
      QString name() const;
      QMutex *mutex();
      QString projectRoot() const;
      void setName(QString newName);
      QUndoStack *undoStack();
      void waitForImageReaderFinished();
      void waitForShapeReaderFinished();
      QList<WorkOrder *> workOrderHistory();

      void setActiveControl(Control *);
      Control  *activeControl();
      void setActiveImageList(ImageList *);
      ImageList *activeImageList();

      static QString cnetRoot(QString projectRoot);
      QString cnetRoot() const;
      QList<ControlList *> controls();
      ControlList *controlList(QString name);

      static QString imageDataRoot(QString projectRoot);
      QString imageDataRoot() const;
      QList<ImageList *> images();

      static QString shapeDataRoot(QString projectRoot);
      QString shapeDataRoot() const;
      QList<ShapeList *> shapes();

      static QString targetBodyRoot(QString projectRoot);
      QString targetBodyRoot() const;
      TargetBodyList targetBodies();

      static QString resultsRoot(QString projectRoot);
      QString resultsRoot() const;
      static QString bundleSolutionInfoRoot(QString projectRoot);
      QString bundleSolutionInfoRoot() const;
      QList<BundleSolutionInfo *> bundleSolutionInfo();

      void deleteAllProjectFiles();
      void relocateProjectRoot(QString newRoot);

      BundleSettings *bundleSettings() {return m_bundleSettings;}

      QProgressBar *progress();

      void removeImages(ImageList &imageList);

      void save();
      void save(FileName newPath, bool verifyPathDoesntExist = true);

      void addToProject(WorkOrder *);

      template<typename Data> void warn(QString text, Data relevantData);

      void warn(QString text);

    signals:
      /**
       * apparently not used?
       */
//      void allImagesClosed();

      /**
       * Emitted when new ControlList added to Project
       * receivers: ProjectTreeWidget
       */
      void controlListAdded(ControlList *controls);

      /**
       * Emitted when new Control added to Project
       * receivers: ProjectTreeWidget
       */
      void controlAdded(Control *control);

      /**
       * Emitted when new images are available.
       * receivers: Directory, Project, WorkOrder
       */
      void imagesAdded(ImageList *images);

      /**
       * Emitted when new shape model images are available.
       * receivers: Directory, Project, WorkOrder
       */
      void shapesAdded(ShapeList *shapes);

      /**
       * Emitted when new BundleSolutionInfo available from jigsaw
       * receivers: ProjectTreeWidget (TODO: should this be the Directory?)
       */
      void bundleSolutionInfoAdded(BundleSolutionInfo *bundleSolutionInfo);

      /**
       * Emitted when new TargetBody objects added to project
       * receivers: Directory
       */
      void targetsAdded(TargetBodyList *targets);

      /**
       * Emitted when new GuiCamera objects added to project
       * receivers: Directory
       */
      void guiCamerasAdded(GuiCameraList *targets);

      /**
       * Emitted when project name is changed
       * receivers: ProjectTreeWidget
       */
      void nameChanged(QString newName);

      /**
       * Emitted when project loaded
       * receivers: CNetSuiteMainWindow, Directory, HistoryTreeWidget
       */
      void projectLoaded(Project *);

      /**
       * Emitted when project location moved
       * receivers: Control, BundleSolutionInfo, Image, TargetBody
       */
      void projectRelocated(Project *);

      void workOrderStarting(WorkOrder *);
      void workOrderFinished(WorkOrder *);

    public slots:
      void open(QString);

    private slots:
      void controlClosed(QObject *control);
      void controlListDeleted(QObject *controlList);
      void imagesReady(ImageList);
      void addTargetsFromImportedImagesToProject(ImageList *imageList);
      void addCamerasFromImportedImagesToProject(ImageList *imageList);
      void imageClosed(QObject *image);
      void imageListDeleted(QObject *imageList);
      void bundleSolutionInfoClosed(QObject *bundleSolutionInfo);
      void targetBodyClosed(QObject *targetBodyObj);
      void shapesReady(ShapeList shapes);
      void shapeClosed(QObject *shape);
      void shapeListDeleted(QObject *shapeList);

    private:
      Project(const Project &other);
      Project &operator=(const Project &rhs);
      void createFolders();
      ControlList *createOrRetrieveControlList(QString name);
      ImageList *createOrRetrieveImageList(QString name);
      ShapeList *createOrRetrieveShapeList(QString name);


      QString nextImageListGroupName();
//       void removeImage(Image *image);

      void save(QXmlStreamWriter &stream, FileName newProjectRoot) const;
      void saveHistory(QXmlStreamWriter &stream) const;
      void saveWarnings(QXmlStreamWriter &stream) const;

      void storeWarning(QString text);
      void storeWarning(QString text, const ImageList &relevantData);

    private:
      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(Project *project);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Project *m_project;
          QList<ImageList *> m_imageLists;
          QList<ShapeList *> m_shapeLists;
          QList<ControlList *> m_controls;
          WorkOrder *m_workOrder;
      };

    private:
      QDir *m_projectRoot;
      QDir *m_cnetRoot;
      QDir m_currentCnetFolder;
      QPointer<Directory> m_directory;
      QList<ImageList *> *m_images;
      QList<ControlList *> *m_controls;
      QList<ShapeList *> *m_shapes;
      TargetBodyList *m_targets;
      GuiCameraList *m_guiCameras;

      QPointer<Control> m_activeControl;
      QPointer<ImageList> m_activeImageList;

      QList<BundleSolutionInfo *> *m_bundleSolutionInfo;

      // TODO: kle testing - this will almost certainly be changed
      BundleSettings *m_bundleSettings;
      /**
       * This variable will probably go away when we add the bundle results object because it will
       *  be under:
       *           BundleSolutionInfo
       *                 BundleResults
       *                       CorrelationMatrix
       */
//       CorrelationMatrix *m_correlationMatrix;

      QMap<QString, Control *> *m_idToControlMap;
      QMap<QString, Image *> *m_idToImageMap;
      QMap<QString, Shape *> *m_idToShapeMap;
      QMap<QString, BundleSolutionInfo *> *m_idToBundleSolutionInfoMap;
      QMap<QString, TargetBody *> *m_idToTargetBodyMap;
      QMap<QString, GuiCamera *> *m_idToGuiCameraMap;

      QString m_name;
      QStringList *m_warnings;
      QList< QPointer<WorkOrder> > *m_workOrderHistory;

      QPointer<ImageReader> m_imageReader;
      //QList<QPair<QString, Data> > m_storedWarnings;
      bool m_isTemporaryProject;

      int m_numImagesCurrentlyReading;

      QMutex *m_mutex;
      QMutex *m_imageReadingMutex;

      int m_numShapesCurrentlyReading;
      QMutex *m_shapeMutex;
      QPointer<ShapeReader> m_shapeReader;
      QMutex *m_shapeReadingMutex;

      QUndoStack m_undoStack;

  };
}

Q_DECLARE_METATYPE(Isis::Project *);

#endif // Project_H

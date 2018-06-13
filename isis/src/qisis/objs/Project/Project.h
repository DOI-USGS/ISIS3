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
#include "TemplateList.h"
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
  class Template;
  class TemplateList;
  class WorkOrder;

  /**
   *
   * @brief The main project for ipce
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
   *   @history 2016-11-22 Tracie Sucharski - When saving a new project, if it is currently a
   *                           temporary project, save project name as the base pathname for the
   *                           project.
   *   @history 2016-12-02 Tracie Sucharski - Changed the the tag name in ::endElement from
   *                           "project" to "imageLists" and "shapeLists", so that images and shapes
   *                           are added to the project when their end tags are found instead of the
   *                           project end tag.
   *   @history 2016-12-29 Tracie Sucharski - Changed setActiveControl and setActiveImageList to
   *                           take a displayName instead of a Control/ImageList so that restoration
   *                           of Project which contains an active control/imageList can be used.
   *                           The only piece of info that can be saved to a project is the
   *                           displayName.
   *   @history 2017-02-06 Tracie Sucharski - When adding a work order to the project, check the
   *                           work order to determine if it should be put on the QUndoStack.
   *                           Fixes #4598.
   *   @history 2017-03-30 Tracie Sucharski - Cleaned up some documentation regarding last change.
   *   @history 2017-04-04 Makayla Shepherd - Updated addToProject to support the new WorkOrder
   *                           design. Fixes #4729.
   *   @history 2017-04-06 Tracie Sucharski - Added call to child WorkOrder::execute() even if it
   *                           a CleanState.
   *   @history 2017-04-16 Ian Humphrey - Added activeControlSet and activeImageListSet,
   *                           activeControlAndImageListSet signals. Added
   *                           checkActiveControlAndImageList slot. This facilitates enabling
   *                           the JigsawWorkOrder on the main window menu. Fixes #4749. Also,
   *                           modified addToProject so that not undoable work orders have their
   *                           redo called instead of execute.
   *   @history 2017-04-25 Ian Humphrey - Added checkControlsAndImagesAvailable() slot and
   *                           controlsAndImagesAvailble() signal. These are used by internally
   *                           by Project constructor to listen for when a control and image are
   *                           added, used externally by directory to enable the jigsaw work order
   *                           when a cnet and image are available in the project. Fixes #4819.
   *   @history 2017-05-02 Tracie Sucharski - Added saving and resoring of BundleSolutionInfo.
   *                           Fixes #4822.
   *   @history 2017-05-15 Tracie Sucharski - Moved creation of BundleSolutionInfo results folder to
   *                           JigsawDialog::acceptBundleResults.  It was in
   *                           Project::addBundleSolutionInfo which is called for both adding to the
   *                           project and reading a saved project (which already has the folder).
   *                           Backed out changes make for deciding whether to copy cube dn data
   *                           because this broke importing images.
   *   @history 2017-05-17 Tracie Sucharski - Changed activeControl and activeImageList methods to
   *                           return default values if project contains a single control and a
   *                           single image list.  Fixes #4867.
   *   @history 2017-07-13 Makayla Shepherd - Added the ability to change the name of image
   *                           imports, shape imports, and bundle solution info. Fixes #4855,
   *                           #4979, #4980.
   *   @history 2017-07-17 Cole Neubauer - Changed activeControl signal to emit a bool to be able
   *                           to slot a setEnabled(bool) call to a QAction. This was necessary to
   *                           reenable the CNet Tool when a control net is made active.
   *                           Fixes #5046.
   *   @history 2017-07-24 Cole Neubauer - Added isOpen, isClean, setClean, and clear functions to
   *                           allow for opening of a new project. Fixes #4969.
   *   @history 2017-07-27 Cole Neubauer - Added check before emmiting workOrderStarting()
   *                           Fixes #4715.
   *   @history 2017-07-27 Cole Neubauer - Added a workordermutex to be used in workorder accessors
   *                           Fixes #5082.
   *   @history 2017-08-02 Cole Neubauer - Made setClean emit a signal from undoStack. Fixes #4960
   *   @history 2017-08-03 Cole Neubauer - Parsed XML to remove leftover files not in project
   *                           Fixes #5046.
   *   @history 2017-08-08 Makayla Shepherd - Fixed a seg fault that occurs when trying to edit a
   *                           control net without having an active control net set. Fixes #5048.
   *   @history 2017-08-07 Cole Neubauer - Added functionality to switch between active controls and
   *                           ImageList Fixes #4567
   *   @history 2017-08-11 Cole Neubauer - Removed unnecessary code in controlClosed that was
   *                           a segfault causing. Fixes #5064
   *   @history 2017-08-11 Cole Neubauer - Updated documentation for setClean and isClean #5113
   *   @history 2017-08-11 Christopher Combs - Added addTemplates(), removeTemplate(),
   *                           addTemplateFolder(), templateRoot(), and m_templates as well as
   *                           serialization and structure for importing template filenames
   *                           Fixes #5086.
   *   @history 2017-09-13 Tracie Sucharski - Fixed problems with cleanup on temporary projects.
   *                           Remove shapes, controls, and results.
   *   @history 2017-09-26 Tracie Sucharski - Close Image cube in
   *                           ::addTargetsFromImportedImagesToProject and
   *                           ::addCamerasFromImportedImagesToProject.  Fixes #4955.
   *   @history 2017-10-04 Tracie Sucharski - Comment out connections for
   *                           addTargetsFromImportedImagesToProject and
   *                           addCamerasFromImportedImagesToProject.  This functionality needs to
   *                           be put into the asynchronous process of importing images for speed
   *                           and memory efficiency.  See ticket #5181.  Fixes #4955.
   *   @history 2017-10-16 Ian Humphrey - Modified activeControl() to check if any images have been
   *                           imported into the project before trying to set an active control
   *                           when there is only one control in the project. Fixes #5160.
   *   @history 2017-11-01 Tracie Sucharski - Added new member variable for the
   *                           new project root when a Save As is being executed.  Both the old and
   *                           new project roots are needed for copying files into the new project
   *                           folders. Also updated the project name based on the new project.
   *                           Fixes #4849.
   *   @history 2017-11-02 Tyler Wilson - Added support for opening Recent Projects from the
   *                           File Menu.  Fixes #4492.
   *   @history 2017-11-08 Ian Humphrey - Changed save() from a void to a bool return value. This
   *                           indicates if the save dialog (for a temp project) is successfully
   *                           saved (i.e. not cancelled). Fixes #5205.
   *   @history 2017-11-03 Christopher Combs - Added support for new Template and TemplateList
   *                           classes. Fixes #5117.
   *   @history 2017-11-13 Makayla Shepherd - Modifying the name of an ImageList, ShapeList or
   *                           BundeSolutionInfo on the ProjectTree now sets the project to
   *                           not clean. Fixes #5174.
   *   @history 2017-11-15 Cole Neubauer - Added a check if there was an arg for the command line
   *                           to avoid creation of new temp project if a user is opening one from
   *                           the command line #5222
   *   @history 2017-12-01 Adam Goins - Added the maxRecentProjects() function to return the max
   *                           number of recent projects to be displayed. Fixes #5216.
   *   @history 2017-12-05 Christopher Combs - Added support for TemplateEditorWidget and
   *                           TemplateEditViewWorkOrder. Fixes #5168. Also fixed issue with saving
   *                           a project before save as where isOpen was not set to true.
   *   @history 2017-12-08 Tracie Sucharski - Added public method to add an Image to the
   *                           idToImageMap.  This was needed to add Images from the results item.
   *                           We need to access the map when opening saved projects that contain
   *                           images from groups other than the main project data area.  This is
   *                           a temporary fix until the project and model/view is improved.
   *                           Corrected the setting of the project root when pening a project from
   *                           the command line. Removed m_projectPath, it is no longer needed since
   *                           m_projectRoot contains the correct path. References #5104.
   *   @history 2018-03-14 Ken Edmundson - Modified save method to reopen project if we are saving
   *                           a temporary project to ensure all project files are pointing to the
   *                           correct directory. Note that this is NOT ideal, particularly it the
   *                           project has many files.
   *   @history 2018-03-14 Tracie Sucharski - Call the appropriate workorder from the methods
   *                           activeControl and activeImageList when returning a default value.
   *                           This ensures that all the proper error checking is handled and
   *                           prevents duplicate code.
   *   @history 2018-03-23 Ken Edmundson - Modified loadBundleSolutionInfo method to add the
   *                           BundleSolutionInfo's output control id to the project member variable
   *                           m_idToControlMap.
   *   @history 2018-03-26 Tracie Sucharski - When setting a new active control do not close the old
   *                           active control net if it is still being viewed in a CnetEditorWidget.
   *                           References #5026.
   *   @history 2018-03-27 Tracie Sucharski - Removed the calls to work orders from activeImageList
   *                           and activeControl methods.  Additional errors checks needed for
   *                           default values that are not in work orders.  Fixes #5256.
   *   @history 2018-03-30 Tracie Sucharski - Added public slot, activeControlModified, which sets
   *                           the modified state on the active Control. This was done, so that a
   *                           Control knows if its control net has been modified. Also added
   *                           signal, discardActiveControlEdits if user does not want to save
   *                           edits.  This is needed for CnetEditorWidgets that are displaying
   *                           the modified active control, it will effectively close that
   *                           CnetEditorView and reload with the original control net.  It was
   *                           done this way because there is no easy way to reload a control net in
   *                           the CnetEditor widgets. When saving Project, if there is an active
   *                           control and it has been modified, write active control to disk.
   *                           Unfortunately this is done in 2 different places depending on whether
   *                           a project "Save" or "Save As" is being done.  If "Save As", a
   *                           modified active cnet is not written out to the original project only
   *                           to the new project, so this had to be done in
   *                           Control::copyToNewProjectRoot.  If simply saving current projct,
   *                           the write is done here in the save method.
   *  @history 2018-04-25 Tracie Sucharski - Fixed typo in XmlHandler::startElement reading
   *                           imported shapes from a project which caused the shapes to be put in
   *                           the wrong place on the project tree. Fixes #5274.
   *  
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
      void addImagesToIdMap(ImageList images);
      QDir addShapeFolder(QString prefix);
      void addShapes(QStringList shapeFiles);
      void addShapes(ShapeList newShapes);
      void addTemplates(TemplateList *templateFiles);
      QDir addTemplateFolder(QString prefix);
      void addBundleSolutionInfo(BundleSolutionInfo *bundleSolutionInfo);
      void addTarget(Target *target);
      void addCamera(Camera *camera);

      void loadBundleSolutionInfo(BundleSolutionInfo *bundleSolutionInfo);
      void clear();
      bool clearing(); //! Accessor for if the project is clearing or not
      Control *control(QString id);
      Directory *directory() const;
      Image *image(QString id);
      ImageList *imageList(QString name);
      Shape *shape(QString id);
      ShapeList *shapeList(QString name);
      // CorrelationMatrix *correlationMatrix();
      bool isTemporaryProject() const;
      bool isOpen();
      bool isClean();
      WorkOrder *lastNotUndoneWorkOrder();
      const WorkOrder *lastNotUndoneWorkOrder() const;
      QString name() const;
      QMutex *workOrderMutex();
      QMutex *mutex();
      QString projectRoot() const;
      QString newProjectRoot() const;

      void setName(QString newName);
      QUndoStack *undoStack();
      void waitForImageReaderFinished();
      void waitForShapeReaderFinished();
      QList<WorkOrder *> workOrderHistory();
      void writeSettings(FileName projName) const;

      void setActiveControl(QString displayName);
      Control  *activeControl();
      void setActiveImageList(QString displayName);
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

      static QString templateRoot(QString projectRoot);
      QString templateRoot() const;
      QList<TemplateList *> templates();
      void removeTemplate(FileName file);

      void deleteAllProjectFiles();
      void relocateProjectRoot(QString newRoot);

      /**
       * Return BundleSettings objects in Project
       * @return BundleSettings
       */
      BundleSettings *bundleSettings() {return m_bundleSettings;}

      /**
       * Return max number of recent projects to be displayed.
       * @return Max number of recent Projects
       */
      static int maxRecentProjects() { return m_maxRecentProjects; }

      QProgressBar *progress();

      void removeImages(ImageList &imageList);

      bool save();
      void save(FileName projectPath, bool verifyPathDoesntExist = true);

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
       * Emitted when an active control is set.
       * receivers: Project::checkActiveControlAndImageList
       */
      void activeControlSet(bool boolean);

      /**
       * Emitted when all controls have been removed from the Project.
       * receivers: WorkOrder::disableWorkOrder
       *
       * Currently does not work (there is no work order to remove cnets).
       *
       * @see Project::controlListDeleted(QObject *controlListObj)
       */
      void allControlsRemoved();

      /**
       * Emitted when new ImageList added to Project
       * receivers: ProjectTreeWidget
       */
      void imageListAdded(ImageList *images);

      /**
       * Emitted when new images are available.
       * receivers: Directory, Project, WorkOrder
       */
      void imagesAdded(ImageList *images);

      /**
       * Emitted when an active image list is set.
       * receivers: Project::checkActiveControlAndImageList
       */
      void activeImageListSet();

      /**
       * Emitted when both an active control and active image list have been set.
       * receivers: WorkOrder::enableWorkOrder
       */
      void activeControlAndImageListSet();

      /**
       * Emitted when at least one cnet and image have been added to the project.
       * This is used to enable the JigsawWorkOrder in the main menu.
       * receivers: WorkOrder::enableWorkOrder
       */
      void controlsAndImagesAvailable();

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
       * receivers: IpceMainWindow, Directory, HistoryTreeWidget
       */
      void projectLoaded(Project *);

      /**
       * Emitted when project is saved.
       *
       */
      void projectSave(FileName projectName);

      /**
       * Emitted when project location moved
       * receivers: Control, BundleSolutionInfo, Image, TargetBody
       */
      void projectRelocated(Project *);
      /**
       * Emitted when work order starts
       */
      void workOrderStarting(WorkOrder *);
      /**
       * Emitted when work order ends
       */
      void workOrderFinished(WorkOrder *);

      void templatesAdded(TemplateList *newTemplates);

      void discardActiveControlEdits();

    public slots:
      void open(QString);
      void setClean(bool value);
      void activeControlModified();

    private slots:
      void controlClosed(QObject *control);
      void controlListDeleted(QObject *controlList);
      void imagesReady(ImageList);
      void imageClosed(QObject *image);
      void imageListDeleted(QObject *imageList);
      void bundleSolutionInfoClosed(QObject *bundleSolutionInfo);
      void targetBodyClosed(QObject *targetBodyObj);
      void shapesReady(ShapeList shapes);
      void shapeClosed(QObject *shape);
      void shapeListDeleted(QObject *shapeList);
      void checkActiveControlAndImageList();
      void checkControlsAndImagesAvailable();

    private:
      Project(const Project &other);
      Project &operator=(const Project &rhs);
      void createFolders();

      ControlList *createOrRetrieveControlList(QString name, QString path = "");
      ImageList *createOrRetrieveImageList(QString name, QString path = "");
      ShapeList *createOrRetrieveShapeList(QString name, QString path = "");

      void writeSettings();


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
          QList<BundleSolutionInfo *> m_bundleSolutionInfos;
          QList<TemplateList *> m_templates;
          WorkOrder *m_workOrder;
      };

    private:

      static const int m_maxRecentProjects = 5;
      QDir *m_projectRoot;
      QString m_newProjectRoot;
      QDir *m_cnetRoot;
      QDir m_currentCnetFolder;
      QPointer<Directory> m_directory;
      QList<ImageList *> *m_images;
      QList<ControlList *> *m_controls;
      QList<ShapeList *> *m_shapes;
      TargetBodyList *m_targets;
      QList<TemplateList *> *m_templates;
      GuiCameraList *m_guiCameras;
      QList<BundleSolutionInfo *> *m_bundleSolutionInfo;

      QPointer<Control> m_activeControl;
      QPointer<ImageList> m_activeImageList;


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
      bool m_isOpen; //! used to determine whether a project is currently open
      bool m_isClean; //! used to determine whether a project's changes are unsaved
      bool m_clearing; //! used to negate segfaults happening in post undos when clearning project
      int m_numImagesCurrentlyReading;

      QMutex *m_mutex;
      QMutex *m_workOrderMutex;
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

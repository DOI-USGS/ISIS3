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
#include "ImageList.h"
#include "XmlStackedHandler.h"

namespace Isis {
  class Control;
  class ControlList;
  class FileName;
  class Image;
  class ImageReader;
  class ProgressBar;
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
   */
  class Project : public QObject {
    Q_OBJECT
    public:
      Project(Directory &directory, QObject *parent = 0);
      ~Project();

      static QStringList images(QStringList);
//      static QStringList verifyCNets(QStringList);

      QList<QAction *> userPreferenceActions();
      QDir addCnetFolder(QString prefix);
      void addControl(Control *control);
      QDir addImageFolder(QString prefix);
      void addImages(QStringList imageFiles);
      void addImages(ImageList newImages);
      Control *control(QString id);
      Directory *directory() const;
      Image *image(QString id);
      ImageList *imageList(QString name);
      bool isTemporaryProject() const;
      static QString targetBodyRoot(QString projectRoot);
      QString targetBodyRoot() const;
      WorkOrder *lastNotUndoneWorkOrder();
      const WorkOrder *lastNotUndoneWorkOrder() const;
      QString name() const;
      QMutex *mutex();
      QString projectRoot() const;
      void setName(QString newName);
      QUndoStack *undoStack();
      void waitForImageReaderFinished();
      QList<WorkOrder *> workOrderHistory();

      static QString cnetRoot(QString projectRoot);
      QString cnetRoot() const;
      QList<ControlList *> controls();
      ControlList *controlList(QString name);

      static QString imageDataRoot(QString projectRoot);
      QString imageDataRoot() const;
      QList<ImageList *> images();

      void deleteAllProjectFiles();
      void relocateProjectRoot(QString newRoot);

      QProgressBar *progress();

//       void removeImages(ImageList &imageList);

      void save();
      void save(FileName newPath, bool verifyPathDoesntExist = true);

      void addToProject(WorkOrder *);

      template<typename Data>
      void warn(QString text, Data relevantData) {
        storeWarning(text, relevantData);
        directory()->showWarning(text, relevantData);
      }

      void warn(QString text);

    signals:
      void allImagesClosed();
      void controlListAdded(ControlList *controls);
      void controlAdded(Control *control);
      // Emitted when new images are available.
      void imagesAdded(ImageList *images);
      void nameChanged(QString newName);
      void projectLoaded(Project *);
      void projectRelocated(Project *);
      void workOrderStarting(WorkOrder *);
      void workOrderFinished(WorkOrder *);

    public slots:
      void open(QString);

    private slots:
      void controlClosed(QObject *control);
      void controlListDeleted(QObject *controlList);
      void imagesReady(ImageList);
      void imageClosed(QObject *image);
      void imageListDeleted(QObject *imageList);

    private:
      Project(const Project &other);
      Project &operator=(const Project &rhs);
      void createFolders();
      ControlList *createOrRetrieveControlList(QString name);
      ImageList *createOrRetrieveImageList(QString name);


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
      QMap<QString, Control *> *m_idToControlMap;
      QMap<QString, Image *> *m_idToImageMap;
      QString m_name;
      QStringList *m_warnings;
      QList< QPointer<WorkOrder> > *m_workOrderHistory;

      QPointer<ImageReader> m_imageReader;
      //QList<QPair<QString, Data> > m_storedWarnings;
      bool m_isTemporaryProject;

      int m_numImagesCurrentlyReading;

      QMutex *m_mutex;
      QMutex *m_imageReadingMutex;

      QUndoStack m_undoStack;

  };
}

#endif // Project_H

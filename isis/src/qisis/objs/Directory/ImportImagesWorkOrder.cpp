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
#include "ImportImagesWorkOrder.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "Project.h"
#include "SaveProjectWorkOrder.h"
#include "TextFile.h"

namespace Isis {

  ImportImagesWorkOrder::ImportImagesWorkOrder(Project *project) :
      WorkOrder(project) {
    m_newImages = NULL;

    QAction::setText(tr("Import &Images..."));
    QUndoCommand::setText(tr("Import Images"));
    setModifiesDiskState(true);
  }


  ImportImagesWorkOrder::ImportImagesWorkOrder(const ImportImagesWorkOrder &other) :
      WorkOrder(other) {
    m_newImages = NULL;
  }


  ImportImagesWorkOrder::~ImportImagesWorkOrder() {
    delete m_newImages;
    m_newImages = NULL;
  }


  ImportImagesWorkOrder *ImportImagesWorkOrder::clone() const {
    return new ImportImagesWorkOrder(*this);
  }


  bool ImportImagesWorkOrder::execute() {
    WorkOrder::execute();

    QStringList fileNames = QFileDialog::getOpenFileNames(
        qobject_cast<QWidget *>(parent()),
        tr("Import Images"), "",
        tr("Isis cubes and list files (*.cub *.lis);;All Files (*)"));

    QStringList stateToSave;

    if (!fileNames.isEmpty()) {
      foreach (FileName fileName, fileNames) {
        if (fileName.extension() == "lis") {
          TextFile listFile(fileName.expanded());
          QString lineOfListFile;

          while (listFile.GetLine(lineOfListFile)) {
            stateToSave.append(lineOfListFile);
          }
        }
        else {
          stateToSave.append(fileName.original());
        }
      }
    }

    QMessageBox::StandardButton saveProjectAnswer = QMessageBox::No;
    if (stateToSave.count() >= 100 && project()->isTemporaryProject()) {
      saveProjectAnswer = QMessageBox::question(qobject_cast<QWidget *>(parent()),
               tr("Save Project Before Importing Images"),
               tr("Would you like to save your project <b>before</b> importing images? It can be "
                  "slow to save your project after these images have been loaded if you do not "
                  "save now."),
               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
               QMessageBox::Yes);
    }

    if (saveProjectAnswer == QMessageBox::Yes) {
      SaveProjectWorkOrder saveWorkOrder(project());
      saveWorkOrder.trigger();
    }

    QMessageBox::StandardButton copyImagesAnswer = QMessageBox::No;
    if (!fileNames.isEmpty() && saveProjectAnswer != QMessageBox::Cancel) {
      copyImagesAnswer = QMessageBox::question(qobject_cast<QWidget *>(parent()),
               tr("Copy Images into Project"),
               tr("Should images (DN data) be copied into project?"),
               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
               QMessageBox::Yes);
    }

    bool copyDnData = (copyImagesAnswer == QMessageBox::Yes);

    stateToSave.prepend(copyDnData? "copy" : "nocopy");

    if (fileNames.count() > 1) {
      QUndoCommand::setText(tr("Import %1 Images").arg(stateToSave.count() - 1));
    }
    else if (fileNames.count() == 1) {
      QUndoCommand::setText(tr("Import %1").arg(fileNames.first()));
    }

    setInternalData(stateToSave);

    bool doImport = fileNames.count() > 0 && saveProjectAnswer != QMessageBox::Cancel &&
                    copyImagesAnswer != QMessageBox::Cancel;

    return doImport;
  }


  void ImportImagesWorkOrder::asyncUndo() {
    project()->waitForImageReaderFinished();
    project()->images().last()->deleteFromDisk(project());
  }


  void ImportImagesWorkOrder::postSyncUndo() {
    QPointer<ImageList> imagesWeAdded = project()->images().last();

    foreach (Image *image, *imagesWeAdded) {
      delete image;
    }
    delete imagesWeAdded;
  }


  void ImportImagesWorkOrder::asyncRedo() {
    if (internalData().count() > 0) {
      importConfirmedImages(internalData().mid(1), (internalData()[0] == "copy"));
    }
  }


  void ImportImagesWorkOrder::postSyncRedo() {
    if (!m_newImages->isEmpty()) {
      project()->addImages(*m_newImages);

      delete m_newImages;
      m_newImages = NULL;
    }

    if (m_warning != "") {
      project()->warn(m_warning);
    }
  }


  ImportImagesWorkOrder::OriginalFileToProjectCubeFunctor::OriginalFileToProjectCubeFunctor(
      QThread *guiThread, QDir destinationFolder, bool copyDnData) : m_errors(new IException),
      m_numErrors(new int(0)) {
    m_destinationFolder = destinationFolder;
    m_copyDnData = copyDnData;
    m_guiThread = guiThread;
  }


  ImportImagesWorkOrder::OriginalFileToProjectCubeFunctor::OriginalFileToProjectCubeFunctor(
      const OriginalFileToProjectCubeFunctor &other) : m_errors(other.m_errors),
      m_numErrors(other.m_numErrors) {
    m_destinationFolder = other.m_destinationFolder;
    m_copyDnData = other.m_copyDnData;
    m_guiThread = other.m_guiThread;
  }


  ImportImagesWorkOrder::OriginalFileToProjectCubeFunctor::~OriginalFileToProjectCubeFunctor() {
    m_destinationFolder = QDir();
    m_copyDnData = false;
    m_guiThread = NULL;
  }


  Cube *ImportImagesWorkOrder::OriginalFileToProjectCubeFunctor::operator()(
      const FileName &original) {
    Cube *result = NULL;

    if (*m_numErrors < 20) {
      try {
        QString destination = QFileInfo(m_destinationFolder, original.name())
                                .absoluteFilePath();
        Cube *input = new Cube(original, "r");

        if (m_copyDnData) {
          Cube *copiedCube = input->copy(destination, CubeAttributeOutput());
          delete input;
          input = copiedCube;
        }

        FileName externalLabelFile(destination);
        externalLabelFile = externalLabelFile.setExtension("ecub");

        Cube *projectImage = input->copy(externalLabelFile, CubeAttributeOutput("+External"));

        if (m_copyDnData) {
          // Make sure the external label has a fully relative path to the DN data
          projectImage->relocateDnData(FileName(destination).name());
        }

        delete input;

        result = projectImage;
      }
      catch (IException &e) {
        m_errorsLock.lock();

        m_errors->append(e);
        (*m_numErrors)++;

        m_errorsLock.unlock();
      }
    }

    return result;
  }


  IException ImportImagesWorkOrder::OriginalFileToProjectCubeFunctor::errors() const {
    IException result;

    result.append(*m_errors);

    if (*m_numErrors >= 20) {
      result.append(
          IException(IException::Unknown,
                     tr("Aborted import images due to a high number of errors"),
                     _FILEINFO_));
    }
    return result;
  }


  /**
   * Creates a project image folder and copies the cubes into it. This will create the *.ecub and
   *   *.cub files inside of the project.
   * This can be thought of as:
   * <pre>
   *   mkdir project/images/import1
   *   cp in1.cub in2.cub project/images/import1
   * </pre>
   *
   * This should be called in a non-GUI thread
   *
   * @param confirmedImages This is a list of cube file names outside of the project folder
   * @param copyDnData If this is true, this will create both the *.cub and *.ecub files in the
   *                   project. Otherwise, only the external label files (*.ecub) will be created
   *                   inside of the project.
   */
  void ImportImagesWorkOrder::importConfirmedImages(QStringList confirmedImages, bool copyDnData) {
    if (!confirmedImages.isEmpty()) {
      QDir folder = project()->addImageFolder("import");

      setProgressRange(0, confirmedImages.count());

      m_newImages = new ImageList;
      m_newImages->reserve(confirmedImages.count());

      QStringList confirmedImagesFileNames;
      QStringList confirmedImagesIds;

      foreach (QString confirmedImage, confirmedImages) {
        QStringList fileNameAndId = confirmedImage.split(",");

        confirmedImagesFileNames.append(fileNameAndId.first());

        if (fileNameAndId.count() == 2) {
          confirmedImagesIds.append(fileNameAndId.last());
        }
        else {
          confirmedImagesIds.append(QString());
        }
      }

      OriginalFileToProjectCubeFunctor functor(thread(), folder, copyDnData);
      QFuture<Cube *> future = QtConcurrent::mapped(confirmedImagesFileNames, functor);

      QStringList newInternalData;
      newInternalData.append(internalData().first());

      QThreadPool::globalInstance()->releaseThread();
      for (int i = 0; i < confirmedImages.count(); i++) {
        setProgressValue(i);

        Cube *cube = future.resultAt(i);

        if (cube) {
          Image *newImage = new Image(future.resultAt(i));

          if (confirmedImagesIds[i].isEmpty()) {
            confirmedImagesIds[i] = newImage->id();
          }
          else {
            newImage->setId(confirmedImagesIds[i]);
          }

          QStringList imageInternalData;
          imageInternalData.append(confirmedImagesFileNames[i]);
          imageInternalData.append(confirmedImagesIds[i]);

          newInternalData.append(imageInternalData.join(","));

          m_newImages->append(newImage);

          newImage->moveToThread(thread());
          newImage->displayProperties()->moveToThread(thread());

          newImage->closeCube();
        }
      }
      QThreadPool::globalInstance()->reserveThread();

      m_warning = functor.errors().toString();

      m_newImages->moveToThread(thread());

      setInternalData(newInternalData);
    }
  }
}

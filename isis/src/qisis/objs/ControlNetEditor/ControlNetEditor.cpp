#include "ControlNetEditor.h"

#include <sstream>
#include <vector>
#include <iomanip>

#include <QtGui>

#include "NewControlPointDialog.h"
#include "DeleteControlPointDialog.h"
#include "Workspace.h"

#include "Application.h"
#include "Brick.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlPointEdit.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlEditDialog.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "ToolPad.h"
#include "UniversalGroundMap.h"
#include "ViewportMainWindow.h"

using namespace std;

namespace Isis {
  const int VIEWSIZE = 301;
  const int CHIPVIEWPORT_WIDTH = 310;


  /**
   * Consructs the Qnet Tool window
   *
   * @param parent Pointer to the parent widget for the Qnet tool
   *
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null.
   *
   */
  ControlNetEditor::ControlNetEditor (SerialNumberList *serialNumberList,
                                      ControlNet *controlNet,
                                      QWidget *parent) : QObject(parent) {
    m_parent = parent;
    m_controlNet = controlNet;
    m_serialNumberList = serialNumberList;
    m_controlNet->SetImages(*m_serialNumberList);
    //qDebug()<<"ControlNetEditor::ControlNetEditor  serial#list .size = "<<serialNumberList->Size();
  }


  ControlNetEditor::~ControlNetEditor () {
  }


  /*
  * Change which measure is the reference.
  *  
  * @author 2012-04-26 Tracie Sucharski - moved funcitonality from measureSaved
  *
  * @internal
  *   @history 2012-06-12 Tracie Sucharski - Moved check for ground loaded on left from the
  *                          measureSaved method.
  */
//void ControlNetEditor::checkReference() {
//
//  // Check if ControlPoint has reference measure, if reference Measure is
//  // not the same measure that is on the left chip viewport, set left
//  // measure as reference.
//  ControlMeasure *refMeasure = m_editPoint->GetRefMeasure();
//  if ( (m_leftMeasure->GetCubeSerialNumber() != m_groundSN) &&
//       (refMeasure->GetCubeSerialNumber() != m_leftMeasure->GetCubeSerialNumber()) ) {
//    QString message = "This point already contains a reference measure.  ";
//    message += "Would you like to replace it with the measure on the left?";
//    int  response = QMessageBox::question(m_parent,
//                              "Qnet Tool Save Measure", message,
//                              QMessageBox::Yes | QMessageBox::No,
//                              QMessageBox::Yes);
//    // Replace reference measure
//    if (response == QMessageBox::Yes) {
//      //  Update measure file combo boxes:  old reference normal font,
//      //    new reference bold font
//      QString file = m_serialNumberList->FileName(m_leftMeasure->GetCubeSerialNumber());
//      QString fname = FileName(file).name();
//      int iref = m_leftCombo->findText(fname);
//
//      //  Save normal font from new reference measure
//      QVariant font = m_leftCombo->itemData(iref,Qt::FontRole);
//      m_leftCombo->setItemData(iref,QFont("DejaVu Sans", 12, QFont::Bold), Qt::FontRole);
//      iref = m_rightCombo->findText(fname);
//      m_rightCombo->setItemData(iref,QFont("DejaVu Sans", 12, QFont::Bold), Qt::FontRole);
//
//      file = m_serialNumberList->FileName(refMeasure->GetCubeSerialNumber());
//      fname = FileName(file).name();
//      iref = m_leftCombo->findText(fname);
//      m_leftCombo->setItemData(iref,font,Qt::FontRole);
//      iref = m_rightCombo->findText(fname);
//      m_rightCombo->setItemData(iref,font,Qt::FontRole);
//
//      m_editPoint->SetRefMeasure(m_leftMeasure->GetCubeSerialNumber());
//      // Update reference measure to new reference measure
//      refMeasure = m_editPoint->GetRefMeasure();
//    }
//    // ??? Need to set rest of measures to Candidate and add more warning. ???//
//  }
//
//  // If the right measure is the reference, make sure they really want
//  // to move the reference.
//  if (refMeasure->GetCubeSerialNumber() == m_rightMeasure->GetCubeSerialNumber()) {
//    QString message = "You are making a change to the reference measure.  You ";
//    message += "may need to move all of the other measures to match the new ";
//    message += " coordinate of the reference measure.  Do you really want to ";
//    message += " change the reference measure? ";
//    switch(QMessageBox::question(m_parent, "Qnet Tool Save Measure",
//                                 message, "&Yes", "&No", 0, 0)){
//      // Yes:  Save measure
//      case 0:
//        break;
//      // No:  keep original reference, return without saving
//      case 1:
//        loadPoint();
//        return;
//    }
//  }
//
//}



  /**
   * Save edit point to the Control Network.  Up to this point the point is
   * simply a copy and does not exist in the network.
   *
   * @author 2010-11-19 Tracie Sucharski
   *
   * @internal
   * @history 2011-04-20 Tracie Sucharski - If EditLock set, prompt for changing
   *                        and do not save point if editLock not changed.
   * @history 2011-07-05 Tracie Sucharski - Move point EditLock error checking
   *                        to individual point parameter setting methods, ie.
   *                        SetPointType, SetIgnorePoint.
   *
   */
//void ControlNetEditor::savePoint () {
//
//  //  Make a copy of edit point for updating the control net since the edit
//  //  point is still loaded in the point editor.
//  ControlPoint *updatePoint = new ControlPoint;
//  *updatePoint = *m_editPoint;
//
//  //  If this is a fixed or constrained point, see if there is a temporary
//  //  measure holding the coordinate information from the ground source.
//  //  If so, delete this measure before saving point.  Clear out the
//  //  fixed Measure variable (memory deleted in ControlPoint::Delete).
//  if (updatePoint->GetType() != ControlPoint::Free &&
//      updatePoint->HasSerialNumber(m_groundSN)) {
//    updatePoint->Delete(m_groundSN);
//  }
//
//  //  If edit point exists in the network, save the updated point.  If it
//  //  does not exist, add it.
//  if (m_controlNet->ContainsPoint(updatePoint->GetId())) {
//    ControlPoint *p;
//    p = m_controlNet->GetPoint(QString(updatePoint->GetId()));
//    *p = *updatePoint;
//    delete updatePoint;
//    updatePoint = NULL;
//  }
//  else {
//    m_controlNet->AddPoint(updatePoint);
//  }
//
//  //  Change Save Measure button text back to default
//  m_savePoint->setPalette(m_saveDefaultPalette);
//
//  //  ????  Why was this here??? loadPoint();
//  // emit signal so the nav tool refreshes the list
//  emit refreshNavList();
//  // emit signal so the nav tool can update edit point
//  emit editPointChanged(m_editPoint->GetId());
//  // emit a signal to alert user to save when exiting
//  emit netChanged();
//  //   Refresh chipViewports to show new positions of controlPoints
//  m_pointEditor->refreshChips();
//}


  void ControlNetEditor::createPoint(QString serialNumber, double sample, double line) {
    int index = m_serialNumberList->SerialNumberIndex(serialNumber);
    Camera *cam = m_controlNet->Camera(index);
    cam->SetImage(sample, line);
    createPoint( serialNumber, 
                 Latitude( cam->UniversalLatitude(), Angle::Degrees ),
                 Longitude( cam->UniversalLongitude(), Angle::Degrees ) );
  }



  /**
   *   Create new control point
   *
   * @param lat Latitude value of control point to be created.
   * @param lon Longitude value of control point to be created.
   *
   * @internal
   *   @history 2008-11-20 Jeannie Walldren - Added message box if pointID value
   *                          entered already exists for another ControlPoint.
   *                          Previously this resulted in a PROGRAMMER ERROR from
   *                          ControlPoint. Now, the user will be alerted in a
   *                          message box and prompted to enter a new value for
   *                          pointID.
   *   @history 2008-12-03  Tracie Sucharski - Add error message and cancel
   *                          create new point if the point falls on a single
   *                          image.
   *   @history 2008-12-15  Jeannie Walldren - Throw and catch error before
   *                           creating QMessageBox
   *   @history 2009-03-09  Jeannie Walldren - Clear error message stack after it
   *                           is displayed to user in message box.
   *   @history 2009-04-20  Tracie Sucharski - Set camera for each measure.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-07-21  Tracie Sucharski - Modified for new keywords
   *                           associated with implementation of binary
   *                           control networks.
   *   @history 2010-11-19  Tracie Sucharski - Changed m_controlPoint to
   *                           m_editPoint which is a copy rather than a pointer
   *                           directly to the network.
   *   @history 2010-12-15 Tracie Sucharski - Remove netChanged, the point is
   *                           not changed in the net unless "Save Point" is
   *                           selected.
   *   @history 2011-03-31 Tracie Sucharski - Remove check for point only
   *                           existing on a single image.  This will be
   *                           shown on new point dialog and user can always
   *                           hit "Cancel".
   *   @history 2011-04-08 Tracie Sucharski - Added check for NULL pointer
   *                           before deleting m_editPOint if parent is NULL.
   *   @history 2011-07-19 Tracie Sucharski - Remove call to
   *                           SetAprioriSurfacePoint, this should only be
   *                           done for constrained or fixed points.
   *   @history 2012-05-08 Tracie Sucharski - m_leftFile changed from std::string to QString. 
   *
   */
  void ControlNetEditor::createPoint(QString serialNumber, Latitude lat, Longitude lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    //  Create list box of all files highlighting those that
    //  contain the point.
    QStringList pointFiles;

    Camera *cam;
    //qDebug()<<"ControlNetEditor::createPoint  serialNumberList.size() =  "<<m_serialNumberList->Size();
    for(int i = 0; i < m_serialNumberList->Size(); i++) {
//    if (m_serialNumberList->SerialNumber(i) == m_groundSN) continue;
      cam = m_controlNet->Camera(i);
      if( cam->SetUniversalGround( lat.degrees(), lon.degrees() ) ) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if (samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles << m_serialNumberList->FileName(i);
        }
      }
    }

    NewControlPointDialog *newPointDialog = new NewControlPointDialog(m_lastUsedPointId);
    
    newPointDialog->setFiles(pointFiles, m_serialNumberList);
    if ( newPointDialog->exec() ) {
    m_lastUsedPointId = newPointDialog->pointId();  

      // If this point ID already exists in the control net, message box pops up and user is
      // asked to enter a new value.
      while (m_controlNet->ContainsPoint(m_lastUsedPointId)) {
        m_lastUsedPointId = newPointDialog->pointId();
        QString message = "A ControlPoint with Point Id = [" + m_lastUsedPointId;
        message += "] already exists.  Re-enter Point Id for this ControlPoint.";
        QMessageBox::warning(newPointDialog, "New Point Id", message);
//      return createNewControlPoint(pointFiles);
      }

      // TODO: Do we want to go back to the old functionality where the new point is automatically
      // written to the control net. Talk to processors about their preferences.
      ControlPoint *newPoint = new ControlPoint(m_lastUsedPointId);
      newPoint->SetChooserName(Application::UserName());

      QStringList selectedFiles = newPointDialog->selectedFiles();
      Camera *camera;
      foreach (QString selectedFile, selectedFiles) {
        //  Create measure for any file selected
        ControlMeasure *m = new ControlMeasure;
        //  Find serial number for this file
        QString sn =
                  m_serialNumberList->SerialNumber(selectedFile);
        m->SetCubeSerialNumber(sn);
        int camIndex =
              m_serialNumberList->FileNameIndex(selectedFile);
        camera = m_controlNet->Camera(camIndex);
        camera->SetUniversalGround(lat.degrees(), lon.degrees());
        m->SetCoordinate(camera->Sample(), camera->Line());
        m->SetAprioriSample(camera->Sample());
        m->SetAprioriLine(camera->Line());
        m->SetType(ControlMeasure::Manual);
        m->SetChooserName(Application::UserName());
        m->SetCamera(camera);
        newPoint->Add(m);
      }

      emit controlPointCreated(newPoint);
    }
  }



  /**
   *   Create new Fixed control point
   *
   * @param lat Latitude value of control point to be created.
   * @param lon Longitude value of control point to be created.
   *
   * @author 2010-11-09 Tracie Sucharski
   *
   * @internal
   *
   */
#if 0
  void ControlNetEditor::createFixedPoint(double lat,double lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    //  Create list of list box of all files highlighting those that
    //  contain the point.
    QStringList pointFiles;

    Camera *cam;
    for (int i=0; i<m_serialNumberList->Size(); i++) {
      if (m_serialNumberList->SerialNumber(i) == m_groundSN) continue;
      cam = m_controlNet->Camera(i);
      if (cam->SetUniversalGround(lat,lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if (samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles<<m_serialNumberList->FileName(i);
        }
      }
    }

    if (pointFiles.count() == 0) {
      QString message = "Point does not intersect any images.";
      QMessageBox::critical(m_parent, "No intersection", message);
      return;
    }

    QnetFixedPointDialog *fixedPointDialog = new QnetFixedPointDialog(this, m_lastUsedPointId);
    fixedPointDialog->setFiles(pointFiles);
    if (fixedPointDialog->exec()) {
      ControlPoint *fixedPoint =
      new ControlPoint(fixedPointDialog->pointId());

      if (fixedPointDialog->isFixed()) {
        fixedPoint->SetType(ControlPoint::Fixed);
      }
      else {
        fixedPoint->SetType(ControlPoint::Constrained);
      }

      // If this ControlPointId already exists, message box pops up and user is
      // asked to enter a new value.
      if (m_controlNet->ContainsPoint(fixedPoint->GetId())) {
        QString message = "A ControlPoint with Point Id = [" + fixedPoint->GetId();
        message += "] already exists.  Re-enter Point Id for this ControlPoint.";
        QMessageBox::warning(m_parent, "New Point Id", message);
        pointFiles.clear();
        delete fixedPoint;
        fixedPoint = NULL;
        createFixedPoint(lat,lon);
        return;
      }

      fixedPoint->SetChooserName(Application::UserName());

      QStringList selectedFiles = fixedPointDialog->selectedFiles();
      foreach (QString selectedFile, selectedFiles) {
        //  Create measure for any file selected
        ControlMeasure *m = new ControlMeasure;
        //  Find serial number for this file
        QString sn =
            m_serialNumberList->SerialNumber(selectedFile);

        //  If ground, do not add measure, it will be added in loadPoint
        if (sn == m_groundSN) continue;

        m->SetCubeSerialNumber(sn);
        int camIndex =
                 m_serialNumberList->FileNameIndex(selectedFile);
        cam = m_controlNet->Camera(camIndex);
        cam->SetUniversalGround(lat,lon);
        m->SetCoordinate(cam->Sample(),cam->Line());
        m->SetType(ControlMeasure::Manual);
        m->SetChooserName(Application::UserName());
        m->SetCamera(cam);
        fixedPoint->Add(m);
      }

      //  ??????       What radius , check for dem or shape model
      double radius = 0;
      if (m_demOpen) {
        radius = demRadius(lat,lon);
        if (radius == Null) {
          QString msg = "Could not read radius from DEM, will default to the "
            "local radius of the first measure in the control point.  This "
            "will be updated to the local radius of the chosen reference "
            "measure.";
          QMessageBox::warning(m_parent, "Warning", msg);
          if ((*fixedPoint)[0]->Camera()->SetGround(
               Latitude(lat, Angle::Degrees), Longitude(lon, Angle::Degrees))) {
            radius = (*fixedPoint)[0]->Camera()->LocalRadius().meters();
          }
          else {
            QString msg = "Error trying to get radius at this pt.  "
                "Lat/Lon does not fall on the reference measure.  "
                "Cannot create this point.";
            QMessageBox::critical(m_parent, "Error", msg);
            delete fixedPoint;
            fixedPoint = NULL;
            delete fixedPointDialog;
            fixedPointDialog = NULL;
            return;
          }
        }
      }
      else {
        if ((*fixedPoint)[0]->Camera()->SetGround(
             Latitude(lat, Angle::Degrees), Longitude(lon, Angle::Degrees))) {
          radius = (*fixedPoint)[0]->Camera()->LocalRadius().meters();
        }
        else {
          QString msg = "Error trying to get radius at this pt.  "
              "Lat/Lon does not fall on the reference measure.  "
              "Cannot create this point.";
          QMessageBox::critical(m_parent, "Error", msg);
          delete fixedPoint;
          fixedPoint = NULL;
          delete fixedPointDialog;
          fixedPointDialog = NULL;
          return;
        }
      }

      fixedPoint->SetAprioriSurfacePoint(SurfacePoint(
                                          Latitude(lat, Angle::Degrees),
                                          Longitude(lon, Angle::Degrees),
                                          Distance(radius, Distance::Meters)));

      if (m_editPoint != NULL && m_editPoint->Parent() == NULL) {
        delete m_editPoint;
        m_editPoint = NULL;
      }
      m_editPoint = fixedPoint;

      //  Load new point in ControlNetEditor
      loadPoint();
      m_parent->setShown(true);
      m_parent->raise();

      delete fixedPointDialog;
      fixedPointDialog = NULL;

      // emit signal so the nave tool refreshes the list
      emit refreshNavList();
      // emit signal so the nav tool can update edit point
      emit editPointChanged(m_editPoint->GetId());
      colorizeSaveButton();
    }
  }
#endif



  /**
   * Delete control point
   *
   * @param point Pointer to control point (net memory) to be deleted.
   *
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                           namespace std"
   * @history 2010-07-12 Jeannie Walldren - Fixed bug by setting control point
   *                          to NULL if removed from the control net and check
   *                          for NULL points before emitting editPointChanged
   * @history 2011-04-04 Tracie Sucharski - Move code that was after the exec
   *                          block within, so that if the Cancel button is
   *                          selected, nothing else happens.
   * @history 2011-07-15 Tracie Sucharski - Print info about deleting editLock
   *                          points and reference measures.
   * @history 2013-05-09 Tracie Sucharski - Check for user selecting all measures for deletion and 
   *                          print warning that point will be deleted. 
   *
   */
  void ControlNetEditor::deleteControlPoint(QString pointId) {//change this to cpid
    // Make a copy and make sure editPoint is a copy (which means it does not
    // have a parent network.

    ControlPoint *point = new ControlPoint(*m_controlNet->GetPoint(pointId));
    
    //qDebug() << "ControlNetEditor::deletePoint(ControlPoint *point)";
    if (m_editPoint != NULL && m_editPoint->Parent() == NULL) {
      delete m_editPoint;
      m_editPoint = NULL;
    }
    m_editPoint = new ControlPoint;
    *m_editPoint = *point;
//     loadPoint();

    //  Change point in viewport to red so user can see what point they are about to delete.
    // the nav tool will update edit point
//     emit editPointChanged(m_editPoint->GetId());

    DeleteControlPointDialog *deletePointDialog = new DeleteControlPointDialog;
    QString CPId = m_editPoint->GetId();
    deletePointDialog->pointIdValue->setText(CPId);

    //  Need all files for this point
    for (int i = 0; i < m_editPoint->GetNumMeasures(); i++) {
      ControlMeasure &m = *(*m_editPoint)[i];
      QString file = m_serialNumberList->FileName( m.GetCubeSerialNumber() );
      deletePointDialog->fileList->addItem(file);
    }

    if ( deletePointDialog->exec() ) {

      int numDeleted = deletePointDialog->fileList->selectedItems().count();

      //  Delete entire control point, either through deleteAllCheckBox or all measures selected
      if ( deletePointDialog->deleteAllCheckBox->isChecked() ||
           numDeleted == m_editPoint->GetNumMeasures() ) {
 
        //  If all measures being deleted, let user know and give them the option to quit operation
        if ( !deletePointDialog->deleteAllCheckBox->isChecked() ) {
          QString message = "You have selected all measures in this point to be deleted.  This "
            "control point will be deleted.  Do you want to delete this control point?";
          int  response = QMessageBox::question(m_parent,
                                                "Delete control point", message,
                                                QMessageBox::Yes | QMessageBox::No,
                                                QMessageBox::Yes);
          // If No, do nothing
          if (response == QMessageBox::No) {
            return;
          }
        }

        // First get rid of deleted point from m_filteredPoints list. Need index
        // in control net for pt int i = m_controlNet->m_filteredPoints.
        m_parent->setShown(false);
        // remove this point from the control network
        if (m_controlNet->DeletePoint( m_editPoint->GetId() ) == ControlPoint::PointLocked) {
          QMessageBox::information(m_parent, "EditLocked Point",
              "This point is EditLocked and cannot be deleted.");
          return;
        }
        if (m_editPoint != NULL && m_editPoint->Parent() == NULL) {
          delete m_editPoint;
          m_editPoint = NULL;
        }
        //  emit signal so the nav tool refreshes the list
//         emit refreshNavList();

//        emit signal to refresh mosaicscene widget?
      }

      //  Delete specific measures from control point
      else {
        //  Keep track of editLocked measures for reporting
        int lockedMeasures = 0;
        for (int i = 0; i < deletePointDialog->fileList->count(); i++) {
          QListWidgetItem *item = deletePointDialog->fileList->item(i);
          
          if ( !deletePointDialog->fileList->isItemSelected(item) ) continue;

          //  Do not delete reference without asking user
          if ( m_editPoint->IsReferenceExplicit() &&
               ( m_editPoint->GetRefMeasure()->GetCubeSerialNumber() ==
                 (*m_editPoint)[i]->GetCubeSerialNumber() ) ) {
            QString message = "You are trying to delete the Reference measure."
                              "  Do you really want to delete the Reference measure?";
            switch( QMessageBox::question(m_parent, "Delete Reference measure?",
                                          message, "&Yes", "&No", 0, 0) ) {
              //  Yes:  skip to end of switch todelete the measure
              case 0:
                break;
              //  No:  continue to next measure in the loop
              case 1:
                //  if only a single measure and it's reference and user chooses not to delete,
                //  simply return.  The point has not changed.
                if (numDeleted == 1) {
                  return;
                }
                continue;
            }
          }

          if (m_editPoint->Delete(i) == ControlMeasure::MeasureLocked) {
            lockedMeasures++;
          }
        }

        if (lockedMeasures > 0) {
          QMessageBox::information(m_parent, "EditLocked Measures",
                                   QString::number(lockedMeasures) + " / "
                                   + QString::number(
                                     deletePointDialog->fileList->selectedItems().size() ) +
                                   " measures are EditLocked and were not deleted.");
        }

//         loadPoint();
//         m_parent->setShown(true);
//         m_parent->raise();

//         loadTemplateFile( m_pointEditor->templateFileName() );
      }

      // emit a signal to alert user to save when exiting
//       emit netChanged();

      // emit signal so the nav tool can update edit point
      if (m_editPoint != NULL) {
//         emit editPointChanged( m_editPoint->GetId() );
        //  Change Save Point button text to red
//         colorizeSaveButton();
      }
      else {
        // if the entire point is deleted, update with point Id = "" this signal is connected
        // to ControlNetEditor::paintAllViewports and QnetNavTool::updateEditPoint
      }
        emit controlPointDeleted();
    }
#if 0
#endif
  }


  /**
   * Modify control point
   *
   * @param point Pointer to control point to be modified.
   *
   * @history 2009-09-15 Tracie Sucharski - Add error check for points
   *                       with no measures.
   */
#if 0
  void ControlNetEditor::modifyPoint(ControlPoint *point) {

    //  If no measures, print info and return
    if (point->GetNumMeasures() == 0) {
      QString message = "This point has no measures.";
      QMessageBox::warning(m_parent, "Warning", message);
      // update nav list to re-highlight old point
      if (m_editPoint != NULL) {
        // emit signal so the nav tool can update edit point
        emit editPointChanged(m_editPoint->GetId());
      }
      else {
        // this signal is connected to ControlNetEditor::paintAllViewports
        // and QnetNavTool::updateEditPoint
        emit editPointChanged("");
      }
      return;
    }
    //  Make a copy of point for editing, first make sure memory not already
    //  allocated
    if (m_editPoint != NULL && m_editPoint->Parent() == NULL) {
      delete m_editPoint;
      m_editPoint = NULL;
    }
    m_editPoint = new ControlPoint;
    *m_editPoint = *point;

    //  If navTool modfify button pressed, m_leftFile needs to be reset
    //  TODO: better way - have 2 slots
    if (sender() != this) m_leftFile.clear();
    loadPoint();
    m_qnetTool->setShown(true);
    m_qnetTool->raise();
    loadTemplateFile(m_pointEditor->templateFileName());

    // emit signal so the nav tool can update edit point
    emit editPointChanged(m_editPoint->GetId());

    // New point loaded, make sure Save Measure Button text is default
    m_savePoint->setPalette(m_saveDefaultPalette);
  }
#endif
}


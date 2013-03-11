#include <QtGui>

#include "QnetSetAprioriDialog.h"

#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"
#include "QnetTool.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  QnetSetAprioriDialog::QnetSetAprioriDialog(QnetTool *qnetTool, QWidget *parent) : QDialog(parent) {
    m_qnetTool = qnetTool;

    setupUi(this);
  //  editLockPointsGroupBox->hide();
    connect(editLockPointsListBox, SIGNAL(itemChanged(QListWidgetItem *)),
            this, SLOT(unlockPoint(QListWidgetItem *)));
    connect(setAprioriButton, SIGNAL(clicked()), this, SLOT(setApriori()));
    //connect(closeButton, SIGNAL(clicked()), this, SLOT(closeAprioriDialog()));
  //  connect(closeButton, SIGNAL(clicked()), this, SIGNAL(aprioriDialogClosed()));
    //layout()->setSizeConstraint(QLayout::SetFixedSize);

  }



  /**
   * Set control points in the dialog
   *
   * @param selectedPoints QList<QListWidgetItem *> ControlPoints listed
   *
   * @internal
   * @history 2011-10-03 Tracie Sucharski - Do not enable user Entered Button,
   *                        this will only be enabled if the group box is enabled.
   */
  void QnetSetAprioriDialog::setPoints(QList<QListWidgetItem *> selectedPoints) {

    editLockPointsListBox->clear();
    clearLineEdits();

    m_points = selectedPoints;
    if (m_points.size() == 1) {
      fillLineEdits();
    }
    else {
      userEnteredRadioButton->setEnabled(false);
    }

    // Fill editLock List Box
    for (int i=0; i<m_points.size(); i++) {
      QString id = m_points.at(i)->text();
      ControlPoint *pt = m_qnetTool->controlNet()->GetPoint(id);
      if (pt->IsEditLocked()) {
        QListWidgetItem *item = new QListWidgetItem(*(m_points[i]));
        item->setCheckState(Qt::Checked);
        editLockPointsListBox->addItem(item);
      }
    }
  }


  void QnetSetAprioriDialog::unlockPoint(QListWidgetItem *pointId) {

    ControlPoint *pt = m_qnetTool->controlNet()->GetPoint(pointId->text());
    if (pt->IsEditLocked() && pointId->checkState() == Qt::Unchecked) {
      pt->SetEditLock(false);
      editLockPointsListBox->removeItemWidget(pointId);
      pointId->setHidden(true);
      editLockPointsListBox->repaint();
      this->repaint();
      emit netChanged();
    }
  }



  void QnetSetAprioriDialog::clearLineEdits() {
    aprioriLatEdit->setText("");
    aprioriLonEdit->setText("");
    aprioriRadiusEdit->setText("");
    latSigmaEdit->setText("");
    lonSigmaEdit->setText("");
    radiusSigmaEdit->setText("");
  }


  void QnetSetAprioriDialog::fillLineEdits() {

    QString id = m_points.at(0)->text();
    ControlPoint *pt = m_qnetTool->controlNet()->GetPoint(id);
    SurfacePoint sPt = pt->GetAprioriSurfacePoint();
    vector<Distance> targetRadii = m_qnetTool->controlNet()->GetTargetRadii();
    sPt.SetRadii(Distance(targetRadii[0]),
                 Distance(targetRadii[1]),
                 Distance(targetRadii[2]));
    if (sPt.GetLatitude().degrees() != Null) {
      aprioriLatEdit->setText(
        QString::number(sPt.GetLatitude().degrees()));
    }
    if (sPt.GetLatSigmaDistance().meters() != Null) {
      latSigmaEdit->setText(
        QString::number(sPt.GetLatSigmaDistance().meters()));
    }
    if (sPt.GetLongitude().degrees() != Null) {
      aprioriLonEdit->setText(
        QString::number(sPt.GetLongitude().degrees()));
    }
    if (sPt.GetLonSigmaDistance().meters() != Null) {
      lonSigmaEdit->setText(
        QString::number(sPt.GetLonSigmaDistance().meters()));
    }
    if (sPt.GetLocalRadius().meters() != Null) {
      aprioriRadiusEdit->setText(
        QString::number(sPt.GetLocalRadius().meters()));
    }
    if (sPt.GetLocalRadiusSigma().meters() != Null) {
      radiusSigmaEdit->setText(
        QString::number(sPt.GetLocalRadiusSigma().meters()));
    }
  }



  /**
   * Slot to set apriori on selected Points from Navigator list box
   *
   * @author 2011-03-24 Tracie Sucharski
   *
   * @internal
   * @todo  This method should be temporary until the control point editor
   *           comes online.  If this stick around, needs to be re-disigned-
   *           put in a separate class??
   *
   * @history 2011-04-04 Tracie Sucharski - Grey out userEntered if more than
   *                        a single point is selected.  Grey out lat,lon,radius
   *                        edits if UserEntered is not selected.
   * @history 2011-04-13 Tracie Sucharski - If single point selected, fill in
   *                        LineEdit's with current controlPoint values.
   * @history 2011-04-19 Tracie Sucharski - Redesign using modeless dialog.
   * @history 2011-04-26 Tracie Sucharski - Move from QnetNavTool to
   *                        QnetSetAprioriDialog.
   */
  void QnetSetAprioriDialog::setApriori() {

    double latSigma = Null;
    double lat = Null;
    double lonSigma = Null;
    double lon = Null;
    double radiusSigma = Null;
    double radius = Null;

    if (latitudeConstraintsGroupBox->isChecked()) {

      if (userEnteredRadioButton->isChecked() && aprioriLatEdit->text() != "") {
        lat = aprioriLatEdit->text().toDouble();
      }
      if (latSigmaEdit->text() != "") {
        latSigma = latSigmaEdit->text().toDouble();
      }
    }
    if (longitudeConstraintsGroupBox->isChecked()) {

      if (userEnteredRadioButton->isChecked() && aprioriLonEdit->text() != "") {
        lon = aprioriLonEdit->text().toDouble();
      }
      if (lonSigmaEdit->text() != "") {
        lonSigma = lonSigmaEdit->text().toDouble();
      }
    }

    if (radiusConstraintsGroupBox->isChecked()) {

      if (userEnteredRadioButton->isChecked() && aprioriRadiusEdit->text() != "") {
        radius = aprioriRadiusEdit->text().toDouble();
      }
      if (radiusSigmaEdit->text() != "") {
        radiusSigma = radiusSigmaEdit->text().toDouble();
      }
    }

    //  If the SetAprioriPoint group box selected, set aprioriSurfacePoint for
    //  those points not editLocked.
    for (int i = 0; i < m_points.size(); i++) {
      QString id = m_points.at(i)->text();
      ControlPoint *pt = m_qnetTool->controlNet()->GetPoint(id);
      if (pt->IsEditLocked()) continue;

      if (!pt->HasAprioriCoordinates()) {
        QString msg = "Point [" + id + "] does not have an Apriori coordinate.  "
          "Make sure to save the ground source measurement then the Point before "
          "setting the sigmas.  The sigmas for this point will not be set.";
        QMessageBox::warning((QWidget *)parent(), "Warning", msg);
        continue;
      }

      if (pointSourceGroupBox->isChecked()) {
        if (referenceMeasureRadioButton->isChecked()) {
          ControlMeasure *m = pt->GetRefMeasure();
          // Find camera from network camera list
          int camIndex = m_qnetTool->serialNumberList()->SerialNumberIndex(
              m->GetCubeSerialNumber());
          Camera *cam = m_qnetTool->controlNet()->Camera(camIndex);
          cam->SetImage(m->GetSample(),m->GetLine());
          pt->SetAprioriSurfacePoint(cam->GetSurfacePoint());
          pt->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Reference);
        }
        else if (averageMeasuresRadioButton->isChecked()) {
          pt->ComputeApriori();
          // Do not need to set AprioriSurfacePointSource or AprioriRadiusSource,
          // ComputeApriori does this for us.
        }
        else if (userEnteredRadioButton->isChecked()) {
          pt->SetAprioriSurfacePoint(SurfacePoint(
                                     Latitude(lat, Angle::Degrees),
                                     Longitude(lon, Angle::Degrees),
                                     Distance(radius,Distance::Meters)));
          pt->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::User);
          pt->SetAprioriRadiusSource(ControlPoint::RadiusSource::User);
        }
      }

      try {
        //  Read Surface point from the control point and set the sigmas,
        //  first set the target radii
        SurfacePoint spt = pt->GetAprioriSurfacePoint();
        vector<Distance> targetRadii = m_qnetTool->controlNet()->GetTargetRadii();
        spt.SetRadii(Distance(targetRadii[0]),
                     Distance(targetRadii[1]),
                     Distance(targetRadii[2]));
        spt.SetSphericalSigmasDistance(Distance(latSigma,Distance::Meters),
                                       Distance(lonSigma,Distance::Meters),
                                       Distance(radiusSigma,Distance::Meters));
        //  Write the surface point back out to the controlPoint
        pt->SetAprioriSurfacePoint(spt);
        //  TODO:  Is the following line necessary, should error be thrown
        //  for free or fixed pts?
        //pt->SetType(ControlPoint::Constrained);
        emit pointChanged(id);
        emit netChanged();
      }
      catch (IException &e)  {
        QString message = "Error setting sigmas. \n";
        message += e.toString();
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        QApplication::restoreOverrideCursor();
        // Sigmas failed, but surface pt coordinate was set
        emit pointChanged(id);
        emit netChanged();
        return;
      }
    }

  }
}

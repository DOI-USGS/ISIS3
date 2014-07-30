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
#include "MatrixViewWorkOrder.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "CorrelationMatrix.h"
#include "Directory.h"
#include "IException.h"
#include "MatrixSceneWidget.h"
#include "Project.h"

namespace Isis {

  MatrixViewWorkOrder::MatrixViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText( tr("View Correlation &Matrix...") );
  }


  MatrixViewWorkOrder::MatrixViewWorkOrder(const MatrixViewWorkOrder &other) :
      WorkOrder(other) {
  }


  MatrixViewWorkOrder::~MatrixViewWorkOrder() {
  }


  MatrixViewWorkOrder *MatrixViewWorkOrder::clone() const {

    return new MatrixViewWorkOrder(*this);

  }


  /**
   * This check is used by Directory::supportedActions(DataType data).
   *
   * @param matrix The correlation matrix that we want to view.
   *
   * @return bool True if data (passed to this method as the matrix parameter by supportedActions)
   *         is of type CorrelationMatrix.
   */
  bool MatrixViewWorkOrder::isExecutable(CorrelationMatrix *matrix) {
    return matrix->isValid();
  }


  /**
   * This will create open the MatrixSceneWidgets QGraphicsView
   *
   *
   */
  bool MatrixViewWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
      QStringList viewOptions;

      QList<MatrixSceneWidget *> existingViews = project()->directory()->matrixViews();
      int viewToUse = -1;

      if ( existingViews.count() ) {
        for (int i = 0; i < existingViews.count(); i++) {
          viewOptions.append( existingViews[i]->windowTitle() );
        }
      }

      viewOptions.append( tr("New Matrix View") );

      if (viewOptions.count() > 1) {
        QString selected = QInputDialog::getItem(NULL, tr("View to see matrixs in"),
            tr("Which view would you like your\nimage's matrixs to be put into?"),
            viewOptions, viewOptions.count() - 1, false, &success);

        viewToUse = viewOptions.indexOf(selected);
      }
      else {
        viewToUse = viewOptions.count() - 1;
      }

      bool newView = false;
      if (viewToUse == viewOptions.count() - 1) {
        newView = true;

        QUndoCommand::setText( tr("View matrix in new matrix view") );

      }
      else if (viewToUse != -1) {
        MatrixSceneWidget *matrixView = existingViews[viewToUse];

        matrixView->drawGrid( project()->correlationMatrix() );
        matrixView->drawElements( project()->correlationMatrix() );
      }

      QStringList internalData;
      internalData.append( QString::number(viewToUse) );
      internalData.append(newView? "new view" : "existing view");
      setInternalData(internalData);
    }

    return success;
  }

  

  bool MatrixViewWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<MatrixViewWorkOrder *>(other);
  }
  


  void MatrixViewWorkOrder::syncRedo() {
    MatrixSceneWidget *matrixViewToUse = project()->directory()->addMatrixView();
    if (matrixViewToUse == NULL) {
      QString msg = "The Correlation Matrix for this bundle could not be displayed";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  

  void MatrixViewWorkOrder::syncUndo() {
    if (internalData()[1] == "new view") {
      delete project()->directory()->matrixViews().last();
    }
  }
}


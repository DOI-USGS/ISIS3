#include <QGridLayout>
#include <QMessageBox>
#include <QRegExp>
#include "QnetCubeNameFilter.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Isis::Qnet;
using namespace std;

namespace Isis {
  /**
   * Contructor for the Cube Image filter.  It creates the Cube Name filter window
   * found in the navtool
   *
   * @param parent The parent widget for the
   *               cube points filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null
   *
   */
  QnetCubeNameFilter::QnetCubeNameFilter(QWidget *parent) : QnetFilter(parent) {
    p_cubeNameEdit = NULL;
    // Create the components for the filter window
    QLabel *label = new QLabel("Filter by cube name (Regular Expressions");
    p_cubeNameEdit = new QLineEdit();

    // Create the layout and add the components to it
    QVBoxLayout *vertLayout = new QVBoxLayout();
    vertLayout->addWidget(label);
    vertLayout->addWidget(p_cubeNameEdit);
    vertLayout->addStretch();
    this->setLayout(vertLayout);
  }

  /**
   * Filters a list of images looking for cube names using the regular expression
   * entered.  The filtered list will appear in the navtools cube list display.
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified to create
   *                          new filtered list from images in the
   *                          existing filtered list. Previously,
   *                          a new filtered list was created from
   *                          the entire serial number list each
   *                          time.
   */
  void QnetCubeNameFilter::filter() {
    // Make sure we have a list of images to filter
    if (g_serialNumberList == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No cubes to filter");
      return;
    }

    // Make sure the user has entered a regular expression for filtering
    QRegExp rx(p_cubeNameEdit->text());
    rx.setPatternSyntax(QRegExp::Wildcard);
    if (rx.isEmpty()) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Enter search QString");
      return;
    }


    // Loop through each image in the filtered list
    // Loop in reverse order since removal list of elements affects index number
    for (int i = g_filteredImages.size() - 1; i >= 0; i--) {
      QString tempFileName = g_serialNumberList->FileName(g_filteredImages[i]);
      // this name contains the QString, keep it in the filtered list
      if (rx.indexIn(QString(tempFileName)) != -1) {
        continue;
      }
      // if there is no match, remove image from filtered list
      else
        g_filteredImages.removeAt(i);

    }
    // Tell the navtool a list has been filtered and it needs to update
    emit filteredListModified();
    return;

  }
}

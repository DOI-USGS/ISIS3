#include "ScatterPlotConfigDialog.h"

#include <qwt_interval.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QPair>
#include <QPushButton>
#include <QSpinBox>

#include "Cube.h"
#include "MdiCubeViewport.h"
#include "SpecialPixel.h"
#include "Workspace.h"

namespace Isis {
  /**
   * This method creates all the dialog boxes required for the
   * scatter plot window.
   *
   * @param activeViewport The current user-selected viewport
   * @param workspace The workspace containing all of the viewports
   * @param parent The Qt-parent for this dialog
   */
  ScatterPlotConfigDialog::ScatterPlotConfigDialog(
      MdiCubeViewport *activeViewport, Workspace *workspace, QWidget *parent) :
      QDialog(parent) {
    setWindowTitle("Configure Scatter Plot");

    m_oldXAxisCube = NULL;
    m_workspace = workspace;

    QGridLayout *mainLayout = new QGridLayout;

    /**
     *  The layout is shown below:
     *
     *  |--------------------------------------------------------------|
     *  | Text    rowspan=1, colspan=3                                 |
     *  |--------------------------------------------------------------|
     *  | Config X         rowspan=1, colspan=3                        |
     *  |--------------------------------------------------------------|
     *  |   |   Cube                | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   |   Band                | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   |   Bins (resolution)   | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   |   Use Viewport Ranges | Checkbox    rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  | Config Y  rowspan=1, colspan=3                               |
     *  |--------------------------------------------------------------|
     *  |   |   Cube                | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   |   Band                | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   |   Bins (resolution)   | Input Edit  rowspan=1, colspan=1 |
     *  |--------------------------------------------------------------|
     *  |   Ok Cancel      rowspan=1, colspan=3                        |
     *  |--------------------------------------------------------------|
     */

    int curRow = 0;
    QLabel *titleLabel = new QLabel("<h2>Create Scatter Plot</h2>");
    mainLayout->addWidget(titleLabel, curRow, 0, 1, 3);
    curRow++;

    QLabel *headerLabel = new QLabel("Choose where to gather the scatter plot "
        "data from. The X and Y axes are a single band of a cube and must have "
        "the same dimensions");
    headerLabel->setWordWrap(true);
    mainLayout->addWidget(headerLabel, curRow, 0, 1, 3);
    curRow++;

    int spacerRow = curRow;
    curRow++;

    QLabel *configXLabel = new QLabel("Choose X Input Data");
    mainLayout->addWidget(configXLabel, curRow, 0, 1, 3);
    curRow++;

    QLabel *xAxisCubeLabel = new QLabel("Cube");
    mainLayout->addWidget(xAxisCubeLabel, curRow, 1, 1, 1);

    m_xAxisCubeCombo = new QComboBox;
    m_xAxisCubeCombo->setInsertPolicy(QComboBox::InsertAlphabetically);
    connect(m_xAxisCubeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_xAxisCubeCombo, curRow, 2, 1, 1);
    curRow++;

    QLabel *xAxisCubeBandLabel = new QLabel("Cube Band");
    mainLayout->addWidget(xAxisCubeBandLabel, curRow, 1, 1, 1);

    m_xAxisCubeBandSpinBox = new QSpinBox;
    connect(m_xAxisCubeBandSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_xAxisCubeBandSpinBox, curRow, 2, 1, 1);
    curRow++;

    QLabel *xAxisBinCountLabel = new QLabel("Bin Count (resolution)");
    mainLayout->addWidget(xAxisBinCountLabel, curRow, 1, 1, 1);

    m_xAxisBinCountSpinBox = new QSpinBox;
    m_xAxisBinCountSpinBox->setMinimum(8);
    m_xAxisBinCountSpinBox->setMaximum(1048576);
    m_xAxisBinCountSpinBox->setValue(512);
    connect(m_xAxisBinCountSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_xAxisBinCountSpinBox, curRow, 2, 1, 1);
    curRow++;

    QLabel *useViewportRangesLabel = new QLabel("Use Viewport Visible Range");
    mainLayout->addWidget(useViewportRangesLabel, curRow, 1, 1, 1);

    m_useViewportRangesCheckBox = new QCheckBox;
    connect(m_useViewportRangesCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_useViewportRangesCheckBox, curRow, 2, 1, 1);
    curRow++;

    QLabel *configYLabel = new QLabel("Choose Y Input Data");
    mainLayout->addWidget(configYLabel, curRow, 0, 1, 3);
    curRow++;

    QLabel *yAxisCubeLabel = new QLabel("Cube");
    mainLayout->addWidget(yAxisCubeLabel, curRow, 1, 1, 1);

    m_yAxisCubeCombo = new QComboBox;
    m_yAxisCubeCombo->setInsertPolicy(QComboBox::InsertAlphabetically);
    connect(m_yAxisCubeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_yAxisCubeCombo, curRow, 2, 1, 1);
    curRow++;

    QLabel *yAxisCubeBandLabel = new QLabel("Cube Band");
    mainLayout->addWidget(yAxisCubeBandLabel, curRow, 1, 1, 1);

    m_yAxisCubeBandSpinBox = new QSpinBox;
    connect(m_yAxisCubeBandSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_yAxisCubeBandSpinBox, curRow, 2, 1, 1);
    curRow++;

    QLabel *yAxisBinCountLabel = new QLabel("Bin Count (resolution)");
    mainLayout->addWidget(yAxisBinCountLabel, curRow, 1, 1, 1);

    m_yAxisBinCountSpinBox = new QSpinBox;
    m_yAxisBinCountSpinBox->setMinimum(8);
    m_yAxisBinCountSpinBox->setMaximum(1048576);
    m_yAxisBinCountSpinBox->setValue(512);
    connect(m_yAxisBinCountSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_yAxisBinCountSpinBox, curRow, 2, 1, 1);
    curRow++;

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();

    m_createButton = new QPushButton("Create");
    m_createButton->setIcon(QIcon::fromTheme("window-new"));
    connect(m_createButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    buttonsLayout->addWidget(m_createButton);

    QPushButton *cancelButton = new QPushButton("Cancel");
    cancelButton->setIcon(QIcon::fromTheme("window-close"));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    buttonsLayout->addWidget(cancelButton);

    QWidget *buttonsWrapper = new QWidget;
    buttonsWrapper->setLayout(buttonsLayout);
    mainLayout->addWidget(buttonsWrapper, curRow, 0, 1, 3);

    mainLayout->setColumnMinimumWidth(0, 20);
    mainLayout->setRowMinimumHeight(spacerRow, 20);
    setLayout(mainLayout);

    setTabOrder(m_xAxisCubeCombo, m_xAxisCubeBandSpinBox);
    setTabOrder(m_xAxisCubeBandSpinBox, m_xAxisBinCountSpinBox);
    setTabOrder(m_xAxisBinCountSpinBox, m_yAxisCubeCombo);
    setTabOrder(m_yAxisCubeCombo, m_yAxisCubeBandSpinBox);
    setTabOrder(m_yAxisCubeBandSpinBox, m_yAxisBinCountSpinBox);
    setTabOrder(m_yAxisBinCountSpinBox, m_createButton);
    setTabOrder(m_createButton, cancelButton);

    refreshWidgetStates();

    if (activeViewport) {
      m_xAxisCubeCombo->setCurrentIndex(
          m_xAxisCubeCombo->findData(qVariantFromValue(activeViewport->cube())));
      m_yAxisCubeCombo->setCurrentIndex(
          m_yAxisCubeCombo->findData(qVariantFromValue(activeViewport->cube())));
      m_yAxisCubeBandSpinBox->setValue(2);
    }
  }


  /**
   * This is overridden to give a better default size than what Qt calculates
   *   by default.
   *
   * @return The recommended size for this dialog
   */
  QSize ScatterPlotConfigDialog::sizeHint() const {
    return QSize(qRound(QDialog::sizeHint().width() * 1.3),
                 QDialog::sizeHint().height());
  }


  /**
   * The current user selection for a cube for the x-axis data for a scatter
   *   plot. This never returns null if the dialog was accepted().
   *
   * @return The x-axis cube or NULL if there isn't one selected.
   */
  Cube *ScatterPlotConfigDialog::xAxisCube() const {
    return m_xAxisCubeCombo->itemData(
        m_xAxisCubeCombo->currentIndex()).value<Cube *>();
  }


  /**
   * The current user selection for a cube for the x-axis data for a scatter
   *   plot. This never returns null if the dialog was accepted().
   *
   * @return The y-axis cube or NULL if there isn't one selected.
   */
  Cube *ScatterPlotConfigDialog::yAxisCube() const {
    return m_yAxisCubeCombo->itemData(
        m_yAxisCubeCombo->currentIndex()).value<Cube *>();
  }


  /**
   * The current user selection for the band in the x-axis cube to get data
   *   from. This isn't valid unless there is an xAxisCube().
   *
   * @return The x-axis cube's band to get data from.
   */
  int ScatterPlotConfigDialog::xAxisCubeBand() const {
    return m_xAxisCubeBandSpinBox->value();
  }


  /**
   * The current user selection for the band in the y-axis cube to get data
   *   from. This isn't valid unless there is an yAxisCube().
   *
   * @return The y-axis cube's band to get data from.
   */
  int ScatterPlotConfigDialog::yAxisCubeBand() const {
    return m_yAxisCubeBandSpinBox->value();
  }


  /**
   * The current user selection for the resolution of the scatter plot's x data.
   *
   * @return The x-axis data's bin count for a histogram/spectrogram
   */
  int ScatterPlotConfigDialog::xAxisBinCount() const {
    return m_xAxisBinCountSpinBox->value();
  }


  /**
   * The current user selection for the resolution of the scatter plot's y data.
   *
   * @return The y-axis data's bin count for a histogram/spectrogram
   */
  int ScatterPlotConfigDialog::yAxisBinCount() const {
    return m_yAxisBinCountSpinBox->value();
  }


  /**
   * This is the sample range from which the scatter plot should be created.
   *   The return value is 1-based (samples start at 1) and inclusive. The range
   *   1 and nsamples is the entire sample range.
   *
   * @return The sample range for the scatter plot data
   */
  QwtInterval ScatterPlotConfigDialog::sampleRange() const {
    return range(SampleRange);
  }


  /**
   * This is the line range from which the scatter plot should be created.
   *   The return value is 1-based (lines start at 1) and inclusive. The range
   *   1 and nlines is the entire line range.
   *
   * @return The line range for the scatter plot data
   */
  QwtInterval ScatterPlotConfigDialog::lineRange() const {
    return range(LineRange);
  }


  /**
   * This returns a viewport which contains the x-axis cube.
   *
   * @return a viewport which contains the x-axis cube
   */
  MdiCubeViewport *ScatterPlotConfigDialog::xAxisCubeViewport() const {
    MdiCubeViewport * container = NULL;

    foreach (MdiCubeViewport *viewport, *m_workspace->cubeViewportList()) {
      if (viewport->cube() == xAxisCube())
        container = viewport;
    }

    return container;
  }


  /**
   * This returns a viewport which contains the y-axis cube.
   *
   * @return a viewport which contains the y-axis cube
   */
  MdiCubeViewport *ScatterPlotConfigDialog::yAxisCubeViewport() const {
    MdiCubeViewport * container = NULL;

    foreach (MdiCubeViewport *viewport, *m_workspace->cubeViewportList()) {
      if (viewport->cube() == yAxisCube())
        container = viewport;
    }

    return container;
  }


  /**
   * Update the enabled/disabled states of the various widgets based on the
   *   current user inputs' states. This also updates the cube lists based on
   *   what exists and what is selected.
   */
  void ScatterPlotConfigDialog::refreshWidgetStates() {
    QList<Cube *> listedXCubes;
    for (int i = 0; i < m_xAxisCubeCombo->count(); i++) {
      listedXCubes.append(m_xAxisCubeCombo->itemData(i).value<Cube *>());
    }

    QList<Cube *> allXCubes;
    foreach (MdiCubeViewport *viewport, *m_workspace->cubeViewportList()) {
      allXCubes.append(viewport->cube());
    }

    // We're first going to look for deleted cubes and remove them from the list
    QList<Cube *> extraXCubes = removeFromList(listedXCubes, allXCubes);
    foreach (Cube *cubeToRemove, extraXCubes) {
      m_xAxisCubeCombo->removeItem(
          m_xAxisCubeCombo->findData(qVariantFromValue(cubeToRemove)));
    }

    // Now let's add items that are missing from the list
    QList<Cube *> missingXCubes = removeFromList(allXCubes, listedXCubes);
    foreach (Cube *cubeToAdd, missingXCubes) {
      if (m_xAxisCubeCombo->findData(qVariantFromValue(cubeToAdd)) == -1) {
        QString cubeName = QFileInfo(cubeToAdd->fileName()).baseName();
        m_xAxisCubeCombo->addItem(cubeName, qVariantFromValue(cubeToAdd));
      }
    }

    m_xAxisCubeBandSpinBox->setMinimum(1);

    if (xAxisCube()) {
      m_xAxisCubeBandSpinBox->setEnabled(true);
      m_xAxisBinCountSpinBox->setEnabled(true);
      m_useViewportRangesCheckBox->setEnabled(true);
      m_yAxisCubeCombo->setEnabled(true);

      m_xAxisCubeBandSpinBox->setMaximum(xAxisCube()->bandCount());

      QList<Cube *> listedYCubes;
      for (int i = 0; i < m_yAxisCubeCombo->count(); i++) {
        listedYCubes.append(m_yAxisCubeCombo->itemData(i).value<Cube *>());
      }

      QList<Cube *> allYCubes;
      foreach (MdiCubeViewport *viewport, *m_workspace->cubeViewportList()) {
        Cube *cube = viewport->cube();

        if (cube->sampleCount() == xAxisCube()->sampleCount() &&
            cube->lineCount() == xAxisCube()->lineCount()) {
          allYCubes.append(cube);
        }
      }

      // We're first going to look for deleted cubes and remove them from the list
      QList<Cube *> extraYCubes = removeFromList(listedYCubes, allYCubes);
      foreach (Cube *cubeToRemove, extraYCubes) {
        m_yAxisCubeCombo->removeItem(
            m_yAxisCubeCombo->findData(qVariantFromValue(cubeToRemove)));
      }

      // Now let's add items that are missing from the list
      QList<Cube *> missingYCubes = removeFromList(allYCubes, listedYCubes);
      foreach (Cube *cubeToAdd, missingYCubes) {
        if (m_yAxisCubeCombo->findData(qVariantFromValue(cubeToAdd)) == -1) {
          QString cubeName = QFileInfo(cubeToAdd->fileName()).baseName();
          m_yAxisCubeCombo->addItem(cubeName, qVariantFromValue(cubeToAdd));
        }
      }

      if (m_useViewportRangesCheckBox->isChecked()) {
        m_yAxisCubeCombo->setEnabled(false);
      }

      if (xAxisCube() != m_oldXAxisCube) {
        m_oldXAxisCube = xAxisCube();
        m_yAxisCubeCombo->setCurrentIndex(
            m_yAxisCubeCombo->findData(qVariantFromValue(xAxisCube())));
      }

      m_yAxisCubeBandSpinBox->setMinimum(1);

      m_yAxisCubeBandSpinBox->setEnabled(yAxisCube() != NULL);
      m_yAxisBinCountSpinBox->setEnabled(yAxisCube() != NULL);

      if (yAxisCube()) {
        m_yAxisCubeBandSpinBox->setMaximum(yAxisCube()->bandCount());
      }
    }
    else {
      m_xAxisCubeBandSpinBox->setMaximum(1);
      m_xAxisCubeBandSpinBox->setEnabled(false);
      m_xAxisBinCountSpinBox->setEnabled(false);
      m_useViewportRangesCheckBox->setEnabled(false);
      m_yAxisCubeCombo->setEnabled(false);
      m_yAxisCubeBandSpinBox->setMaximum(1);
      m_yAxisCubeBandSpinBox->setEnabled(false);
      m_yAxisBinCountSpinBox->setEnabled(false);
      m_createButton->setEnabled(false);
    }

    bool allowCreation = true;

    if (!xAxisCube() || !yAxisCube())
      allowCreation = false;

    if (allowCreation) {
      if (xAxisCube() == yAxisCube() &&
          m_xAxisCubeBandSpinBox->value() == m_yAxisCubeBandSpinBox->value()) {
        allowCreation = false;
      }
    }

    if (m_createButton->isEnabled() != allowCreation)
      m_createButton->setEnabled(allowCreation);
  }


  /**
   * This is a helper method. It takes all of the itemsToRemove out of list and
   *   returns the result.
   *
   * @param list The original list of cubes
   * @param itemsToRemove A list of cubes to take out of the original list of
   *                      cubes
   * @return The result of list - itemsToRemove
   */
  QList<Cube *> ScatterPlotConfigDialog::removeFromList(QList<Cube *> list,
      QList<Cube *> itemsToRemove) {
    foreach (Cube *toRemove, itemsToRemove) {
      list.removeAll(toRemove);
    }

    return list;
  }


  /**
   * This returns the sample/line ranges indicated by the user for the scatter
   *   plot to be created on. This is a helper method for sampleRange() and
   *   lineRange(). NOTE: If a cube if opened twice, and we're using the
   *   viewport visible range, it's currently ambiguous which viewport to use.
   *
   * @param rangeType The range type we're getting: sample range or line range
   * @return The sample or line range to create a scatter plot on. The
   *         information is 1-based and inclusive, so a result of [1, nsamples]
   *         is the entire sample range.
   */
  QwtInterval ScatterPlotConfigDialog::range(RangeType rangeType) const {
    QwtInterval sampleRange;
    QwtInterval lineRange;

    if (m_useViewportRangesCheckBox->isChecked()) {
      MdiCubeViewport * container = xAxisCubeViewport();
      if (!container && xAxisCube()) {
        sampleRange.setInterval(1.0, xAxisCube()->sampleCount());
        lineRange.setInterval(1.0, xAxisCube()->lineCount());
      }
      else if (container && xAxisCube()) {
        double startSample = Null;
        double startLine = Null;
        double endSample = Null;
        double endLine = Null;

        container->viewportToCube(0, 0, startSample, startLine);
        container->viewportToCube(container->viewport()->width() - 1,
                                  container->viewport()->height() - 1,
                                  endSample, endLine);
        sampleRange.setInterval(qRound(startSample), qRound(endSample));
        lineRange.setInterval(qRound(startLine), qRound(endLine));
      }
    }
    else {
      sampleRange.setInterval(1.0, xAxisCube()->sampleCount());
      lineRange.setInterval(1.0, xAxisCube()->lineCount());
    }

    if (rangeType == SampleRange)
      return sampleRange;
    else
      return lineRange;
  }
}


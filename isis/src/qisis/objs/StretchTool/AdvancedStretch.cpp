#include "AdvancedStretch.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

#include "Stretch.h"
#include "IString.h"
#include "IException.h"
#include "StretchType.h"
#include "LinearStretchType.h"
#include "SawtoothStretchType.h"
#include "BinaryStretchType.h"
#include "ManualStretchType.h"

namespace Isis {
  /**
   * This constructs an advanced stretch.
   *
   * @param hist Current histogram of visible area
   * @param curStretch Current stretch
   * @param name Graph name
   * @param color Graph color
   */
  AdvancedStretch::AdvancedStretch(Histogram &hist,
                                   const Stretch &curStretch,
                                   const QString &name, const QColor &color) {
    setLayout(new QVBoxLayout());

    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::MinimumExpanding);
    QWidget *typeSelectionArea = new QWidget();
    typeSelectionArea->setLayout(new QHBoxLayout());
    typeSelectionArea->layout()->addWidget(new QLabel("Stretch Type"));

    p_stretchTypeSelection = new QComboBox();
    p_stretchTypeSelection->addItem("Linear",   0);
    p_stretchTypeSelection->addItem("Sawtooth", 1);
    p_stretchTypeSelection->addItem("Binary",   2);
    p_stretchTypeSelection->addItem("Manual",   3);

    typeSelectionArea->layout()->addWidget(p_stretchTypeSelection);
    layout()->addWidget(typeSelectionArea);

    p_stretchTypeStack = new QStackedWidget();
    LinearStretchType *linear = new LinearStretchType(hist, curStretch,
        name, color);
    connect(linear, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    connect(linear, SIGNAL(saveToCube()), this, SIGNAL(saveToCube()));
    connect(linear, SIGNAL(deleteFromCube()), this, SIGNAL(deleteFromCube()));
    connect(linear, SIGNAL(loadStretch()), this, SIGNAL(loadStretch()));
    p_stretchTypeStack->addWidget(linear);

    SawtoothStretchType *sawtooth = new SawtoothStretchType(hist, curStretch,
        name, color);
    connect(sawtooth, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    connect(sawtooth, SIGNAL(saveToCube()), this, SIGNAL(saveToCube()));
    connect(sawtooth, SIGNAL(deleteFromCube()), this, SIGNAL(deleteFromCube()));
    connect(sawtooth, SIGNAL(loadStretch()), this, SIGNAL(loadStretch()));
    p_stretchTypeStack->addWidget(sawtooth);

    BinaryStretchType *binary = new BinaryStretchType(hist, curStretch,
        name, color);
    connect(binary, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    connect(binary, SIGNAL(saveToCube()), this, SIGNAL(saveToCube()));
    connect(binary, SIGNAL(deleteFromCube()), this, SIGNAL(deleteFromCube()));
    connect(binary, SIGNAL(loadStretch()), this, SIGNAL(loadStretch()));
    p_stretchTypeStack->addWidget(binary);

    ManualStretchType *manual = new ManualStretchType(hist, curStretch,
        name, color);
    connect(manual, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    connect(manual, SIGNAL(saveToCube()), this, SIGNAL(saveToCube()));
    connect(manual, SIGNAL(deleteFromCube()), this, SIGNAL(deleteFromCube()));
    connect(manual, SIGNAL(loadStretch()), this, SIGNAL(loadStretch()));
    p_stretchTypeStack->addWidget(manual);

    layout()->addWidget(p_stretchTypeStack);
    connect(p_stretchTypeSelection, SIGNAL(currentIndexChanged(int)),
            p_stretchTypeStack, SLOT(setCurrentIndex(int)));
    connect(p_stretchTypeSelection, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(stretchChanged()));
  }


  /**
   * Destructor
   */
  AdvancedStretch::~AdvancedStretch() {
  }


  /**
   * This returns the current stretch type's stretch.
   *
   * @return Stretch
   */
  Stretch AdvancedStretch::getStretch() {
    return ((StretchType *)p_stretchTypeStack->currentWidget())->getStretch();
  }


  /**
   * This is called when the user creates a stretch outside of the
   * advanced stretch. For example, they do a global stretch. The
   * advanced stretch will be given this stretch and a chance to
   * re-interpret it.
   *
   * @param newStretch
   */
  void AdvancedStretch::setStretch(Stretch newStretch) {
    StretchType *stretchType = (StretchType *)
                               p_stretchTypeStack->currentWidget();
    stretchType->setStretch(newStretch);
  }


  /**
   * Used to restore a saved Stretch from a cube. This function is 
   * distinct from setStretch in that setStretch deliberately _does not_ 
   * change the stretch type, and this function does change the stretch type.  
   *  
   * @param newStretch saved stretch to restore
   */
  void AdvancedStretch::restoreSavedStretch(Stretch newStretch) {
    QString stretchTypeName = newStretch.getType(); 

    int index = 0;
    if (stretchTypeName.compare("LinearStretch") == 0 ) {
      index = 0; 
    }
    else if (stretchTypeName.compare("SawtoothStretch") == 0 ) {
      index = 1; 
    }
    else if (stretchTypeName.compare("BinaryStretch") == 0) {
      index = 2; 
    }
    else if (stretchTypeName.compare("ManualStretch") == 0) {
      index = 3;
    }
    // Fail by defaulting to Linear

    p_stretchTypeSelection->setCurrentIndex(index);
    StretchType *stretchType = (StretchType *)
                               p_stretchTypeStack->currentWidget();
    stretchType->setStretch(newStretch);
  }


  /**
   * This is called when the visible area changes, so that the
   * histogram can be updated. It is essential that the stretch
   * doesn't really change in this, or zooming will affect the
   * stretch.
   *
   * @param newHist
   */
  void AdvancedStretch::setHistogram(const Histogram &newHist) {
    for(int stretch = 0; stretch < p_stretchTypeStack->count(); stretch ++) {
      StretchType *stretchType = (StretchType *)
                                 p_stretchTypeStack->widget(stretch);
      stretchType->setHistogram(newHist);
    }
  }
};

#include "AdvancedStretch.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "Stretch.h"
#include "iString.h"
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

    QComboBox *stretchTypeSelection = new QComboBox();
    stretchTypeSelection->addItem("Linear",   0);
    stretchTypeSelection->addItem("Sawtooth", 1);
    stretchTypeSelection->addItem("Binary",   2);
    stretchTypeSelection->addItem("Manual",   3);

    typeSelectionArea->layout()->addWidget(stretchTypeSelection);
    layout()->addWidget(typeSelectionArea);

    p_stretchTypeStack = new QStackedWidget();
    LinearStretchType *linear = new LinearStretchType(hist, curStretch,
        name, color);
    connect(linear, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    p_stretchTypeStack->addWidget(linear);

    SawtoothStretchType *sawtooth = new SawtoothStretchType(hist, curStretch,
        name, color);
    connect(sawtooth, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    p_stretchTypeStack->addWidget(sawtooth);

    BinaryStretchType *binary = new BinaryStretchType(hist, curStretch,
        name, color);
    connect(binary, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    p_stretchTypeStack->addWidget(binary);

    ManualStretchType *manual = new ManualStretchType(hist, curStretch,
        name, color);
    connect(manual, SIGNAL(stretchChanged()), this, SIGNAL(stretchChanged()));
    p_stretchTypeStack->addWidget(manual);

    layout()->addWidget(p_stretchTypeStack);
    connect(stretchTypeSelection, SIGNAL(currentIndexChanged(int)),
            p_stretchTypeStack, SLOT(setCurrentIndex(int)));
    connect(stretchTypeSelection, SIGNAL(currentIndexChanged(int)),
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

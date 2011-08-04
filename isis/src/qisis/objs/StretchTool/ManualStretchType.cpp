#include "ManualStretchType.h"

#include <QVBoxLayout>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>

#include "CubeViewport.h"
#include "HistogramWidget.h"
#include "Statistics.h"
#include "Stretch.h"

namespace Isis {
  /**
   * This constructs a manual stretch type.
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  ManualStretchType::ManualStretchType(const Histogram &hist,
                                       const Stretch &stretch,
                                       const QString &name, const QColor &color)
    : StretchType(hist, stretch, name, color) {
    p_inputEdit = NULL;
    p_outputEdit = NULL;

    QWidget *sliderWidget = new QWidget();
    QGridLayout *sliderLayout = new QGridLayout();

    QLabel *inputLabel = new QLabel("Input");
    p_inputEdit = new QLineEdit();

    sliderLayout->addWidget(inputLabel,  0, 0);
    sliderLayout->addWidget(p_inputEdit, 0, 1);

    QLabel *outputLabel = new QLabel("Output");
    p_outputEdit = new QLineEdit();

    sliderLayout->addWidget(outputLabel,  1, 0);
    sliderLayout->addWidget(p_outputEdit, 1, 1);

    QWidget *buttonContainer = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addButton = new QPushButton("Add / Edit");
    connect(addButton, SIGNAL(clicked(bool)),
            this, SLOT(addButtonPressed(bool)));
    buttonLayout->addWidget(addButton);

    QPushButton *deleteButton = new QPushButton("Delete");
    connect(deleteButton, SIGNAL(clicked(bool)),
            this, SLOT(deleteButtonPressed(bool)));
    buttonLayout->addWidget(deleteButton);

    buttonContainer->setLayout(buttonLayout);
    sliderLayout->addWidget(buttonContainer, 2, 0, 1, 2);

    sliderWidget->setLayout(sliderLayout);
    p_mainLayout->addWidget(sliderWidget, 1, 0);

    setLayout(p_mainLayout);
    setStretch(stretch);
  }


  /**
   * Destructor
   */
  ManualStretchType::~ManualStretchType() {
  }


  /**
   * Given an arbitrary stretch, this will re-interpret it, as
   * best as possible, into a manual stretch. It is required that
   * a stretch that represents a manual stretch always translate
   * into itself and does not cause a stretchChanged().
   *
   * It is necessary to always update slider positions in this
   * method even if the stretch did not change.
   *
   * Good thing this is a manual stretch so no interpretation is
   * really needed.
   *
   * @param newStretch Stretch to interpret
   */
  void ManualStretchType::setStretch(Stretch newStretch) {
    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }
  }


  /**
   * This is called when a user clicks "Add / Edit" and is
   * responsible for adding the pair into the correct location or
   * editing the pair at that location (input value).
   */
  void ManualStretchType::addButtonPressed(bool) {
    double input = p_inputEdit->text().toDouble();
    double output = p_outputEdit->text().toDouble();

    Stretch newStretch;
    bool added = false;

    for(int stretchPair = 0;
        stretchPair < p_stretch->Pairs();
        stretchPair ++) {
      if(input < p_stretch->Input(stretchPair)) {
        if(!added) {
          newStretch.AddPair(input, output);
          added = true;
        }

        newStretch.AddPair(p_stretch->Input(stretchPair),
                           p_stretch->Output(stretchPair));
      }
      else if(!added && input == p_stretch->Input(stretchPair)) {
        newStretch.AddPair(input, output);
        added = true;
      }
      else {
        newStretch.AddPair(p_stretch->Input(stretchPair),
                           p_stretch->Output(stretchPair));
      }
    }

    if(!added) {
      newStretch.AddPair(input, output);
      added = true;
    }

    p_stretch->CopyPairs(newStretch);
    emit stretchChanged();
  }


  /**
   * This is called when a user clicks "Delete" and is
   * responsible for removing the pair with the given input value.
   */
  void ManualStretchType::deleteButtonPressed(bool) {
    double input = p_inputEdit->text().toDouble();

    Stretch newStretch;

    for(int stretchPair = 0;
        stretchPair < p_stretch->Pairs();
        stretchPair ++) {
      if(input == p_stretch->Input(stretchPair)) {
        continue;
      }
      else {
        newStretch.AddPair(p_stretch->Input(stretchPair),
                           p_stretch->Output(stretchPair));
      }
    }

    p_stretch->CopyPairs(newStretch);
    emit stretchChanged();
  }
}

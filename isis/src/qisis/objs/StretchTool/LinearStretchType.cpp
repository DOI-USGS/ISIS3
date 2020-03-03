#include "LinearStretchType.h"

#include <QVBoxLayout>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>

#include "CubeViewport.h"
#include "HistogramWidget.h"
#include "Statistics.h"
#include "Stretch.h"

namespace Isis {
  /**
   * This constructs a linear stretch type.
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  LinearStretchType::LinearStretchType(const Histogram &hist,
                                       const Stretch &stretch,
                                       const QString &name, const QColor &color)
    : StretchType(hist, stretch, name, color) {
    p_startSlider = NULL;
    p_endSlider = NULL;
    p_startEdit = NULL;
    p_endEdit = NULL;
    p_sliderOverride = false;
    p_editOverride = false;

    QWidget *sliderWidget = new QWidget();
    QGridLayout *sliderLayout = new QGridLayout();
    sliderLayout->setColumnStretch(1, 10);

    QLabel *minHistLabel = new QLabel("Min DN");
    p_startSlider = new QSlider(Qt::Horizontal);
    p_startSlider->setTickPosition(QSlider::NoTicks);
    p_startSlider->setMinimum(0);
    p_startSlider->setMaximum(1000);
    p_startSlider->setPageStep(50);
    connect(p_startSlider, SIGNAL(valueChanged(int)),
            this, SLOT(startSliderMoved(int)));
    p_startEdit = new QLineEdit();
    p_startEdit->setMaximumWidth(75);
    connect(p_startEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(startEditChanged(const QString &)));
    sliderLayout->addWidget(minHistLabel,  0, 0);
    sliderLayout->addWidget(p_startSlider, 0, 1);
    sliderLayout->addWidget(p_startEdit,   0, 2);

    QLabel *maxHistLabel = new QLabel("Max DN");
    p_endSlider = new QSlider(Qt::Horizontal);
    p_endSlider->setTickPosition(QSlider::NoTicks);
    p_endSlider->setMinimum(0);
    p_endSlider->setMaximum(1000);
    p_endSlider->setValue(1000);
    p_endSlider->setPageStep(50);
    connect(p_endSlider, SIGNAL(valueChanged(int)),
            this, SLOT(endSliderMoved(int)));
    p_endEdit = new QLineEdit();
    p_endEdit->setMaximumWidth(75);
    connect(p_endEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(endEditChanged(const QString &)));
    sliderLayout->addWidget(maxHistLabel,  1, 0);
    sliderLayout->addWidget(p_endSlider, 1, 1);
    sliderLayout->addWidget(p_endEdit,   1, 2);

    sliderWidget->setLayout(sliderLayout);
    p_mainLayout->addWidget(sliderWidget, 1, 0);

    p_stretch->setType("LinearStretch");
    setLayout(p_mainLayout);

    setStretch(stretch);
  }


  /**
   * Destructor
   */
  LinearStretchType::~LinearStretchType() {
  }


  /**
   * Given an arbitrary stretch, this will re-interpret it, as
   * best as possible, into a linear stretch. It is required that
   * a stretch that represents a linear stretch always translate
   * into itself and does not cause a stretchChanged().
   *
   * It is necessary to always update slider positions in this
   * method even if the stretch did not change.
   *
   * @param newStretch Stretch to interpret
   */
  void LinearStretchType::setStretch(Stretch newStretch) {
    Stretch interpretted;

    if(newStretch.Pairs() >= 2) {
      double inMin = newStretch.Input(0);
      double inMax = newStretch.Input(1);

      if(inMax < inMin) {
        double tmp = inMax;
        inMax = inMin;
        inMin = tmp;
      }

      interpretted.AddPair(inMin, 0);
      interpretted.AddPair(inMax, 255);
    }
    else {
      double inMin = p_cubeHist->BestMinimum();
      double inMax = p_cubeHist->BestMaximum();

      if(inMin >= inMax) {
        inMin -= p_cubeHist->BinSize();
        inMax += p_cubeHist->BinSize();
      }

      // Handles case where the histogram bin size=0
      if (inMin == inMax) {
        inMin -= 1.0;
        inMax += 1.0;
      }

      interpretted.AddPair(inMin, 0);
      interpretted.AddPair(inMax, 255);
    }

    bool changed = (interpretted.Text() != p_stretch->Text());

    p_editOverride = true;

    if(changed) {
      p_stretch->CopyPairs(interpretted);
      p_startEdit->setText(QString::number(p_stretch->Input(0)));
      p_endEdit->setText(QString::number(p_stretch->Input(1)));
    }

    // regardless of it all, slider positions could need changed...
    startEditChanged(QString());
    endEditChanged(QString());
    p_editOverride = false;

    if(changed) {
      emit stretchChanged();
    }
  }


  /**
   * This is called when the start point slider moves. It ensures
   * a valid state and updates the stretch pairs.
   */
  void LinearStretchType::startSliderMoved(int) {
    if(p_sliderOverride)
      return;

    if(p_startSlider->value() >= p_endSlider->value()) {
      p_startSlider->setValue(p_endSlider->value() - 1);
      return;
    }

    double value = p_cubeHist->Minimum();
    value += p_startSlider->value() *
             (p_cubeHist->Maximum() - p_cubeHist->Minimum()) / 1000.0;
    p_startEdit->setText(QString::number(value));
  }


  /**
   * A new start point was typed in. Update the slider and
   * calculate a new stretch.
   */
  void LinearStretchType::startEditChanged(const QString &) {
    double value = p_startEdit->text().toDouble();

    if(value >= p_endEdit->text().toDouble()) {
      return;
    }

    double percentage = value - p_cubeHist->Minimum();
    percentage /= (p_cubeHist->Maximum() - p_cubeHist->Minimum());
    int valuePos = (int)(percentage * 1000.0);

    p_sliderOverride = true;
    p_startSlider->setValue(valuePos);
    p_sliderOverride = false;

    if(p_editOverride) return;

    Stretch newStretch;
    newStretch.AddPair(value, 0);
    newStretch.AddPair(p_stretch->Input(1), 255);

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }
  }


  /**
   * This is called when the end point slider moves. It ensures
   * a valid state and updates the stretch pairs.
   */
  void LinearStretchType::endSliderMoved(int) {
    if(p_sliderOverride)
      return;

    if(p_endSlider->value() <= p_startSlider->value()) {
      p_endSlider->setValue(p_startSlider->value() + 1);
      return;
    }

    double value = p_cubeHist->Minimum();
    value += p_endSlider->value() *
             (p_cubeHist->Maximum() - p_cubeHist->Minimum()) / 1000.0;
    p_endEdit->setText(QString::number(value));
  }


  /**
   * A new end point was typed in. Update the slider and
   * calculate a new stretch.
   */
  void LinearStretchType::endEditChanged(const QString &) {
    double value = p_endEdit->text().toDouble();

    if(value <= p_startEdit->text().toDouble()) {
      return;
    }

    double percentage = value - p_cubeHist->Minimum();
    percentage /= (p_cubeHist->Maximum() - p_cubeHist->Minimum());
    int valuePos = (int)(percentage * 1000.0);

    p_sliderOverride = true;
    p_endSlider->setValue(valuePos);
    p_sliderOverride = false;

    if(p_editOverride) return;

    Stretch newStretch;
    newStretch.AddPair(p_stretch->Input(0), 0);
    newStretch.AddPair(value, 255);

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }
  }

  Stretch LinearStretchType::getStretch() {
    return *p_stretch;
  }

}

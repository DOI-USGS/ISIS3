#include "BinaryStretchType.h"

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
   * This constructs a binary stretch type.
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  BinaryStretchType::BinaryStretchType(const Histogram &hist,
                                       const Stretch &stretch,
                                       const QString &name, const QColor &color)
    : StretchType(hist, stretch, name, color) {
    p_startSlider = NULL;
    p_startEdit = NULL;
    p_endSlider = NULL;
    p_endEdit = NULL;
    p_sliderOverride = false;
    p_editOverride = false;

    QWidget *sliderWidget = new QWidget();
    QGridLayout *sliderLayout = new QGridLayout();
    sliderLayout->setColumnStretch(1, 10);

    QLabel *startLabel = new QLabel("Start");
    p_startSlider = new QSlider(Qt::Horizontal);
    p_startSlider->setTickPosition(QSlider::NoTicks);
    p_startSlider->setMinimum(0);
    p_startSlider->setMaximum(1000);
    p_startSlider->setPageStep(50);
    connect(p_startSlider, SIGNAL(valueChanged(int)),
            this, SLOT(startSliderMoved(int)));
    p_startEdit = new QLineEdit();
    p_startEdit->setMaximumWidth(75);
    p_startEdit->setText(QString::number(p_cubeHist->Percent(25)));
    connect(p_startEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(startEditChanged(const QString &)));
    sliderLayout->addWidget(startLabel,  0, 0);
    sliderLayout->addWidget(p_startSlider, 0, 1);
    sliderLayout->addWidget(p_startEdit,   0, 2);

    QLabel *endLabel = new QLabel("End");
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
    p_endEdit->setText(QString::number(p_cubeHist->Percent(75)));
    connect(p_endEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(endEditChanged(const QString &)));
    sliderLayout->addWidget(endLabel,  1, 0);
    sliderLayout->addWidget(p_endSlider, 1, 1);
    sliderLayout->addWidget(p_endEdit,   1, 2);

    sliderWidget->setLayout(sliderLayout);
    p_mainLayout->addWidget(sliderWidget, 1, 0);

    p_stretch->setType("BinaryStretch"); 

    setLayout(p_mainLayout);
    setStretch(calculateNewStretch());
  }


  /**
   * Destroys the binary stretch
   */
  BinaryStretchType::~BinaryStretchType() {
  }


  /**
   * Given an arbitrary stretch, this will re-interpret it, as
   * best as possible, into a binary stretch. It is required that
   * a stretch that represents a binary stretch always translate
   * into itself and does not cause a stretchChanged().
   *
   * It is necessary to always update slider positions in this
   * method even if the stretch did not change.
   *
   * @param newStretch Stretch to interpret
   */
  void BinaryStretchType::setStretch(Stretch newStretch) {
    double epsilon = p_cubeHist->BinSize();

    Stretch interpretted;

    double switch1 = 0.0;
    double switch2 = 1.0;

    if(newStretch.Pairs() == 2) {
      // given a flat line?
      if(newStretch.Output(0) == newStretch.Output(1)) {
        // assume its binary all white
        interpretted.AddPair(p_cubeHist->Minimum(), 255);
        interpretted.AddPair(p_cubeHist->Maximum(), 255);
      }
      else {
        // probably linear, figure out something reasonable
        interpretted.AddPair(p_cubeHist->Minimum(), 0);

        switch1 = newStretch.Input(0);
        if(switch1 < p_cubeHist->Minimum())
          switch1 = p_cubeHist->Minimum() + epsilon;

        interpretted.AddPair(switch1, 0);
        interpretted.AddPair(switch1 + epsilon, 255);

        switch2 = newStretch.Input(1);
        if(switch2 <= switch1 + epsilon)
          switch2 = switch1 + epsilon + epsilon;

        interpretted.AddPair(switch2, 255);
        interpretted.AddPair(switch2 + epsilon, 0);

        double end = p_cubeHist->Maximum();
        if(end <= switch2 + epsilon)
          end = switch2 + epsilon + epsilon;

        interpretted.AddPair(end, 0);
      }
    }
    else if(newStretch.Pairs() == 4) {
      if(newStretch.Output(0) > 127) {
        interpretted.AddPair(p_cubeHist->Minimum(), 255);

        switch1 = newStretch.Input(1);
        if(switch1 <= p_cubeHist->Minimum())
          switch1 = p_cubeHist->Minimum() + epsilon;

        interpretted.AddPair(switch1, 255);
        interpretted.AddPair(switch1 + epsilon, 0);

        double end = p_cubeHist->Maximum();
        if(end <= switch1 + epsilon)
          end = switch1 + epsilon + epsilon;

        switch2 = end;

        interpretted.AddPair(end, 0);
      }
      else {
        interpretted.AddPair(p_cubeHist->Minimum(), 0);

        switch1 = newStretch.Input(1);
        if(switch1 < p_cubeHist->Minimum())
          switch1 = p_cubeHist->Minimum() + epsilon;

        interpretted.AddPair(switch1, 0);
        interpretted.AddPair(switch1 + epsilon, 255);

        double end = p_cubeHist->Maximum();
        if(end <= switch1 + epsilon)
          end = switch1 + epsilon + epsilon;
        switch2 = end;

        interpretted.AddPair(end, 255);
      }
    }
    // 6 pairs means the 255 values are in the middle (typical)
    else if(newStretch.Pairs() == 6) {
      interpretted.AddPair(p_cubeHist->Minimum(), 0);

      switch1 = newStretch.Input(1);
      if(switch1 <= p_cubeHist->Minimum())
        switch1 = p_cubeHist->Minimum() + epsilon;

      interpretted.AddPair(switch1, 0);
      interpretted.AddPair(switch1 + epsilon, 255);


      switch2 = newStretch.Input(3);
      if(switch2 <= switch1 + epsilon)
        switch2 = switch1 + epsilon + epsilon;

      interpretted.AddPair(switch2, 255);
      interpretted.AddPair(switch2 + epsilon, 0);

      double end = p_cubeHist->Maximum();
      if(end <= switch2 + epsilon)
        end = switch2 + epsilon + epsilon;

      interpretted.AddPair(end, 0);
    }

    if(!interpretted.Pairs()) {
      interpretted.CopyPairs(calculateNewStretch());
      switch1 = p_startEdit->text().toDouble();
      switch2 = p_endEdit->text().toDouble();
    }

    bool changed = (interpretted.Text() != p_stretch->Text());

    p_editOverride = true;
    if(changed) {
      p_stretch->CopyPairs(interpretted);
      p_startEdit->setText(QString::number(switch1));
      p_endEdit->setText(QString::number(switch2));
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
  void BinaryStretchType::startSliderMoved(int) {
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
  void BinaryStretchType::startEditChanged(const QString &) {
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

    Stretch newStretch = calculateNewStretch();

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }
  }


  /**
   * This is called when the end point slider moves. It ensures
   * a valid state and updates the stretch pairs.
   */
  void BinaryStretchType::endSliderMoved(int) {
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
   * A new end point was typed in. Update the slider and calclate
   * a new stretch.
   */
  void BinaryStretchType::endEditChanged(const QString &) {
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

    Stretch newStretch = calculateNewStretch();

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);;
      emit stretchChanged();
    }
  }


  /**
   * This uses the GUI elements to calculate the current binary
   * stretch.
   *
   * @return Stretch
   */
  Stretch BinaryStretchType::calculateNewStretch() {
    double epsilon = p_cubeHist->BinSize();

    if(epsilon == 0) {
      epsilon = (p_cubeHist->Maximum() - p_cubeHist->Minimum()) / 65536;
    }

    Stretch newStretch;

    if(epsilon == 0) {
      return newStretch;
    }
    // Start high?
    double startPt = p_startEdit->text().toDouble();
    if(fabs(startPt - p_cubeHist->Minimum()) < epsilon ||
        startPt <= p_cubeHist->Minimum()) {
      newStretch.AddPair(p_cubeHist->Minimum(), 255);
      startPt = p_cubeHist->Minimum() - epsilon;
    }
    else {
      newStretch.AddPair(p_cubeHist->Minimum(), 0);
      newStretch.AddPair(startPt, 0);
      newStretch.AddPair(startPt + epsilon, 255);
    }

    // End high?
    double endPt = p_endEdit->text().toDouble();
    if(endPt <= startPt + epsilon) {
      endPt = startPt + 2 * epsilon;
    }

    if(fabs(endPt + epsilon - p_cubeHist->Maximum()) < epsilon ||
        endPt + epsilon >= p_cubeHist->Maximum()) {
      newStretch.AddPair(p_cubeHist->Maximum(), 255);
    }
    else {
      newStretch.AddPair(endPt, 255);
      newStretch.AddPair(endPt + epsilon, 0);
      newStretch.AddPair(p_cubeHist->Maximum(), 0);
    }

    return newStretch;
  }
}

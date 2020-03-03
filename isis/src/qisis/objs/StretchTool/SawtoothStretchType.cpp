#include "SawtoothStretchType.h"

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
   * This initializes a sawtooth stretch type
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  SawtoothStretchType::SawtoothStretchType(const Histogram &hist,
      const Stretch &stretch, const QString &name, const QColor &color) :
    StretchType(hist, stretch, name, color) {
    p_offsetSlider = NULL;
    p_widthSlider = NULL;
    p_offsetEdit = NULL;
    p_widthEdit = NULL;
    p_sliderOverride = false;

    QWidget *sliderWidget = new QWidget();
    QGridLayout *sliderLayout = new QGridLayout();
    sliderLayout->setColumnStretch(1, 10);

    QLabel *startLabel = new QLabel("Offset");
    p_offsetSlider = new QSlider(Qt::Horizontal);
    p_offsetSlider->setTickPosition(QSlider::NoTicks);
    p_offsetSlider->setMinimum(0);
    p_offsetSlider->setMaximum(1000);
    p_offsetSlider->setPageStep(50);
    connect(p_offsetSlider, SIGNAL(valueChanged(int)),
            this, SLOT(offsetSliderMoved(int)));
    p_offsetEdit = new QLineEdit();
    p_offsetEdit->setMaximumWidth(75);
    p_offsetEdit->setText(QString::number(
                            p_cubeHist->Maximum() - p_cubeHist->Minimum()
                          ));
    connect(p_offsetEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(offsetEditChanged(const QString &)));
    sliderLayout->addWidget(startLabel,  0, 0);
    sliderLayout->addWidget(p_offsetSlider, 0, 1);
    sliderLayout->addWidget(p_offsetEdit,   0, 2);

    QLabel *widthLabel = new QLabel("Width");
    p_widthSlider = new QSlider(Qt::Horizontal);
    p_widthSlider->setTickPosition(QSlider::NoTicks);
    p_widthSlider->setMinimum(0);
    p_widthSlider->setMaximum(1000);
    p_widthSlider->setPageStep(50);
    connect(p_widthSlider, SIGNAL(valueChanged(int)),
            this, SLOT(widthSliderMoved(int)));
    p_widthEdit = new QLineEdit();
    p_widthEdit->setMaximumWidth(75);
    connect(p_widthEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(widthEditChanged(const QString &)));
    sliderLayout->addWidget(widthLabel,  1, 0);
    sliderLayout->addWidget(p_widthSlider, 1, 1);
    sliderLayout->addWidget(p_widthEdit,   1, 2);

    sliderWidget->setLayout(sliderLayout);
    p_mainLayout->addWidget(sliderWidget, 1, 0);

    p_stretch->setType("SawtoothStretch"); 

    setLayout(p_mainLayout);

    p_widthEdit->setText(QString::number(
                           p_cubeHist->Median() - p_cubeHist->Minimum()
                         ));
    setStretch(calculateNewStretch());
  }


  /**
   * Destructor
   */
  SawtoothStretchType::~SawtoothStretchType() {
  }


  /**
   * Given an arbitrary stretch, this will re-interpret it, as
   * best as possible, into a sawtooth stretch. It is required
   * that a stretch that represents a sawtooth stretch always
   * translate into itself and does not cause a stretchChanged().
   * To ensure this the sliders have been restricted to not be
   * ambiguous.
   *
   * It is necessary to always update slider positions in this
   * method even if the stretch did not change.
   *
   * @param newStretch Stretch to interpret
   */
  void SawtoothStretchType::setStretch(const Stretch newStretch) {
    Stretch interpretted;
    double offset = 0.0;
    double width = 0.0;
    bool changed = false;

    // Too few pairs to do a saw tooth with
    if(newStretch.Pairs() < 3) {
      return;
    }
    else {
      // find an offset... should be the second or third point
      offset = newStretch.Input(1);

      // offset should always be a low point
      if(newStretch.Output(1) > 127)
        offset = newStretch.Input(2);

      width = (newStretch.Input(2) - newStretch.Input(0)) / 2;

      interpretted.CopyPairs(calculateNewStretch(offset, width));

      double deltaOffset = fabs(p_offsetEdit->text().toDouble() - offset);
      changed = (changed || (deltaOffset > p_cubeHist->BinSize()));

      double deltaWidth = fabs(p_widthEdit->text().toDouble() - width);
      changed = (changed || (deltaWidth > p_cubeHist->BinSize()));
    }

    if(changed) {
      p_stretch->CopyPairs(interpretted);
      p_offsetEdit->setText(QString::number(offset));
      p_widthEdit->setText(QString::number(width));
    }

    // regardless of it all, slider positions could need changed...
    offsetEditChanged(QString());
    widthEditChanged(QString());

    if(changed) {
      emit stretchChanged();
    }
  }


  /**
   * This is called when the sawtooth offset slider moves. It
   * updates the edit.
   */
  void SawtoothStretchType::offsetSliderMoved(int) {
    if(p_offsetSlider->value() >= p_widthSlider->value())
      p_offsetSlider->setValue(p_widthSlider->value() - 1);

    if(p_sliderOverride)
      return;

    double value = p_cubeHist->Minimum();
    value += p_offsetSlider->value() * 2 *
             (p_cubeHist->Maximum() - p_cubeHist->Minimum()) / 1000.0;
    p_offsetEdit->setText(QString::number(value));
  }


  /**
   * This is called when the the sawtooth offset edit changes. It
   * updates the slider (while blocking the slider from calling
   * this method again) and calculates a new stretch.
   */
  void SawtoothStretchType::offsetEditChanged(const QString &) {
    double value = p_offsetEdit->text().toDouble();

    double percentage = value - p_cubeHist->Minimum();
    percentage /= (p_cubeHist->Maximum() - p_cubeHist->Minimum()) * 2;
    int valuePos = (int)(percentage * 1000.0);

    p_sliderOverride = true;
    p_offsetSlider->setValue(valuePos);
    p_sliderOverride = false;

    Stretch newStretch = calculateNewStretch();

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }

  }


  /**
   * This is called when the sawtooth width slider moves. It
   * updates the edit.
   */
  void SawtoothStretchType::widthSliderMoved(int) {
    if(p_widthSlider->value() <= p_offsetSlider->value())
      p_widthSlider->setValue(p_offsetSlider->value() + 1);

    if(p_sliderOverride)
      return;

    double highVal = p_cubeHist->Maximum() - p_cubeHist->Minimum();
    double lowVal = p_cubeHist->BinSize();
    double value = lowVal + p_widthSlider->value() *
                   (highVal - lowVal) / 1000.0;
    p_widthEdit->setText(QString::number(value));
  }


  /**
   * This is called when the the sawtooth width edit changes. It
   * updates the slider (while blocking the slider from calling
   * this method again) and calculates a new stretch.
   */
  void SawtoothStretchType::widthEditChanged(const QString &) {
    double value = p_widthEdit->text().toDouble();

    double highVal = p_cubeHist->Maximum() - p_cubeHist->Minimum();
    double lowVal = p_cubeHist->BinSize();

    double percentage = value - lowVal;
    percentage /= (highVal - lowVal);
    int valuePos = (int)(percentage * 1000.0);

    p_sliderOverride = true;
    p_widthSlider->setValue(valuePos);
    p_sliderOverride = false;

    Stretch newStretch = calculateNewStretch();

    if(newStretch.Text() != p_stretch->Text()) {
      p_stretch->CopyPairs(newStretch);
      emit stretchChanged();
    }
  }


  /**
   * This uses the parameters offset, width to create a sawtooth
   * stretch.
   *
   * @param offset A pair mapping (offset, 0) will be created (or
   *               would if the histogram had the appropriate
   *               range)
   * @param width How wide the sawtooth is
   *
   * @return Stretch
   */
  Stretch SawtoothStretchType::calculateNewStretch(double offset,
      double width) {
    Stretch stretch;
    width = fabs(width);

    if(width < p_cubeHist->BinSize())
      width = p_cubeHist->BinSize();

    // Still can't do it? Give up
    if(width <= 0) return Stretch();

    bool high = false;

    // We want leftPoint to be our starting point for the sawtooth, one left of
    //  the minimum
    double leftPoint = offset;

    // If leftPoint is too far left, move it right
    while(leftPoint < p_cubeHist->Minimum() - width) {
      leftPoint += width;
      high = !high;
    }

    // If leftPoint is too far fight, move it left
    while(leftPoint >= p_cubeHist->Minimum()) {
      leftPoint -= width;
      high = !high;
    }

    double currPoint = leftPoint;

    bool terminated = false;
    while(!terminated) {
      int outValue = (high) ? 255 : 0;

      // This conversion to a string & back prevents infinite loops due to
      //   rounding errors and disagreements betweeen double to string
      //   conversions later on (in setStretch)
      stretch.AddPair(QString::number(currPoint).toDouble(), outValue);

      if(currPoint > p_cubeHist->Maximum()) {
        terminated = true;
      }

      high = !high;
      currPoint += width;
    }

    return stretch;
  }


  /**
   * This calculates a new stretch using the GUI edits.
   *
   * @return Stretch
   */
  Stretch SawtoothStretchType::calculateNewStretch() {
    return calculateNewStretch(p_offsetEdit->text().toDouble(),
                               p_widthEdit->text().toDouble());
  }
}


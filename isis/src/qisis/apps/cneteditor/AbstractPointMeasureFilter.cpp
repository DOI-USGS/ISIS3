#include "IsisDebug.h"

#include "AbstractPointMeasureFilter.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  AbstractPointMeasureFilter::AbstractPointMeasureFilter(
      int minimumForImageSuccess) : AbstractFilter(minimumForImageSuccess)
  {
    cerr << "AbstractPointMeasureFilter: " << minimumForImageSuccess << "\n";
  }


  AbstractPointMeasureFilter::~AbstractPointMeasureFilter()
  {
  }


  void AbstractPointMeasureFilter::createWidget()
  {
    AbstractFilter::createWidget();
    
    QLabel * label = new QLabel("Effect: ");
    label->setFont(QFont("SansSerif", 10, QFont::DemiBold));

    QComboBox * combo = new QComboBox;
    combo->addItem("points only");
    combo->addItem("measuresOnly");
    combo->addItem("both");
    connect(combo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(changeEffectiveness(int)));
        
    // FIXME: QSettings should handle this
    combo->setCurrentIndex(0);
    
    mainLayout->addWidget(label);
    mainLayout->addWidget(combo);
  }


  void AbstractPointMeasureFilter::changeEffectiveness(int button)
  {
    effectiveness = (Effectiveness) button;
    emit filterChanged();
  }
}

#include "IsisDebug.h"

#include <iostream>
#include <limits>

#include "AbstractFilter.h"

#include <QAction>
#include <QButtonGroup>
#include <QFlags>
#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QRadioButton>
#include <QReadWriteLock>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QWriteLocker>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"

using std::cerr;


namespace Isis
{
  AbstractFilter::AbstractFilter(FilterEffectivenessFlag effectiveness,
      int minimumForImageSuccess) : minForImageSuccess(minimumForImageSuccess)
  {
    lock = NULL;
    effectivenessFlags = NULL;
    
    lock = new QReadWriteLock;
    effectivenessFlags = new FilterEffectivenessFlag(effectiveness);
  }


  AbstractFilter::~AbstractFilter()
  {
    if (effectivenessFlags)
    {
      delete effectivenessFlags;
      effectivenessFlags = NULL;
    }
  }
  
  
  bool AbstractFilter::canFilterImages() const
  {
    return effectivenessFlags->testFlag(Images);
  }
  
  
  bool AbstractFilter::canFilterPoints() const
  {
    return effectivenessFlags->testFlag(Points);
  }
  
  
  bool AbstractFilter::canFilterMeasures() const
  {
    return effectivenessFlags->testFlag(Measures);
  }

  
  bool AbstractFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    bool evaluation = true;
//     
//     if (canFilterImages())
//     {
//       int passedMeasures = 0;
//       int passedPoints = 0;
//       
//       QList< ControlMeasure * > measures = node->getMeasures();
//       foreach (ControlMeasure * measure, measures)
//       {
//         ASSERT(measure);
//         if (measure && evaluate(measure))
//           passedMeasures++;
//         
//         ControlPoint * point = measure->Parent();
//         ASSERT(point);
//         if (point && evaluate(point))
//           passedPoints++;
//       }
//       
//       bool pointsPass = true;
//       bool measuresPass = true;
//       
//       if (effectivenessFlags->testFlag(Points))
//       {
//         pointsPass = passedPoints >= getMinForImageSuccess();
//       }
//       
//       if (effectiveness != AbstractPointMeasureFilter::PointsOnly)
//       {
//         measuresPass = passedMeasures >= getMinForImageSuccess();
//       }
//     
//       evaluation = pointsPass && measuresPass;
//     }
//     
    return evaluation;
  }
  
  
  QString AbstractFilter::getImageDescription() const
  {
    return "have at least " + QString::number(getMinForImageSuccess()) + " ";
  }

  
  QString AbstractFilter::getPointDescription() const { return QString(); }


  QString AbstractFilter::getMeasureDescription() const { return QString(); }
  
  
  void AbstractFilter::nullify()
  {
    effectivenessMenu = NULL;
    inclusiveExclusiveGroup = NULL;
    mainLayout = NULL;
  }


  void AbstractFilter::createWidget()
  {
    QRadioButton * inclusiveButton = new QRadioButton("Inclusive");
    QRadioButton * exclusiveButton = new QRadioButton("Exclusive");

    inclusiveExclusiveGroup = new QButtonGroup;
    connect(inclusiveExclusiveGroup, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    inclusiveExclusiveGroup->addButton(inclusiveButton, 0);
    inclusiveExclusiveGroup->addButton(exclusiveButton, 1);
    
    QVBoxLayout * inclusiveExclusiveLayout = new QVBoxLayout;
    inclusiveExclusiveLayout->addWidget(inclusiveButton);
    inclusiveExclusiveLayout->addWidget(exclusiveButton);
    
    mainLayout = new QHBoxLayout;
    QMargins margins = mainLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    mainLayout->setContentsMargins(margins);
    mainLayout->addLayout(inclusiveExclusiveLayout);
    
    effectivenessMenu = new QMenu("Effectiveness");
    
    if (effectivenessFlags->testFlag(Images))
    {
      effectivenessMenu->addAction(createEffectivenessAction("&Images"));
      
      QLabel * label = new QLabel("Min Count");
      QSpinBox * spinBox = new QSpinBox;
      spinBox->setRange(1, std::numeric_limits< int >::max());
      spinBox->setValue(1);  // FIXME: QSettings should handle this
      connect(spinBox, SIGNAL(valueChanged(int)),
              this, SLOT(updateMinForImageSuccess(int)));
      QVBoxLayout * minLayout = new QVBoxLayout;
      minLayout->addWidget(label);
      minLayout->addWidget(spinBox);
      
      mainLayout->addLayout(minLayout);
    }
    
    if (effectivenessFlags->testFlag(Points))
      effectivenessMenu->addAction(createEffectivenessAction("&Points"));
    
    if (effectivenessFlags->testFlag(Measures))
      effectivenessMenu->addAction(createEffectivenessAction("&Measures"));
    
    if (effectivenessMenu->actions().size() > 1)
    {
      QLabel * label = new QLabel("Effect");
      label->setFont(QFont("SansSerif", 10, QFont::DemiBold));
      
      QMenuBar * bar = new QMenuBar;
      bar->addMenu(effectivenessMenu);
      
      QVBoxLayout * effectivenessLayout = new QVBoxLayout;
//       effectivenessLayout->addWidget(label);
      effectivenessLayout->addWidget(bar);

      mainLayout->addLayout(effectivenessLayout);
    }
    else
    {
      delete effectivenessMenu;
      effectivenessMenu = NULL;
    }

    setLayout(mainLayout);

    // FIXME: QSettings should handle this
    inclusiveButton->click();
  }
  
  
  QAction * AbstractFilter::createEffectivenessAction(QString text)
  {
    QAction * act = new QAction(text, this);
    act->setCheckable(true);
    connect(act, SIGNAL(toggled(bool)), this, SLOT(updateEffectiveness()));
    return act;
  }


  bool AbstractFilter::inclusive() const
  {
    return inclusiveExclusiveGroup->checkedId() == 0;
  }
  
  
  AbstractFilter::FilterEffectivenessFlag *
      AbstractFilter::getEffectivenessFlags() const
  {
    return effectivenessFlags;
  }
  
  
  bool AbstractFilter::evaluate(const ControlPoint * point,
      bool (ControlPoint::*meth)() const) const
  {
    bool evaluation = true;
    
    if (canFilterPoints())
      evaluation = !((point->*meth)() ^ inclusive());
      
    return evaluation;
  }
  
  
  bool AbstractFilter::evaluate(const ControlMeasure * measure,
      bool (ControlMeasure::*meth)() const) const
  {
    bool evaluation = true;
    
    if (canFilterMeasures())
      evaluation = !((measure->*meth)() ^ inclusive());
    
    return evaluation;
  }
  
  
  void AbstractFilter::updateEffectiveness()
  {
    ASSERT(effectivenessMenu);
    
    if (effectivenessMenu)
    {
      FilterEffectivenessFlag newFlags;
      
      QWriteLocker locker(lock);
      QList< QAction * > actions = effectivenessMenu->actions();
      
      for (int i = 0; i < actions.size(); i++)
      {
        if (actions[i]->isChecked())
        {
          if (actions[i]->text() == "Images")
            newFlags |= Images;
          else
            if (actions[i]->text() == "Points")
              newFlags |= Points;
            else
              if (actions[i]->text() == "Measures")
                newFlags |= Measures;
              else
                ASSERT(0);
        }
      }
      
      *effectivenessFlags = newFlags;
    }
  }


  void AbstractFilter::updateMinForImageSuccess(int newMin)
  {
    QWriteLocker locker(lock);
    minForImageSuccess = newMin;
    locker.unlock();
    emit filterChanged();
  }
}

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

#include "AbstractFilterSelector.h"
#include <ControlPoint.h>

using std::cerr;


namespace Isis
{
  AbstractFilter::AbstractFilter(FilterEffectivenessFlag effectiveness,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      minForSuccess(minimumForSuccess)
  {
    parentSelector = parent;
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

  
  QString AbstractFilter::getImageDescription() const
  {
    return "have at least " + QString::number(getMinForSuccess()) + " ";
  }
  
  
  QString AbstractFilter::getPointDescription() const { return QString(); }
  
  
  QString AbstractFilter::getMeasureDescription() const { return QString(); }
  
  
  void AbstractFilter::nullify()
  {
    effectivenessMenu = NULL;
    inclusiveExclusiveGroup = NULL;
    inclusiveExclusiveLayout = NULL;
    mainLayout = NULL;
    minWidget = NULL;
  }
  
  
  void AbstractFilter::createWidget()
  {
    QFont inclusiveExclusiveFont("SansSerif", 9);
    QRadioButton * inclusiveButton = new QRadioButton("Inclusive");
    inclusiveButton->setFont(inclusiveExclusiveFont);
    QRadioButton * exclusiveButton = new QRadioButton("Exclusive");
    exclusiveButton->setFont(inclusiveExclusiveFont);

    inclusiveExclusiveGroup = new QButtonGroup;
    connect(inclusiveExclusiveGroup, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    inclusiveExclusiveGroup->addButton(inclusiveButton, 0);
    inclusiveExclusiveGroup->addButton(exclusiveButton, 1);
    
    inclusiveExclusiveLayout = new QHBoxLayout;
    QMargins margins = inclusiveExclusiveLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    inclusiveExclusiveLayout->setContentsMargins(margins);
    inclusiveExclusiveLayout->addWidget(inclusiveButton);
    inclusiveExclusiveLayout->addWidget(exclusiveButton);
    
    QHBoxLayout * controlsLayout = new QHBoxLayout;
    margins = controlsLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    controlsLayout->setContentsMargins(margins);

    controlsLayout->addLayout(inclusiveExclusiveLayout);

    
    effectivenessMenu = new QMenu("Effect");
    connect(effectivenessMenu, SIGNAL(aboutToHide()),
            this, SLOT(showHideEffectivenessMenu()));
    
    if (effectivenessFlags->testFlag(Images))
      effectivenessMenu->addAction(createEffectivenessAction("&Images"));
    
    if (effectivenessFlags->testFlag(Points))
      effectivenessMenu->addAction(createEffectivenessAction("&Points"));
    
    if (effectivenessFlags->testFlag(Measures))
      effectivenessMenu->addAction(createEffectivenessAction("&Measures"));

    QString firstMenuEntry;
    ASSERT(effectivenessMenu->actions().size());
    if (effectivenessMenu->actions().size())
    {
      firstMenuEntry = effectivenessMenu->actions()[0]->text();
      firstMenuEntry.remove(0, 1);
    }
    
    if (effectivenessMenu->actions().size() >= 2)
    {
      QMenuBar * bar = new QMenuBar;
      bar->addMenu(effectivenessMenu);
      controlsLayout->addWidget(bar);
      controlsLayout->setAlignment(bar, Qt::AlignTop);
    }
    else
    {
      delete effectivenessMenu;
      effectivenessMenu = NULL;
    }

    if (minForSuccess != -1)
    {
      QLabel * label = new QLabel;
      label->setText("<span>Min Count<br/>for " + firstMenuEntry + "</span>");
      label->setFont(QFont("SansSerif", 7));
      QSpinBox * spinBox = new QSpinBox;
      spinBox->setRange(1, std::numeric_limits< int >::max());
      spinBox->setValue(1);  // FIXME: QSettings should handle this
      connect(spinBox, SIGNAL(valueChanged(int)),
              this, SLOT(updateMinForImageSuccess(int)));
      QHBoxLayout * minLayout = new QHBoxLayout;
      margins = minLayout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      minLayout->setContentsMargins(margins);
      minLayout->addWidget(label);
      minLayout->addWidget(spinBox);
      minWidget = new QWidget;
      minWidget->setLayout(minLayout);
      
      controlsLayout->addWidget(minWidget);
      controlsLayout->setAlignment(minWidget, Qt::AlignTop);
      minWidget->setVisible(true); // FIXME: QSettings should handle this
    }
    
    controlsLayout->addStretch();
    
    mainLayout = new QVBoxLayout;
    margins = mainLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    mainLayout->setContentsMargins(margins);
    mainLayout->addLayout(controlsLayout);
    

    setLayout(mainLayout);

    // FIXME: QSettings should handle this
    inclusiveButton->click();
  }
  
  
  QAction * AbstractFilter::createEffectivenessAction(QString text)
  {
    QAction * act = new QAction(text, this);
    act->setCheckable(true);
    act->setChecked(true);
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
  
  
  QBoxLayout * AbstractFilter::getMainLayout() const
  {
    ASSERT(mainLayout);
    
    return mainLayout;
  }
  
  
  QBoxLayout * AbstractFilter::getInclusiveExclusiveLayout() const
  {
    ASSERT(inclusiveExclusiveLayout);
    
    return inclusiveExclusiveLayout;
  }
  
  
  bool AbstractFilter::evaluateFromCount(QList< ControlMeasure * > measures,
      bool usePoints) const
  {
    int passedCount = 0;
      
    foreach (ControlMeasure * measure, measures)
    {
      ASSERT(measure);
      
      if (usePoints)
      {
        ControlPoint * point = measure->Parent();
        ASSERT(point);
        if (point && evaluate(point))
          passedCount++;
      }
      else
      {
        if (measure && evaluate(measure))
          passedCount++;
      }
    }
    
    return passedCount >= getMinForSuccess();
  }

  
  bool AbstractFilter::evaluateImageFromPointFilter(
      const ControlCubeGraphNode * node) const
  {
    ASSERT(node);
    
    bool evaluation = true;
    
    if (canFilterImages())
      evaluation = evaluateFromCount(node->getMeasures(), true);
      
    return evaluation;
  }
  
  
  bool AbstractFilter::evaluateImageFromMeasureFilter(
      const ControlCubeGraphNode * node) const
  {
    ASSERT(node);
    
    bool evaluation = true;
    
    if (canFilterImages())
      evaluation = evaluateFromCount(node->getMeasures(), false);
    
    return evaluation;
  }
  
  
  bool AbstractFilter::evaluatePointFromMeasureFilter(
      const ControlPoint * point) const
  {
    ASSERT(point);
    
    bool evaluation = true;
    
    if (canFilterPoints())
      evaluation = evaluateFromCount(point->getMeasures(), false);
    
    return evaluation;
  }
  
  
  bool AbstractFilter::evaluate(const ControlPoint * point,
      bool (ControlPoint::*meth)() const) const
  {
    ASSERT(point);
    
    return !((point->*meth)() ^ inclusive());
  }
  
  
  bool AbstractFilter::evaluate(const ControlMeasure * measure,
      bool (ControlMeasure::*meth)() const) const
  {
    ASSERT(measure);

    return !((measure->*meth)() ^ inclusive());
  }
  
  
  void AbstractFilter::showHideEffectivenessMenu()
  {
    ASSERT(effectivenessMenu);
    
    if (effectivenessMenu)
    {
      QRect menuRect(effectivenessMenu->pos(), effectivenessMenu->size());
      QPoint cursorPos = effectivenessMenu->cursor().pos();
      
      if (menuRect.contains(cursorPos))
      {
        effectivenessMenu->show();
      }
      else
      {
        effectivenessMenu->hide();
        
        // if there are no checked actions when the menu closes then close
        // this filter
        bool noCheckedActions = true;
        QList< QAction * > actions = effectivenessMenu->actions();
        for (int i = 0; noCheckedActions && i < actions.size(); i++)
        {
          if (actions[i]->isChecked())
            noCheckedActions = false;
        }
        
        if (noCheckedActions)
          parentSelector->sendClose();
        else
          emit filterChanged();
      }
    }
  }

  
  void AbstractFilter::updateEffectiveness()
  {
    ASSERT(effectivenessMenu);
    
    if (effectivenessMenu)
    {
      FilterEffectivenessFlag newFlags;
      
      QWriteLocker locker(lock);
      QList< QAction * > actions = effectivenessMenu->actions();
      
      if (minWidget)
        minWidget->setVisible(false);
      
      for (int i = 0; i < actions.size(); i++)
      {
        if (actions[i]->isChecked())
        {
          if (actions[i]->text() == "&Images")
            newFlags |= Images;
          else
            if (actions[i]->text() == "&Points")
              newFlags |= Points;
            else
              if (actions[i]->text() == "&Measures")
                newFlags |= Measures;
              
          if (i == 0 && minWidget)
            minWidget->setVisible(true);
        }
      }
      
      *effectivenessFlags = newFlags;
    }
  }


  void AbstractFilter::updateMinForImageSuccess(int newMin)
  {
    QWriteLocker locker(lock);
    minForSuccess = newMin;
    locker.unlock();
    emit filterChanged();
  }
}

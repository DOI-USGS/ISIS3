#include "IsisDebug.h"

#include "AbstractMultipleChoiceFilter.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QMargins>
#include <QString>
#include <QStringList>


using std::cerr;


namespace Isis
{
  namespace CnetViz
  {
    AbstractMultipleChoiceFilter::AbstractMultipleChoiceFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess)
    {
      nullify();
      curChoice = new QString;
    }

    AbstractMultipleChoiceFilter::AbstractMultipleChoiceFilter(
        AbstractMultipleChoiceFilter const & other)
        : AbstractFilter(other)
    {
      nullify();
      curChoice = new QString;
      
      QStringList options;
      for (int i = 0; i < other.combo->count(); i++)
        options.append(other.combo->itemText(i));
      
      createWidget(options);

      combo->setCurrentIndex(other.combo->currentIndex());
    }



    AbstractMultipleChoiceFilter::~AbstractMultipleChoiceFilter()
    {
      if (curChoice)
      {
        delete curChoice;
        curChoice = NULL;
      }
    }


    void AbstractMultipleChoiceFilter::nullify()
    {
      combo = NULL;
      curChoice = NULL;
    }


    void AbstractMultipleChoiceFilter::createWidget(QStringList options)
    {
      combo = new QComboBox;
      foreach (QString option, options)
        combo->addItem(option);
      
      combo->setCurrentIndex(0);
      
      *curChoice = combo->currentText();

      connect(combo, SIGNAL(currentIndexChanged(QString)),
              this, SLOT(updateCurChoice(QString)));
      
      QHBoxLayout * layout = new QHBoxLayout;
      QMargins margins = layout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      layout->setContentsMargins(margins);
      layout->addWidget(combo);
      layout->addStretch();

      getMainLayout()->addLayout(layout);
    }
    
    
    QString const & AbstractMultipleChoiceFilter::getCurrentChoice() const
    {
      ASSERT(curChoice);
      return *curChoice;
    }

    
    void AbstractMultipleChoiceFilter::updateCurChoice(QString newChoice)
    {
      ASSERT(curChoice);
      
      *curChoice = newChoice;
      
      emit filterChanged();
    }
  }
}

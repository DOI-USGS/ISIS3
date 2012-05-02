#include "IsisDebug.h"

#include "AbstractStringFilter.h"

#include <iostream>

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  namespace CnetViz
  {
    AbstractStringFilter::AbstractStringFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess)
    {
      nullify();
      createWidget();
    }

    AbstractStringFilter::AbstractStringFilter(
        const AbstractStringFilter & other) : AbstractFilter(other)
    {
      nullify();
      createWidget();

      lineEdit->setText(other.lineEdit->text());
    }



    AbstractStringFilter::~AbstractStringFilter()
    {
      if (lineEditText)
      {
        delete lineEditText;
        lineEditText = NULL;
      }
    }


    void AbstractStringFilter::nullify()
    {
      lineEdit = NULL;
      lineEditText = NULL;
    }


    void AbstractStringFilter::createWidget()
    {
      lineEditText = new QString;

      lineEdit = new QLineEdit;
      lineEdit->setMinimumWidth(250);
      connect(lineEdit, SIGNAL(textChanged(QString)),
          this, SLOT(updateLineEditText(QString)));
      connect(lineEdit, SIGNAL(textChanged(QString)),
          this, SIGNAL(filterChanged()));

      QHBoxLayout * layout = new QHBoxLayout;
      QMargins margins = layout->contentsMargins();
      margins.setTop(0);
      margins.setBottom(0);
      layout->setContentsMargins(margins);
      layout->addWidget(lineEdit);
      layout->addStretch();

      getMainLayout()->addLayout(layout);
    }


    bool AbstractStringFilter::evaluate(QString str) const
    {
      bool evaluation = true;

      // multiple threads reading the lineEditText so lock it
      QString text = *lineEditText;

      if (text.size() >= 1)
      {
        bool match = str.contains(text, Qt::CaseInsensitive);

        // inclusive() and match must either be both true or both false
        evaluation = !(inclusive() ^ match);
      }

      return evaluation;
    }


    QString AbstractStringFilter::descriptionSuffix() const
    {
      QString suffix;

      if (inclusive())
        suffix += "containing \"";
      else
        suffix += "not containing \"";

      suffix += *lineEditText;

      suffix += "\"";
      return suffix;
    }


    void AbstractStringFilter::updateLineEditText(QString newText)
    {
      *lineEditText = newText;
    }
  }
}

#include "TabBar.h"

#include <cstddef>
#include <iostream>

#include <QVector>

#include "Tab.h"

using std::cerr;


namespace Isis
{
  /**
   * construct a TabBar
   */
  TabBar::TabBar()
  {
    tabs = NULL;

    tabs = new QVector< Tab * > ();
    radioStyleTabs = false;
  }


  //! destruct a TabBar
  TabBar::~TabBar()
  {
    if (tabs)
    {
      delete tabs;
      tabs = NULL;
    }
  }


  /**
   * Adds a Tab to the TabBar.  Note that The TabBar takes ownership of Tabs
   * once they are added!
   *
   * @param newTab New Tab to be added
   */
  void TabBar::addTab(Tab * newTab)
  {
    const int & tabCountBeforeAddition = tabs->size();
    newTab->setPosition(tabCountBeforeAddition);
    connect(newTab, SIGNAL(clicked(const int &)), this,
        SLOT(tabClicked(const int &)));
    tabs->push_back(newTab);

    newTab->setSelected(false);
    addAction(newTab);
  }


  /**
   * If set to true then the effect is that Tabs in the same radio group can
   * only be selected one at a time.  When a Tab is selected all other Tabs in
   * the same radio group would automatically be deselected.  If set to false
   * then all Tabs in the TabBar will behave independent from eachother
   * regardless of how their radio groups are set.
   *
   * @param radioStyle True if radio style should be used, false otherwise
   */
  void TabBar::setRadioStyle(const bool & radioStyle)
  {
    radioStyleTabs = radioStyle;
  }


  /**
   * @returns The current radio style being used
   */
  bool TabBar::radioStyle()
  {
    return radioStyleTabs;
  }


  /**
   * @returns The number of Tabs currently in the TabBar
   */
  int TabBar::size() const
  {
    return tabs->size();
  }


  /**
   * Sets whether the Tab at the specified index is selected or not
   *
   * @param index Index of the Tab.  The first Tab added to the bar has an index
   *              of 0.  The last Tab added has an index of size() - 1
   *
   * @param status True if the Tab should be selected, false otherwise
   */
  void TabBar::setSelected(const int & index, const bool & status)
  {
    if (index >= 0 && index < tabs->size())
      (*tabs)[index]->setSelected(status);
  }


  /**
   * @param index Index of the Tab.  The first Tab added to the bar has an index
   *              of 0.  The last Tab added has an index of size() - 1.
   *
   * @returns True if the Tab at the given index is selected, false otherwise
   */
  bool TabBar::isSelected(const int &index)
  {
    if (index >= 0 && index < tabs->size() && (*tabs)[index]->isSelected())
      return true;

    return false;
  }


  /**
   * @returns True if no Tabs are currently selected, false otherwise
   */
  bool TabBar::noneSelected()
  {
    for (int i = 0; i < tabs->size(); i++)
      if ((*tabs)[i]->isSelected())
        return false;

    return true;
  }


  /**
   * Custom setEnabled method that also calls setEnabled for each of our Tabs
   *
   * @param newEnabledStatus The new enabled status that will be applied to the
   *                         TabBar and all of its Tabs
   */
  void TabBar::setEnabled(bool newEnabledStatus)
  {
    QToolBar::setEnabled(newEnabledStatus);

    for (int i = 0; i < size(); i++)
      (*tabs)[i]->setEnabled(newEnabledStatus);
  }


  /**
   * SLOT which performs actions that need to be done when we get a SIGNAL from
   * a Tab telling us that it has been clicked.  What happens is that first the
   * Tab is clicked.  The Tab catches this signal and then emits its own clicked
   * SIGNAL which contains its index.  This SIGNAL is connected to this SLOT
   * which then takes the appropriate action depending on a couple things.  If
   * radioStyle is not being used or if it is but there are no other Tabs in
   * this Tab's radio group then we simply want to toggle the state of the Tab.
   * If radio style is being used and there are others in this Tab's radio group
   * which are selected then the other Tabs are first deselected before the Tab
   * is set to be selected (We know it was not selected before since there was
   * another Tab in its group which was).
   *
   * @param index Index of the Tab which emitted the clicked SIGNAL.
   */
  void TabBar::tabClicked(const int & index)
  {
    if (!radioStyleTabs || noOthersInGrpSelected(index))
    {
      (*tabs)[index]->setSelected(!(*tabs)[index]->isSelected());
    }
    else
    {
      deselectOthersInGrp(index);
      (*tabs)[index]->setSelected(true);
    }
  }


  /**
   * @param index Index of the Tab.  The first Tab added to the bar has an index
   *              of 0.  The last Tab added has an index of size() - 1.
   *
   * @returns True if there are no other Tabs in the given Tab's radio group
   *          which are selected, false otherwise.
   */
  bool TabBar::noOthersInGrpSelected(const int & index) const
  {
    const int & thisGrp = tabs->at(index)->getRadioGroup();
    for (int i = 0; i < tabs->size(); i++)
    {
      if (index != i)
      {
        const int & otherGrp = tabs->at(i)->getRadioGroup();
        if (otherGrp == thisGrp && tabs->at(i)->isSelected())
          return false;
      }
    }
    return true;
  }


  /**
   * @param index Index of the Tab.  The first Tab added to the bar has an index
   *              of 0.  The last Tab added has an index of size() - 1.
   *
   * Deselects any other selected Tabs in the given Tab's radio group
   */
  void TabBar::deselectOthersInGrp(const int & index)
  {
    const int & thisGrp = tabs->at(index)->getRadioGroup();
    for (int i = 0; i < tabs->size(); i++)
    {
      if (index != i)
      {
        const int & otherGrp = tabs->at(i)->getRadioGroup();
        if (otherGrp == thisGrp)
          (*tabs)[i]->setSelected(false);
      }
    }
  }
}

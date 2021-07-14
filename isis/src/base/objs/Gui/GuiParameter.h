#ifndef Isis_GuiParameter_h
#define Isis_GuiParameter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QToolButton>

namespace Isis {
  class UserInterface;

  /**
   * @author 2006-10-31 ???
   *
   * @internal
   *   @history 2009-11-10 Mackenzie Boyd - Refactored to reduce
   *            code duplication in children GuiCubeParameter and
   *            GuiFileNameParameter, specifically, SelectFile
   *            method.
   *   @history 2009-12-15 Travis Addair - Moved the SelectFile
   *            method back to children.
   */

  class GuiParameter : public QObject {

      Q_OBJECT

    public:

      GuiParameter(QGridLayout *grid, UserInterface &ui, int group, int param);
      virtual ~GuiParameter();

      //! Return the name of the parameter
      QString Name() const {
        return p_name;
      };

      void SetToDefault();

      void SetToCurrent();

      virtual QString Value() = 0;

      virtual void Set(QString newValue) = 0;

      void SetEnabled(bool enabled, bool isParentCombo=false);

      //! Is the parameter enabled
      bool IsEnabled() const {
        return p_label->isEnabled();
      }

      virtual bool IsModified();

      void Update();

      void RememberWidget(QWidget *w);

      QWidget *AddHelpers(QObject *lo);

      virtual std::vector<QString> Exclusions();

      enum ParameterType { IntegerWidget, DoubleWidget, StringWidget,
                           ListWidget, FileNameWidget, CubeWidget,
                           BooleanWidget, ComboWidget
                         };
      ParameterType Type() {
        return p_type;
      };

    protected:

      QToolButton *p_fileButton;
      QLineEdit *p_lineEdit;

      int p_group;
      int p_param;
      QString p_name;
      UserInterface *p_ui;

      QLabel *p_label;

      QList<QWidget *> p_widgetList;

      ParameterType p_type;

    private:
      QMenu *p_helperMenu;

    signals:
      void ValueChanged();
      void HelperTrigger(const QString &);

  };
};

#endif

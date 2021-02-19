/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QAction>
#include <QString>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiHelperAction : public QAction {

      Q_OBJECT

    public:

      GuiHelperAction(QObject *parent, const QString &funct);
      ~GuiHelperAction();

    signals:
      void trigger(const QString &funct);

    private slots:
      void retrigger();

    private:
      QString p_funct;
  };
}


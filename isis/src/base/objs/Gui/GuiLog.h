#ifndef Isis_GuiLog_h
#define Isis_GuiLog_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QTextEdit>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiLog : public QWidget {

      Q_OBJECT

    public:

      GuiLog(QWidget *parent = 0);
      ~GuiLog();

      void Write(const QString &string);

    public slots:
      void Clear();
      void Save();

    private:
      QTextEdit *p_textEdit;
  };
};



#endif

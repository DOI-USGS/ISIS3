
#ifndef Isis_GuiLog_h
#define Isis_GuiLog_h

#include <QTextEdit>

namespace Isis {

  class GuiLog : public QWidget {

    Q_OBJECT

    public:

      GuiLog (QWidget *parent=0);
      ~GuiLog ();

      void Write(const QString &string);

    public slots:
      void Clear();
      void Save();

    private:
      QTextEdit *p_textEdit;
  };
};



#endif

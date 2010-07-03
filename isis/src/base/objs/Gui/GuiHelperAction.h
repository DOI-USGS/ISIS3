#include <QAction>
#include "iString.h"

namespace Isis {
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


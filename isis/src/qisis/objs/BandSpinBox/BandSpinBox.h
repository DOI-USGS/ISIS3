#ifndef BandSpinBox_h
#define BandSpinBox_h

#include <QSpinBox>
#include <QMap>

#include "Pvl.h"

namespace Qisis {
  class BandSpinBox : public QSpinBox {
    Q_OBJECT

    public:
      BandSpinBox (QWidget *parent = 0);
      void setBandBin (Isis::Pvl &pvl, const QString &key = "BandNumber");
      QStringList BandBinKeys ();
      QSize sizeHint() const;
      QValidator::State validate(QString &input, int &pos) const;

    public slots:
      void setKey(QString key);
      void setKey(int key);

    protected:
      QString textFromValue (int val) const;
      int valueFromText (const QString &text) const;

    private:
      int p_bands;  //!< Number of bands

      QMap<QString, QStringList > p_map;  //!< The maps the last key to all the keys
      QString p_lastKey; //!< The last key
      QStringList p_keys; //!< List of all the keys
  };
}

#endif

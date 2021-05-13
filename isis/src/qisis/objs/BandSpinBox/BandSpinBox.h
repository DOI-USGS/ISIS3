#ifndef BandSpinBox_h
#define BandSpinBox_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSpinBox>
#include <QMap>

#include "Pvl.h"

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class BandSpinBox : public QSpinBox {
      Q_OBJECT

    public:
      BandSpinBox(QWidget *parent = 0);
      void setBandBin(Pvl &pvl, const QString &key = "BandNumber");
      QStringList BandBinKeys();
      QSize sizeHint() const;
      QValidator::State validate(QString &input, int &pos) const;

    public slots:
      void setKey(QString key);
      void setKey(int key);

    protected:
      QString textFromValue(int val) const;
      int valueFromText(const QString &text) const;

    private:
      int p_bands;  //!< Number of bands

      QMap<QString, QStringList > p_map;  //!< The maps the last key to all the keys
      QString p_lastKey; //!< The last key
      QStringList p_keys; //!< List of all the keys
  };
}

#endif

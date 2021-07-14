#ifndef IsisGuiOutputAttribute_h
#define IsisGuiOutputAttribute_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-07-02 Steven Lambright and Stuart Sides - Modified to
   *                           have similar display values after the CubeAttribute
   *                           refactor that better preserves strings that are
   *                           passsed into CubeAttributeOutput. One of the strings
   *                           was also invalid - didn't match any attributes, but was
   *                           the default so there were no symptoms of a bug. This has
   *                           been fixed. References #961.
   *   @history 2016-04-21 Makayla Shepherd - Added UnsignedWord handling.
   *   @history 2018-07-27 Kaitlyn Lee - Added signed/unsigned integer handling.
   */
  class GuiOutputAttribute : public QDialog {
      Q_OBJECT

    public:
      GuiOutputAttribute(QWidget *parent = 0);

      ~GuiOutputAttribute();

      QString GetAttributes();

      void SetAttributes(const QString &value);

      static int GetAttributes(const QString &defaultAttribute,
                               QString &newAttribute,
                               const QString &title,
                               bool allowProp,
                               QWidget *parent);

      void SetPropagation(bool enabled);

    private:
      QRadioButton *p_propagate;
      QRadioButton *p_unsignedByte;
      QRadioButton *p_signedWord;
      QRadioButton *p_unsignedWord;
      QRadioButton *p_signedInteger;
      QRadioButton *p_unsignedInteger;
      QRadioButton *p_real;
      QLineEdit *p_minEdit;
      QLineEdit *p_maxEdit;
      QRadioButton *p_attached;
      QRadioButton *p_detached;
      QRadioButton *p_tiled;
      QRadioButton *p_bsq;
      QRadioButton *p_lsb;
      QRadioButton *p_msb;
      bool p_propagationEnabled;
  };
};

#endif

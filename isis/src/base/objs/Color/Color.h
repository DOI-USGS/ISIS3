#ifndef Color_H
#define Color_H

class QColor;
class QRegExp;
class QString;

namespace Isis {
  /**
   * This class is designed to serialize QColor in a human-readable form. QColor provides no
   *   string conversions that include the alpha value (probably because argb and rgba both look
   *   the same and neither are standard). But encoding the colors using hex is worse than the
   *   non-standard conversion -- so this class will convert to/from rgba.
   *
   * @author 2012-08-?? Steven Lambright and Stuart Sides
   *
   * @internal
   */
  class Color {
    public:
      static QColor fromRGBAString(QString);
      static QString toRGBAString(QColor);
      static QRegExp colorRGBAFormat();

    private:
      Color();
      Color(const Color &);
      ~Color();
      Color &operator=(const Color &);
  };
}

#endif

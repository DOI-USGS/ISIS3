/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Kernel.h"

#include <QString>
#include <QStringList>

using namespace std;
namespace Isis {
  /**
   * Constructs a Kernel object with "Unknown" Type
   */
  Kernel::Kernel() {
    m_kernelType = static_cast<Type>(0);
    m_kernels.clear();
  }

  /**
   * Destructs Kernel object.
   */
  Kernel::~Kernel() {
  }

  /**
   * Constructs a Kernel object with given Type and kernels data.
   *
   * @param type Type of kernel to be constructed, see Kernel::Type
   *             enumerations
   * @param data Vector containing kernel file name(s)
   */
  Kernel::Kernel(Type type, const QStringList &data) {
    m_kernelType = type;
    m_kernels = data;
  }

  /**
   * Converts the given string to a character as follows
   * <ul>
   *   <li> "Predicted" = 1</li>
   *   <li> "Nadir" = 2</li>
   *   <li> "Reconstructed" = 4</li>
   *   <li> "Smithed" = 8</li>
   *   <li> Anything else = 0</li>
   * </ul>
   *
   * @param type A QString containing a Kernel Type. These strings are not case
   *             dependent (the type maybe written all caps, all lower, camel).
   * @return @b Type The Kernel type enumeration value
   *
   */
  Kernel::Type Kernel::typeEnum(const QString &type) {
    QString strng = type.simplified().trimmed().toUpper();
    if (strng == "PREDICTED") return Predicted;
    if (strng == "NADIR") return Nadir;
    if (strng == "RECONSTRUCTED") return Reconstructed;
    if (strng == "SMITHED") return Smithed;

    return static_cast<Kernel::Type>(0);
  };

  /**
   * Converts the given Type to a character as follows
   * <ul>
   *   <li> 1 = Predicted</li>
   *   <li> 2 = Nadir</li>
   *   <li> 4 = Reconstructed</li>
   *   <li> 8 = Smithed</li>
   *   <li> Anything else = Unknown</li>
   * </ul>
   * @param type The Kernel type enumeration value
   * @return @b const @b char* A character array corresponding to the
   *            passed in Kernel::Type.
   */
  const char *Kernel::typeEnum(const Type &type) {
    if (type == Predicted) return "Predicted";
    if (type == Nadir) return "Nadir";
    if (type == Reconstructed) return "Reconstructed";
    if (type == Smithed) return "Smithed";

    return "Unknown";
  };

  /**
   * Returns the list of kernel data file names.
   * @return @b QStringList List of file names for the kernel
   */
  QStringList Kernel::kernels() {
    return m_kernels;
  }

  /**
   * Returns the stored Kernel::Type.
   * @return @b Type Enumerated kernel type value
   */
  Kernel::Type Kernel::type() {
    return m_kernelType;
  }

  /**
   * Sets the kernel data file names to the given list.
   * @param data A list containing kernel data (file names)
   */
  void Kernel::setKernels(QStringList data) {
    m_kernels = data;
  }

  /**
   * Sets the Kernel type.
   * @param type The Kernel type enumeration value to be set.
   */
  void Kernel::setType(const Type &type) {
    m_kernelType = type;
  }

  /**
   * Accessor method to determine the number of kernel files.
   * @return @b const @b int Number of items in the kernel data list
   */
  int Kernel::size() {
    return m_kernels.size();
  }

  /**
   * Adds the string (file name) to the kernel data file list.
   * @param str String containing kernel file name to be added.
   */
  void Kernel::push_back(const QString &str) {
    m_kernels.push_back(str);
  }

  /**
   * Returns the value (file name) in the kernel data list at the given
   * index value.
   *
   * @param index The index value of the kernel data list
   * @return @b QString The kernel file name stored in the given index
   */
  QString &Kernel::operator[](const int index) {
    return m_kernels[index];
  }

  /**
   * A constant method that returns the value of the kernel data list at
   * the given index value.
   *
   * @param index The index value of the kernel data list
   * @return @b QString The kernel data stored in the given index
   *
   */
  QString Kernel::operator[](const int index) const {
    return m_kernels[index];
  }

  /**
   * Compares which Kernel object has a higher quality Type, i.e.
   * which type has a higher enumeration value.
   * @param other The Kernel whose type value will be compared
   * @return @b bool Indicates whether the Kernel object to the left of
   *         the operator has a larger Type enumeration than the
   *         object on the right of the operator.
   */
  bool Kernel::operator<(const Kernel &other) const {
    return (this->m_kernelType < other.m_kernelType);
  }


  /**
   * Logical operator for combining Type flags.
   *
   * @param a The first Type flag.
   * @param b The second Type flag.
   *
   * @return Type flag that contains all Types in a and all Types in b.
   */
  Kernel::Type operator|(Kernel::Type a, Kernel::Type b) {
      return static_cast<Kernel::Type>(static_cast<int>(a) | static_cast<int>(b));
  }
} //end namespace isis

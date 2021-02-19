#ifndef Kernel_h
#define Kernel_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QString>
#include <QStringList>

namespace Isis {
  /**
   * This class stores Kernel information, including Type and kernel
   * file names.
   *
   * @ingroup System
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2013-02-26 Jeannie Backer - Moved this class from the KernelDb
   *                           header file into it's own directory.  Moved
   *                           method implementation into a cpp file. Moved the
   *                           KernelType enumeration from a global "spiceInit"
   *                           namespace into the Kernel class and renamed it
   *                           Type. Added accessor and mutator methods. Changed
   *                           method names to lower camel case and changed
   *                           member variable prefix to m_ to comply with Isis
   *                           standards. Documented enumeration, methods, member
   *                           variables. Created unitTest.References #924.
   */
  class Kernel {
    public:
      /**
       * Enumeration for type of kernel
       */
      enum Type {
        Predicted = 1,     /**< Predicted Kernels are based on predicted location
                                of the spacecraft*/
        Nadir = 2,         /**< Nadir Kernels mimic spacecraft pointing*/
        Reconstructed = 4, /**< Reconstructed Kernels are supplemented with
                                information on the actual position of targets
                                and/or spacecrafts*/
        Smithed = 8        /**< Smithed Kernels are generally reconstructed
                                kernels that have been corrected */
      };

      // constructors
      Kernel();
      Kernel(Type type, const QStringList &data);
      ~Kernel();
      // static kerneltype enum converters
      static Type typeEnum(const QString &type);
      static const char *typeEnum(const Type &type);
      // accessors
      QStringList kernels();
      Type type();
      // mutators
      void setKernels(QStringList data);
      void setType(const Type &type);
      // kernels vector operations
      int size();
      void push_back(const QString &str);
      QString &operator[](const int index);
      QString operator[](const int index) const;
      // kernel type operations
      bool operator<(const Kernel &other) const;

    private:
      QStringList m_kernels; //!< List of kernel file names
      Type m_kernelType;     //!< Enumeration value indicating the kernel type
  };


  Kernel::Type operator|(Kernel::Type a, Kernel::Type b);
};
#endif

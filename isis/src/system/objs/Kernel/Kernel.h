#ifndef Kernel_h
#define Kernel_h

/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2009/07/28 21:01:18 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
   *                           member variable prefix to m_ to comply with Isis3
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
};
#endif

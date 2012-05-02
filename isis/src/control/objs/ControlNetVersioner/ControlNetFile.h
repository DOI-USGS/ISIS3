#ifndef ControlNetFile_h
#define ControlNetFile_h

namespace Isis {
  class FileName;
  class Pvl;

  /**
   * @brief Generic Binary Control Net File Representation
   *
   * This class is the parent for all binary forms of the control network
   *   files. Each one must be readable, writable, and convertable to Pvl.
   *   Other than that, they can be (and probably will be) literally anything as
   *   long as it has a Pvl header.
   *
   * @author 2011-04-07 Steven Lambright
   *
   * @internal
   */
  class ControlNetFile {
    public:
      /**
       * Constructor. No data so this does nothing.
       */
      ControlNetFile() {};
      /**
       * Destructor. No data so this does nothing.
       */
      virtual ~ControlNetFile() {};

      /**
       * This reads the binary file into memory. The header is the Pvl that must
       *   be at the top of the file (it's how we could tell it was binary in
       *   the first place).
       *
       * @param header The pvl at the top of the file down to the "End" keyword
       * @param file The filename of the binary file to be read
       */
      virtual void Read(const Pvl &header, const FileName &file) = 0;

      /**
       * This writes the binary file that is in memory to disk. The behavior of
       *   this method is undefined if the required data is not set
       *   (ControlNetVersioner::LatestPvlToBinary guarantees they are, and this
       *   should never be called for old versions).
       *
       * @param file The filename of the binary file to be written
       */
      virtual void Write(const FileName &file) const = 0;

      /**
       * Convert the binary representation to Pvl (any pvl version).
       */
      virtual Pvl ToPvl() const = 0;

    private:
      /**
       * Disallow copy construction. This should never happen.
       *
       * @param other File to copy from
       */
      ControlNetFile(const ControlNetFile &other);

      /**
       * Disallow assignment. This should never happen.
       *
       * @param other File to copy from
       */
      ControlNetFile &operator=(const ControlNetFile &other);
  };

}

// Always include the latest version only
#include "ControlNetFileV0002.h"

namespace Isis {
  class ControlNetFileV0002;

  /**
   * To minimize changes in other places, allow others to use "Latest"
   */
  typedef ControlNetFileV0002 LatestControlNetFile;
}

#endif

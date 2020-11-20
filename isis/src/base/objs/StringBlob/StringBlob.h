#ifndef StringBlob_h
#define StringBlob_h

/**
 */

#include <string>

#include "Blob.h"

namespace Isis {
  /**
   * @brief Read and store std::strings on the cube.
   *
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2020-11-19 Kristin Berry - Original Version
   *
   * @internal
   *   @history 2020-11-19 Kristin Berry - Original Version
    */
  class StringBlob : public Isis::Blob {
    public:
      StringBlob();
      StringBlob(const QString &file);
      StringBlob(std::string str, QString name);
      ~StringBlob();

    protected:
      // prepare data for writing
      void WriteInit();
      void WriteData(std::fstream &os);

    private:
      std::string m_string;
  };
};

#endif


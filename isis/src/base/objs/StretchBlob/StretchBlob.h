#ifndef StretchBlob_h
#define StretchBlob_h
/**
 * Still needed? 
 */

#include "Blob.h"
#include "CubeStretch.h"

namespace Isis {
  class CubeStretch;
  /**
   * @brief Blob to store stretch information to cube.
   *
   * @ingroup Utility
   *
   * @author 2020-07-28 Kristin Berry Stuart Sides
   *
   * @internal
   *  @history 2020-07-28 Kristin Berry Stuart Sides - Original Version
   */
  class StretchBlob : public Isis::Blob {
    public: 
      StretchBlob();
      StretchBlob(CubeStretch stretch);
      StretchBlob(QString name);
      ~StretchBlob();

      CubeStretch getStretch(); 

    protected:
      void WriteInit();
      void ReadData(std::istream &is);
      void WriteData(std::fstream &os);

    private:
      CubeStretch *m_stretch; //! Stretch associated with the blob
  };
};

#endif


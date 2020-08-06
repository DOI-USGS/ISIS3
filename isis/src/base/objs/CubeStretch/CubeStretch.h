#ifndef CubeStretch_h
#define CubeStretch_h

/**
 * Do we still need the big block of info up here?  
 */ 

#include "Stretch.h"
#include "StretchBlob.h"

namespace Isis {
  /**
   * @brief Stores stretch information for a cube. 
   *
   * @ingroup Utility
   *
   * @author 2020-07-28 Kristin Berry
   *
   * @internal
   *  @history 2020-07-28 Kristin Berry - Original Version
   */
  class CubeStretch : public Stretch { 
    public: 
      CubeStretch();
      CubeStretch(QString name);
      CubeStretch(QString name, QString stretchType, int bandNumber=1);
      ~CubeStretch();

      CubeStretch(Stretch const& stretch);
      CubeStretch(Stretch const& stretch, QString type);
      CubeStretch(Stretch const& stretch, QString type, QString name, int bandNumber=1);

      QString getType();
      void setType(QString stretchType);

      QString getName();
      void setName(QString name);

      int getBandNumber();
      void setBandNumber(int bandNumber);
           
    private:                      
      QString m_name; //! The name of the stretch. 
      QString m_type; //! Type of  stretch. This is only currently used in the AdvancedStretchTool.
      int m_bandNumber; //! The band number associated with this stretch
  };
};                                

#endif


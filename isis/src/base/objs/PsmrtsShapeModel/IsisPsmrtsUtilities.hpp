#ifndef IsisPsmrtsUtilities_hpp
#define IsisPsmrtsUtilities_hpp
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ShapeModel.h"

#include <vector>
#include <string>

#include <QString>
#include "FileName.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlFlatMap.h"
#include "Target.h"


#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsTracerSystem.hpp>


namespace Isis {

  /**
   * @brief Convert a QString to a std::string
   * 
   * This method converts a QString to a std::string with preservation of
   * UTF-8 data.
   * 
   * @param qts           The QT QString value to convert
   * @return std::string  The converted std::string
   */
  inline std::string qt_to_string( const QString &qts ) {
    QByteArray bstr = qts.toUtf8();
    return ( std::string( bstr.constData(), bstr.length() ) );
  }

  /** Extract and expand the name of a Pvl source file otherwise use default */
  inline QString get_file_name( const Pvl &label, 
                                const QString &name_default = "none" ) {
    QString fname = FileName( label.fileName() ).expanded();
    if ( fname.size() == 0 ) fname = name_default;
    return ( fname );
  }


  /**
   * @brief Create a isis path translator object from ISIS ennvironment
   * 
   * This function creates a path translator object that contains the current
   * state of shell environment variables and the parameters contained in the
   * DataDirectory group preferences file.
   * 
   * @param name Name of the translator object 
   * @return psmrts::PsmrtsTranslations The translator object
   */
  inline psmrts::PsmrtsTranslations create_isis_path_translator( const QString &name = "isis_path_translator") {

    // Set up ISIS DataDirectory translations. Using the create() method
    // automatically loads the environment.
    psmrts::PsmrtsTranslations isis_t = psmrts::PsmrtsTranslations::create( qt_to_string( name ) );
    if ( Preference::Preferences().hasGroup("DataDirectory") ) {
      const PvlGroup &data_t = Preference::Preferences().findGroup("DataDirectory");
      PvlContainer::ConstPvlKeywordIterator key = data_t.begin();
      while ( key != data_t.end() ) {
        isis_t.add_parameter( qt_to_string( key->name() ), qt_to_string( (*key)[0] ) );
        ++key;
      }
    }

    return ( isis_t );
  }

  /**
   * @brief Return a PvlFlatMap of a PVL group
   * 
   * @param pvl   PVL-type container to search for a group 
   * @param group Name of group to find/extract
   * @return PvlFlatMap Returns the contents or an empty map if not found
   */
  inline PvlFlatMap extract_pvl_group( const PvlObject &pvl, 
                                       const QString &group ) {
    PvlFlatMap kmap_t;
    if ( pvl.hasGroup( group ) ) {
      kmap_t = PvlFlatMap( pvl.findGroup( group ) ); 
    }
    else {
      PvlObject::ConstPvlObjectIterator obj;
      for ( obj = pvl.beginObject() ; obj != pvl.endObject() ; ++obj ) {
        if ( obj->hasGroup( group ) ) {
          kmap_t = PvlFlatMap( obj->findGroup( group ) ); 
          return ( kmap_t );       
        }
      }
    }
    return ( kmap_t );
  }

  /** Return shapemodel preferences from current state */
  inline PvlFlatMap get_shapemodel_preferences( ) {
    return ( extract_pvl_group( Preference::Preferences(), "ShapeModel" ) );
  }

}
#endif

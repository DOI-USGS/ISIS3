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

  /**
   * @brief Load a text file containing a list of files
   * 
   * This function reads a text file that is assumed to contain a list of files
   * of single line content. It ignores blank lines and (optionally) comments
   * indicated a line starting with a "#".
   * 
   * It trims whitespace from the lines and returns each line in string vector.
   * 
   * @param listfile Name of file contain content  lines
   * @return std::vector<std::string> The processed lines of data
   */
  inline std::vector<std::string> load_list_file( const QString &listfile ) {
    FileName ifile( listfile );
    std::string file_t = qt_to_string( ifile.expanded() );
    std::vector<std::string> list_t;
    (void) psmrts::read_list_file( file_t, list_t );
    return ( list_t );
  }


  /**
   * @brief Return substrings between delimeter string of arbitrary length
   * 
   * This function returns strings between a delimiter string, such as "::". The
   * strings are assumed to be trimmed but if not, whitespace is preserved. 
   * 
   * When parsing CSV type strings see psmrts::string_tokenizer().
   * 
   * @param s         String to extract between delimiter string
   * @param delimiter Delimiter/separator string between tokens
   * @return std::vector<std::string> List of tokens extracted
   */
  inline std::vector<std::string> string_delimeter_tokenizer( const std::string &s,
                                                              const std::string &delimiter) {

    // Tokens!
    std::vector<std::string> values;

    size_t s_len  = s.length();
    size_t t_len  = delimiter.length();

    std::string::size_type start_t = 0;
    std::string::size_type spos    = 0;

    // Loop by occurance of the token
    while ( (spos = s.find( delimiter, start_t ) ) != std::string::npos )  {
      size_t chars_t = spos - start_t;
      values.push_back( s.substr(start_t, chars_t) );
      start_t += t_len;
    }

    // Get the last part of the token
    values.push_back( s.substr(start_t,  s_len - start_t ) );
    return ( values );
  } 

  /**
   * @brief Get the reference ellipsoid from an ISIS Target
   * 
   * Returns a PSMRTS ellipsoid from an ISIS Target object. It extracts the
   * contents of the radii data in this object and the name (from the instrument
   * Target keyword, typically).
   * 
   * @param target A valid ISIS Target object
   * @return PsmrtsTracer A PSMRTS tracer ellipsoid object
   */
  inline psmrts::PsmrtsTracer get_ellipsoid( const Target *target ) {
    if ( nullptr == target ) {
      return ( psmrts::PsmrtsTracer() );
    }

    std::vector<double> radii;
    for ( const auto &radius : target->radii() ) {
      radii.push_back( radius.kilometers() );
    }

    // Gut check for valid radii from the Target object
    if ( radii.size() == 0 ) {
      return ( psmrts::PsmrtsTracer() );
    }

    // Return the ellipsoid
    return ( psmrts::PsmrtsTracer::ellipsoid( Eigen::Vector3d( radii.data() ), 
                                              qt_to_string( target->name() ) ) );
  }
 
  /**
   * @brief Create the initial state of the Psmrts Tracer system
   * 
   * This function creates the initial features of the PsmrtsTracerSystem object
   * that is tailored to ISIS shape model and ray tracing system using
   * prioritized shape models.
   * 
   * An ISIS file path translation object is created to evaluate the use of
   * mission file paths indicated with a "$" and separated by "/" delimiters. It
   * uses the users environment variables and the state of the ISIS Preferences
   * DataDirectory state within the current running ISIS envrionment.
   * 
   * The Target and potentially the Pvl objects are used to create a reference
   * ellipsoid for the target body. This is typically represented in the Target
   * object that typically originates from NAIF SPICE PCKs.
   * 
   * @param target  The target body data object
   * @param pvl     Typically the ISIS cube label contents
   * @return psmrts::PsmrtsTracerSystem The initialized priority tracing system
   *                   object that is prepared to create multiple ray tracers
   */
  inline psmrts::PsmrtsTracerSystem init_tracer_system( const Target *target, 
                                                        const Pvl &pvl ) {

    QString fname = get_file_name( pvl, "isis_cube" );
    psmrts::PsmrtsTranslations trans_t = create_isis_path_translator( fname );
    psmrts::PsmrtsTracerSystem tracer_t = psmrts::PsmrtsTracerSystem( qt_to_string( fname ), trans_t );

    // Set the reference ellipsoid if it exists
    psmrts::PsmrtsTracer ellipsoid_t( get_ellipsoid( target ) );
    if ( ellipsoid_t.isValid() ) {
      tracer_t.set_reference_ellipsoid( ellipsoid_t );
    }

    return ( tracer_t );
  }

  /**
   * @brief Update the ISIS PvlGroup with a specified value
   * 
   * This function creates a PvlKeyword in an PvlGroup with the specified value
   * if it is provided by the caller.
   * 
   * @param key       Name of the keyword to create in the group
   * @param group_p   The PvlGroup object to create the keyword
   * @param defvalue  The value of the keyword. If "" then no action is taken
   * @return true     If the keyword is successfully created
   * @return false    If the keyword already exists on the value is empty
   */
  inline bool update_kernel_group( const QString &key,
                                   PvlGroup &group_p,
                                   const QString &defvalue = ""  ) {
    if ( !group_p.hasKeyword( key ) ) {
      if ( "" != defvalue ) {
        group_p.addKeyword( PvlKeyword( key, defvalue ) );
        return ( true );
      }
    }
    return ( false );
  }

  /** Create a PvlKeyword wuth a vector of QStrings */
  inline PvlKeyword keyword_vector( const QString &name, 
                                    const QStringList &vlist,
                                    const QString &unit = "" ) {
    PvlKeyword key( name );
    for ( const QString &v : vlist ) {
      key.addValue( v, unit );
    }

    return ( key );
  }

}
#endif

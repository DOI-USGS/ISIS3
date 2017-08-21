#ifndef IsisBullet_h
#define IsisBullet_h
/**
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

/**
 * @brief Bullet Physics ISIS-specific include file 
 *  
 * This file *must* be included before any other Bullet includes because there 
 * are some very specific configuration/build settings in the version of Bullet 
 * we are using. 
 *  
 * First, ISIS requires Bullet Physics to be built with the BtScalar type set to
 * double precision. The Bullet build settings in the CMAKE system explicitly 
 * sets the BtScalar type to double via a command line MACRO definition. This 
 * file enforces the proper Bullet environment for use in the ISIS system. 
 *  
 * Second, we made an explicit declaration of the number of bits to be used in 
 * the Bullet triangle compression scheme that specifies parts/object and the 
 * number of triangles/part. We allow for 16 parts (4 bits) and  134,217,728 
 * triangles/part (28, really 27 due to sign bit). This definition is made in 
 * both btQuantizedBvh.h and b3QuantizedBvh.h.  The settings is: 
 *  
 *   #define MAX_NUM_PARTS_IN_BITS 4
 *  
 *   NOTE: ISIS requires a unique Bullet binary dependancy that may not be
 *   compatable in other applications linked using a different configuration of
 *   their Bullet system.
 *  
 * This defines limits of 16 separate sections of a target body where each part
 * can contain up to 134M triangles.
 *  
 * And, Python support for the Mac platform was broken since we use the MacPorts 
 * version 3.5 (at the time we built Bullet). For Apple platforms, incorrect 
 * assumptions were made for the include path in examples/pybullet/pybullet.c. 
 *  
 * We also check for compatibility with the Bullet 3 (B3*) environment but do 
 * not include the environment. Only the BT* environment is invoked for 
 * consistency. Include the B3 includes anywhere after this file to use that 
 * API. 
 *  
 * @author 2017-03-16 Kris Becker 
 * @internal 
 * @history 2017-03-16 Kris Becker Original Version 
 */

#if defined(BT_SCALAR_H) || defined(B3_SCALAR_H)
#if !defined(BT_USE_DOUBLE_PRECISION) && !defined(B3_USE_DOUBLE_PRECISION) 
#error "*** You must include IsisBullet.h prior to any Bullet includes ***"
#endif
#endif

// ISIS using the Bullet library requires double precision!! This sets the 
// Bullet::btScalar type to double precision throughout the Bullet API. 
#define   BT_USE_DOUBLE_PRECISION   1 
#define   B3_USE_DOUBLE_PRECISION   1 

// Go ahead and include all of Bullet here so the define will be applied 
// universally. 
#include <btBulletDynamicsCommon.h>

/** Namespace for ISIS/Bullet specific routines   */
namespace Isis {

/**
 * Maximum number of parts/object
 * 
 * @return @b int The maximum number of parts allowed in each collision objects.
 */
inline int bt_MaxBodyParts() {
  return ( 1<<MAX_NUM_PARTS_IN_BITS );
}


/**
 * Maximum number of triangles/part
 * 
 * @return @b long The maximum number of triangles allowed in each collision objects.
 */
inline unsigned long bt_MaxTriangles() {
  return ( 1<<(31-MAX_NUM_PARTS_IN_BITS) );
}


/**
 * Maximum number of parts/object in the Bullet 3 API
 * 
 * @return @b int The maximum number of parts allowed in each collision objects.
 */
inline int b3_MaxBodyParts() {
  return ( bt_MaxBodyParts() );
}


/**
 * Maximum number of triangles/part in the Bullet 3 API
 * 
 * @return @b long The maximum number of triangles allowed in each collision objects.
 */
inline unsigned long b3_MaxTriangles() {
  return ( bt_MaxTriangles() );
}

};

#endif

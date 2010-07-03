Debbie A Cook/GD/USGS/DOI 
08/21/2007 05:36 PM	To
Kris J Becker/GD/USGS/DOI@USGS
cc

bcc

Subject
Fw: Bug fix for SMRIMG
	
	
	History:	
This message has been replied to.


----- Forwarded by Debbie A Cook/GD/USGS/DOI on 08/21/2007 05:36 PM -----
Nat Bachman <Nathaniel.Bachman@jpl.nasa.gov> 
08/21/2007 12:50 AM
Please respond to
Nathaniel.Bachman@jpl.nasa.gov
	
To
Debbie A Cook <dcook@usgs.gov>, Charles Acton <Charles.H.Acton-Jr@jpl.nasa.gov>, Boris 
Semenov <Boris.Semenov@jpl.nasa.gov>, Edward Wright <Edward.D.Wright@jpl.nasa.gov>, Nat 
Bachman <Nathaniel.Bachman@jpl.nasa.gov>
cc

Subject
Bug fix for SMRIMG
	

	



Hi Debbie,

It turned out that there was a bug in SMRIMG: the effect of
the target body rotation on the smear velocity was not
handled properly due to a coding error (the derivation
in the comments was correct, but the code didn't match
the comments). The effect is small enough so that the
original version of the routine produces "reasonable"
results when the target body is the Moon. Cranking up
the target body rotation rate by a few orders of
magnitude makes the presence of the bug obvious.

Anyway, I've attached a fixed version of the routine.

 -Nat

C$Procedure      SMRIMG ( Smear image )

      SUBROUTINE SMRIMG ( STARG,
     .                    SURFX,
     .                    TICAM,
     .                    AV,
     .                    TSIPM,
     .                    FOCLEN,
     .                    PXLSCL,
     .                    EXPLEN,
     .                    SMRLEN,
     .                    SMRAZ,
     .                    FOUND    )

      IMPLICIT NONE

C$ Abstract
C
C     Determine the smear in an image due to observer and target
C     motion.
C
C$ Disclaimer
C
C     THIS SOFTWARE AND ANY RELATED MATERIALS WERE CREATED BY THE
C     CALIFORNIA INSTITUTE OF TECHNOLOGY (CALTECH) UNDER A U.S.
C     GOVERNMENT CONTRACT WITH THE NATIONAL AERONAUTICS AND SPACE
C     ADMINISTRATION (NASA). THE SOFTWARE IS TECHNOLOGY AND SOFTWARE
C     PUBLICLY AVAILABLE UNDER U.S. EXPORT LAWS AND IS PROVIDED "AS-IS"
C     TO THE RECIPIENT WITHOUT WARRANTY OF ANY KIND, INCLUDING ANY
C     WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR FITNESS FOR A
C     PARTICULAR USE OR PURPOSE (AS SET FORTH IN UNITED STATES UCC
C     SECTIONS 2312-2313) OR FOR ANY PURPOSE WHATSOEVER, FOR THE
C     SOFTWARE AND RELATED MATERIALS, HOWEVER USED.
C
C     IN NO EVENT SHALL CALTECH, ITS JET PROPULSION LABORATORY, OR NASA
C     BE LIABLE FOR ANY DAMAGES AND/OR COSTS, INCLUDING, BUT NOT
C     LIMITED TO, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND,
C     INCLUDING ECONOMIC DAMAGE OR INJURY TO PROPERTY AND LOST PROFITS,
C     REGARDLESS OF WHETHER CALTECH, JPL, OR NASA BE ADVISED, HAVE
C     REASON TO KNOW, OR, IN FACT, SHALL KNOW OF THE POSSIBILITY.
C
C     RECIPIENT BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF
C     THE SOFTWARE AND ANY RELATED MATERIALS, AND AGREES TO INDEMNIFY
C     CALTECH AND NASA FOR ALL THIRD-PARTY CLAIMS RESULTING FROM THE
C     ACTIONS OF RECIPIENT IN THE USE OF THE SOFTWARE.
C
C$ Required_Reading
C
C     None.
C
C$ Keywords
C
C     GEOMETRY
C
C$ Declarations
C
      DOUBLE PRECISION      STARG  ( 6 )
      DOUBLE PRECISION      SURFX  ( 3 )
      DOUBLE PRECISION      TICAM  ( 3, 3 )
      DOUBLE PRECISION      AV     ( 3 )
      DOUBLE PRECISION      TSIPM  ( 6, 6 )
      DOUBLE PRECISION      FOCLEN
      DOUBLE PRECISION      PXLSCL
      DOUBLE PRECISION      EXPLEN
      DOUBLE PRECISION      SMRLEN
      DOUBLE PRECISION      SMRAZ
      LOGICAL               FOUND
C
C$ Brief_I/O
C
C     Variable  I/O  Description
C     --------  ---  --------------------------------------------------
C     STARG      I   Inertial target state.
C     SURFX      I   Surface intercept of camera boresight.
C     TICAM      I   Inertial-to-camera transformation matrix.
C     AV         I   Angular velocity of camera (radians/second).
C     TSIPM      I   Inertial-to-bodyfixed state transformation matrix.
C     FOCLEN     I   Camera focal length.
C     PXLSCL     I   Camera pixel scale, (pixels/mm).
C     EXPLEN     I   Exposure length (seconds).
C     SMRLEN     O   Length of smear (pixels).
C     SMRAZ      O   Azimuth of smear (degrees).
C     FOUND      O   Found flag.
C
C$ Detailed_Input
C
C     STARG          is the aberration-corrected state of the target as
C                    seen from observer's position.
C
C                    The state is given in inertial coordinates.
C
C                    The epoch of the state should be the midpoint of
C                    the interval during which the camera's shutter was
C                    open.
C
C     SURFX          is the surface intercept point of the camera's
C                    boresight ray with the surface of the target body.
C                    The position of the target should be aberration-
C                    corrected, and the orientation of the target should
C                    be corrected for light time.  SURFX is given in
C                    body-fixed coordinates.
C
C     TICAM          is a 3x3 matrix that transforms vectors from
C                    inertial to camera coordinates.
C
C     AV             is the camera's angular velocity in inertial
C                    coordinates.  The reference frame must be the
C                    same as that used for BORE. The epoch at which AV
C                    should be obtained is the epoch of the image.
C
C     TSIPM          is the 6x6 matrix that transforms states from
C                    inertial to camera coordinates.  TSIPM is used to
C                    find the state of the camera's boresight vector
C                    in body-fixed coordinates.
C
C                    The epoch at which this transformation matrix
C                    should be evaluated is the epoch of the image minus
C                    the one-way light time from the target to the
C                    observer.
C
C     FOCLEN         is the focal length of the camera in millimeters.
C
C     PXLSCL         is the pixel scale in units of pixels/millimeter.
C                    It is assumed that the camera has square pixels.
C
C     EXPLEN         is the duration of the exposure for which the
C                    smear is to be computed.  Units are seconds.
C
C$ Detailed_Output
C
C     SMRLEN         is an estimate of the length of the smear at the
C                    center of the image defined by the input geometry
C                    and camera attributes.  The units are pixels.
C                    SMRLEN defined only if FOUND is TRUE.
C
C     SMRAZ          is the azimuth angle of the smear at the center of
C                    the image defined by the input geometry.  The units
C                    are degrees. SMRAZ defined only if FOUND is TRUE.
C
C     FOUND          is a logical flag that is set to .TRUE. if the
C                    smear could be computed, and otherwise, .FALSE.
C                    With valid inputs, the smear should be
C                    computable.  This flag is used in lieu of error
C                    checking on the inputs.
C
C$ Parameters
C
C     None.
C
C$ Exceptions
C
C     1)  In the interest of speed, this routine performs no error
C         checking.
C
C     2)  If the boresight intercept point defined by the inputs is on
C         or  too close to the target body's limb, FOUND will be
C         returned with the value .FALSE. and both SMRLEN and SMRAZ will
C         be unchanged.
C
C$ Files
C
C     None.
C
C$ Particulars
C
C     The definition of `smear' used by this routine is intended to
C     provide an estimate magnitude and direction of the smear in an
C     image taken by a camera.  The smear in question is that of
C     features of an extended target body; this routine does not
C     compute a smear estimate applicable to star images.
C
C     The amount of smear in an image is not generally exactly uniform
C     throughout the image; for simplicity, this routine estimates smear
C     at the image center.  The smear estimate takes into account the
C     following motions:
C
C        -- Center-of-mass motion of the observer.
C        -- Rotation of the camera with respect to inertial space.
C        -- Center-of-mass motion of the target.
C        -- Rotation of the target with respect to inertial space.
C
C     This routine computes an estimate of the length of the smear of
C     the image of a surface feature that appears in the center of the
C     image at the midpoint of the exposure; this normally should be a
C     good estimate of the length of the smear of any surface feature.
C     Additionally, the direction in which surface features appear to be
C     smeared in the image is estimated by the direction of the smear of
C     the image of this surface point.
C
C$ Examples
C
C     Maybe later.
C
C$ Restrictions
C
C     None.
C
C$ Literature_References
C
C     None.
C
C$ Author_and_Institution
C
C     N.J. Bachman    (JPL)
C
C$ Version
C
C-    Beta Version 1.1.0, 21-AUG-2007 (NJB)
C
C        Bug fix: body-fixed velocity of surface point is
C        now correctly converted to the inertial frame
C        defined by TSIPM.
C
C-    Beta Version 1.0.0, 18-JAN-1994 (NJB)
C
C-&

 
C$ Index_Entries
C
C     smear magnitude and direction
C
C-&

C$ Revisions
C
C-    Beta Version 1.1.0, 21-AUG-2007 (NJB)
C
C        Bug fix: body-fixed velocity of surface point is
C        now correctly converted to the inertial frame
C        defined by TSIPM.
C
C        In the computation of DVI, the code did not match
C        the derivation in the comments. The erroneous line
C        in step 1
C
C           CALL MXV ( DTIPM,    SURFX,  DVB    )
C
C        was changed to 
C
C           CALL MTXV ( DTIPM,   SURFX,  DVB    )
C
C-& 

C
C     SPICELIB functions
C

C
C     Local Parameters
C
      DOUBLE PRECISION      DPR
      DOUBLE PRECISION      VNORMG

C
C     Local variables
C
      DOUBLE PRECISION      DTICAM ( 3, 3 )
      DOUBLE PRECISION      DTIPM  ( 3, 3 )
      DOUBLE PRECISION      DVB    ( 3 )
      DOUBLE PRECISION      DVC    ( 3 )
      DOUBLE PRECISION      DVC1   ( 3 )
      DOUBLE PRECISION      DVC2   ( 3 )
      DOUBLE PRECISION      DVF    ( 2 )
      DOUBLE PRECISION      DVI    ( 3 )
      DOUBLE PRECISION      SMEAR  ( 2 )
      DOUBLE PRECISION      SURFXI ( 3 )
      DOUBLE PRECISION      TIPM   ( 3, 3 )
      DOUBLE PRECISION      VC     ( 3 )
      DOUBLE PRECISION      VI     ( 3 )

      INTEGER               I
      INTEGER               J

C
C     No error handling.
C

C
C     Initialization
C
      FOUND = .FALSE.

C
C     We're going to compute the time derivative of the image of
C     the feature that appears at the image center at the input
C     epoch.
C
C     We'll use the following variables in the derivation:
C
C        V             Body-fixed location of feature.
C         B
C
C        V             Inertial vector from observer to feature.
C         I
C
C        V             V  expressed in camera frame.
C         C             I
C
C        V             Image of V  in focal plane.
C         F                      C
C
C        TRGPOS        Inertial vector from observer to target center.
C
C        TICAM         Transformation from inertial to camera
C                      coordinates.
C
C        TIPM          Transformation from inertial to body-fixed
C                      ("body equator and prime meridian")
C                      coordinates.
C
C        ET            Epoch of observation.
C
C        LT            One-way light time from target to observer.
C
C        F             Camera focal length.
C
C
C     The following relationships hold:
C
C                                     t
C        V (ET)  =  TRGPOS(ET) +  TIPM (ET-LT) * V
C         I                                       B
C
C
C        V (ET)  =  TIMCAM(ET) * V (ET)
C         C                       I
C
C                 f
C        V   =  ----- *  (  V (1),  V (2)  )
C         F     V (3)        C       C
C                C
C
C
C     Differentiating these expressions, we obtain:
C
C
C      d [ V  ]  |            d TRGPOS |
C           I    |     =      -------- |
C     ---------- |               dt    |ET
C         dt     | ET
C                                   t
C                            d [TIPM ] |
C                       +   ---------- |      *  ( 1 - d(LT)/dt)  *   V
C                               dt     |ET-LT                          B
C
C
C     which, assuming that light time varies slowly, yields the
C     approximate equality
C
C
C      d [ V  ]  |                                      t
C           I    |          d TRGPOS |           d [TIPM ] |
C     ---------- |     =    -------- |      +   ---------- |      *   V
C         dt     | ET          dt    |ET            dt     |ET-LT      B
C
C
C     We also have
C
C
C      d [ V  ]  |
C           C    |              d [TICAM]|
C     ---------- |     =        -------- |     *   V (ET)
C         dt     | ET              dt    |ET        I
C
C
C                                                  d [ V  ]|
C                                                       I  |
C                           +    TICAM(ET)     *   --------|
C                                                     dt   |ET
C
C     and finally
C
C      d [ V  ]  |                           d [ V (3)]|
C           F    |             -F                 C    |
C     ---------- |     =     -------   *    --------   |
C         dt     | ET              2             dt    |ET
C                             V (3)
C                              C
C
C                                      *   (  V (1),  V (2)  )
C                                              C       C
C
C
C                                        d [ (  V (1),  V (2)  ) ]
C                               F                C       C
C                         +  -------  * --------------------------
C                             V  (3)                dt
C                              C
C
C
C
C     We proceed to compute these derivatives.  First we'll need TIPM
C     and the time derivatives of TIPM and TICAM:
C
      DO I = 1, 3

         DO J = 1, 3
            TIPM (J,I)  =  TSIPM(J,  I)
            DTIPM(J,I)  =  TSIPM(3+J,I)
         END DO

      END DO


      CALL RAV2DR ( TICAM, AV, DTICAM )

C
C     Now for the main event.  Compute:
C
C     1) V  and the derivative of V , which we'll call DVI:
C         I                        I
C
      CALL MTXV ( TIPM,     SURFX,  SURFXI )
      CALL VADD ( STARG,    SURFXI, VI     )

      CALL MTXV ( DTIPM,    SURFX,  DVB    )
      CALL VADD ( STARG(4), DVB,    DVI    )

C
C     2) V  and the derivative of V , which we'll call DVC:
C         C                        C
C
      CALL MXV ( TICAM, VI, VC )

      CALL MXV  ( TICAM,  DVI,  DVC1 )
      CALL MXV  ( DTICAM, VI,   DVC2 )
      CALL VADD ( DVC1,   DVC2, DVC  )

C
C     Make sure that V  can be computed:
C                     F

      IF ( VC(3) .EQ. 0.D0 ) THEN
         RETURN
      END IF

C
C     3) Compute the derivative of V , which we'll call DVF:
C                                   F
C
      CALL VLCOMG (  2,   -FOCLEN * DVC(3) / VC(3)**2,   VC,
     .                     FOCLEN          / VC(3),      DVC,    DVF  )


C
C     DVF has units of mm/sec.  Scale by pixels/mm and multiply by
C     the exposure length to obtain the smear vector.
C
      CALL VSCLG ( PXLSCL * EXPLEN,  DVF,  2,  SMEAR )

C
C     Compute the norm and azimuth angle of this smear vector.
C
      SMRLEN  =  VNORMG ( SMEAR, 2 )

      IF ( SMRLEN .EQ. 0.D0 ) THEN

         SMRAZ  =  0.D0

      ELSE

         SMRAZ  =  -DATAN2 ( SMEAR(2), SMEAR(1) ) * DPR()

         IF ( SMRAZ .LT. 0.D0 ) THEN
            SMRAZ  =  SMRAZ + 360.D0
         END IF

      END IF


      FOUND = .TRUE.

      RETURN
      END



Debbie A Cook/GD/USGS/DOI 
08/20/2007 06:28 PM	To
Kris J Becker/GD/USGS/DOI@USGS
cc

bcc

Subject
Fw: Image smear magnitude and azimuth
	
		
		
		----- Forwarded by Debbie A Cook/GD/USGS/DOI on 08/20/2007 06:22 PM -----
		Nat Bachman <Nathaniel.Bachman@jpl.nasa.gov> 
08/18/2007 06:59 PM
Please respond to
Nathaniel.Bachman@jpl.nasa.gov
	
To
Debbie A Cook <dcook@usgs.gov>
cc
Charles Acton <Charles.H.Acton-Jr@jpl.nasa.gov>, Boris Semenov 
<Boris.Semenov@jpl.nasa.gov>, Edward Wright <Edward.D.Wright@jpl.nasa.gov>, Nat Bachman 
<Nathaniel.Bachman@jpl.nasa.gov>
Subject
Image smear magnitude and azimuth
	

	

Kris,

I'm sorry to pass on this bad news, but it looks like these parameters need to be 
revisited.  I was in late today, but I will try to catch you Tuesday to discuss this some 
more.

 Debbie

Hi Debbie,

You probably won't be excited to hear this, but---I think you
need to take a hard look at that PICGEO smear routine SMRIMG before 
using it.
If you do use it, you may need to modify the outputs to get the correct
azimuth origin and sign.

The computation that SMRIMG performs was based on my idea of what a 
reasonable
smear definition was---this predated the SIS that you cited. The Clementine
EDR SIS in fact uses a significantly different definition.

Here are the basic technical points I think need to be considered:

1) SMRIMG works with an input "camera" reference frame whose +Z axis
   is assumed to point to a surface point on the target body. Smear
   azimuth is computed as a clockwise rotation about the +Z axis,
   measured from the +X axis.

   Depending on instrument characteristics, the +X and +Y axes of
   this frame may happen to coincide with, respectively, the +sample
   and -line directions in a photo, or they may not. This relationship
   simply must be checked, and azimuth must be adjusted accordingly.
   My guess is that you usually *will* need to adjust the azimuth
   direction.

2) SMRIMG computes the smear of the projection of a fixed surface point
   on the X-Y plane of the frame defined by TICAM. The "direction" of
   the smear points in the direction of the velocity of the point's image
   on the camera's X-Y plane. Imagine a point light source fixed on
   the target body's surface; the smear direction points from the
   image of the light source at shutter open to the image of the
   source at shutter close.

   The EDR SIS, on the other hand, considers the smear direction to
   be the direction of the projection onto the camera's X-Y plane
   of the velocity of the boresight intercept. Imagine the boresight
   spray-painting the target as it travels along the target
   surface; the velocity vector points from the surface point
   at the start of the paint trail to that at the end. So the EDR
   SIS definition calls for a smear direction that points in the
   opposite direction of that found by SMRIMG, all other factors
   being equal.

3) SMRIMG was intended, as the header says, to be used in the
   context of a specific program, namely PICGEO. SMRIMG does
   basically no error checking. So your software needs to make
   sure that SMRIMG is fed reasonable inputs.

4) You need to decide whether camera angular velocity should be
   used or zeroed out. Boris has observed that angular velocity
   data from CKs can be garbage---it depends on the situation.
   As you can imagine, smear is very sensitive to camera angular 
   velocity, so bad angular velocity data can invalidate the
   results.

I personally feel that my own smear definition is superior to
that of the EDR SIS, because 1) it doesn't depend on any
model of the target body's surface (in particular, it applies
without modification to plate models and DEMs) and 2) it's computationally
simpler than what's prescribed by the EDR SIS, even if the target is
assumed to be an ellipsoid. This is not to say that you should use the
SMRIMG code: it's probably not useful to you have a routine
that I prefer but that doesn't implement the specifications
you're using. :) In any case, a SPICE-quality smear routine
would rely on frame kernels to define both the camera frame and the
photo frame, so the correct smear sign and offset could be automatically
determined by the routine. It would also have robust error checking.

Now, about software implementing the EDR SIS smear definition: NAIF
did create a routine called SMEARV_M, originally intended for use by 
Mars Observer,
I think, that does part of this computation: it finds a boresight intercept
and the velocity of that intercept on the target surface. This routine
predates SRFXPT by many years; it doesn't do accurate aberration corrections
for a surface point, and it presumes that TISBOD can be used to find
the inertial-to-bodyfixed reference frame transformation for the target,
which means it can't be used with body-fixed frames other than the
IAU_* defaults. However, if you like, you could modify this routine to
make it do what you want. The pure math routine it calls, SURFPV_M, I
think is carefully written; it may be useful even if you prefer to ignore
SMEARV_M.

I'm attaching the source code of the *_M routines to this e-mail. SMRIMG
is included for reference.

 -Nat

















C$Procedure      SMEARV_M ( Smear velocity )
 
      SUBROUTINE SMEARV_M ( TARGET,
     .                      OBSRVR,
     .                      ET,
     .                      U,
     .                      DU,
     .                      REFIN,
     .                      ABCORR,
     .                      SP,
     .                      DSP,
     .                      FOUND   )
 
C$ Abstract
C
C     Determine the instantaneous velocity of a surface intercept
C     vector with respect to the surface of the target body at the
C     intercept point. Also find the coordinates of the intercept point.
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
 
      INTEGER               TARGET
      INTEGER               OBSRVR
      DOUBLE PRECISION      ET
      DOUBLE PRECISION      U      ( 3 )
      DOUBLE PRECISION      DU     ( 3 )
      CHARACTER*(*)         REFIN
      CHARACTER*(*)         ABCORR
      DOUBLE PRECISION      SP     ( 3 )
      DOUBLE PRECISION      DSP    ( 3 )
      LOGICAL               FOUND
 
C$ Brief_I/O
C
C     Variable  I/O  Description
C     --------  ---  --------------------------------------------------
C     TARGET     I   NAIF integer code of target body.
C     OBSRVR     I   NAIF integer code of observing body.
C     ET         I   Epoch at which the velocity is to be computed.
C     U          I   An inertially referenced vector.
C     DU         I   Velocity of U.
C     REFIN      I   Reference frame of U and DU.
C     ABCORR     I   Aberration correction.
C     SP         O   Location on the target body towards which U points.
C     DSP        O   Velocity of SP.
C     FOUND      O   True if SP exists and DSP can be computed.
C
C$ Detailed_Input
C
C     TARGET     is the NAIF integer code of a target body.
C                This routine assumes that this body is modelled
C                by a tri-axial ellipsoid, and that a PCK file
C                containing its radii and rotation model constants
C                has been loaded into the kernel pool.
C
C     OBSRVR     is the NAIF integer code of the observing body
C                which is normally a spacecraft.
C
C     ET         is the time, measured in ephemeris seconds past
C                J2000, at which the surface intercept point and its
C                velocity are to be computed.
C
C                Note that SPK ephemeris data covering the epoch ET
C                for the TARGET and OBSRVR must be loaded via SPKLEF
C                before calling SMEARV_M.
C
C     U          is the inertially referenced pointing or line-of-
C                sight vector of an instrument on the observing body
C                at ET.  U is time-varying, but the length of U is
C                constant. Normally, the ray emanating from the
C                observing body in the direction of U intersects
C                the target body.
C
C     DU         is the inertially referenced velocity of the pointing
C                vector U at ET.  Note that if AV is the angular
C                velocity vector, then DU is the cross product of AV
C                and U:
C
C                     DU =  AV  X  U
C
C                The angular velocity vector is the vector whose
C                direction gives the axis about which an instrument-
C                fixed reference frame is rotating, and whose
C                magnitude is equal to the magnitude of the rotation
C                velocity in radians per second.  (In the SPICE system,
C                angular velocity vectors are stored in CK files.)
C
C     REFIN      is the name of the inertial frame to which U and DU
C                are referenced.  This must be a frame supported
C                by the SPICELIB subroutine CHGIRF, such as 'J2000'.
C
C     ABCORR     indicates the aberration corrections to be applied
C                in order to determine the state of the target body.
C                ABCORR may be any of the following; case and spacing
C                are not significant.
C
C                   'NONE'     Apply no correction. Return the true
C                              state of the target body relative to
C                              the observer.
C
C                   'LT'       Correct for planetary (light time)
C                              aberration.
C
C                   'LT+S'     Correct for planetary and stellar
C                              aberrations.
C
C$ Detailed_Output
C
C     SP,
C     DSP        are, respectively, the body-fixed coordinates of the
C                surface intercept point, and its instantaneous
C                velocity vector at time ET.
C
C                That is, if the ray with direction vector U,
C                emanating from the position of the observer,
C                intersects the target, AND if the velocity of
C                the intercept point is small enough to be computed,
C                then SP will be the body-fixed coordinates of the
C                intercept point, and DSP will be the instantaneous
C                velocity of that point on the surface of the target
C                body.  This velocity vector lies in the plane that
C                is tangent to the target body at SP.
C
C                If the ray in the direction of U misses the target,
C                or if the velocity of the intercept point is too large
C                to be computed, as when the intercept point is on the
C                limb where its velocity is infinite, SP and DSP are
C                undefined.
C
C                In the body-fixed coordinate system, the center of
C                the ellipsoid model of the target body is at the
C                origin, and the semi-axes of the ellipsoid are
C                aligned with the x, y, and z-axes.  SP and DSP
C                are referenced to this body-fixed frame.
C
C     FOUND      is a logical flag that is set to .TRUE. if both SP
C                exists and DSP can be computed, and otherwise, .FALSE.
C
C$ Parameters
C
C     None.
C
C$ Exceptions
C
C     1) If OBSRVR is equal to TARGET, the error
C        SPICE(BODIESNOTDISTINCT) is signalled.
C
C     2) If the pointing vector U is the zero vector, then
C        a subroutine that SMEARV_M calls will diagnose and
C        signal an error.
C
C     3) If the reference frame REFIN is not supported by the
C        current version of CHGIRF, an error is diagnosed and
C        signalled by a routine that SMEARV_M calls.
C
C     4) If the value of ABCORR is not recognized, an error is
C        diagnosed and signalled by a routine that SMEARV_M calls.
C
C     5) If no SPK ephemeris file has been loaded prior to calling
C        SMEARV_M, or if the SPK data has insufficient coverage, an
C        error will be diagnosed and signalled by a routine that
C        SMEARV_M calls.
C
C     6) If a PCK file containing the radii and rotation model constants
C        of the target body has not been loaded prior to calling
C        SMEARV_M, an error will be diagnosed and signalled by a
C        routine that SMEARV_M calls.
C
C     7) If SP is on or too close to the limb, FOUND will be returned
C        with the value .FALSE. and both SP and DSP will be unchanged.
C
C$ Files
C
C     No files are input to this routine, however, SMEARV_M expects
C     that the appropriate SPK and PCK files have been loaded using
C     SPKLEF and LDPOOL, respectively.
C
C$ Particulars
C
C     SMEARV_M determines the state of the observing body with respect
C     to the target body at the requested time.  It computes the
C     intercept point of a ray emanating from the position of the
C     observing body in the direction of the pointing vector, U,
C     with the target body.
C
C     SMEARV_M also returns the velocity vector of that intercept point
C     which is tangent to the target body's surface at the intercept
C     point.  This velocity includes the effects of the velocity of the
C     observer, the velocity of the pointing vector, the rotation of
C     the target body, and the light-time between the observing and
C     the target bodies.
C
C$ Examples
C
C     The following code example computes the coordinates and
C     velocity of the point on Mars where the boresight ray of the
C     Mars Observer camera intersects the surface at a given
C     spacecraft clock time.
C
C     The following data files are needed:
C
C        1) A Mars Observer C-kernel file.
C        2) An SPK file containing ephemeris data for MO and Mars.
C        3) An MOC I-kernel file.
C        4) A Mars Observer SCLK file.
C        5) A planetary constants kernel file.
C
C     Light time correction will be used when computing the state of
C     Mars relative to Mars Observer.
C
C           INTEGER               FILEN
C           PARAMETER           ( FILEN  = 128     )
C
C           INTEGER               TIMLEN
C           PARAMETER           ( TIMLEN = 40      )
C
C           INTEGER               MO
C           PARAMETER           ( MO     = -94     )
C
C           INTEGER               MOC
C           PARAMETER           ( MOC    = -94030  )
C
C           INTEGER               MARS
C           PARAMETER           ( MARS   = 499     )
C
C           CHARACTER*(*)         ABCORR
C           PARAMETER           ( ABCORR = 'LT'    )
C
C           CHARACTER*(*)         REF
C           PARAMETER           ( REF    = 'J2000' )
C
C           CHARACTER*(FILEN)     BSPFIL
C           CHARACTER*(FILEN)     BCFIL
C           CHARACTER*(FILEN)     TSCFIL
C           CHARACTER*(FILEN)     TPCFIL
C           CHARACTER*(FILEN)     TIFIL
C           CHARACTER*(TIMLEN)    SCLKCH
C           CHARACTER*(TIMLEN)    TOLCH
C
C           INTEGER               BCHAN
C           INTEGER               BSPHAN
C
C           DOUBLE PRECISION      AV     ( 3 )
C           DOUBLE PRECISION      CLKOUT
C           DOUBLE PRECISION      DSP    ( 3 )
C           DOUBLE PRECISION      DU     ( 3 )
C           DOUBLE PRECISION      ET
C           DOUBLE PRECISION      SP     ( 3 )
C           DOUBLE PRECISION      U      ( 3 )
C
C           LOGICAL               FOUND
C     C
C     C     SPICELIB functions
C     C
C           DOUBLE PRECISION      VNORM
C
C     C
C     C     Get all the data files and load them.
C     C
C           WRITE (*,*) 'Enter name of MO SPK file'
C           READ  (*,FMT='(A)') BSPFIL
C
C           WRITE (*,*) 'Enter name of MO CK file'
C           READ  (*,FMT='(A)') BCFIL
C
C           WRITE (*,*) 'Enter name of MO SCLK file'
C           READ  (*,FMT='(A)') TSCFIL
C
C           WRITE (*,*) 'Enter name of MOC I-kernel file'
C           READ  (*,FMT='(A)') TIFIL
C
C           WRITE (*,*) 'Enter name of planetary constants kernel file'
C           READ  (*,FMT='(A)') TPCFIL
C
C           CALL SPKLEF ( BSPFIL, BSPHAN )
C           CALL CKLPF  ( BCFIL,  BCHAN  )
C           CALL LDPOOL ( TSCFIL         )
C           CALL LDPOOL ( TIFIL          )
C           CALL LDPOOL ( TPCFIL         )
C
C     C
C     C     At what time and tolerance do we need the pointing?
C     C
C           WRITE (*,*) 'Enter the SCLK time for which you wish ' //
C                       'to find the velocity of the point where'
C           WRITE (*,*) 'the MOC''s boresight intersects the surface'
C           READ  (*,FMT='(A)') SCLKCH
C
C           WRITE (*,*) 'Enter C-kernel tolerance'
C           READ  (*,FMT='(A)') TOLCH
C
C     C
C     C     Get the MOC's boresight vector and its angular velocity
C     C     vector; the MOSPICE routine INSVAV_M returns this given
C     C     an instrument ID, SCLK time and tolerance, and an inertial
C     C     reference frame. The inertial reference frame, REF, in this
C     C     case, is 'J2000'.
C     C
C           CALL INSVAV_M ( MOC,
C          .                SCLKCH,
C          .                TOLCH,
C          .                REF,
C          .                U,
C          .                AV,
C          .                CLKOUT,
C          .                FOUND )
C
C           CALL SCT2E  ( MO, CLKOUT, ET  )
C
C      C
C      C    Compute the velocity of U, the boresight vector.
C      C
C           CALL VCRSS ( AV,   U, DU )
C
C      C
C      C    Now we have all of the inputs.  Compute the smear velocity.
C      C
C           CALL SMEARV_M ( MARS,
C          .                MO,
C          .                ET,
C          .                U,
C          .                DU,
C          .                REF,
C          .                ABCORR,
C          .                SP,
C          .                DSP,
C          .                FOUND   )
C
C     C
C     C     Write the results.
C     C
C           WRITE (*,*) 'Mars body fixed coordinates where the ' //
C          .            'MOC''s boresight vector intersects the'
C           WRITE (*,*) 'surface:'
C           WRITE (*,*)
C           WRITE (*,*) '        x = ', SP (1)
C           WRITE (*,*) '        y = ', SP (2)
C           WRITE (*,*) '        z = ', SP (3)
C
C           WRITE (*,*) 'Velocity of the boresight-surface '     //
C          .            'intercept point:'
C           WRITE (*,*)
C           WRITE (*,*) '    dx/dt = ', DSP (1)
C           WRITE (*,*) '    dy/dt = ', DSP (2)
C           WRITE (*,*) '    dz/dt = ', DSP (3)
C           WRITE (*,*) 'magnitude = ', VNORM ( DSP)
C
C           END
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
C     J.E. McLean    (JPL)
C     M.J. Spencer   (JPL)
C
C$ Version
C
C-    MOSPICE Version 1.0.1, 10-MAR-1992 (WLT)
C
C        Comment section for permuted index source lines was added
C        following the header.
C
C-    MOSPICE Version 1.0.0, 05-SEP-1990 (MJS)
C
C        This subroutine is an MO version of the GLLSPICE subroutine
C        SMEARV_G, version 1.0.0, dated 06-NOV-1990, by JEM. The
C        following changes were performed:
C
C           1) The name of the module was changed from SMEARV_G to
C              SMEARV_M;
C           2) A new argument was added to the calling sequence:
C              ABCORR, the aberration correction that is to be applied
C              when computing the state of the target body. Previously,
C              the type of aberration correction applied was hard-
C              coded; 'LT', light time, was used.  If this argument
C              equals 'NONE', the epoch passed to TISBOD is ET;
C              otherwise, ET-LT. Since this argument must be checked
C              locally, ABCORR is compressed and placed in the local
C              variable CORR; CORR, then, is converted to upper case.
C           3) The names of all the GLLSPICE routines mentioned in this
C              module were changed to their MOSPICE equivalents
C              (SURFPV_G was changed to SURFPV_M);
C           4) The examples section was re-done -- instead of
C              computing the smear velocity vector on Earth in an SSI
C              picture, the coordinates and the velocity of the point
C              on Mars where the MOC boresight vector intersects the
C              surface is computed.
C
C-&
 
C$ Index_Entries
C
C     smear velocity
C
C-&
 
 
C
C     SPICELIB functions
C
      LOGICAL               RETURN
 
C
C     Local Parameters
C
C     A state is a 6 dimensional array.  The first three components
C     represent a position, the second three represent the time
C     derivative of that position.  POS and VEL are indices into
C     a state array.
C
      INTEGER               POS
      PARAMETER           ( POS   = 1 )
 
      INTEGER               STDIM
      PARAMETER           ( STDIM = 6 )
 
      INTEGER               VEL
      PARAMETER           ( VEL   = 4 )
 
C
C     Local variables
C
      CHARACTER*(5)         CORR
 
      DOUBLE PRECISION      LT
      DOUBLE PRECISION      RADII  ( 3 )
      DOUBLE PRECISION      STIBF  ( STDIM )
      DOUBLE PRECISION      STOBF  ( STDIM )
      DOUBLE PRECISION      STOI   ( STDIM )
      DOUBLE PRECISION      STUBF  ( STDIM )
      DOUBLE PRECISION      STUI   ( STDIM )
      DOUBLE PRECISION      STTI   ( STDIM )
      DOUBLE PRECISION      TSIBF  ( STDIM, STDIM )
 
      INTEGER               NRADII
 
C
C     Standard SPICE error handling.
C
      IF ( RETURN () ) THEN
         RETURN
      ELSE
         CALL CHKIN ( 'SMEARV_M' )
      END IF
 
C
C     Initialization
C
      FOUND = .FALSE.
 
C
C     Remove all spaces from ABCORR and place the resulting string
C     in CORR; then, convert CORR to upper case.
C
      CALL CMPRSS ( ' ',  0, ABCORR, CORR )
      CALL UCASE  ( CORR,   CORR )
 
C
C     Check the input body codes.  If they are equal, signal
C     an error.
C
      IF ( OBSRVR .EQ. TARGET ) THEN
 
         CALL SETMSG ( 'In computing smear velocity, the observing ' //
     .                 'and the target body are the same.' )
         CALL SIGERR ( 'SPICE(BODIESNOTDISTINCT)' )
         CALL CHKOUT ( 'SMEARV_M' )
         RETURN
 
      END IF
 
C
C     Get the state of the target body, using the requested aberration
C     correction, as seen from the observing body at ET in the same
C     inertial frame as the pointing vectors.
C
      CALL SPKEZ ( TARGET, ET, REFIN, CORR, OBSRVR, STTI, LT )
 
C
C     Negate STTI so that it is the inertially referenced state of
C     the observer as seen by the target.
C
      CALL VMINUG ( STTI, STDIM, STOI )
 
C
C     Now we need the state of the observer in target body-fixed
C     coordinates, STOBF:
C
C     Let POI and VOI be the position and velocity vectors of the
C     observer in inertial coordinates.  That is STOI = ( POI, VOI ).
C     Let TIBF be the 3x3 transformation matrix that rotates vectors
C     from inertial to body-fixed coordinates.  Then the position of
C     the observer in body-fixed coordinates is the product of TIBF and
C     POI:
C
C       POBF = TIBF * POI
C
C     The velocity of the observer in body-fixed coordinates is the
C     derivative of POBF,
C
C                d POBF    d (TIBF * POI)
C       VOBF  =  ------ =  -------------- = DTIBF * POI + TIBF * VOI
C                  dt          dt
C
C     where DTIBF is the derivative of the transformation matrix TIBF.
C     Thus the following 6x6 matrix, TSIBF, when multiplied by STOI,
C     transforms the entire state to body-fixed coordinates.
C
C                        +-           -+   +-   -+   +-    -+
C                        | TIBF     0  |   | POI |   | POBF |
C       TSIBF * STOI  =  |             | * |     | = |      | = STOBF
C                        | DTIBF  TIBF |   | VOI |   | VOBF |
C                        +-           -+   +-   -+   +-    -+
C
C
C     TISBOD generates this 6x6 matrix, TSIBF, to transform an inertial
C     state to body-fixed coordinates at a particular epoch. If we are
C     taking into account light time corrections, our epoch must be
C     ET-LT.
C
      IF ( CORR .EQ. 'NONE' ) THEN
 
         CALL TISBOD ( REFIN, TARGET, ET,    TSIBF  )
 
      ELSE
 
         CALL TISBOD ( REFIN, TARGET, ET-LT, TSIBF  )
 
      ENDIF
 
      CALL MXVG   ( TSIBF, STOI, STDIM, STDIM, STOBF )
 
C
C     We also need U and DU in body-fixed coordinates.  Pack
C     U and DU into a state, STUI, and multiply by TSIBF.
C
      CALL VEQU (  U, STUI ( POS ) )
      CALL VEQU ( DU, STUI ( VEL ) )
 
      CALL MXVG ( TSIBF, STUI, STDIM, STDIM, STUBF )
 
C
C     Now we get the radii of the ellipsoid model of the target
C     body.
C
      CALL BODVAR ( TARGET, 'RADII', NRADII, RADII )
 
C
C     Finally, we call the geometry routine SURFPV_M, which
C     takes the body-fixed states of the observer and
C     pointing vector and the ellipsoid model, and
C     computes the intercept point and the velocity of
C     that point on the surface of the ellipsoid.
C
      CALL SURFPV_M ( STOBF, STUBF,
     .                RADII(1), RADII(2), RADII(3),
     .                STIBF, FOUND )
 
      IF ( .NOT. FOUND ) THEN
         CALL CHKOUT ( 'SMEARV_M' )
         RETURN
      END IF
 
C
C     STIBF, is the body-fixed state of the intercept point.
C
      CALL VEQU ( STIBF ( POS ),  SP )
      CALL VEQU ( STIBF ( VEL ), DSP )
 
      CALL CHKOUT ( 'SMEARV_M' )
      RETURN
      END
C$Procedure      SURFPV_M ( Surface point and velocity )
 
      SUBROUTINE SURFPV_M ( STO, STU, A, B, C, STI, FOUND )
 
C$ Abstract
C
C     Determine the intersection of a line-of-sight vector with the
C     surface of an ellipsoid and the velocity of the intercept
C     point on the surface.
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
C     ELLIPSOID
C     GEOMETRY
C
C$ Declarations
 
      DOUBLE PRECISION      STO    ( 6 )
      DOUBLE PRECISION      STU    ( 6 )
      DOUBLE PRECISION      A
      DOUBLE PRECISION      B
      DOUBLE PRECISION      C
      DOUBLE PRECISION      STI    ( 6 )
      LOGICAL               FOUND
 
C$ Brief_I/O
C
C     Variable  I/O  Description
C     --------  ---  --------------------------------------------------
C     STO        I   State of the observer in body-fixed coordinates
C     STU        I   State of a line-of-sight vector:  U and dU/dt.
C     A          I   Length of the ellipsoid semi-axis along the x-axis
C     B          I   Length of the ellipsoid semi-axis along the y-axis
C     C          I   Length of the ellipsoid semi-axis along the z-axis
C     STI        O   State of the intercept point on the ellipsoid.
C     FOUND      O   Can the state of the intercept point be determined?
C
C$ Detailed_Input
C
C     STO        is a 6-vector giving the body-fixed state of an
C                observer.  The first three elements are the body-fixed
C                coordinates of the position of the observer; the last
C                three elements are the components of the observer's
C                velocity in the body-fixed frame.  In the body-fixed
C                frame, the center of the ellipsoid is at the origin,
C                and the semi-axes of the ellipsoid are aligned with
C                the x, y, and z-axes.
C
C     STU        is a 6-vector giving the body-fixed state of a
C                line-of-sight vector.  The first three elements are
C                a vector, U, that presumably points in the direction
C                of the ellipsoid; the length of this vector is
C                constant. The last three elements are the velocity of
C                the vector, U; we'll call it DU.  So, STU = ( U, DU ).
C
C                Note that if AV is the angular velocity vector,
C                then DU is the cross product of AV and U:
C
C                     DU =  AV  X  U
C
C                The angular velocity vector is the vector whose
C                direction gives the axis about which an instrument-
C                fixed reference frame is rotating, and whose
C                magnitude is equal to the magnitude of the rotation
C                velocity in radians per second.  (In the SPICE system,
C                angular velocity vectors are stored in CK files.)
C
C     A          is the length of the semi-axis of the ellipsoid
C                that is parallel to the x-axis of the body-fixed
C                coordinate system.
C
C     B          is the length of the semi-axis of the ellipsoid
C                that is parallel to the y-axis of the body-fixed
C                coordinate system.
C
C     C          is the length of the semi-axis of the ellipsoid
C                that is parallel to the z-axis of the body-fixed
C                coordinate system.
C
C$ Detailed_Output
C
C     STI        is the body-fixed state of the surface intercept point,
C                if it exists.
C
C                That is, if the ray with direction vector U,
C                emanating from the position of the observer,
C                intersects the ellipsoid, AND if the velocity of
C                the intercept point is small enough to be computed,
C                then the first three elements of STI will be the
C                body-fixed coordinates of the first intercept
C                point, and the last three elements of STI will be the
C                instantaneous velocity of that point on the surface
C                of the ellipsoid.  This velocity vector lies in
C                the plane that is tangent to the ellipsoid at the
C                intercept point.
C
C                If there is no intersection, or if the velocity is
C                too large to be computed, as when the intercept point
C                is on the limb where its velocity is infinite, STI
C                is undefined.
C
C     FOUND      is a logical flag indicating whether or not the
C                state of the intercept point exists and can be
C                computed.  If STI is defined, FOUND will be
C                returned as .TRUE. otherwise it will be false.
C
C$ Parameters
C
C     None.
C
C$ Exceptions
C
C      1) If the input line-of-sight vector is the zero vector, that
C         is, if the first three elements of STU are all zero, then
C         a subroutine that SURFPV_M calls will diagnose and signal
C         an error.
C
C      2) If any of the body's axes, A, B, or C, is nonpositive,
C         a subroutine that SURFPV_M calls will diagnose and signal
C         an error.
C
C$ Files
C
C     None.
C
C$ Particulars
C
C     The position and velocity of the observer as well as the
C     line-of-sight vector and its velocity vary with time.  The
C     inputs to SURFPV_M may be considered the values of these
C     vector functions at a particular time, say t0.  Thus
C
C        State of observer:         STO = ( P(t0), P'(t0) )
C
C        State of pointing vector:  STU = ( U(t0), U'(t0) )
C
C     To determine the intercept point, I(t0), we simply compute the
C     geometrical intersection of the ray originating at P(t0)
C     in the direction of U(t0) with the ellipsoid
C
C                    2      2      2
C                   x      y      z
C                  --- +  --- +  ---  =  1
C                    2      2      2
C                   A      B      C
C
C     I(t) is the path of the intercept point along the surface of
C     the ellipsoid.  To determine the velocity of the intercept point,
C     we need to take the time derivative of I(t), and evaluate it at
C     t0.  Unfortunately I(t) is a complicated expression, and its
C     derivative is even more complicated.
C
C     However, we know that the derivative of I(t) at t0, I'(t0),
C     is tangent to I(t) at t0.  Thus I'(t0) lies in the plane that
C     is tangent to the ellipsoid at t0.  Let J(t) be the curve in the
C     in the tangent plane that represents the intersection of the
C     ray emanating from P(t0) with direction U(t0) with that tangent
C     plane.
C
C                J'(t0) = I'(t0).
C
C     The expression for J'(t) is much simpler than that of I'(t);
C     SURFPV_M evaluates J'(t) at t0.
C
C
C     Derivation of J(t) and J'(t)
C     ----------------------------------------------------------------
C
C     I(t0) is the intercept point.  Let N be a surface normal at I(t0).
C     Then the tangent plane at I(t0) is the set of points X such that
C
C                 < X - I(t0), N > = 0.
C
C     J(t) can be expressed as the vector sum of the position of the
C     observer and some scalar multiple of the line-of-sight vector,
C
C                 J(t) = P(t) + s(t) * U(t),
C
C     where s(t) is a scalar function of time.  The derivative of
C     J(t) is given by
C
C                 J'(t) = P'(t) + s(t) * U'(t) + s'(t) * U(t).
C
C     We have P(t0), P'(t0), U(t0), U'(t0), I(t0), and N, but to
C     evaluate J'(t0), we need s(t0) and s'(t0).  We derive an
C     expression for s(t) as follows.
C
C     Because J(t) is in the tangent plane, it must satisfy
C
C                 < J(t) - I(t0), N > = 0.
C
C     Substituting the expression for J(t) into the equation above
C     gives
C
C                 < P(t) + s(t) * U(t) - I(t0), N > = 0.
C
C     Thus
C
C                 < P(t) - I(t0), N > + s(t) * < U(t), N > = 0,
C
C     and
C                         - < P(t) - I(t0), N >
C                 s(t)  =   --------------------
C                               < U(t), N >
C
C     The derivative of s(t) is given by
C
C                 s'(t) =
C
C   - (  < U(t),N > * < P'(t),N >  -  < P(t)-I(t0),N > * < U'(t),N >  )
C     -----------------------------------------------------------------
C                                        2
C                             < U(t), N >
C
C
C$ Examples
C
C     This code example shows how the routine SMEARV_M uses SPKEZ to
C     get a state vector and then transforms it to body-fixed
C     coordinates for input to SURFPV_M.  The output from SURFPV_M
C     and SMEARV_M are the same.  SMEARV_M is really a high level
C     interface to the geometry routine SURFPV_M.
C
C     In this example, we assume U and DU are the direction and
C     velocity of the line-of-sight vector and are referenced to
C     the inertial frame, REFIN.  Also, we assume that the
C     appropriate SPK and PCK files have been loaded.
C
C     C
C     C     Get the state of the target body as seen from the
C     C     observing body at time ET in the same inertial frame as the
C     C     pointing vectors.  The argument ABCORR indicates the
C     C     aberration corrections to be applied in order to determine
C     C     this state.  ABCORR is passed as an argument to SMEARV_M; it
C     C     may be any of the following.
C     C
C     C        'NONE'     Apply no correction. Return the true
C     C                   state of the target body relative to
C     C                   the observer.
C     C
C     C        'LT'       Correct for planetary (light time)
C     C                   aberration.
C     C
C     C        'LT+S'     Correct for planetary and stellar
C     C                   aberrations.
C     C
C           CALL SPKEZ ( TARGET, ET, REFIN, ABCORR, OBSRVR, STTI, LT )
C
C     C
C     C     Negate STTI so that it is the inertially referenced state
C     C     of the observer as seen by the target.
C     C
C           CALL VMINUG ( STTI, 6, STOI )
C
C     C
C     C     Now we need the state of the observer in target body-fixed
C     C     coordinates, STOBF.  TISBOD generates the 6x6 matrix TSIBF
C     C     that transforms an inertial state to body-fixed coordinates
C     C     at a particular epoch. If we are taking into account light
C     C     time corrections, our epoch must be ET-LT.
C     C
C           IF ( ABCORR .EQ. 'NONE' ) THEN
C
C              CALL TISBOD ( REFIN, TARGET, ET,    TSIBF  )
C
C           ELSE
C
C              CALL TISBOD ( REFIN, TARGET, ET-LT, TSIBF  )
C
C           ENDIF
C
C           CALL MXVG   ( TSIBF, STOI, 6, 6, STOBF )
C
C     C
C     C     We also need U and DU in body-fixed coordinates.  Pack
C     C     U and DU into a state, STUI, and multiply by TSIBF.
C     C
C           CALL VEQU (  U, STUI ( 1 ) )
C           CALL VEQU ( DU, STUI ( 4 ) )
C
C           CALL MXVG ( TSIBF, STUI, 6, 6, STUBF )
C
C     C
C     C     Now we get the radii of the ellipsoid model of the target
C     C     body from the kernel pool.
C     C
C           CALL BODVAR ( TARGET, 'RADII', NRADII, RADII )
C
C     C
C     C     Finally, we call the geometry routine SURFPV_M, which
C     C     takes the body-fixed states of the observer and
C     C     pointing vector and the ellipsoid model, and
C     C     computes the intercept point and the velocity of
C     C     that point on the surface of the ellipsoid.
C     C
C           CALL SURFPV_M ( STOBF, STUBF,
C          .                RADII(1), RADII(2), RADII(3),
C          .                STIBF, FOUND )
C
C     C
C     C     STIBF, is the body-fixed state of the intercept point.
C     C
C           IF ( FOUND ) THEN
C
C              WRITE (*,*) 'Intercept point:'
C              WRITE (*,*) '                   x = ', STIBF ( 1 )
C              WRITE (*,*) '                   y = ', STIBF ( 2 )
C              WRITE (*,*) '                   z = ', STIBF ( 3 )
C              WRITE (*,*) '               dx/dt = ', STIBF ( 4 )
C              WRITE (*,*) '               dy/dt = ', STIBF ( 5 )
C              WRITE (*,*) '               dz/dt = ', STIBF ( 6 )
C
C           END IF
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
C     J.E. McLean    (JPL)
C
C$ Version
C
C-    MOSPICE Version 1.0.1, 10-MAR-1992 (WLT)
C
C        Comment section for permuted index source lines was added
C        following the header.
C
C-    MOSPICE Version 1.0.0, 05-SEP-1991 (MJS)
C
C        This subroutine is an MO version of the GLLSPICE subroutine
C        SURFPV_G, version 1.0.0, dated 19-OCT-1990, by JEM. The name
C        of the routine was changed from SURFPV_G to SURFPV_M, and the
C        names of all the GLLSPICE routines mentioned in this module
C        were changed to their MOSPICE equivalents (SMEARV_G was
C        changed to SMEARV_M). It was also noted in the example section
C        that the routine SMEARV_M accepts, as an argument, the
C        aberration correction that is to be applied in order to
C        compute the state of the target body; this was not mentioned
C        previously because the routine SMEARV_G did not accept an
C        aberration correction argument, rather, it assumed light time
C        correction.
C
C-&
 
C$ Index_Entries
C
C     surface point and velocity
C
C-&
 
 
C
C     SPICELIB functions
C
      LOGICAL               RETURN
 
      DOUBLE PRECISION      DPMAX
      DOUBLE PRECISION      VDOT
      DOUBLE PRECISION      VNORM
 
 
C
C     Local variables
C
      DOUBLE PRECISION      DP     ( 3 )
      DOUBLE PRECISION      DU     ( 3 )
      DOUBLE PRECISION      I      ( 3 )
      DOUBLE PRECISION      M
      DOUBLE PRECISION      N      ( 3 )
      DOUBLE PRECISION      P      ( 3 )
      DOUBLE PRECISION      PMI    ( 3 )
      DOUBLE PRECISION      S
      DOUBLE PRECISION      SECOND ( 3 )
      DOUBLE PRECISION      STUHAT ( 6 )
      DOUBLE PRECISION      THIRD  ( 3 )
      DOUBLE PRECISION      U      ( 3 )
 
 
 
C
C     Standard SPICE error handling.
C
      IF ( RETURN () ) THEN
         RETURN
      ELSE
         CALL CHKIN ( 'SURFPV_M' )
      END IF
 
 
C
C     Determine the ellipsoid surface intercept point of the
C     ray emanating from the observer in the direction of U.
C     We'll call it I and it will go in the first three
C     elements of STI once we determine the velocity.  If there
C     is no intersection, we check out.
C
C     SURFPT takes care of some error checking too.  It signals
C     an error if U is the zero vector or if A, B, or C are bad
C     axis lengths.
C
      CALL SURFPT ( STO, STU, A, B, C, I, FOUND)
 
      IF ( .NOT. FOUND ) THEN
         CALL CHKOUT ( 'SURFPV_M' )
         RETURN
      END IF
 
C
C     U is useful for its direction only.  Normalize U and divide
C     DU by the same scalar.  We know that the norm of U is not zero
C     because SURFPT checked it.
C
      CALL VSCLG ( 1.0D0 / VNORM( STU ), STU, 6, STUHAT )
 
 
C
C     The velocity vector of the intercept point goes in the last three
C     elements of STI.  Let
C
C           I = I(t0)              DI = I'(t0)
C           P = P(t0)              DP = P'(t0)
C           U = U(t0) / | U(t0) |  DU = U'(t0) / | U(t0) |
C
C     and N be the unit normal to the ellipsoid surface at I.
C     Then, from the derivation in $ Particulars above,
C
C           DI  =
C
C
C            < P-I,N >       < U,N > < DP,N > - < P-I,N > < DU,N >
C      DP -  --------- DU -  ------------------------------------- U
C             < U,N >                            2
C                                          < U,N >
C
C
 
C
C     Compute the unit normal at the intercept point, and unpack
C     the input states into P, U, DP, and DU.  Let P-I = PMI
C
      CALL SURFNM ( A, B, C, I, N )
 
      CALL VEQU ( STO,        P )
      CALL VEQU ( STUHAT,     U )
 
      CALL VEQU ( STO(4),    DP )
      CALL VEQU ( STUHAT(4), DU )
 
      CALL VSUB ( P, I,     PMI )
 
C
C     As the intercept point nears the limb, it's velocity goes to
C     infinity.  We must check the value of < U,N > before dividing
C     by it.  If the intercept point is on the limb < U,N > = 0.
C     If it is near the limb, < U,N > may be so small that dividing
C     by would result in a number that is greater than the maximum
C     double precision number for the computer.
C
 
      IF ( VDOT ( U,N ) .EQ. 0.D0 ) THEN
C
C        The intercept point is on the the limb, so its velocity
C        is infinite.  This means we can't "find" the state
C        of the intercept point.
C
         FOUND = .FALSE.
         CALL CHKOUT ( 'SURFPV_M' )
         RETURN
      END IF
 
C
C     Evaluate the second term of the equation for DI, but don't
C     divide by < U,N > just yet.
C
      CALL VSCL ( VDOT ( PMI,N ), DU, SECOND )
C
C                                                         2
C     Evaluate the third term, but don't divide by < U,N >  just yet.
C
      S = VDOT ( U,N ) * VDOT ( DP,N ) - VDOT ( PMI,N ) * VDOT ( DU,N )
 
      CALL VSCL ( S, U, THIRD )
 
C
C     We'll use the following test.
C
C     Let
C
      M = MAX (  VNORM (SECOND), VNORM (THIRD), 1.0D0  )
 
C
C
C     If
C
C             M          DPMAX
C          -------   >   -----
C                 2
C          < U,N >        10
C
C
C     That is, if
C                                  2
C             M  >  DPMAX * < U,N >  / 10
C
C
C     where DPMAX is the largest double precision number,
C     then, the velocity is probably too large to compute.
C     We know that we can perform the multiplication above
C     because U and N are both unit vectors, so the dot
C     product of U and N is less than or equal to one.
C
 
      IF ( M .GT. ( DPMAX() / 10.0D0 ) *  VDOT ( U,N ) ** 2 ) THEN
         FOUND = .FALSE.
         CALL CHKOUT ( 'SURFPV_M' )
         RETURN
      END IF
 
 
C
C     If < U,N > passed the tests above, we can solve for the velocity.
C
C                                                      2
C     DI =  DP  -  SECOND / < U,N >  -  THIRD / < U,N >
C
C
      S = 1.0D0 / VDOT ( U, N )
 
      CALL VLCOM3 ( 1.0D0, DP, -S, SECOND, -(S**2), THIRD, STI(4) )
 
C
C     Since we could compute the velocity, we can assign the
C     intercept point, and set the found flag to .TRUE.
C
      CALL VEQU  ( I, STI(1) )
 
      FOUND = .TRUE.
 
      CALL CHKOUT ( 'SURFPV_M' )
      RETURN
      END
 
C$Procedure      SMRIMG ( Smear image )
C
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

C
C$ Abstract
C
C     Determine the smear in an image due to observer and target
C     motion.
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
C                    are degrees.
C                    SMRAZ defined only if FOUND is TRUE.
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
C-    Beta Version 1.0.0, 18-JAN-1994 (NJB)
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

      CALL MXV  ( DTIPM,    SURFX,  DVB    )
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



#include <cfloat>
#include <cmath>
#include <iomanip>

#include "Camera.h"

using namespace std;
using namespace Isis;
//#include "f2c.h"

/* Table of constant values */

static int c__4 = 4;
/* Builtin functions */
//    double sqrt(doublereal), asin(doublereal);
extern int qmini(NaifContextPtr naif, double *init, double *final, double frac, double *qintrp);

/* $Procedure  CK3SDN( Down sample type 3 CK data prepared for writing ) */
/* Subroutine */
int ck3sdn(NaifContextPtr naif, double sdntol, bool avflag, int *nrec,
           double *sclkdp, double *quats, double *avvs,
           int nints, double *starts, double *dparr, int *intarr) {
  /* System generated locals */
  int i__1, i__2;

  /* Local variables */
  doublereal frac, dneg;
  integer left;
  doublereal dpos, dist2;
  integer i__, j;
  doublereal angle;
  integer keepf;
  integer keepl;
  doublereal qlneg[4];
  logical fitok;
  integer right;
  doublereal dist2a, dist2b;
  doublereal qkeepf[4];
  doublereal qkeepl[4];
  integer intcrf, ndropd, intcrl;
  integer intnrf;
  logical skipit;
  doublereal qlinpt[4], qintrp[4];

  /* $ Abstract */

  /*     Down sample type 3 CK data prepared for writing. */

  /* $ Disclaimer */

  /*     THIS SOFTWARE AND ANY RELATED MATERIALS WERE CREATED BY THE */
  /*     CALIFORNIA INSTITUTE OF TECHNOLOGY (CALTECH) UNDER A U.S. */
  /*     GOVERNMENT CONTRACT WITH THE NATIONAL AERONAUTICS AND SPACE */
  /*     ADMINISTRATION (NASA). THE SOFTWARE IS TECHNOLOGY AND SOFTWARE */
  /*     PUBLICLY AVAILABLE UNDER U.S. EXPORT LAWS AND IS PROVIDED "AS-IS" */
  /*     TO THE RECIPIENT WITHOUT WARRANTY OF ANY KIND, INCLUDING ANY */
  /*     WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR FITNESS FOR A */
  /*     PARTICULAR USE OR PURPOSE (AS SET FORTH IN UNITED STATES UCC */
  /*     SECTIONS 2312-2313) OR FOR ANY PURPOSE WHATSOEVER, FOR THE */
  /*     SOFTWARE AND RELATED MATERIALS, HOWEVER USED. */

  /*     IN NO EVENT SHALL CALTECH, ITS JET PROPULSION LABORATORY, OR NASA */
  /*     BE LIABLE FOR ANY DAMAGES AND/OR COSTS, INCLUDING, BUT NOT */
  /*     LIMITED TO, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND, */
  /*     INCLUDING ECONOMIC DAMAGE OR INJURY TO PROPERTY AND LOST PROFITS, */
  /*     REGARDLESS OF WHETHER CALTECH, JPL, OR NASA BE ADVISED, HAVE */
  /*     REASON TO KNOW, OR, IN FACT, SHALL KNOW OF THE POSSIBILITY. */

  /*     RECIPIENT BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF */
  /*     THE SOFTWARE AND ANY RELATED MATERIALS, AND AGREES TO INDEMNIFY */
  /*     CALTECH AND NASA FOR ALL THIRD-PARTY CLAIMS RESULTING FROM THE */
  /*     ACTIONS OF RECIPIENT IN THE USE OF THE SOFTWARE. */

  /* $ Required_Reading */

  /*     CK */
  /*     DAF */
  /*     ROTATIONS */
  /*     SCLK */

  /* $ Keywords */

  /*     POINTING */
  /*     UTILITY */

  /* $ Declarations */
  /* $ Brief_I/O */

  /*     Variable  I/O  Description */
  /*     --------  ---  -------------------------------------------------- */
  /*     SDNTOL     I   Tolerance used for sampling down. */
  /*     AVFLAG     I   True if angular velocity data is set. */
  /*     NREC      I/O  Number of pointing records. */
  /*     SCLKDP    I/O  Encoded SCLK times. */
  /*     QUATS     I/O  Quaternions representing instrument pointing. */
  /*     AVVS      I/O  Angular velocity vectors. */
  /*     NINTS      I   Number of intervals. */
  /*     STARTS     I   Encoded SCLK interval start times. */
  /*     DPARR      I   Double precision work array. */
  /*     INTARR     I   Integer work array. */

  /* $ Detailed_Input */

  /*     SDNTOL     is the angular tolerance, in radians, to be used to */
  /*                down sample the input CK type 3 pointing data. */
  /*                SDNTOL must be a non-negative number. */

  /*     AVFLAG     is a logical flag indicating whether or not */
  /*                the angular velocity data should be processed. */

  /*     NREC       is the number of pointing instances in the input */
  /*                buffer. */

  /*     SCLKDP     are the encoded spacecraft clock times associated with */
  /*                each pointing instance. These times must be strictly */
  /*                increasing. */

  /*     QUATS      is the quaternion buffer. */

  /*     AVVS       is the angular velocity vector buffer. */

  /*                If AVFLAG is FALSE then this array is ignored by the */
  /*                routine; however it still must be supplied as part of */
  /*                the calling sequence. */

  /*     NINTS      is the number of intervals that the pointing instances */
  /*                are partitioned into. */

  /*     STARTS     are the start times of each of the interpolation */
  /*                intervals. These times must be strictly increasing */
  /*                and must coincide with times for which the input */
  /*                quaternion buffer contains pointing. */

  /*     DPARR      is a double precision work array. */

  /*     INTARR     is an integer work array. */

  /* $ Detailed_Output */

  /*     NREC       is the number of pointing instances in the buffer */
  /*                after down sampling. */

  /*     SCLKDP     is the encoded spacecraft clock time buffer after */
  /*                down sampling. */

  /*     QUATS      is the quaternion buffer after down sampling. */

  /*     AVVS       is the angular velocity vector buffer after down */
  /*                sampling. */

  /* $ Parameters */

  /*     None. */

  /* $ Exceptions */

  /*      1) If the number of pointing records is not greater than zero, */
  /*         the error SPICE(INVALIDNUMBEROFRECORDS) is signaled. */

  /*      2) If the number of interval starts is not greater than zero, */
  /*         the error SPICE(INVALIDNUMBEROFINTERVALS) is signaled. */

  /*      3) If the number of interval starts is not is not less than */
  /*         or equal to the number of records, the error */
  /*         SPICE(BUFFERSIZESMISMATCH) is signaled. */

  /*      4) If the first interval start time is not the same as the */
  /*         first record time, the error SPICE(FIRSTRECORDMISMATCH) */
  /*         is signaled. */

  /*      5) If the down sampling tolerance is not a non-negative number, */
  /*         the error SPICE(BADDOWNSAMPLINGTOL) is signaled. */

  /*      6) If record times buffer does not contain any of the times */
  /*         from interval start times buffers, the error */
  /*         SPICE(INTERVALSTARTNOTFOUND) is signaled. */

  /* $ Files */

  /*     None. */

  /* $ Particulars */

  /*     This routine eliminates from the input quaternion and angular */
  /*     rate buffers all data points for which type 3 CK interpolation */
  /*     between bounding points that are not eliminated would produce */
  /*     result that is within specified tolerance of the input attitude. */
  /*     The elimination, refered to in these comments as "down sampling", */
  /*     is done within each individual interpolation interval (as */
  /*     specified in the input interval starts buffer), with intervals */
  /*     boundaries unchanged. */

  /* $ Examples */

  /*     Normally this routine would be called immediately before the */
  /*     CKW03 is called and be supplied with the input time, quaternion, */
  /*     angular rate, and interval start buffers that were fully and */
  /*     properly prepared for the CKW03 input, like this: */

  /*         CALL CK3SDN ( SDNTOL, ARFLAG, */
  /*        .              NREC, SCLKDP, QUATS, AVVS, NINTS, STARTS, */
  /*        .              DPARR, INTARR ) */

  /*         CALL CKW03  ( HANDLE, SCLKDP(1), SCLKDP(NREC), */
  /*        .              INSTID, FRMNAM, ARFLAG, SEGID, */
  /*        .              NREC, SCLKDP, QUATS, AVVS, NINTS, STARTS ) */

  /* $ Restrictions */

  /*     None. */

  /* $ Literature_References */

  /*     None. */

  /* $ Author_and_Institution */

  /*     N.J. Bachman    (JPL) */
  /*     B.V. Semenov    (JPL) */

  /* $ Version */

  /* -    Beta Version 1.1.0, 19-SEP-2005 (BVS)(FST) */

  /*        Incorporated Scott's shrinking window search algorithm to */
  /*        speed up down sampling. */

  /* -    Beta Version 1.0.0, 29-JUL-2005 (BVS)(NJB) */

  /* -& */
  /* $ Index_Entries */

  /*     sample down ck type_3 pointing data prepared for writing */

  /* -& */

  /*     Local variables. */


  /*     SPICELIB functions. */


  /*     Standard SPICE error handling. */

  if(naif->return_c()) {
    return 0;
  }
  else {
    naif->chkin_c("CK3SDN");
  }

  /*     Let's do some sanity checks that needed to make sure that future */
  /*     loops and comparisons don't blow up. First, verify that the */
  /*     number pointing records is greater that zero. */

  if(*nrec <= 0) {
    naif->setmsg_c("The number of pointing records must be greater than zero. I"
             "t was #.");
    naif->errint_c("#", *nrec);
    naif->sigerr_c("SPICE(INVALIDNUMBEROFRECORDS)");
    naif->chkout_c("CK3SDN");
    return 0;
  }

  /*     Then, verify that the number intervals is greater that zero. */

  if(nints <= 0) {
    naif->setmsg_c("The number of interval starts must be greater than zero. It"
             " was #.");
    naif->errint_c("#", nints);
    naif->sigerr_c("SPICE(INVALIDNUMBEROFINTERVALS)");
    naif->chkout_c("CK3SDN");
    return 0;
  }

  /*     Then, verify that the number intervals is less than or equal to */
  /*     the number of records. */

  if(nints > *nrec) {
    naif->setmsg_c("The number of interval starts, #, is not less than or equal"
             " to the number of records, #.");
    naif->errint_c("#", nints);
    naif->errint_c("#", *nrec);
    naif->sigerr_c("SPICE(BUFFERSIZESMISMATCH)");
    naif->chkout_c("CK3SDN");
    return 0;
  }

  /*     Then verify that the first time in the intervals array is the same */
  /*     as the first time in the records array. */

  if(sclkdp[0] != starts[0]) {
    naif->setmsg_c("The first interval start time, #, is not the same as the fi"
             "rst record time, #.");
    naif->errdp_c("#", *sclkdp);
    naif->errdp_c("#", *starts);
    naif->sigerr_c("SPICE(FIRSTRECORDMISMATCH)");
    naif->chkout_c("CK3SDN");
    return 0;
  }

  /*     Finally verify that input down sampling tolerance is not positive */
  /*     number. */

  if(sdntol < 0.) {
    naif->setmsg_c("The down sampling tolerance must be a non-negative number. "
             "It was #.");
    naif->errdp_c("#", sdntol);
    naif->sigerr_c("SPICE(BADDOWNSAMPLINGTOL)");
    naif->chkout_c("CK3SDN");
    return 0;
  }

  /*     This variable will hold to the index of the pointing record that */
  /*     matches the start of the next interval. For the first interval */
  /*     it is set to one. */

  intnrf = 1;

  /*     We will count the number of points that were dropped. */

  ndropd = 0;

  /*     Loop through interpolation intervals. */

  i__1 = nints;
  for(i__ = 1; i__ <= i__1; ++i__) {

    /*        Assign the index of the pointing record that matches the */
    /*        begin time of this interval. */

    intcrf = intnrf;

    /*        Find the index of the pointing record that ends this interval. */
    /*        If this the last interval, it is the last pointing record in */
    /*        pointing buffer. */

    if(i__ == nints) {
      intcrl = *nrec;
    }
    else {

      /*           This is not the last interval. To get its end time we need */
      /*           to find the pointing record that matches the start of the */
      /*           next interval and pick the record before it. */

      /*           First we find index of the pointing record that corresponds */
      /*           to the start of the next interval. */

      i__2 = *nrec - intcrf + 1;
      intnrf = naif->bsrchd_c(starts[i__], i__2, &sclkdp[intcrf - 1]);
      if(intnrf != 0) {

        /*              Found index must be adjusted to be relative to the */
        /*              beginning of the buffer. Currently it is relative to the */
        /*              start of the current interval. */

        intnrf = intnrf + intcrf - 1;

        /*              The index of the last record belonging to this interval */
        /*              in the found index minus 1. */

        intcrl = intnrf - 1;
      }
      else {

        /*              We did not find such record. The input buffer must have */
        /*              been formed improperly for this to happen. Signal an */
        /*              error. */

        naif->setmsg_c("Cannot find pointing record with time that matches "
                 "the start time # (encoded SCLK ticks) of the interpo"
                 "lation interval number #.");
        naif->errdp_c("#", starts[i__]);
        i__2 = i__ + 1;
        naif->errint_c("#", i__2);
        naif->sigerr_c("SPICE(INTERVALSTARTNOTFOUND)");
        naif->chkout_c("CK3SDN");
        return 0;
      }
    }

    /*        Let's look at the indexes of the pointing records */
    /*        corresponding to the begin and end of this interval. If they */
    /*        are the same (meaning it's a singleton interval) or if they */
    /*        are next to each other (meaning that the whole set of */
    /*        interval's pointing data is comprised of only its begin */
    /*        and end points) there is no down sampling to do. */

    skipit = intcrf == intcrl || intcrf == intcrl - 1;

    /*        Set initial values for a binary search. */

    keepf = intcrf;
    left = intcrf;
    right = intcrl;
    while(! skipit && keepf < intcrl) {

      /*           Set the right endpoint of the interval by dividing the */
      /*           binary search region in half. */

      keepl = (left + right) / 2;

      /*           Unitize bracketing quaternions as QMINI seems to be */
      /*           very sensitive to that. :) */

      naif->vhatg_c(&quats[(keepf << 2) - 4], c__4, qkeepf);
      naif->vhatg_c(&quats[(keepl << 2) - 4], c__4, qkeepl);

      /*           Pick the closer of the right quaternion or its negative to */
      /*           QKEEPF for input into QMINI to ensure that QMINI does */
      /*           interpolation in the "shortest arc" direction. */

      naif->vminug_c(qkeepl, c__4, qlneg);
      dpos = naif->vdistg_c(qkeepl, qkeepf, c__4);
      dneg = naif->vdistg_c(qlneg, qkeepf, c__4);
      if(dneg < dpos) {
        naif->moved_(qlneg, (integer *) &c__4, qlinpt);
      }
      else {
        naif->moved_(qkeepl, (integer *) &c__4, qlinpt);
      }

      /*           Check all records between the currently picked window ends */
      /*           to see if interpolated pointing is within tolerance of the */
      /*           actual pointing. */

      fitok = true;
      j = keepf + 1;
      while(j <= keepl - 1 && fitok) {

        /*              Compute interpolation fraction for this pointing record. */

        if(sclkdp[keepl - 1] - sclkdp[keepf - 1] != 0.) {
          frac = (sclkdp[j - 1] - sclkdp[keepf - 1]) / (sclkdp[
                   keepl - 1] - sclkdp[keepf - 1]);
        }
        else {
          naif->sigerr_c("SPICE(CK3SDNBUG)");
          naif->chkout_c("CK3SDN");
          return 0;
        }

        /*              Call Nat's fast quaternion interpolation routine to */
        /*              compute interpolated rotation for this point. */

        qmini(naif, qkeepf, qlinpt, frac, qintrp);

        /*              Find the squared distance between the interpolated */
        /*              and input quaternions. */

        dist2a = (quats[(j << 2) - 4] - qintrp[0]) * (quats[(j << 2)
                 - 4] - qintrp[0]) + (quats[(j << 2) - 3] - qintrp[1])
                 * (quats[(j << 2) - 3] - qintrp[1]) + (quats[(j << 2)
                     - 2] - qintrp[2]) * (quats[(j << 2) - 2] - qintrp[2])
                 + (quats[(j << 2) - 1] - qintrp[3]) * (quats[(j << 2)
                     - 1] - qintrp[3]);
        dist2b = (quats[(j << 2) - 4] + qintrp[0]) * (quats[(j << 2)
                 - 4] + qintrp[0]) + (quats[(j << 2) - 3] + qintrp[1])
                 * (quats[(j << 2) - 3] + qintrp[1]) + (quats[(j << 2)
                     - 2] + qintrp[2]) * (quats[(j << 2) - 2] + qintrp[2])
                 + (quats[(j << 2) - 1] + qintrp[3]) * (quats[(j << 2)
                     - 1] + qintrp[3]);
        dist2 = min(dist2a, dist2b);

        /*              The rotation angle theta is related to the distance by */
        /*              the formula */

        /*                 || Q1 - Q2 ||     =  2 * | sin(theta/4) | */

        angle = asin(sqrt(dist2) / 2.) * 4.;

        /*              Compare the angle with specified threshold. */

        fitok = fitok && abs(angle) <= sdntol;

        /*              Increment index to move to the next record. */

        ++j;
      }

      /*           Was the fit OK? */

      if(fitok) {

        /*              Fit was OK. Check if left and right are equal; if so we */
        /*              found the point that were were looking for. */

        if(left == right) {

          /*                 Mark all records between fist and last with DPMAX. */

          i__2 = keepl - 1;
          for(j = keepf + 1; j <= i__2; ++j) {
            sclkdp[j - 1] = naif->dpmax_c();
            ++ndropd;
          }

          /*                 Set first point for the next search to be equal to */
          /*                 the to the found point. */

          keepf = keepl;

          /*                 Reset window boundaries for binary search. */

          left = keepl;
          right = intcrl;
        }
        else {

          /*                 Left and right sides haven't converged yet; shift */
          /*                 left side of the binary search window forward. */

          left = keepl + 1;
        }
      }
      else {

        /*              No fit; shift right side of the binary search window */
        /*              backwards. */

        right = keepl - 1;

        /*              If right side when "over" the left side, set left side */
        /*              to be equal to the right side. */

        if(right < left) {
          left = right;
        }
      }
    }
  }

  /*     At this point all records that are to be removed, if any, have */
  /*     been "tagged" with DPMAX in the times buffer. We need to re-sort */
  /*     the buffers to push these records to the bottom and re-set the */
  /*     number of records to indicate that only the top portion should be */
  /*     used. */

  if(ndropd != 0) {

    /*        Since SCLKs were the ones "marked" by DPMAX, we will use them */
    /*        to get the order vector. */

    naif->orderd_c(sclkdp, *nrec, (SpiceInt *) intarr);

    /*        Now, with the order vector in hand, sort the SCLKs ... */

    naif->reordd_c(intarr, *nrec, sclkdp);

    /*        ... then sort quaternions (element by element) ... */

    for(i__ = 0; i__ <= 3; ++i__) {
      i__1 = *nrec;
      for(j = 1; j <= i__1; ++j) {
        dparr[j - 1] = quats[i__ + (j << 2) - 4];
      }
      naif->reordd_c(intarr, *nrec, dparr);
      i__1 = *nrec;
      for(j = 1; j <= i__1; ++j) {
        quats[i__ + (j << 2) - 4] = dparr[j - 1];
      }
    }

    /*        ... and, finally, if requested, sort AVs (also element by */
    /*        element) ... */

    if(avflag) {
      for(i__ = 1; i__ <= 3; ++i__) {
        i__1 = *nrec;
        for(j = 1; j <= i__1; ++j) {
          dparr[j - 1] = avvs[i__ + j * 3 - 4];
        }
        naif->reordd_c(intarr, *nrec, dparr);
        i__1 = *nrec;
        for(j = 1; j <= i__1; ++j) {
          avvs[i__ + j * 3 - 4] = dparr[j - 1];
        }
      }
    }

    /*        Reset the number of points. */

    *nrec -= ndropd;
  }

  /*     All done. Check out. */

  naif->chkout_c("CK3SDN");
  return 0;
} /* ck3sdn_ */


#ifndef NaifContext_h
#define NaifContext_h

/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2021/08/21 20:02:37 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <boost/shared_ptr.hpp>

#include <SpiceZdf.h>
#include <SpiceCK.h>
#include <SpiceOsc.h>
#include <SpiceCel.h>
#include <SpiceEll.h>
#include <SpiceDLA.h>
#include <SpiceDSK.h>
#include <SpiceEK.h>
#include <SpicePln.h>
#include <SpiceSPK.h>

#include "IException.h"

/*
 * SPICE DEFINITIONS
 */
#define VOID      void

typedef VOID      H_f;
typedef int       integer;
typedef double    doublereal;
typedef int       logical;
typedef int       ftnlen;


/*
Type H_fp is used for character return type.
Type S_fp is used for subroutines.
Type U_fp is used for functions of unknown type.
*/
typedef VOID       (*H_fp)(void *state, ...);
typedef doublereal (*D_fp)(void *state, ...);
typedef doublereal (*E_fp)(void *state, ...);
typedef int        (*S_fp)(void *state, ...);
typedef int        (*U_fp)(void *state, ...);
typedef integer    (*I_fp)(void *state, ...);
typedef logical    (*L_fp)(void *state, ...);

namespace Isis {

#define NAIF_GETSET(type, val, name)                \
private:                                            \
  type m_##name = val;                              \
public:                                             \
  inline type name() { return m_##name; }           \
  inline void set_##name(type v) { m_##name = v; }

  /**
   * @brief Manages the main lifecycle of f2c'd NAIF state.
   *
   * @internal
   */
  class NaifContext {
      boost::shared_ptr<void> m_naif;

    public:
      // Lifecycle management.
      static void           createForThread();
      static void           destroyForThread();

      /// Get the thread local NaifContext.
      /// This is safe to cache as long as it's only called from the same thread.
      /// If an object is entirely resident in 1 thread - safe to cache as a member.
      /// If the object is used in multiple threads - cache it within method calls.
      static NaifContext *  acquire();
      
      static NaifContext *  useOrAcquire(NaifContext *n) {
        return n ? n : acquire();
      }
      
      NaifContext();
      ~NaifContext();

      NAIF_GETSET(bool, false, naifStatusInitialized);
      NAIF_GETSET(bool, false, iTimeInitialized);
      NAIF_GETSET(bool, false, targetPckLoaded);
      NAIF_GETSET(bool, false, amicaTimingLoaded);
      NAIF_GETSET(bool, false, hayabusaTimingLoaded);
      NAIF_GETSET(bool, false, mdisTimingLoaded);
      NAIF_GETSET(bool, false, mocWagoLoaded);
      NAIF_GETSET(bool, false, hiJitCubeLoaded);
      NAIF_GETSET(bool, false, hiCalTimingLoaded);

    public:
      void CheckErrors(bool resetNaif = true);

      //
      // ISIS imports.
      //
      int ckfrot_(integer *inst, doublereal *et, doublereal *rotate, integer *ref, logical *found);
      int drotat_(doublereal *angle, integer *iaxis, doublereal *dmout);
      int frmchg_(integer *frame1, integer *frame2, doublereal *et, doublereal *rotate);
      int invstm_(doublereal *mat, doublereal *invmat);
      int refchg_(integer *frame1, integer *frame2, doublereal *et, doublereal *rotate);
      int tkfram_(integer *id, doublereal *rot, integer *frame, logical *found);
      int zzdynrot_(integer *infram,  integer *center, doublereal *et, doublereal *rotate, integer *basfrm);

      //
      // C wrappers.
      //
      void appndc_c(ConstSpiceChar *item, SpiceCell *cell);
      void appndd_c(SpiceDouble item, SpiceCell *cell);
      void appndi_c(SpiceInt item, SpiceCell *cell);
      void axisar_c(ConstSpiceDouble axis[3], SpiceDouble angle, SpiceDouble r[3][3]);
      SpiceBoolean badkpv_c(ConstSpiceChar *caller, ConstSpiceChar *name, ConstSpiceChar *comp, SpiceInt size, SpiceInt divby, SpiceChar type);
      void bltfrm_c(SpiceInt frmcls, SpiceCell *idset);
      void bodc2n_c(SpiceInt code, SpiceInt namelen, SpiceChar *name, SpiceBoolean *found);
      void bodc2s_c(SpiceInt code, SpiceInt lenout, SpiceChar *name);
      void boddef_c(ConstSpiceChar *name, SpiceInt code);
      SpiceBoolean bodfnd_c(SpiceInt body, ConstSpiceChar *item);
      void bodn2c_c(ConstSpiceChar *name, SpiceInt *code, SpiceBoolean *found);
      void bods2c_c(ConstSpiceChar *name, SpiceInt *code, SpiceBoolean *found);
      void bodvar_c(SpiceInt body, ConstSpiceChar *item, SpiceInt *dim, SpiceDouble *values);
      void bodvcd_c(SpiceInt body, ConstSpiceChar *item, SpiceInt maxn, SpiceInt *dim, SpiceDouble *values);
      void bodvrd_c(ConstSpiceChar *body, ConstSpiceChar *item, SpiceInt maxn, SpiceInt *dim, SpiceDouble *values);
      SpiceDouble brcktd_c(SpiceDouble number, SpiceDouble end1, SpiceDouble end2);
      SpiceInt brckti_c(SpiceInt number, SpiceInt end1, SpiceInt end2);
      SpiceInt bschoc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array,ConstSpiceInt *order);
      SpiceInt bschoi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array, ConstSpiceInt *order);
      SpiceInt bsrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array);
      SpiceInt bsrchd_c(SpiceDouble value, SpiceInt ndim, ConstSpiceDouble *array);
      SpiceInt bsrchi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array);
      SpiceDouble b1900_c();
      SpiceDouble b1950_c();
      SpiceInt card_c(SpiceCell *cell);
      void ccifrm_c(SpiceInt frclss, SpiceInt clssid, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceInt *center, SpiceBoolean *found);
      void cgv2el_c(ConstSpiceDouble center[3], ConstSpiceDouble vec1[3], ConstSpiceDouble vec2[3], SpiceEllipse *ellipse);
      void chbder_c(ConstSpiceDouble *cp, SpiceInt degp, SpiceDouble x2s[2], SpiceDouble x, SpiceInt nderiv, SpiceDouble *partdp, SpiceDouble *dpdxs);
      void chkin_c(ConstSpiceChar *module);
      void chkout_c(ConstSpiceChar *module);
      void cidfrm_c(SpiceInt cent, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceBoolean *found);
      void ckcls_c(SpiceInt handle);
      void ckcov_c(ConstSpiceChar *ck, SpiceInt idcode, SpiceBoolean needav, ConstSpiceChar *level, SpiceDouble tol, ConstSpiceChar *timsys, SpiceCell *cover);
      void ckobj_c(ConstSpiceChar *ck, SpiceCell *ids);
      void ckgp_c(SpiceInt inst, SpiceDouble sclkdp, SpiceDouble tol, ConstSpiceChar *ref, SpiceDouble cmat[3][3], SpiceDouble *clkout, SpiceBoolean *found);
      void ckgpav_c(SpiceInt inst, SpiceDouble sclkdp, SpiceDouble tol, ConstSpiceChar *ref, SpiceDouble cmat[3][3], SpiceDouble av[3], SpiceDouble *clkout, SpiceBoolean *found);
      void cklpf_c(ConstSpiceChar *fname, SpiceInt *handle);
      void ckopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle);
      void ckupf_c(SpiceInt handle);
      void ckw01_c(SpiceInt handle, SpiceDouble begtime, SpiceDouble endtime, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble sclkdp[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3]);
      void ckw02_c(SpiceInt handle, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble start[], ConstSpiceDouble stop[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3], ConstSpiceDouble rates[]);
      void ckw03_c(SpiceInt handle, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble sclkdp[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3], SpiceInt nints, ConstSpiceDouble starts[]);
      void ckw05_c(SpiceInt handle, SpiceCK05Subtype subtyp, SpiceInt degree, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt n, ConstSpiceDouble sclkdp[], const void *packets,SpiceDouble rate,SpiceInt nints,ConstSpiceDouble starts[]);
      void cleard_c(SpiceInt ndim, SpiceDouble *array);
      SpiceDouble clight_c();
      void clpool_c();
      void cmprss_c(SpiceChar delim, SpiceInt n, ConstSpiceChar *input, SpiceInt lenout, SpiceChar *output);
      void cnmfrm_c(ConstSpiceChar *cname, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceBoolean *found);
      void conics_c(ConstSpiceDouble elts[8], SpiceDouble et, SpiceDouble state[6]);
      void convrt_c(SpiceDouble x, ConstSpiceChar *in,ConstSpiceChar *out, SpiceDouble *y);
      void copy_c(SpiceCell *a, SpiceCell *b);
      SpiceInt cpos_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start);
      SpiceInt cposr_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start);
      void cvpool_c(ConstSpiceChar *agent, SpiceBoolean *update);
      void cyllat_c(SpiceDouble r, SpiceDouble lonc, SpiceDouble z, SpiceDouble *radius, SpiceDouble *lon, SpiceDouble *lat);
      void cylrec_c(SpiceDouble r, SpiceDouble lon, SpiceDouble z, SpiceDouble rectan[3]);
      void cylsph_c(SpiceDouble r, SpiceDouble lonc, SpiceDouble z, SpiceDouble *radius, SpiceDouble *colat, SpiceDouble *lon);
      void dafac_c(SpiceInt handle, SpiceInt n, SpiceInt lenvals, const void *buffer);
      void dafbbs_c(SpiceInt handle);
      void dafbfs_c(SpiceInt handle);
      void dafcls_c(SpiceInt handle);
      void dafcs_c(SpiceInt handle);
      void dafdc_c(SpiceInt handle);
      void dafec_c(SpiceInt handle, SpiceInt bufsiz, SpiceInt lenout, SpiceInt *n, void *buffer, SpiceBoolean *done);
      void daffna_c(SpiceBoolean *found);
      void daffpa_c(SpiceBoolean *found);
      void dafgda_c(SpiceInt handle, SpiceInt begin, SpiceInt end, SpiceDouble *data);
      void dafgh_c(SpiceInt *handle);
      void dafgn_c(SpiceInt lenout, SpiceChar *name);
      void dafgs_c(SpiceDouble sum[]);
      void dafgsr_c(SpiceInt handle, SpiceInt recno, SpiceInt begin, SpiceInt end, SpiceDouble *data, SpiceBoolean *found);
      void dafopr_c(ConstSpiceChar *fname, SpiceInt *handle);
      void dafopw_c(ConstSpiceChar *fname, SpiceInt *handle);
      void dafps_c(SpiceInt nd, SpiceInt ni, ConstSpiceDouble dc[], ConstSpiceInt ic[], SpiceDouble sum[]);
      void dafrda_c(SpiceInt handle, SpiceInt begin, SpiceInt end, SpiceDouble *data);
      void dafrfr_c(SpiceInt handle, SpiceInt lenout, SpiceInt *nd, SpiceInt *ni, SpiceChar *ifname, SpiceInt *fward, SpiceInt *bward, SpiceInt *free);
      void dafrs_c(ConstSpiceDouble *sum);
      void dafus_c(ConstSpiceDouble sum[], SpiceInt nd, SpiceInt ni, SpiceDouble dc[], SpiceInt ic[]);
      void dasac_c(SpiceInt handle, SpiceInt n, SpiceInt buflen, const void *buffer);
      void dascls_c(SpiceInt handle);
      void dasdc_c(SpiceInt handle);
      void dasec_c(SpiceInt handle, SpiceInt bufsiz, SpiceInt buflen, SpiceInt *n, void *buffer, SpiceBoolean *done);
      void dashfn_c(SpiceInt handle, SpiceInt namlen, SpiceChar *fname);
      void dasopr_c(ConstSpiceChar *fname, SpiceInt *handle);
      void dasopw_c(ConstSpiceChar *fname, SpiceInt *handle);
      void dasrfr_c(SpiceInt handle, SpiceInt idwlen, SpiceInt ifnlen, SpiceChar *idword, SpiceChar *ifname, SpiceInt *nresvr, SpiceInt *nresvc, SpiceInt *ncomr, SpiceInt *ncomc);
      void dcyldr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]);
      void deltet_c(SpiceDouble epoch, ConstSpiceChar *eptype, SpiceDouble *delta);
      SpiceDouble det_c(ConstSpiceDouble m1[3][3]);
      void diags2_c(ConstSpiceDouble symmat[2][2], SpiceDouble diag[2][2], SpiceDouble rotate[2][2]);
      void diff_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      void dgeodr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]);
      void dlabbs_c(SpiceInt handle, SpiceDLADescr *descr, SpiceBoolean *found);
      void dlabfs_c(SpiceInt handle, SpiceDLADescr *descr, SpiceBoolean *found);
      void dlafns_c(SpiceInt handle, ConstSpiceDLADescr *descr, SpiceDLADescr *nxtdsc, SpiceBoolean *found);
      void dlafps_c(SpiceInt handle, ConstSpiceDLADescr *descr, SpiceDLADescr *prvdsc, SpiceBoolean *found);
      void dlatdr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]);
      void dp2hx_c(SpiceDouble number, SpiceInt lenout, SpiceChar *string, SpiceInt *length);
      void dpgrdr_c(ConstSpiceChar *body, SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]);
      SpiceDouble dpmax_c();
      SpiceDouble dpmax_();
      SpiceDouble dpmin_c();
      SpiceDouble dpmin_();
      SpiceDouble dpr_c();
      void drdcyl_c(SpiceDouble r, SpiceDouble lon, SpiceDouble z, SpiceDouble jacobi[3][3]);
      void drdgeo_c(SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]);
      void drdlat_c(SpiceDouble r, SpiceDouble lon, SpiceDouble lat, SpiceDouble jacobi[3][3]);
      void drdpgr_c(ConstSpiceChar *body, SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]);
      void drdsph_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lon, SpiceDouble jacobi[3][3]);
      void dskb02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt *nv, SpiceInt *np, SpiceInt *nvxtot, SpiceDouble vtxbds[3][2], SpiceDouble *voxsiz, SpiceDouble voxori[3], SpiceInt vgrext[3], SpiceInt *cgscal, SpiceInt *vtxnpl, SpiceInt *voxnpt, SpiceInt *voxnpl);
      void dskcls_c(SpiceInt handle, SpiceBoolean optmiz);
      void dskd02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt item, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble *values);
      void dskgd_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceDSKDescr *dskdsc);
      void dskgtl_c(SpiceInt keywrd, SpiceDouble *dpval);
      void dski02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt item, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt *values);
      void dskobj_c(ConstSpiceChar *dsk, SpiceCell *bodids);
      void dskopn_c(ConstSpiceChar *fname, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle);
      void dskn02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt plid, SpiceDouble normal[3]);
      void dskmi2_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], SpiceDouble finscl, SpiceInt corscl, SpiceInt worksz, SpiceInt voxpsz, SpiceInt voxlsz, SpiceBoolean makvtl, SpiceInt spxisz, SpiceInt work[][2], SpiceDouble spaixd[], SpiceInt spaixi[]);
      void dskp02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt plates[][3]);
      void dskrb2_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], SpiceInt corsys, ConstSpiceDouble corpar[], SpiceDouble *mncor3, SpiceDouble *mxcor3);
      void dsksrf_c(ConstSpiceChar *dsk, SpiceInt bodyid, SpiceCell *srfids);
      void dskstl_c(SpiceInt keywrd, SpiceDouble dpval);
      void dskv02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble vrtces[][3]);
      void dskw02_c(SpiceInt handle, SpiceInt center, SpiceInt surfce, SpiceInt dclass, ConstSpiceChar *frame, SpiceInt corsys, ConstSpiceDouble corpar[], SpiceDouble mncor1, SpiceDouble mxcor1, SpiceDouble mncor2, SpiceDouble mxcor2, SpiceDouble mncor3, SpiceDouble mxcor3, SpiceDouble first, SpiceDouble last, SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], ConstSpiceDouble spaixd[], ConstSpiceInt spaixi[]);
      void dskx02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceDouble vertex[3], ConstSpiceDouble raydir[3], SpiceInt *plid, SpiceDouble xpt[3], SpiceBoolean *found);
      void dskxsi_c(SpiceBoolean pri, ConstSpiceChar *target, SpiceInt nsurf, ConstSpiceInt srflst[], SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceDouble vertex[3], ConstSpiceDouble raydir[3], SpiceInt maxd, SpiceInt maxi, SpiceDouble xpt[3], SpiceInt *handle, SpiceDLADescr *dladsc, SpiceDSKDescr *dskdsc, SpiceDouble dc[], SpiceInt ic[], SpiceBoolean *found);
      void dskxv_c(SpiceBoolean pri, ConstSpiceChar *target, SpiceInt nsurf, ConstSpiceInt srflst[], SpiceDouble et, ConstSpiceChar *fixref, SpiceInt nrays, ConstSpiceDouble vtxarr[][3], ConstSpiceDouble dirarr[][3], SpiceDouble xptarr[][3], SpiceBoolean fndarr[]);
      void dskz02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt *nv, SpiceInt *np);
      void dsphdr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]);
      void dtpool_c(ConstSpiceChar *name, SpiceBoolean *found, SpiceInt *n, SpiceChar type[1]);
      void ducrss_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6], SpiceDouble sout[6]);
      void dvcrss_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6], SpiceDouble sout[6]);
      SpiceDouble dvdot_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6]);
      void dvhat_c(ConstSpiceDouble s1[6], SpiceDouble sout[6]);
      SpiceDouble dvnorm_c(ConstSpiceDouble state[6]);
      void dvpool_c(ConstSpiceChar *name);
      SpiceDouble dvsep_c(ConstSpiceDouble *s1, ConstSpiceDouble *s2);
      void edlimb_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble viewpt[3], SpiceEllipse *limb);
      void edterm_c(ConstSpiceChar *trmtyp, ConstSpiceChar *source, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixfrm, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npts, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble termpts[][3]);
      void ekacec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, SpiceInt vallen, const void *cvals,SpiceBoolean isnull);
      void ekaced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceDouble *dvals, SpiceBoolean isnull);
      void ekacei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceInt *ivals, SpiceBoolean isnull);
      void ekaclc_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, SpiceInt vallen, const void *cvals,ConstSpiceInt *entszs,ConstSpiceBoolean *nlflgs,ConstSpiceInt *rcptrs,SpiceInt *wkindx);
      void ekacld_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, ConstSpiceDouble *dvals, ConstSpiceInt *entszs, ConstSpiceBoolean *nlflgs, ConstSpiceInt *rcptrs, SpiceInt *wkindx);
      void ekacli_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, ConstSpiceInt *ivals, ConstSpiceInt *entszs, ConstSpiceBoolean *nlflgs, ConstSpiceInt *rcptrs, SpiceInt *wkindx);
      void ekappr_c(SpiceInt handle, SpiceInt segno, SpiceInt *recno);
      void ekbseg_c(SpiceInt handle, ConstSpiceChar *tabnam, SpiceInt ncols, SpiceInt cnmlen, const void *cnames,SpiceInt declen, const void *decls,  SpiceInt *segno);
      void ekccnt_c(ConstSpiceChar *table, SpiceInt *ccount);
      void ekcii_c(ConstSpiceChar *table, SpiceInt cindex, SpiceInt lenout, SpiceChar *column, SpiceEKAttDsc *attdsc);
      void ekcls_c(SpiceInt handle);
      void ekdelr_c(SpiceInt handle, SpiceInt segno, SpiceInt recno);
      void ekffld_c(SpiceInt handle, SpiceInt segno, SpiceInt *rcptrs);
      void ekfind_c(ConstSpiceChar *query, SpiceInt lenout, SpiceInt *nmrows, SpiceBoolean *error, SpiceChar *errmsg);
      void ekgc_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceInt lenout, SpiceChar *cdata, SpiceBoolean *null, SpiceBoolean *found);
      void ekgd_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceDouble *ddata, SpiceBoolean *null, SpiceBoolean *found);
      void ekgi_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceInt *idata, SpiceBoolean *null, SpiceBoolean *found);
      void ekifld_c(SpiceInt handle, ConstSpiceChar *tabnam, SpiceInt ncols, SpiceInt nrows, SpiceInt cnmlen, const void *cnames,SpiceInt declen, const void *decls,  SpiceInt *segno,  SpiceInt *rcptrs);
      void ekinsr_c(SpiceInt handle, SpiceInt segno, SpiceInt recno);
      void eklef_c(ConstSpiceChar *fname, SpiceInt *handle);
      SpiceInt eknelt_c(SpiceInt selidx, SpiceInt row);
      SpiceInt eknseg_c(SpiceInt handle);
      void ekntab_c(SpiceInt *n);
      void ekopn_c(ConstSpiceChar *fname, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle);
      void ekopr_c(ConstSpiceChar *fname, SpiceInt *handle);
      void ekops_c(SpiceInt *handle);
      void ekopw_c(ConstSpiceChar *fname, SpiceInt *handle);
      void ekpsel_c(ConstSpiceChar *query, SpiceInt msglen, SpiceInt tablen, SpiceInt collen, SpiceInt *n, SpiceInt *xbegs, SpiceInt *xends, SpiceEKDataType *xtypes, SpiceEKExprClass *xclass, void *tabs, void *cols, SpiceBoolean *error, SpiceChar *errmsg);
      void ekrcec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt lenout, SpiceInt *nvals, void *cvals, SpiceBoolean *isnull);
      void ekrced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt *nvals, SpiceDouble *dvals, SpiceBoolean *isnull);
      void ekrcei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt *nvals, SpiceInt *ivals, SpiceBoolean *isnull);
      void ekssum_c(SpiceInt handle, SpiceInt segno, SpiceEKSegSum *segsum);
      void ektnam_c(SpiceInt n, SpiceInt lenout, SpiceChar *table);
      void ekucec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, SpiceInt vallen, const void *cvals,SpiceBoolean isnull);
      void ekuced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceDouble *dvals, SpiceBoolean isnull);
      void ekucei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceInt *ivals, SpiceBoolean isnull);
      void ekuef_c(SpiceInt handle);
      SpiceBoolean elemc_c(ConstSpiceChar *item, SpiceCell *set);
      SpiceBoolean elemd_c(SpiceDouble item, SpiceCell *set);
      SpiceBoolean elemi_c(SpiceInt item, SpiceCell *set);
      void eqncpv_c(SpiceDouble et, SpiceDouble epoch, ConstSpiceDouble eqel[9], SpiceDouble rapol, SpiceDouble decpol, SpiceDouble state[6]);
      SpiceBoolean eqstr_c(ConstSpiceChar *a, ConstSpiceChar *b);
      void el2cgv_c(ConstSpiceEllipse *ellipse, SpiceDouble center[3], SpiceDouble smajor[3], SpiceDouble sminor[3]);
      void erract_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *action);
      void errch_c(ConstSpiceChar *marker, ConstSpiceChar *string);
      void errdev_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *device);
      void errdp_c(ConstSpiceChar *marker, SpiceDouble number);
      void errint_c(ConstSpiceChar *marker, SpiceInt number);
      void errprt_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *list);
      SpiceInt esrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array);
      void etcal_c(SpiceDouble et, SpiceInt lenout, SpiceChar *string);
      void et2lst_c(SpiceDouble et, SpiceInt body, SpiceDouble lon, ConstSpiceChar *type, SpiceInt timlen, SpiceInt ampmlen, SpiceInt *hr, SpiceInt *mn, SpiceInt *sc, SpiceChar *time, SpiceChar *ampm);
      void et2utc_c(SpiceDouble et, ConstSpiceChar *format, SpiceInt prec, SpiceInt lenout, SpiceChar *utcstr);
      void eul2m_c(SpiceDouble angle3, SpiceDouble angle2, SpiceDouble angle1, SpiceInt axis3, SpiceInt axis2, SpiceInt axis1, SpiceDouble r[3][3]);
      void eul2xf_c(ConstSpiceDouble eulang[6], SpiceInt axisa, SpiceInt axisb, SpiceInt axisc, SpiceDouble xform[6][6]);
      SpiceBoolean exists_c(ConstSpiceChar *name);
      void expool_c(ConstSpiceChar *name, SpiceBoolean *found);
      SpiceBoolean failed_c();
      void fovray_c(ConstSpiceChar *inst, ConstSpiceDouble raydir[3], ConstSpiceChar *rframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble *et, SpiceBoolean *visible);
      void fovtrg_c(ConstSpiceChar *inst, ConstSpiceChar *target, ConstSpiceChar *tshape, ConstSpiceChar *tframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble *et, SpiceBoolean *visible);
      void frame_c(SpiceDouble x[3], SpiceDouble y[3], SpiceDouble z[3]);
      void frinfo_c(SpiceInt frcode, SpiceInt *cent, SpiceInt *clss, SpiceInt *clssid, SpiceBoolean *found);
      void frmnam_c(SpiceInt frcode, SpiceInt lenout, SpiceChar *frname);
      void ftncls_c(SpiceInt unit);
      void furnsh_c(ConstSpiceChar *file);
      void gcpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt lenout, SpiceInt *n, void *cvals, SpiceBoolean *found);
      void gdpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble *values, SpiceBoolean *found);
      void georec_c(SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble rectan[3]);
      void getcml_c(SpiceInt *argc, SpiceChar ***argv);
      void getelm_c(SpiceInt frstyr, SpiceInt lineln, const void *lines,SpiceDouble *epoch,SpiceDouble *elems);
      void getfat_c(ConstSpiceChar *file, SpiceInt arclen, SpiceInt typlen, SpiceChar *arch, SpiceChar *type);
      void getfov_c(SpiceInt instid, SpiceInt room, SpiceInt shapelen, SpiceInt framelen, SpiceChar *shape, SpiceChar *frame, SpiceDouble bsight[3], SpiceInt *n, SpiceDouble bounds[][3]);
      void getmsg_c(ConstSpiceChar *option, SpiceInt lenout, SpiceChar *msg);
      SpiceBoolean gfbail_c();
      void gfclrh_c();
      void gfdist_c(ConstSpiceChar *target, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfevnt_c(void(*udstep)(  SpiceDouble et,SpiceDouble *step), void(*udrefn)(  SpiceDouble t1,SpiceDouble t2,SpiceBoolean s1,SpiceBoolean s2,SpiceDouble *t), ConstSpiceChar *gquant, SpiceInt qnpars, SpiceInt lenvals, const void *qpnams, const void *qcpars,  ConstSpiceDouble *qdpars,  ConstSpiceInt *qipars,  ConstSpiceBoolean *qlpars,  ConstSpiceChar *op,  SpiceDouble refval,  SpiceDouble tol,  SpiceDouble adjust,  SpiceBoolean rpt,  void(*udrepi)(SpiceCell *cnfine, ConstSpiceChar *srcpre, ConstSpiceChar *srcsuf),  void(*udrepu)(SpiceDouble ivbeg, SpiceDouble ivend, SpiceDouble et),  void(*udrepf)(),  SpiceInt nintvls,  SpiceBoolean bail,  SpiceBoolean(*udbail)(),  SpiceCell *cnfine,  SpiceCell *result);
      void gffove_c(ConstSpiceChar *inst, ConstSpiceChar *tshape, ConstSpiceDouble raydir[3], ConstSpiceChar *target, ConstSpiceChar *tframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble tol, void(*udstep)(  SpiceDouble et,SpiceDouble *step), void(*udrefn)(  SpiceDouble t1,SpiceDouble t2,SpiceBoolean s1,SpiceBoolean s2,SpiceDouble *t), SpiceBoolean rpt, void(*udrepi)(  SpiceCell *cnfine,ConstSpiceChar *srcpre,ConstSpiceChar *srcsuf), void(*udrepu)(  SpiceDouble ivbeg,SpiceDouble ivend,SpiceDouble et), void(*udrepf)(), SpiceBoolean bail, SpiceBoolean(*udbail)(), SpiceCell *cnfine, SpiceCell *result);
      void gfilum_c(ConstSpiceChar *method, ConstSpiceChar *angtyp, ConstSpiceChar *target, ConstSpiceChar *illum, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfinth_c(int sigcode);
      void gfocce_c(ConstSpiceChar *occtyp, ConstSpiceChar *front, ConstSpiceChar *fshape, ConstSpiceChar *fframe, ConstSpiceChar *back, ConstSpiceChar *bshape, ConstSpiceChar *bframe, ConstSpiceChar *obsrvr, ConstSpiceChar *abcorr, SpiceDouble tol, void(*udstep)(  SpiceDouble et,SpiceDouble *step), void(*udrefn)(  SpiceDouble t1,SpiceDouble t2,SpiceBoolean s1,SpiceBoolean s2,SpiceDouble *t), SpiceBoolean rpt, void(*udrepi)(  SpiceCell *cnfine,ConstSpiceChar *srcpre,ConstSpiceChar *srcsuf), void(*udrepu)(  SpiceDouble ivbeg,SpiceDouble ivend,SpiceDouble et), void(*udrepf)(), SpiceBoolean bail, SpiceBoolean(*udbail)(), SpiceCell *cnfine, SpiceCell *result);
      void gfoclt_c(ConstSpiceChar *occtyp, ConstSpiceChar *front, ConstSpiceChar *fshape, ConstSpiceChar *fframe, ConstSpiceChar *back, ConstSpiceChar *bshape, ConstSpiceChar *bframe, ConstSpiceChar *obsrvr, ConstSpiceChar *abcorr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result);
      void gfpa_c(ConstSpiceChar *target, ConstSpiceChar *illum, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfposc_c(ConstSpiceChar *target, ConstSpiceChar *frame, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfrefn_c(SpiceDouble t1, SpiceDouble t2, SpiceBoolean s1, SpiceBoolean s2, SpiceDouble *t);
      void gfrepf_c();
      void gfrepi_c(SpiceCell *window, ConstSpiceChar *begmss, ConstSpiceChar *endmss);
      void gfrepu_c(SpiceDouble ivbeg, SpiceDouble ivend, SpiceDouble time);
      void gfrfov_c(ConstSpiceChar *inst, ConstSpiceDouble raydir[3], ConstSpiceChar *rframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result);
      void gfrr_c(ConstSpiceChar *target, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfsep_c(ConstSpiceChar *targ1, ConstSpiceChar *shape1, ConstSpiceChar *frame1, ConstSpiceChar *targ2, ConstSpiceChar *shape2, ConstSpiceChar *frame2, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfsntc_c(ConstSpiceChar *target, ConstSpiceChar *fixref, ConstSpiceChar *method, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gfsstp_c(SpiceDouble step);
      void gfstep_c(SpiceDouble time, SpiceDouble *step);
      void gfstol_c(SpiceDouble value);
      void gfsubc_c(ConstSpiceChar *target, ConstSpiceChar *fixref, ConstSpiceChar *method, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gftfov_c(ConstSpiceChar *inst, ConstSpiceChar *target, ConstSpiceChar *tshape, ConstSpiceChar *tframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result);
      void gfudb_c(void(*udfuns)(  SpiceDouble et,SpiceDouble *value), void(*udfunb)(  void(*udfuns)( SpiceDouble et,  SpiceDouble *value),SpiceDouble et,SpiceBoolean *xbool), SpiceDouble step, SpiceCell *cnfine, SpiceCell *result);
      void gfuds_c(void(*udfuns)(  SpiceDouble et,SpiceDouble *value), void(*udfunb)(  void(*udfuns)( SpiceDouble et,  SpiceDouble *value),SpiceDouble x,SpiceBoolean *xbool), ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result);
      void gipool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt *ivals, SpiceBoolean *found);
      void gnpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt lenout, SpiceInt *n, void *kvars, SpiceBoolean *found);
      void hrmint_c(SpiceInt n, ConstSpiceDouble *xvals, ConstSpiceDouble *yvals, SpiceDouble x, SpiceDouble *work, SpiceDouble *f, SpiceDouble *df);
      SpiceDouble halfpi_c();
      void hx2dp_c(ConstSpiceChar *string, SpiceInt lenout, SpiceDouble *number, SpiceBoolean *error, SpiceChar *errmsg);
      void ident_c(SpiceDouble matrix[3][3]);
      void ilumin_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn);
      void illum_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn);
      void illum_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn);
      void illum_plid_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceInt plid, SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn, SpiceBoolean *visible, SpiceBoolean *lit);
      void illumf_c(ConstSpiceChar *method, ConstSpiceChar *target, ConstSpiceChar *ilusrc, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *incdnc, SpiceDouble *emissn, SpiceBoolean *visibl, SpiceBoolean *lit);
      void illumg_c(ConstSpiceChar *method, ConstSpiceChar *target, ConstSpiceChar *illum, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn);
      void inedpl_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpicePlane *plane, SpiceEllipse *ellipse, SpiceBoolean *found);
      void inelpl_c(ConstSpiceEllipse *ellips, ConstSpicePlane *plane, SpiceInt *nxpts, SpiceDouble xpt1[3], SpiceDouble xpt2[3]);
      void insrtc_c(ConstSpiceChar *item, SpiceCell *set);
      void insrtd_c(SpiceDouble item, SpiceCell *set);
      void insrti_c(SpiceInt item, SpiceCell *set);
      void inter_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      void inrypl_c(ConstSpiceDouble vertex[3], ConstSpiceDouble dir[3], ConstSpicePlane *plane, SpiceInt *nxpts, SpiceDouble xpt[3]);
      SpiceInt intmax_c();
      SpiceInt intmax_();
      SpiceInt intmin_c();
      SpiceInt intmin_();
      void invert_c(ConstSpiceDouble m1[3][3], SpiceDouble m2[3][3]);
      void invort_c(ConstSpiceDouble m[3][3], SpiceDouble mit[3][3]);
      SpiceBoolean isordv_c(ConstSpiceInt *array, SpiceInt n);
      SpiceBoolean isrot_c(ConstSpiceDouble m[3][3], SpiceDouble ntol, SpiceDouble dtol);
      SpiceInt isrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array);
      SpiceInt isrchd_c(SpiceDouble value, SpiceInt ndim, ConstSpiceDouble *array);
      SpiceInt isrchi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array);
      SpiceBoolean iswhsp_c(ConstSpiceChar *string);
      SpiceDouble j1900_c();
      SpiceDouble j1950_c();
      SpiceDouble j2000_c();
      SpiceDouble j2100_c();
      SpiceDouble jyear_c();
      void kclear_c();
      void kdata_c(SpiceInt which, ConstSpiceChar *kind, SpiceInt fillen, SpiceInt typlen, SpiceInt srclen, SpiceChar *file, SpiceChar *filtyp, SpiceChar *source, SpiceInt *handle, SpiceBoolean *found);
      void kinfo_c(ConstSpiceChar *file, SpiceInt typlen, SpiceInt srclen, SpiceChar *filtyp, SpiceChar *source, SpiceInt *handle, SpiceBoolean *found);
      void kplfrm_c(SpiceInt frmcls, SpiceCell *idset);
      void ktotal_c(ConstSpiceChar *kind, SpiceInt *count);
      void kxtrct_c(ConstSpiceChar *keywd, SpiceInt termlen, const void *terms,SpiceInt nterms,SpiceInt stringlen,SpiceInt substrlen,SpiceChar *string,SpiceBoolean *found,SpiceChar *substr);
      SpiceInt lastnb_c(ConstSpiceChar *string);
      void latcyl_c(SpiceDouble radius, SpiceDouble lon, SpiceDouble lat, SpiceDouble *r, SpiceDouble *lonc, SpiceDouble *z);
      void latrec_c(SpiceDouble radius, SpiceDouble longitude, SpiceDouble latitude, SpiceDouble rectan[3]);
      void latsph_c(SpiceDouble radius, SpiceDouble lon, SpiceDouble lat, SpiceDouble *rho, SpiceDouble *colat, SpiceDouble *lons);
      void latsrf_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, SpiceInt npts, ConstSpiceDouble lonlat[][2], SpiceDouble srfpts[][3]);
      void lcase_c(SpiceChar *in,SpiceInt lenout, SpiceChar *out);
      void ldpool_c(ConstSpiceChar *filename);
      void lgrind_c(SpiceInt n, ConstSpiceDouble *xvals, ConstSpiceDouble *yvals, SpiceDouble *work, SpiceDouble x, SpiceDouble *p, SpiceDouble *dp);
      void limb_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npoints, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble limbpts[][3], SpiceInt plateIDs[]);
      void limbpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *corloc, ConstSpiceChar *obsrvr, ConstSpiceDouble refvec[3], SpiceDouble rolstp, SpiceInt ncuts, SpiceDouble schstp, SpiceDouble soltol, SpiceInt maxn, SpiceInt npts[], SpiceDouble points[][3], SpiceDouble epochs[], SpiceDouble tangts[][3]);
      void llgrid_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt npoints, ConstSpiceDouble grid[][2], SpiceDouble spoints[][3], SpiceInt plateIDs[]);
      void lmpool_c(const void *cvals,SpiceInt lenvals,SpiceInt n);
      void lparse_c(ConstSpiceChar *list, ConstSpiceChar *delim, SpiceInt nmax, SpiceInt lenout, SpiceInt *n, void *items);
      void lparsm_c(ConstSpiceChar *list, ConstSpiceChar *delims, SpiceInt nmax, SpiceInt lenout, SpiceInt *n, void *items);
      void lparss_c(ConstSpiceChar *list, ConstSpiceChar *delims, SpiceCell *set);
      SpiceDouble lspcn_c(ConstSpiceChar *body, SpiceDouble et, ConstSpiceChar *abcorr);
      SpiceInt lstlec_c(ConstSpiceChar *string, SpiceInt n, SpiceInt lenvals, const void *array);
      SpiceInt lstled_c(SpiceDouble x, SpiceInt n, ConstSpiceDouble *array);
      SpiceInt lstlei_c(SpiceInt x, SpiceInt n, ConstSpiceInt *array);
      SpiceInt lstltc_c(ConstSpiceChar *string, SpiceInt n, SpiceInt lenvals, const void *array);
      SpiceInt lstltd_c(SpiceDouble x, SpiceInt n, ConstSpiceDouble *array);
      SpiceInt lstlti_c(SpiceInt x, SpiceInt n, ConstSpiceInt *array);
      void ltime_c(SpiceDouble etobs, SpiceInt obs, ConstSpiceChar *dir, SpiceInt targ, SpiceDouble *ettarg, SpiceDouble *elapsd);
      void lx4dec_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar);
      void lx4num_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar);
      void lx4sgn_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar);
      void lx4uns_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar);
      void lxqstr_c(ConstSpiceChar *string, SpiceChar qchar, SpiceInt first, SpiceInt *last, SpiceInt *nchar);
      void m2eul_c(ConstSpiceDouble r[3][3], SpiceInt axis3, SpiceInt axis2, SpiceInt axis1, SpiceDouble *angle3, SpiceDouble *angle2, SpiceDouble *angle1);
      void m2q_c(ConstSpiceDouble r[3][3], SpiceDouble q[4]);
      SpiceBoolean matchi_c(ConstSpiceChar *string, ConstSpiceChar *templ, SpiceChar wstr, SpiceChar wchr);
      SpiceBoolean matchw_c(ConstSpiceChar *string, ConstSpiceChar *templ, SpiceChar wstr, SpiceChar wchr);
      SpiceDouble maxd_c(SpiceInt n, ...);
      SpiceInt maxi_c(SpiceInt n, ...);
      void mequ_c(ConstSpiceDouble m1[3][3], SpiceDouble mout[3][3]);
      void mequg_c(const void *m1,SpiceInt nr,SpiceInt nc,void *mout);
      SpiceDouble mind_c(SpiceInt n, ...);
      SpiceInt mini_c(SpiceInt n, ...);
      int moved_(SpiceDouble *arrfrm, SpiceInt *ndim, SpiceDouble *arrto);
      void mtxm_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]);
      void mtxmg_c(const void *m1, const void *m2,  SpiceInt row1,  SpiceInt col1,  SpiceInt col2,  void *mout);
      void mtxv_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble vin[3], SpiceDouble vout[3]);
      void mtxvg_c(const void *m1, const void *v2,  SpiceInt ncol1,  SpiceInt nr1r2,  void *vout);
      void mxm_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]);
      void mxmg_c(const void *m1, const void *m2,  SpiceInt row1,  SpiceInt col1,  SpiceInt col2,  void *mout);
      void mxmt_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]);
      void mxmtg_c(const void *m1, const void *m2,  SpiceInt nrow1,  SpiceInt nc1c2,  SpiceInt nrow2,  void *mout);
      void mxv_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble vin[3], SpiceDouble vout[3]);
      void mxvg_c(const void *m1, const void *v2,  SpiceInt nrow1,  SpiceInt nc1r2,  void *vout);
      void namfrm_c(ConstSpiceChar *frname, SpiceInt *frcode);
      SpiceInt ncpos_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start);
      SpiceInt ncposr_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start);
      void nearpt_c(ConstSpiceDouble positn[3], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble npoint[3], SpiceDouble *alt);
      void npedln_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble linept[3], ConstSpiceDouble linedr[3], SpiceDouble pnear[3], SpiceDouble *dist);
      void npelpt_c(ConstSpiceDouble point[3], ConstSpiceEllipse *ellips, SpiceDouble pnear[3], SpiceDouble *dist);
      void nplnpt_c(ConstSpiceDouble linpt[3], ConstSpiceDouble lindir[3], ConstSpiceDouble point[3], SpiceDouble pnear[3], SpiceDouble *dist);
      void nvc2pl_c(ConstSpiceDouble normal[3], SpiceDouble constant, SpicePlane *plane);
      void nvp2pl_c(ConstSpiceDouble normal[3], ConstSpiceDouble point[3], SpicePlane *plane);
      void occult_c(ConstSpiceChar *target1, ConstSpiceChar *shape1, ConstSpiceChar *frame1, ConstSpiceChar *target2, ConstSpiceChar *shape2, ConstSpiceChar *frame2, ConstSpiceChar *abcorr, ConstSpiceChar *observer, SpiceDouble time, SpiceInt *occult_code);
      SpiceInt ordc_c(ConstSpiceChar *item, SpiceCell *set);
      SpiceInt ordd_c(SpiceDouble item, SpiceCell *set);
      SpiceInt ordi_c(SpiceInt item, SpiceCell *set);
      void orderc_c(SpiceInt lenvals, const void *array,SpiceInt ndim,SpiceInt *iorder);
      void orderd_c(ConstSpiceDouble *array, SpiceInt ndim, SpiceInt *iorder);
      void orderi_c(ConstSpiceInt *array, SpiceInt ndim, SpiceInt *iorder);
      void oscelt_c(ConstSpiceDouble state[6], SpiceDouble et, SpiceDouble mu, SpiceDouble elts[8]);
      void oscltx_c(ConstSpiceDouble state[6], SpiceDouble et, SpiceDouble mu, SpiceDouble elts[SPICE_OSCLTX_NELTS]);
      void pckcls_c(SpiceInt handle);
      void pckcov_c(ConstSpiceChar *pck, SpiceInt idcode, SpiceCell *cover);
      void pckfrm_c(ConstSpiceChar *pck, SpiceCell *ids);
      void pcklof_c(ConstSpiceChar *fname, SpiceInt *handle);
      void pckopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle);
      void pckuof_c(SpiceInt handle);
      void pckw02_c(SpiceInt handle, SpiceInt clssid, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, SpiceDouble cdata[], SpiceDouble btime);
      void pcpool_c(ConstSpiceChar *name, SpiceInt n, SpiceInt lenvals, const void *cvals);
      void pdpool_c(ConstSpiceChar *name, SpiceInt n, ConstSpiceDouble *dvals);
      void pgrrec_c(ConstSpiceChar *body, SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble rectan[3]);
      SpiceDouble phaseq_c(SpiceDouble et, ConstSpiceChar *target, ConstSpiceChar *illumn, ConstSpiceChar *obsrvr, ConstSpiceChar *abcorr);
      SpiceDouble pi_c();
      void pipool_c(ConstSpiceChar *name, SpiceInt n, ConstSpiceInt *ivals);
      void pjelpl_c(ConstSpiceEllipse *elin, ConstSpicePlane *plane, SpiceEllipse *elout);
      void pl2nvc_c(ConstSpicePlane *plane, SpiceDouble normal[3], SpiceDouble *constant);
      void pl2nvp_c(ConstSpicePlane *plane, SpiceDouble normal[3], SpiceDouble point[3]);
      void pl2psv_c(ConstSpicePlane *plane, SpiceDouble point[3], SpiceDouble span1[3], SpiceDouble span2[3]);
      SpiceDouble pltar_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3]);
      void pltexp_c(ConstSpiceDouble iverts[3][3], SpiceDouble delta, SpiceDouble overts[3][3]);
      void pltnp_c(ConstSpiceDouble point[3], ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], ConstSpiceDouble v3[3], SpiceDouble pnear[3], SpiceDouble *dist);
      void pltnrm_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], ConstSpiceDouble v3[3], SpiceDouble normal[3]);
      SpiceDouble pltvol_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3]);
      void polyds_c(ConstSpiceDouble *coeffs, SpiceInt deg, SpiceInt nderiv, SpiceDouble t, SpiceDouble *p);
      SpiceInt pos_c(ConstSpiceChar *str, ConstSpiceChar *substr, SpiceInt start);
      SpiceInt posr_c(ConstSpiceChar *str, ConstSpiceChar *substr, SpiceInt start);
      void prefix_c(ConstSpiceChar *pref, SpiceInt spaces, SpiceInt lenout, SpiceChar *string);
      SpiceChar* prompt_c(ConstSpiceChar *prmptStr, SpiceInt lenout, SpiceChar *buffer);
      void prop2b_c(SpiceDouble gm, ConstSpiceDouble pvinit[6], SpiceDouble dt, SpiceDouble pvprop[6]);
      void prsdp_c(ConstSpiceChar *string, SpiceDouble *dpval);
      void prsint_c(ConstSpiceChar *string, SpiceInt *intval);
      void psv2pl_c(ConstSpiceDouble point[3], ConstSpiceDouble span1[3], ConstSpiceDouble span2[3], SpicePlane *plane);
      void putcml_c(SpiceInt argc, SpiceChar **argv);
      void pxform_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble et, SpiceDouble rotate[3][3]);
      void pxfrm2_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble etfrom, SpiceDouble etto, SpiceDouble rotate[3][3]);
      void q2m_c(ConstSpiceDouble q[4], SpiceDouble r[3][3]);
      void qcktrc_c(SpiceInt tracelen, SpiceChar *trace);
      void qdq2av_c(ConstSpiceDouble q[4], ConstSpiceDouble dq[4], SpiceDouble av[3]);
      void qxq_c(ConstSpiceDouble q1[4], ConstSpiceDouble q2[4], SpiceDouble qout[4]);
      void radrec_c(SpiceDouble range, SpiceDouble ra, SpiceDouble dec, SpiceDouble rectan[3]);
      void rav2xf_c(ConstSpiceDouble rot[3][3], ConstSpiceDouble av[3], SpiceDouble xform[6][6]);
      void raxisa_c(ConstSpiceDouble matrix[3][3], SpiceDouble axis[3], SpiceDouble *angle);
      void rdtext_c(ConstSpiceChar *file, SpiceInt lenout, SpiceChar *line, SpiceBoolean *eof);
      void reccyl_c(ConstSpiceDouble rectan[3], SpiceDouble *r, SpiceDouble *lon, SpiceDouble *z);
      void recgeo_c(ConstSpiceDouble rectan[3], SpiceDouble re, SpiceDouble f, SpiceDouble *lon, SpiceDouble *lat, SpiceDouble *alt);
      void reclat_c(ConstSpiceDouble rectan[3], SpiceDouble *radius, SpiceDouble *longitude, SpiceDouble *latitude);
      void recpgr_c(ConstSpiceChar *body, SpiceDouble rectan[3], SpiceDouble re, SpiceDouble f, SpiceDouble *lon, SpiceDouble *lat, SpiceDouble *alt);
      void recrad_c(ConstSpiceDouble rectan[3], SpiceDouble *radius, SpiceDouble *ra, SpiceDouble *dec);
      void reordc_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceInt lenvals, void *array);
      void reordd_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceDouble *array);
      void reordi_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceInt *array);
      void reordl_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceBoolean *array);
      void removc_c(ConstSpiceChar *item, SpiceCell *set);
      void removd_c(SpiceDouble item, SpiceCell *set);
      void removi_c(SpiceInt item, SpiceCell *set);
      void repmc_c(ConstSpiceChar *in,ConstSpiceChar *marker, ConstSpiceChar *value, SpiceInt lenout, SpiceChar *out);
      void repmct_c(ConstSpiceChar *in,ConstSpiceChar *marker, SpiceInt value, SpiceChar strCase, SpiceInt lenout, SpiceChar *out);
      void repmd_c(ConstSpiceChar *in,ConstSpiceChar *marker, SpiceDouble value, SpiceInt sigdig, SpiceInt lenout, SpiceChar *out);
      void repmf_c(ConstSpiceChar *in,ConstSpiceChar *marker, SpiceDouble value, SpiceInt sigdig, SpiceChar format, SpiceInt lenout, SpiceChar *out);
      void repmi_c(ConstSpiceChar *in,ConstSpiceChar *marker, SpiceInt value, SpiceInt lenout, SpiceChar *out);
      void repmot_c(ConstSpiceChar *in,ConstSpiceChar *marker, SpiceInt value, SpiceChar strCase, SpiceInt lenout, SpiceChar *out);
      void reset_c();
      SpiceBoolean return_c();
      void recsph_c(ConstSpiceDouble rectan[3], SpiceDouble *r, SpiceDouble *colat, SpiceDouble *lon);
      void rotate_c(SpiceDouble angle, SpiceInt iaxis, SpiceDouble mout[3][3]);
      void rotmat_c(ConstSpiceDouble m1[3][3], SpiceDouble angle, SpiceInt iaxis, SpiceDouble mout[3][3]);
      void rotvec_c(ConstSpiceDouble v1[3], SpiceDouble angle, SpiceInt iaxis, SpiceDouble vout[3]);
      SpiceDouble rpd_c();
      void rquad_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble root1[2], SpiceDouble root2[2]);
      void saelgv_c(ConstSpiceDouble vec1[3], ConstSpiceDouble vec2[3], SpiceDouble smajor[3], SpiceDouble sminor[3]);
      void scard_c(SpiceInt card, SpiceCell *cell);
      void scdecd_c(SpiceInt sc, SpiceDouble sclkdp, SpiceInt sclklen, SpiceChar *sclkch);
      void sce2s_c(SpiceInt sc, SpiceDouble et, SpiceInt sclklen, SpiceChar *sclkch);
      void sce2c_c(SpiceInt sc, SpiceDouble et, SpiceDouble *sclkdp);
      void sce2t_c(SpiceInt sc, SpiceDouble et, SpiceDouble *sclkdp);
      void scencd_c(SpiceInt sc, ConstSpiceChar *sclkch, SpiceDouble *sclkdp);
      void scfmt_c(SpiceInt sc, SpiceDouble ticks, SpiceInt clkstrlen, SpiceChar *clkstr);
      void scpart_c(SpiceInt sc, SpiceInt *nparts, SpiceDouble *pstart, SpiceDouble *pstop);
      void scs2e_c(SpiceInt sc, ConstSpiceChar *sclkch, SpiceDouble *et);
      void sct2e_c(SpiceInt sc, SpiceDouble sclkdp, SpiceDouble *et);
      void sctiks_c(SpiceInt sc, ConstSpiceChar *clkstr, SpiceDouble *ticks);
      void sdiff_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      SpiceBoolean set_c(SpiceCell *a, ConstSpiceChar *op, SpiceCell *b);
      void setmsg_c(ConstSpiceChar *msg);
      void shellc_c(SpiceInt ndim, SpiceInt lenvals, void *array);
      void shelld_c(SpiceInt ndim, SpiceDouble *array);
      void shelli_c(SpiceInt ndim, SpiceInt *array);
      void sigerr_c(ConstSpiceChar *message);
      void sincpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceBoolean *found);
      SpiceInt size_c(SpiceCell *size);
      SpiceDouble spd_c();
      void sphcyl_c(SpiceDouble radius, SpiceDouble colat, SpiceDouble slon, SpiceDouble *r, SpiceDouble *lon, SpiceDouble *z);
      void sphlat_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lons, SpiceDouble *radius, SpiceDouble *lon, SpiceDouble *lat);
      void sphrec_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lon, SpiceDouble rectan[3]);
      void spk14a_c(SpiceInt handle, SpiceInt ncsets, ConstSpiceDouble coeffs[], ConstSpiceDouble epochs[]);
      void spk14b_c(SpiceInt handle, ConstSpiceChar *segid, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, SpiceInt chbdeg);
      void spk14e_c(SpiceInt handle);
      void spkapo_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceDouble sobs[6], ConstSpiceChar *abcorr, SpiceDouble ptarg[3], SpiceDouble *lt);
      void spkapp_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceDouble sobs[6], ConstSpiceChar *abcorr, SpiceDouble starg[6], SpiceDouble *lt);
      void spkacs_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, SpiceInt obs, SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt);
      void spkaps_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceDouble stobs[6], ConstSpiceDouble accobs[6], SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt);
      void spkcls_c(SpiceInt handle);
      void spkcov_c(ConstSpiceChar *spk, SpiceInt idcode, SpiceCell *cover);
      void spkcpo_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceDouble obssta[3], ConstSpiceChar *obsctr, ConstSpiceChar *obsref, SpiceDouble state[6], SpiceDouble *lt);
      void spkcpt_c(ConstSpiceDouble trgpos[3], ConstSpiceChar *trgctr, ConstSpiceChar *trgref, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble state[6], SpiceDouble *lt);
      void spkcvo_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceDouble obssta[6], SpiceDouble obsepc, ConstSpiceChar *obsctr, ConstSpiceChar *obsref, SpiceDouble state[6], SpiceDouble *lt);
      void spkcvt_c(ConstSpiceDouble trgsta[6], SpiceDouble trgepc, ConstSpiceChar *trgctr, ConstSpiceChar *trgref, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble state[6], SpiceDouble *lt);
      void spkez_c(SpiceInt target, SpiceDouble epoch, ConstSpiceChar *frame, ConstSpiceChar *abcorr, SpiceInt observer, SpiceDouble state[6], SpiceDouble *lt);
      void spkezp_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, SpiceInt obs, SpiceDouble ptarg[3], SpiceDouble *lt);
      void spkezr_c(ConstSpiceChar *target, SpiceDouble epoch, ConstSpiceChar *frame, ConstSpiceChar *abcorr, ConstSpiceChar *observer, SpiceDouble state[6], SpiceDouble *lt);
      void spkgeo_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceInt obs, SpiceDouble state[6], SpiceDouble *lt);
      void spkgps_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceInt obs, SpiceDouble pos[3], SpiceDouble *lt);
      void spklef_c(ConstSpiceChar *filename, SpiceInt *handle);
      void spkltc_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceDouble stobs[6], SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt);
      void spkobj_c(ConstSpiceChar *spk, SpiceCell *ids);
      void spkopa_c(ConstSpiceChar *file, SpiceInt *handle);
      void spkopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle);
      void spkpds_c(SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceInt type, SpiceDouble first, SpiceDouble last, SpiceDouble descr[5]);
      void spkpos_c(ConstSpiceChar *targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceChar *obs, SpiceDouble ptarg[3], SpiceDouble *lt);
      void spkpvn_c(SpiceInt handle, ConstSpiceDouble descr[5], SpiceDouble et, SpiceInt *ref, SpiceDouble state[6], SpiceInt *center);
      void spksfs_c(SpiceInt body, SpiceDouble et, SpiceInt idlen, SpiceInt *handle, SpiceDouble descr[5], SpiceChar *ident, SpiceBoolean *found);
      void spkssb_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceDouble starg[6]);
      void spksub_c(SpiceInt handle, SpiceDouble descr[5], ConstSpiceChar *ident, SpiceDouble begin, SpiceDouble end, SpiceInt newh);
      void spkuds_c(ConstSpiceDouble descr[5], SpiceInt *body, SpiceInt *center, SpiceInt *frame, SpiceInt *type, SpiceDouble *first, SpiceDouble *last, SpiceInt *begin, SpiceInt *end);
      void spkuef_c(SpiceInt handle);
      void spkw02_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble btime);
      void spkw03_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble btime);
      void spkw05_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble gm, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]);
      void spkw08_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], SpiceDouble epoch1, SpiceDouble step);
      void spkw09_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]);
      void spkw10_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, ConstSpiceDouble consts[8], SpiceInt n, ConstSpiceDouble elems[], ConstSpiceDouble epochs[]);
      void spkw12_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], SpiceDouble epoch0, SpiceDouble step);
      void spkw13_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]);
      void spkw15_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble epoch, ConstSpiceDouble tp[3], ConstSpiceDouble pa[3], SpiceDouble p, SpiceDouble ecc, SpiceDouble j2flg, ConstSpiceDouble pv[3], SpiceDouble gm, SpiceDouble j2, SpiceDouble radius);
      void spkw17_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble epoch, ConstSpiceDouble eqel[9], SpiceDouble rapol, SpiceDouble decpol);
      void spkw18_c(SpiceInt handle, SpiceSPK18Subtype subtyp, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, const void *packts,ConstSpiceDouble epochs[]);
      void spkw20_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble dscale, SpiceDouble tscale, SpiceDouble initjd, SpiceDouble initfr);
      void srfrec_c(SpiceInt body, SpiceDouble lon, SpiceDouble lat, SpiceDouble rectan[3]);
      void srfc2s_c(SpiceInt code, SpiceInt bodyid, SpiceInt srflen, SpiceChar *srfstr, SpiceBoolean *isname);
      void srfcss_c(SpiceInt code, ConstSpiceChar *bodstr, SpiceInt srflen, SpiceChar *srfstr, SpiceBoolean *isname);
      void srfnrm_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, SpiceInt npts, ConstSpiceDouble srfpts[][3], SpiceDouble normls[][3]);
      void srfs2c_c(ConstSpiceChar *srfstr, ConstSpiceChar *bodstr, SpiceInt *code, SpiceBoolean *found);
      void srfscc_c(ConstSpiceChar *surfce, SpiceInt bodyid, SpiceInt *surfid, SpiceBoolean *found);
      void srfxpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], SpiceDouble spoint[3], SpiceDouble *dist, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceBoolean *found);
      void ssize_c(SpiceInt size, SpiceCell *cell);
      void stelab_c(ConstSpiceDouble pobj[3], ConstSpiceDouble vobs[3], SpiceDouble appobj[3]);
      void stpool_c(ConstSpiceChar *item, SpiceInt nth, ConstSpiceChar *contin, SpiceInt lenout, SpiceChar *string, SpiceInt *size, SpiceBoolean *found);
      void str2et_c(ConstSpiceChar *date, SpiceDouble *et);
      void subpnt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3]);
      void subpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *alt);
      void subpt_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *alt, SpiceInt *plateID);
      void subslr_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3]);
      void subsol_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3]);
      void subsol_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *dist, SpiceInt *plateID);
      SpiceDouble sumad_c(ConstSpiceDouble array[], SpiceInt n);
      SpiceInt sumai_c(ConstSpiceInt array[], SpiceInt n);
      void surfnm_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble point[3], SpiceDouble normal[3]);
      void surfpt_c(ConstSpiceDouble positn[3], ConstSpiceDouble u[3], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble point[3], SpiceBoolean *found);
      void surfpv_c(ConstSpiceDouble stvrtx[6], ConstSpiceDouble stdir[6], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble stx[6], SpiceBoolean *found);
      void swpool_c(ConstSpiceChar *agent, SpiceInt nnames, SpiceInt lenvals, const void *names);
      void sxform_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble et, SpiceDouble xform[6][6]);
      void szpool_c(ConstSpiceChar *name, SpiceInt *n, SpiceBoolean *found);
      void term_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *trmtyp, ConstSpiceChar *source, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npoints, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble trmpts[][3], SpiceInt plateIDs[]);
      void termpt_c(ConstSpiceChar *method, ConstSpiceChar *ilusrc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *corloc, ConstSpiceChar *obsrvr, ConstSpiceDouble refvec[3], SpiceDouble rolstp, SpiceInt ncuts, SpiceDouble schstp, SpiceDouble soltol, SpiceInt maxn, SpiceInt npts[], SpiceDouble points[][3], SpiceDouble epochs[], SpiceDouble tangts[][3]);
      void timdef_c(ConstSpiceChar *action, ConstSpiceChar *item, SpiceInt lenout, SpiceChar *value);
      void timout_c(SpiceDouble et, ConstSpiceChar *pictur, SpiceInt lenout, SpiceChar *output);
      void tipbod_c(ConstSpiceChar *ref, SpiceInt body, SpiceDouble et, SpiceDouble tipm[3][3]);
      void tisbod_c(ConstSpiceChar *ref, SpiceInt body, SpiceDouble et, SpiceDouble tsipm[6][6]);
      ConstSpiceChar* tkvrsn_c(ConstSpiceChar *item);
      void tparse_c(ConstSpiceChar *string, SpiceInt lenout, SpiceDouble *sp2000, SpiceChar *errmsg);
      void tpictr_c(ConstSpiceChar *sample, SpiceInt lenpictur, SpiceInt lenerror, SpiceChar *pictur, SpiceBoolean *ok, SpiceChar *error);
      SpiceDouble trace_c(ConstSpiceDouble matrix[3][3]);
      void trcdep_c(SpiceInt *depth);
      void trcnam_c(SpiceInt index, SpiceInt namelen, SpiceChar *name);
      void trcoff_c();
      void tsetyr_c(SpiceInt year);
      SpiceDouble twopi_c();
      void twovec_c(ConstSpiceDouble axdef[3], SpiceInt indexa, ConstSpiceDouble plndef[3], SpiceInt indexp, SpiceDouble mout[3][3]);
      SpiceDouble tyear_c();
      void ucase_c(SpiceChar *in,SpiceInt lenout, SpiceChar *out);
      void ucrss_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]);
      void uddc_c(void(*udfunc)(  SpiceDouble x,SpiceDouble *value), SpiceDouble x, SpiceDouble dx, SpiceBoolean *isdecr);
      void uddf_c(void(*udfunc)(  SpiceDouble x,SpiceDouble *value), SpiceDouble x, SpiceDouble dx, SpiceDouble *deriv);
      void udf_c(SpiceDouble x, SpiceDouble *value);
      void union_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      SpiceDouble unitim_c(SpiceDouble epoch, ConstSpiceChar *insys, ConstSpiceChar *outsys);
      void unload_c(ConstSpiceChar *file);
      void unorm_c(ConstSpiceDouble v1[3], SpiceDouble vout[3], SpiceDouble *vmag);
      void unormg_c(ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout, SpiceDouble *vmag);
      void utc2et_c(ConstSpiceChar *utcstr, SpiceDouble *et);
      void vadd_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]);
      void vaddg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim, SpiceDouble *vout);
      void valid_c(SpiceInt size, SpiceInt n, SpiceCell *a);
      void vcrss_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]);
      SpiceDouble vdist_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]);
      SpiceDouble vdistg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim);
      SpiceDouble vdot_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]);
      SpiceDouble vdotg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim);
      void vequ_c(ConstSpiceDouble vin[3], SpiceDouble vout[3]);
      void vequg_c(ConstSpiceDouble *vin, SpiceInt ndim, SpiceDouble *vout);
      void vhat_c(ConstSpiceDouble v1[3], SpiceDouble vout[3]);
      void vhatg_c(ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout);
      void vlcom_c(SpiceDouble a, ConstSpiceDouble v1[3], SpiceDouble b, ConstSpiceDouble v2[3], SpiceDouble sum[3]);
      void vlcom3_c(SpiceDouble a, ConstSpiceDouble v1[3], SpiceDouble b, ConstSpiceDouble v2[3], SpiceDouble c, ConstSpiceDouble v3[3], SpiceDouble sum[3]);
      void vlcomg_c(SpiceInt n, SpiceDouble a, ConstSpiceDouble *v1, SpiceDouble b, ConstSpiceDouble *v2, SpiceDouble *sum);
      void vminug_c(ConstSpiceDouble *vin, SpiceInt ndim, SpiceDouble *vout);
      void vminus_c(ConstSpiceDouble v1[3], SpiceDouble vout[3]);
      SpiceDouble vnorm_c(ConstSpiceDouble v1[3]);
      SpiceDouble vnormg_c(ConstSpiceDouble *v1, SpiceInt ndim);
      void vpack_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble v[3]);
      void vperp_c(ConstSpiceDouble a[3], ConstSpiceDouble b[3], SpiceDouble p[3]);
      void vprjp_c(ConstSpiceDouble vin[3], ConstSpicePlane *plane, SpiceDouble vout[3]);
      void vprjpi_c(ConstSpiceDouble vin[3], ConstSpicePlane *projpl, ConstSpicePlane *invpl, SpiceDouble vout[3], SpiceBoolean *found);
      void vproj_c(ConstSpiceDouble a[3], ConstSpiceDouble b[3], SpiceDouble p[3]);
      SpiceDouble vrel_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]);
      SpiceDouble vrelg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim);
      void vrotv_c(ConstSpiceDouble v[3], ConstSpiceDouble axis[3], SpiceDouble theta, SpiceDouble r[3]);
      void vscl_c(SpiceDouble s, ConstSpiceDouble v1[3], SpiceDouble vout[3]);
      void vsclg_c(SpiceDouble s, ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout);
      SpiceDouble vsep_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]);
      void vsub_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]);
      void vsubg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim, SpiceDouble *vout);
      SpiceDouble vsepg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim);
      SpiceDouble vtmv_c(ConstSpiceDouble v1[3], ConstSpiceDouble matrix[3][3], ConstSpiceDouble v2[3]);
      SpiceDouble vtmvg_c(const void *v1, const void *matrix, const void *v2, SpiceInt nrow, SpiceInt ncol);
      void vupack_c(ConstSpiceDouble v[3], SpiceDouble *x, SpiceDouble *y, SpiceDouble *z);
      SpiceBoolean vzero_c(ConstSpiceDouble v[3]);
      SpiceBoolean vzerog_c(ConstSpiceDouble *v, SpiceInt ndim);
      SpiceInt wncard_c(SpiceCell *window);
      void wncomd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window, SpiceCell *result);
      void wncond_c(SpiceDouble left, SpiceDouble right, SpiceCell *window);
      void wndifd_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      SpiceBoolean wnelmd_c(SpiceDouble point, SpiceCell *window);
      void wnexpd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window);
      void wnextd_c(SpiceChar side, SpiceCell *window);
      void wnfetd_c(SpiceCell *window, SpiceInt n, SpiceDouble *left, SpiceDouble *right);
      void wnfild_c(SpiceDouble sml, SpiceCell *window);
      void wnfltd_c(SpiceDouble sml, SpiceCell *window);
      SpiceBoolean wnincd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window);
      void wninsd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window);
      void wnintd_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      SpiceBoolean wnreld_c(SpiceCell *a, ConstSpiceChar *op, SpiceCell *b);
      void wnsumd_c(SpiceCell *window, SpiceDouble *meas, SpiceDouble *avg, SpiceDouble *stddev, SpiceInt *shortest, SpiceInt *longest);
      void wnunid_c(SpiceCell *a, SpiceCell *b, SpiceCell *c);
      void wnvald_c(SpiceInt size, SpiceInt n, SpiceCell *window);
      void xf2eul_c(ConstSpiceDouble xform[6][6], SpiceInt axisa, SpiceInt axisb, SpiceInt axisc, SpiceDouble eulang[6], SpiceBoolean *unique);
      void xf2rav_c(ConstSpiceDouble xform[6][6], SpiceDouble rot[3][3], SpiceDouble av[3]);
      void xfmsta_c(ConstSpiceDouble input_state[6], ConstSpiceChar *input_coord_sys, ConstSpiceChar *output_coord_sys, ConstSpiceChar *body, SpiceDouble output_state[6]);
      void xpose_c(ConstSpiceDouble m1[3][3], SpiceDouble mout[3][3]);
      void xpose6_c(ConstSpiceDouble m1[6][6], SpiceDouble mout[6][6]);
      void xposeg_c(const void *matrix,SpiceInt nrow,SpiceInt ncol,void *xposem);
      void zzgetcml_c(SpiceInt *argc, SpiceChar ***argv, SpiceBoolean init);
      SpiceBoolean zzgfgeth_c();
      void zzgfsavh_c(SpiceBoolean status);
      void zzsynccl_c(SpiceTransDir xdir, SpiceCell *cell);
  };

  // In case we need to go to shared pointers.
  typedef NaifContext* NaifContextPtr;

  /**
   * Helper class to manage the lifecycle of a NaifContext.
   */
  class NaifContextLifecycle {
    public:
      NaifContextLifecycle() { NaifContext::createForThread(); }
      ~NaifContextLifecycle() { NaifContext::destroyForThread(); }
  };

}

#endif

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
#include "NaifContext.h"

#include <boost/make_shared.hpp>

extern "C" {
#include <SpiceZpr.h>
}

namespace Isis {
void NaifContext::appndc_c(ConstSpiceChar *item, SpiceCell *cell) { return ::appndc_c(m_naif.get(), item, cell); }
void NaifContext::appndd_c(SpiceDouble item, SpiceCell *cell) { return ::appndd_c(m_naif.get(), item, cell); }
void NaifContext::appndi_c(SpiceInt item, SpiceCell *cell) { return ::appndi_c(m_naif.get(), item, cell); }
void NaifContext::axisar_c(ConstSpiceDouble axis[3], SpiceDouble angle, SpiceDouble r[3][3]) { return ::axisar_c(m_naif.get(), axis, angle, r); }
SpiceBoolean NaifContext::badkpv_c(ConstSpiceChar *caller, ConstSpiceChar *name, ConstSpiceChar *comp, SpiceInt size, SpiceInt divby, SpiceChar type) { return ::badkpv_c(m_naif.get(), caller, name, comp, size, divby, type); }
void NaifContext::bltfrm_c(SpiceInt frmcls, SpiceCell *idset) { return ::bltfrm_c(m_naif.get(), frmcls, idset); }
void NaifContext::bodc2n_c(SpiceInt code, SpiceInt namelen, SpiceChar *name, SpiceBoolean *found) { return ::bodc2n_c(m_naif.get(), code, namelen, name, found); }
void NaifContext::bodc2s_c(SpiceInt code, SpiceInt lenout, SpiceChar *name) { return ::bodc2s_c(m_naif.get(), code, lenout, name); }
void NaifContext::boddef_c(ConstSpiceChar *name, SpiceInt code) { return ::boddef_c(m_naif.get(), name, code); }
SpiceBoolean NaifContext::bodfnd_c(SpiceInt body, ConstSpiceChar *item) { return ::bodfnd_c(m_naif.get(), body, item); }
void NaifContext::bodn2c_c(ConstSpiceChar *name, SpiceInt *code, SpiceBoolean *found) { return ::bodn2c_c(m_naif.get(), name, code, found); }
void NaifContext::bods2c_c(ConstSpiceChar *name, SpiceInt *code, SpiceBoolean *found) { return ::bods2c_c(m_naif.get(), name, code, found); }
void NaifContext::bodvar_c(SpiceInt body, ConstSpiceChar *item, SpiceInt *dim, SpiceDouble *values) { return ::bodvar_c(m_naif.get(), body, item, dim, values); }
void NaifContext::bodvcd_c(SpiceInt body, ConstSpiceChar *item, SpiceInt maxn, SpiceInt *dim, SpiceDouble *values) { return ::bodvcd_c(m_naif.get(), body, item, maxn, dim, values); }
void NaifContext::bodvrd_c(ConstSpiceChar *body, ConstSpiceChar *item, SpiceInt maxn, SpiceInt *dim, SpiceDouble *values) { return ::bodvrd_c(m_naif.get(), body, item, maxn, dim, values); }
SpiceDouble NaifContext::brcktd_c(SpiceDouble number, SpiceDouble end1, SpiceDouble end2) { return ::brcktd_c(m_naif.get(), number, end1, end2); }
SpiceInt NaifContext::brckti_c(SpiceInt number, SpiceInt end1, SpiceInt end2) { return ::brckti_c(m_naif.get(), number, end1, end2); }
SpiceInt NaifContext::bschoc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array, ConstSpiceInt *order) { return ::bschoc_c(m_naif.get(), value, ndim, lenvals, array, order); }
SpiceInt NaifContext::bschoi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array, ConstSpiceInt *order) { return ::bschoi_c(m_naif.get(), value, ndim, array, order); }
SpiceInt NaifContext::bsrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array) { return ::bsrchc_c(m_naif.get(), value, ndim, lenvals, array); }
SpiceInt NaifContext::bsrchd_c(SpiceDouble value, SpiceInt ndim, ConstSpiceDouble *array) { return ::bsrchd_c(m_naif.get(), value, ndim, array); }
SpiceInt NaifContext::bsrchi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array) { return ::bsrchi_c(m_naif.get(), value, ndim, array); }
SpiceDouble NaifContext::b1900_c() { return ::b1900_c(m_naif.get()); }
SpiceDouble NaifContext::b1950_c() { return ::b1950_c(m_naif.get()); }
SpiceInt NaifContext::card_c(SpiceCell *cell) { return ::card_c(m_naif.get(), cell); }
void NaifContext::ccifrm_c(SpiceInt frclss, SpiceInt clssid, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceInt *center, SpiceBoolean *found) { return ::ccifrm_c(m_naif.get(), frclss, clssid, lenout, frcode, frname, center, found); }
void NaifContext::cgv2el_c(ConstSpiceDouble center[3], ConstSpiceDouble vec1[3], ConstSpiceDouble vec2[3], SpiceEllipse *ellipse) { return ::cgv2el_c(m_naif.get(), center, vec1, vec2, ellipse); }
void NaifContext::chbder_c(ConstSpiceDouble *cp, SpiceInt degp, SpiceDouble x2s[2], SpiceDouble x, SpiceInt nderiv, SpiceDouble *partdp, SpiceDouble *dpdxs) { return ::chbder_c(m_naif.get(), cp, degp, x2s, x, nderiv, partdp, dpdxs); }
void NaifContext::chkin_c(ConstSpiceChar *module) { return ::chkin_c(m_naif.get(), module); }
void NaifContext::chkout_c(ConstSpiceChar *module) { return ::chkout_c(m_naif.get(), module); }
void NaifContext::cidfrm_c(SpiceInt cent, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceBoolean *found) { return ::cidfrm_c(m_naif.get(), cent, lenout, frcode, frname, found); }
void NaifContext::ckcls_c(SpiceInt handle) { return ::ckcls_c(m_naif.get(), handle); }
void NaifContext::ckcov_c(ConstSpiceChar *ck, SpiceInt idcode, SpiceBoolean needav, ConstSpiceChar *level, SpiceDouble tol, ConstSpiceChar *timsys, SpiceCell *cover) { return ::ckcov_c(m_naif.get(), ck, idcode, needav, level, tol, timsys, cover); }
void NaifContext::ckobj_c(ConstSpiceChar *ck, SpiceCell *ids) { return ::ckobj_c(m_naif.get(), ck, ids); }
void NaifContext::ckgp_c(SpiceInt inst, SpiceDouble sclkdp, SpiceDouble tol, ConstSpiceChar *ref, SpiceDouble cmat[3][3], SpiceDouble *clkout, SpiceBoolean *found) { return ::ckgp_c(m_naif.get(), inst, sclkdp, tol, ref, cmat, clkout, found); }
void NaifContext::ckgpav_c(SpiceInt inst, SpiceDouble sclkdp, SpiceDouble tol, ConstSpiceChar *ref, SpiceDouble cmat[3][3], SpiceDouble av[3], SpiceDouble *clkout, SpiceBoolean *found) { return ::ckgpav_c(m_naif.get(), inst, sclkdp, tol, ref, cmat, av, clkout, found); }
void NaifContext::cklpf_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::cklpf_c(m_naif.get(), fname, handle); }
void NaifContext::ckopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle) { return ::ckopn_c(m_naif.get(), name, ifname, ncomch, handle); }
void NaifContext::ckupf_c(SpiceInt handle) { return ::ckupf_c(m_naif.get(), handle); }
void NaifContext::ckw01_c(SpiceInt handle, SpiceDouble begtime, SpiceDouble endtime, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble sclkdp[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3]) { return ::ckw01_c(m_naif.get(), handle, begtime, endtime, inst, ref, avflag, segid, nrec, sclkdp, quats, avvs); }
void NaifContext::ckw02_c(SpiceInt handle, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble start[], ConstSpiceDouble stop[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3], ConstSpiceDouble rates[]) { return ::ckw02_c(m_naif.get(), handle, begtim, endtim, inst, ref, segid, nrec, start, stop, quats, avvs, rates); }
void NaifContext::ckw03_c(SpiceInt handle, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt nrec, ConstSpiceDouble sclkdp[], ConstSpiceDouble quats[][4], ConstSpiceDouble avvs[][3], SpiceInt nints, ConstSpiceDouble starts[]) { return ::ckw03_c(m_naif.get(), handle, begtim, endtim, inst, ref, avflag, segid, nrec, sclkdp, quats, avvs, nints, starts); }
void NaifContext::ckw05_c(SpiceInt handle, SpiceCK05Subtype subtyp, SpiceInt degree, SpiceDouble begtim, SpiceDouble endtim, SpiceInt inst, ConstSpiceChar *ref, SpiceBoolean avflag, ConstSpiceChar *segid, SpiceInt n, ConstSpiceDouble sclkdp[], const void *packets, SpiceDouble rate, SpiceInt nints, ConstSpiceDouble starts[]) { return ::ckw05_c(m_naif.get(), handle, subtyp, degree, begtim, endtim, inst, ref, avflag, segid, n, sclkdp, packets, rate, nints, starts); }
void NaifContext::cleard_c(SpiceInt ndim, SpiceDouble *array) { return ::cleard_c(m_naif.get(), ndim, array); }
SpiceDouble NaifContext::clight_c() { return ::clight_c(m_naif.get()); }
void NaifContext::clpool_c() { return ::clpool_c(m_naif.get()); }
void NaifContext::cmprss_c(SpiceChar delim, SpiceInt n, ConstSpiceChar *input, SpiceInt lenout, SpiceChar *output) { return ::cmprss_c(m_naif.get(), delim, n, input, lenout, output); }
void NaifContext::cnmfrm_c(ConstSpiceChar *cname, SpiceInt lenout, SpiceInt *frcode, SpiceChar *frname, SpiceBoolean *found) { return ::cnmfrm_c(m_naif.get(), cname, lenout, frcode, frname, found); }
void NaifContext::conics_c(ConstSpiceDouble elts[8], SpiceDouble et, SpiceDouble state[6]) { return ::conics_c(m_naif.get(), elts, et, state); }
void NaifContext::convrt_c(SpiceDouble x, ConstSpiceChar *in, ConstSpiceChar *out, SpiceDouble *y) { return ::convrt_c(m_naif.get(), x, in, out, y); }
void NaifContext::copy_c(SpiceCell *a, SpiceCell *b) { return ::copy_c(m_naif.get(), a, b); }
SpiceInt NaifContext::cpos_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start) { return ::cpos_c(m_naif.get(), str, chars, start); }
SpiceInt NaifContext::cposr_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start) { return ::cposr_c(m_naif.get(), str, chars, start); }
void NaifContext::cvpool_c(ConstSpiceChar *agent, SpiceBoolean *update) { return ::cvpool_c(m_naif.get(), agent, update); }
void NaifContext::cyllat_c(SpiceDouble r, SpiceDouble lonc, SpiceDouble z, SpiceDouble *radius, SpiceDouble *lon, SpiceDouble *lat) { return ::cyllat_c(m_naif.get(), r, lonc, z, radius, lon, lat); }
void NaifContext::cylrec_c(SpiceDouble r, SpiceDouble lon, SpiceDouble z, SpiceDouble rectan[3]) { return ::cylrec_c(m_naif.get(), r, lon, z, rectan); }
void NaifContext::cylsph_c(SpiceDouble r, SpiceDouble lonc, SpiceDouble z, SpiceDouble *radius, SpiceDouble *colat, SpiceDouble *lon) { return ::cylsph_c(m_naif.get(), r, lonc, z, radius, colat, lon); }
void NaifContext::dafac_c(SpiceInt handle, SpiceInt n, SpiceInt lenvals, const void *buffer) { return ::dafac_c(m_naif.get(), handle, n, lenvals, buffer); }
void NaifContext::dafbbs_c(SpiceInt handle) { return ::dafbbs_c(m_naif.get(), handle); }
void NaifContext::dafbfs_c(SpiceInt handle) { return ::dafbfs_c(m_naif.get(), handle); }
void NaifContext::dafcls_c(SpiceInt handle) { return ::dafcls_c(m_naif.get(), handle); }
void NaifContext::dafcs_c(SpiceInt handle) { return ::dafcs_c(m_naif.get(), handle); }
void NaifContext::dafdc_c(SpiceInt handle) { return ::dafdc_c(m_naif.get(), handle); }
void NaifContext::dafec_c(SpiceInt handle, SpiceInt bufsiz, SpiceInt lenout, SpiceInt *n, void *buffer, SpiceBoolean *done) { return ::dafec_c(m_naif.get(), handle, bufsiz, lenout, n, buffer, done); }
void NaifContext::daffna_c(SpiceBoolean *found) { return ::daffna_c(m_naif.get(), found); }
void NaifContext::daffpa_c(SpiceBoolean *found) { return ::daffpa_c(m_naif.get(), found); }
void NaifContext::dafgda_c(SpiceInt handle, SpiceInt begin, SpiceInt end, SpiceDouble *data) { return ::dafgda_c(m_naif.get(), handle, begin, end, data); }
void NaifContext::dafgh_c(SpiceInt *handle) { return ::dafgh_c(m_naif.get(), handle); }
void NaifContext::dafgn_c(SpiceInt lenout, SpiceChar *name) { return ::dafgn_c(m_naif.get(), lenout, name); }
void NaifContext::dafgs_c(SpiceDouble sum[]) { return ::dafgs_c(m_naif.get(), sum); }
void NaifContext::dafgsr_c(SpiceInt handle, SpiceInt recno, SpiceInt begin, SpiceInt end, SpiceDouble *data, SpiceBoolean *found) { return ::dafgsr_c(m_naif.get(), handle, recno, begin, end, data, found); }
void NaifContext::dafopr_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::dafopr_c(m_naif.get(), fname, handle); }
void NaifContext::dafopw_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::dafopw_c(m_naif.get(), fname, handle); }
void NaifContext::dafps_c(SpiceInt nd, SpiceInt ni, ConstSpiceDouble dc[], ConstSpiceInt ic[], SpiceDouble sum[]) { return ::dafps_c(m_naif.get(), nd, ni, dc, ic, sum); }
void NaifContext::dafrda_c(SpiceInt handle, SpiceInt begin, SpiceInt end, SpiceDouble *data) { return ::dafrda_c(m_naif.get(), handle, begin, end, data); }
void NaifContext::dafrfr_c(SpiceInt handle, SpiceInt lenout, SpiceInt *nd, SpiceInt *ni, SpiceChar *ifname, SpiceInt *fward, SpiceInt *bward, SpiceInt *free) { return ::dafrfr_c(m_naif.get(), handle, lenout, nd, ni, ifname, fward, bward, free); }
void NaifContext::dafrs_c(ConstSpiceDouble *sum) { return ::dafrs_c(m_naif.get(), sum); }
void NaifContext::dafus_c(ConstSpiceDouble sum[], SpiceInt nd, SpiceInt ni, SpiceDouble dc[], SpiceInt ic[]) { return ::dafus_c(m_naif.get(), sum, nd, ni, dc, ic); }
void NaifContext::dasac_c(SpiceInt handle, SpiceInt n, SpiceInt buflen, const void *buffer) { return ::dasac_c(m_naif.get(), handle, n, buflen, buffer); }
void NaifContext::dascls_c(SpiceInt handle) { return ::dascls_c(m_naif.get(), handle); }
void NaifContext::dasdc_c(SpiceInt handle) { return ::dasdc_c(m_naif.get(), handle); }
void NaifContext::dasec_c(SpiceInt handle, SpiceInt bufsiz, SpiceInt buflen, SpiceInt *n, void *buffer, SpiceBoolean *done) { return ::dasec_c(m_naif.get(), handle, bufsiz, buflen, n, buffer, done); }
void NaifContext::dashfn_c(SpiceInt handle, SpiceInt namlen, SpiceChar *fname) { return ::dashfn_c(m_naif.get(), handle, namlen, fname); }
void NaifContext::dasopr_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::dasopr_c(m_naif.get(), fname, handle); }
void NaifContext::dasopw_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::dasopw_c(m_naif.get(), fname, handle); }
void NaifContext::dasrfr_c(SpiceInt handle, SpiceInt idwlen, SpiceInt ifnlen, SpiceChar *idword, SpiceChar *ifname, SpiceInt *nresvr, SpiceInt *nresvc, SpiceInt *ncomr, SpiceInt *ncomc) { return ::dasrfr_c(m_naif.get(), handle, idwlen, ifnlen, idword, ifname, nresvr, nresvc, ncomr, ncomc); }
void NaifContext::dcyldr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]) { return ::dcyldr_c(m_naif.get(), x, y, z, jacobi); }
void NaifContext::deltet_c(SpiceDouble epoch, ConstSpiceChar *eptype, SpiceDouble *delta) { return ::deltet_c(m_naif.get(), epoch, eptype, delta); }
SpiceDouble NaifContext::det_c(ConstSpiceDouble m1[3][3]) { return ::det_c(m_naif.get(), m1); }
void NaifContext::diags2_c(ConstSpiceDouble symmat[2][2], SpiceDouble diag[2][2], SpiceDouble rotate[2][2]) { return ::diags2_c(m_naif.get(), symmat, diag, rotate); }
void NaifContext::diff_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::diff_c(m_naif.get(), a, b, c); }
void NaifContext::dgeodr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]) { return ::dgeodr_c(m_naif.get(), x, y, z, re, f, jacobi); }
void NaifContext::dlabbs_c(SpiceInt handle, SpiceDLADescr *descr, SpiceBoolean *found) { return ::dlabbs_c(m_naif.get(), handle, descr, found); }
void NaifContext::dlabfs_c(SpiceInt handle, SpiceDLADescr *descr, SpiceBoolean *found) { return ::dlabfs_c(m_naif.get(), handle, descr, found); }
void NaifContext::dlafns_c(SpiceInt handle, ConstSpiceDLADescr *descr, SpiceDLADescr *nxtdsc, SpiceBoolean *found) { return ::dlafns_c(m_naif.get(), handle, descr, nxtdsc, found); }
void NaifContext::dlafps_c(SpiceInt handle, ConstSpiceDLADescr *descr, SpiceDLADescr *prvdsc, SpiceBoolean *found) { return ::dlafps_c(m_naif.get(), handle, descr, prvdsc, found); }
void NaifContext::dlatdr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]) { return ::dlatdr_c(m_naif.get(), x, y, z, jacobi); }
void NaifContext::dp2hx_c(SpiceDouble number, SpiceInt lenout, SpiceChar *string, SpiceInt *length) { return ::dp2hx_c(m_naif.get(), number, lenout, string, length); }
void NaifContext::dpgrdr_c(ConstSpiceChar *body, SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]) { return ::dpgrdr_c(m_naif.get(), body, x, y, z, re, f, jacobi); }
SpiceDouble NaifContext::dpmax_c() { return ::dpmax_c(m_naif.get()); }
SpiceDouble NaifContext::dpmax_() { return ::dpmax_(m_naif.get()); }
SpiceDouble NaifContext::dpmin_c() { return ::dpmin_c(m_naif.get()); }
SpiceDouble NaifContext::dpmin_() { return ::dpmin_(m_naif.get()); }
SpiceDouble NaifContext::dpr_c() { return ::dpr_c(m_naif.get()); }
void NaifContext::drdcyl_c(SpiceDouble r, SpiceDouble lon, SpiceDouble z, SpiceDouble jacobi[3][3]) { return ::drdcyl_c(m_naif.get(), r, lon, z, jacobi); }
void NaifContext::drdgeo_c(SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]) { return ::drdgeo_c(m_naif.get(), lon, lat, alt, re, f, jacobi); }
void NaifContext::drdlat_c(SpiceDouble r, SpiceDouble lon, SpiceDouble lat, SpiceDouble jacobi[3][3]) { return ::drdlat_c(m_naif.get(), r, lon, lat, jacobi); }
void NaifContext::drdpgr_c(ConstSpiceChar *body, SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble jacobi[3][3]) { return ::drdpgr_c(m_naif.get(), body, lon, lat, alt, re, f, jacobi); }
void NaifContext::drdsph_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lon, SpiceDouble jacobi[3][3]) { return ::drdsph_c(m_naif.get(), r, colat, lon, jacobi); }
void NaifContext::dskb02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt *nv, SpiceInt *np, SpiceInt *nvxtot, SpiceDouble vtxbds[3][2], SpiceDouble *voxsiz, SpiceDouble voxori[3], SpiceInt vgrext[3], SpiceInt *cgscal, SpiceInt *vtxnpl, SpiceInt *voxnpt, SpiceInt *voxnpl) { return ::dskb02_c(m_naif.get(), handle, dladsc, nv, np, nvxtot, vtxbds, voxsiz, voxori, vgrext, cgscal, vtxnpl, voxnpt, voxnpl); }
void NaifContext::dskcls_c(SpiceInt handle, SpiceBoolean optmiz) { return ::dskcls_c(m_naif.get(), handle, optmiz); }
void NaifContext::dskd02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt item, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble *values) { return ::dskd02_c(m_naif.get(), handle, dladsc, item, start, room, n, values); }
void NaifContext::dskgd_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceDSKDescr *dskdsc) { return ::dskgd_c(m_naif.get(), handle, dladsc, dskdsc); }
void NaifContext::dskgtl_c(SpiceInt keywrd, SpiceDouble *dpval) { return ::dskgtl_c(m_naif.get(), keywrd, dpval); }
void NaifContext::dski02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt item, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt *values) { return ::dski02_c(m_naif.get(), handle, dladsc, item, start, room, n, values); }
void NaifContext::dskobj_c(ConstSpiceChar *dsk, SpiceCell *bodids) { return ::dskobj_c(m_naif.get(), dsk, bodids); }
void NaifContext::dskopn_c(ConstSpiceChar *fname, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle) { return ::dskopn_c(m_naif.get(), fname, ifname, ncomch, handle); }
void NaifContext::dskn02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt plid, SpiceDouble normal[3]) { return ::dskn02_c(m_naif.get(), handle, dladsc, plid, normal); }
void NaifContext::dskmi2_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], SpiceDouble finscl, SpiceInt corscl, SpiceInt worksz, SpiceInt voxpsz, SpiceInt voxlsz, SpiceBoolean makvtl, SpiceInt spxisz, SpiceInt work[][2], SpiceDouble spaixd[], SpiceInt spaixi[]) { return ::dskmi2_c(m_naif.get(), nv, vrtces, np, plates, finscl, corscl, worksz, voxpsz, voxlsz, makvtl, spxisz, work, spaixd, spaixi); }
void NaifContext::dskp02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt plates[][3]) { return ::dskp02_c(m_naif.get(), handle, dladsc, start, room, n, plates); }
void NaifContext::dskrb2_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], SpiceInt corsys, ConstSpiceDouble corpar[], SpiceDouble *mncor3, SpiceDouble *mxcor3) { return ::dskrb2_c(m_naif.get(), nv, vrtces, np, plates, corsys, corpar, mncor3, mxcor3); }
void NaifContext::dsksrf_c(ConstSpiceChar *dsk, SpiceInt bodyid, SpiceCell *srfids) { return ::dsksrf_c(m_naif.get(), dsk, bodyid, srfids); }
void NaifContext::dskstl_c(SpiceInt keywrd, SpiceDouble dpval) { return ::dskstl_c(m_naif.get(), keywrd, dpval); }
void NaifContext::dskv02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble vrtces[][3]) { return ::dskv02_c(m_naif.get(), handle, dladsc, start, room, n, vrtces); }
void NaifContext::dskw02_c(SpiceInt handle, SpiceInt center, SpiceInt surfce, SpiceInt dclass, ConstSpiceChar *frame, SpiceInt corsys, ConstSpiceDouble corpar[], SpiceDouble mncor1, SpiceDouble mxcor1, SpiceDouble mncor2, SpiceDouble mxcor2, SpiceDouble mncor3, SpiceDouble mxcor3, SpiceDouble first, SpiceDouble last, SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3], ConstSpiceDouble spaixd[], ConstSpiceInt spaixi[]) { return ::dskw02_c(m_naif.get(), handle, center, surfce, dclass, frame, corsys, corpar, mncor1, mxcor1, mncor2, mxcor2, mncor3, mxcor3, first, last, nv, vrtces, np, plates, spaixd, spaixi); }
void NaifContext::dskx02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceDouble vertex[3], ConstSpiceDouble raydir[3], SpiceInt *plid, SpiceDouble xpt[3], SpiceBoolean *found) { return ::dskx02_c(m_naif.get(), handle, dladsc, vertex, raydir, plid, xpt, found); }
void NaifContext::dskxsi_c(SpiceBoolean pri, ConstSpiceChar *target, SpiceInt nsurf, ConstSpiceInt srflst[], SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceDouble vertex[3], ConstSpiceDouble raydir[3], SpiceInt maxd, SpiceInt maxi, SpiceDouble xpt[3], SpiceInt *handle, SpiceDLADescr *dladsc, SpiceDSKDescr *dskdsc, SpiceDouble dc[], SpiceInt ic[], SpiceBoolean *found) { return ::dskxsi_c(m_naif.get(), pri, target, nsurf, srflst, et, fixref, vertex, raydir, maxd, maxi, xpt, handle, dladsc, dskdsc, dc, ic, found); }
void NaifContext::dskxv_c(SpiceBoolean pri, ConstSpiceChar *target, SpiceInt nsurf, ConstSpiceInt srflst[], SpiceDouble et, ConstSpiceChar *fixref, SpiceInt nrays, ConstSpiceDouble vtxarr[][3], ConstSpiceDouble dirarr[][3], SpiceDouble xptarr[][3], SpiceBoolean fndarr[]) { return ::dskxv_c(m_naif.get(), pri, target, nsurf, srflst, et, fixref, nrays, vtxarr, dirarr, xptarr, fndarr); }
void NaifContext::dskz02_c(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt *nv, SpiceInt *np) { return ::dskz02_c(m_naif.get(), handle, dladsc, nv, np); }
void NaifContext::dsphdr_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble jacobi[3][3]) { return ::dsphdr_c(m_naif.get(), x, y, z, jacobi); }
void NaifContext::dtpool_c(ConstSpiceChar *name, SpiceBoolean *found, SpiceInt *n, SpiceChar type[1]) { return ::dtpool_c(m_naif.get(), name, found, n, type); }
void NaifContext::ducrss_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6], SpiceDouble sout[6]) { return ::ducrss_c(m_naif.get(), s1, s2, sout); }
void NaifContext::dvcrss_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6], SpiceDouble sout[6]) { return ::dvcrss_c(m_naif.get(), s1, s2, sout); }
SpiceDouble NaifContext::dvdot_c(ConstSpiceDouble s1[6], ConstSpiceDouble s2[6]) { return ::dvdot_c(m_naif.get(), s1, s2); }
void NaifContext::dvhat_c(ConstSpiceDouble s1[6], SpiceDouble sout[6]) { return ::dvhat_c(m_naif.get(), s1, sout); }
SpiceDouble NaifContext::dvnorm_c(ConstSpiceDouble state[6]) { return ::dvnorm_c(m_naif.get(), state); }
void NaifContext::dvpool_c(ConstSpiceChar *name) { return ::dvpool_c(m_naif.get(), name); }
SpiceDouble NaifContext::dvsep_c(ConstSpiceDouble *s1, ConstSpiceDouble *s2) { return ::dvsep_c(m_naif.get(), s1, s2); }
void NaifContext::edlimb_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble viewpt[3], SpiceEllipse *limb) { return ::edlimb_c(m_naif.get(), a, b, c, viewpt, limb); }
void NaifContext::edterm_c(ConstSpiceChar *trmtyp, ConstSpiceChar *source, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixfrm, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npts, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble termpts[][3]) { return ::edterm_c(m_naif.get(), trmtyp, source, target, et, fixfrm, abcorr, obsrvr, npts, trgepc, obspos, termpts); }
void NaifContext::ekacec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, SpiceInt vallen, const void *cvals, SpiceBoolean isnull) { return ::ekacec_c(m_naif.get(), handle, segno, recno, column, nvals, vallen, cvals, isnull); }
void NaifContext::ekaced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceDouble *dvals, SpiceBoolean isnull) { return ::ekaced_c(m_naif.get(), handle, segno, recno, column, nvals, dvals, isnull); }
void NaifContext::ekacei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceInt *ivals, SpiceBoolean isnull) { return ::ekacei_c(m_naif.get(), handle, segno, recno, column, nvals, ivals, isnull); }
void NaifContext::ekaclc_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, SpiceInt vallen, const void *cvals, ConstSpiceInt *entszs, ConstSpiceBoolean *nlflgs, ConstSpiceInt *rcptrs, SpiceInt *wkindx) { return ::ekaclc_c(m_naif.get(), handle, segno, column, vallen, cvals, entszs, nlflgs, rcptrs, wkindx); }
void NaifContext::ekacld_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, ConstSpiceDouble *dvals, ConstSpiceInt *entszs, ConstSpiceBoolean *nlflgs, ConstSpiceInt *rcptrs, SpiceInt *wkindx) { return ::ekacld_c(m_naif.get(), handle, segno, column, dvals, entszs, nlflgs, rcptrs, wkindx); }
void NaifContext::ekacli_c(SpiceInt handle, SpiceInt segno, ConstSpiceChar *column, ConstSpiceInt *ivals, ConstSpiceInt *entszs, ConstSpiceBoolean *nlflgs, ConstSpiceInt *rcptrs, SpiceInt *wkindx) { return ::ekacli_c(m_naif.get(), handle, segno, column, ivals, entszs, nlflgs, rcptrs, wkindx); }
void NaifContext::ekappr_c(SpiceInt handle, SpiceInt segno, SpiceInt *recno) { return ::ekappr_c(m_naif.get(), handle, segno, recno); }
void NaifContext::ekbseg_c(SpiceInt handle, ConstSpiceChar *tabnam, SpiceInt ncols, SpiceInt cnmlen, const void *cnames, SpiceInt declen, const void *decls, SpiceInt *segno) { return ::ekbseg_c(m_naif.get(), handle, tabnam, ncols, cnmlen, cnames, declen, decls, segno); }
void NaifContext::ekccnt_c(ConstSpiceChar *table, SpiceInt *ccount) { return ::ekccnt_c(m_naif.get(), table, ccount); }
void NaifContext::ekcii_c(ConstSpiceChar *table, SpiceInt cindex, SpiceInt lenout, SpiceChar *column, SpiceEKAttDsc *attdsc) { return ::ekcii_c(m_naif.get(), table, cindex, lenout, column, attdsc); }
void NaifContext::ekcls_c(SpiceInt handle) { return ::ekcls_c(m_naif.get(), handle); }
void NaifContext::ekdelr_c(SpiceInt handle, SpiceInt segno, SpiceInt recno) { return ::ekdelr_c(m_naif.get(), handle, segno, recno); }
void NaifContext::ekffld_c(SpiceInt handle, SpiceInt segno, SpiceInt *rcptrs) { return ::ekffld_c(m_naif.get(), handle, segno, rcptrs); }
void NaifContext::ekfind_c(ConstSpiceChar *query, SpiceInt lenout, SpiceInt *nmrows, SpiceBoolean *error, SpiceChar *errmsg) { return ::ekfind_c(m_naif.get(), query, lenout, nmrows, error, errmsg); }
void NaifContext::ekgc_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceInt lenout, SpiceChar *cdata, SpiceBoolean *null, SpiceBoolean *found) { return ::ekgc_c(m_naif.get(), selidx, row, elment, lenout, cdata, null, found); }
void NaifContext::ekgd_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceDouble *ddata, SpiceBoolean *null, SpiceBoolean *found) { return ::ekgd_c(m_naif.get(), selidx, row, elment, ddata, null, found); }
void NaifContext::ekgi_c(SpiceInt selidx, SpiceInt row, SpiceInt elment, SpiceInt *idata, SpiceBoolean *null, SpiceBoolean *found) { return ::ekgi_c(m_naif.get(), selidx, row, elment, idata, null, found); }
void NaifContext::ekifld_c(SpiceInt handle, ConstSpiceChar *tabnam, SpiceInt ncols, SpiceInt nrows, SpiceInt cnmlen, const void *cnames, SpiceInt declen, const void *decls, SpiceInt *segno, SpiceInt *rcptrs) { return ::ekifld_c(m_naif.get(), handle, tabnam, ncols, nrows, cnmlen, cnames, declen, decls, segno, rcptrs); }
void NaifContext::ekinsr_c(SpiceInt handle, SpiceInt segno, SpiceInt recno) { return ::ekinsr_c(m_naif.get(), handle, segno, recno); }
void NaifContext::eklef_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::eklef_c(m_naif.get(), fname, handle); }
SpiceInt NaifContext::eknelt_c(SpiceInt selidx, SpiceInt row) { return ::eknelt_c(m_naif.get(), selidx, row); }
SpiceInt NaifContext::eknseg_c(SpiceInt handle) { return ::eknseg_c(m_naif.get(), handle); }
void NaifContext::ekntab_c(SpiceInt *n) { return ::ekntab_c(m_naif.get(), n); }
void NaifContext::ekopn_c(ConstSpiceChar *fname, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle) { return ::ekopn_c(m_naif.get(), fname, ifname, ncomch, handle); }
void NaifContext::ekopr_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::ekopr_c(m_naif.get(), fname, handle); }
void NaifContext::ekops_c(SpiceInt *handle) { return ::ekops_c(m_naif.get(), handle); }
void NaifContext::ekopw_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::ekopw_c(m_naif.get(), fname, handle); }
void NaifContext::ekpsel_c(ConstSpiceChar *query, SpiceInt msglen, SpiceInt tablen, SpiceInt collen, SpiceInt *n, SpiceInt *xbegs, SpiceInt *xends, SpiceEKDataType *xtypes, SpiceEKExprClass *xclass, void *tabs, void *cols, SpiceBoolean *error, SpiceChar *errmsg) { return ::ekpsel_c(m_naif.get(), query, msglen, tablen, collen, n, xbegs, xends, xtypes, xclass, tabs, cols, error, errmsg); }
void NaifContext::ekrcec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt lenout, SpiceInt *nvals, void *cvals, SpiceBoolean *isnull) { return ::ekrcec_c(m_naif.get(), handle, segno, recno, column, lenout, nvals, cvals, isnull); }
void NaifContext::ekrced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt *nvals, SpiceDouble *dvals, SpiceBoolean *isnull) { return ::ekrced_c(m_naif.get(), handle, segno, recno, column, nvals, dvals, isnull); }
void NaifContext::ekrcei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt *nvals, SpiceInt *ivals, SpiceBoolean *isnull) { return ::ekrcei_c(m_naif.get(), handle, segno, recno, column, nvals, ivals, isnull); }
void NaifContext::ekssum_c(SpiceInt handle, SpiceInt segno, SpiceEKSegSum *segsum) { return ::ekssum_c(m_naif.get(), handle, segno, segsum); }
void NaifContext::ektnam_c(SpiceInt n, SpiceInt lenout, SpiceChar *table) { return ::ektnam_c(m_naif.get(), n, lenout, table); }
void NaifContext::ekucec_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, SpiceInt vallen, const void *cvals, SpiceBoolean isnull) { return ::ekucec_c(m_naif.get(), handle, segno, recno, column, nvals, vallen, cvals, isnull); }
void NaifContext::ekuced_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceDouble *dvals, SpiceBoolean isnull) { return ::ekuced_c(m_naif.get(), handle, segno, recno, column, nvals, dvals, isnull); }
void NaifContext::ekucei_c(SpiceInt handle, SpiceInt segno, SpiceInt recno, ConstSpiceChar *column, SpiceInt nvals, ConstSpiceInt *ivals, SpiceBoolean isnull) { return ::ekucei_c(m_naif.get(), handle, segno, recno, column, nvals, ivals, isnull); }
void NaifContext::ekuef_c(SpiceInt handle) { return ::ekuef_c(m_naif.get(), handle); }
SpiceBoolean NaifContext::elemc_c(ConstSpiceChar *item, SpiceCell *set) { return ::elemc_c(m_naif.get(), item, set); }
SpiceBoolean NaifContext::elemd_c(SpiceDouble item, SpiceCell *set) { return ::elemd_c(m_naif.get(), item, set); }
SpiceBoolean NaifContext::elemi_c(SpiceInt item, SpiceCell *set) { return ::elemi_c(m_naif.get(), item, set); }
void NaifContext::eqncpv_c(SpiceDouble et, SpiceDouble epoch, ConstSpiceDouble eqel[9], SpiceDouble rapol, SpiceDouble decpol, SpiceDouble state[6]) { return ::eqncpv_c(m_naif.get(), et, epoch, eqel, rapol, decpol, state); }
SpiceBoolean NaifContext::eqstr_c(ConstSpiceChar *a, ConstSpiceChar *b) { return ::eqstr_c(m_naif.get(), a, b); }
void NaifContext::el2cgv_c(ConstSpiceEllipse *ellipse, SpiceDouble center[3], SpiceDouble smajor[3], SpiceDouble sminor[3]) { return ::el2cgv_c(m_naif.get(), ellipse, center, smajor, sminor); }
void NaifContext::erract_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *action) { return ::erract_c(m_naif.get(), operation, lenout, action); }
void NaifContext::errch_c(ConstSpiceChar *marker, ConstSpiceChar *string) { return ::errch_c(m_naif.get(), marker, string); }
void NaifContext::errdev_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *device) { return ::errdev_c(m_naif.get(), operation, lenout, device); }
void NaifContext::errdp_c(ConstSpiceChar *marker, SpiceDouble number) { return ::errdp_c(m_naif.get(), marker, number); }
void NaifContext::errint_c(ConstSpiceChar *marker, SpiceInt number) { return ::errint_c(m_naif.get(), marker, number); }
void NaifContext::errprt_c(ConstSpiceChar *operation, SpiceInt lenout, SpiceChar *list) { return ::errprt_c(m_naif.get(), operation, lenout, list); }
SpiceInt NaifContext::esrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array) { return ::esrchc_c(m_naif.get(), value, ndim, lenvals, array); }
void NaifContext::etcal_c(SpiceDouble et, SpiceInt lenout, SpiceChar *string) { return ::etcal_c(m_naif.get(), et, lenout, string); }
void NaifContext::et2lst_c(SpiceDouble et, SpiceInt body, SpiceDouble lon, ConstSpiceChar *type, SpiceInt timlen, SpiceInt ampmlen, SpiceInt *hr, SpiceInt *mn, SpiceInt *sc, SpiceChar *time, SpiceChar *ampm) { return ::et2lst_c(m_naif.get(), et, body, lon, type, timlen, ampmlen, hr, mn, sc, time, ampm); }
void NaifContext::et2utc_c(SpiceDouble et, ConstSpiceChar *format, SpiceInt prec, SpiceInt lenout, SpiceChar *utcstr) { return ::et2utc_c(m_naif.get(), et, format, prec, lenout, utcstr); }
void NaifContext::eul2m_c(SpiceDouble angle3, SpiceDouble angle2, SpiceDouble angle1, SpiceInt axis3, SpiceInt axis2, SpiceInt axis1, SpiceDouble r[3][3]) { return ::eul2m_c(m_naif.get(), angle3, angle2, angle1, axis3, axis2, axis1, r); }
void NaifContext::eul2xf_c(ConstSpiceDouble eulang[6], SpiceInt axisa, SpiceInt axisb, SpiceInt axisc, SpiceDouble xform[6][6]) { return ::eul2xf_c(m_naif.get(), eulang, axisa, axisb, axisc, xform); }
SpiceBoolean NaifContext::exists_c(ConstSpiceChar *name) { return ::exists_c(m_naif.get(), name); }
void NaifContext::expool_c(ConstSpiceChar *name, SpiceBoolean *found) { return ::expool_c(m_naif.get(), name, found); }
SpiceBoolean NaifContext::failed_c() { return ::failed_c(m_naif.get()); }
void NaifContext::fovray_c(ConstSpiceChar *inst, ConstSpiceDouble raydir[3], ConstSpiceChar *rframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble *et, SpiceBoolean *visible) { return ::fovray_c(m_naif.get(), inst, raydir, rframe, abcorr, obsrvr, et, visible); }
void NaifContext::fovtrg_c(ConstSpiceChar *inst, ConstSpiceChar *target, ConstSpiceChar *tshape, ConstSpiceChar *tframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble *et, SpiceBoolean *visible) { return ::fovtrg_c(m_naif.get(), inst, target, tshape, tframe, abcorr, obsrvr, et, visible); }
void NaifContext::frame_c(SpiceDouble x[3], SpiceDouble y[3], SpiceDouble z[3]) { return ::frame_c(m_naif.get(), x, y, z); }
void NaifContext::frinfo_c(SpiceInt frcode, SpiceInt *cent, SpiceInt *clss, SpiceInt *clssid, SpiceBoolean *found) { return ::frinfo_c(m_naif.get(), frcode, cent, clss, clssid, found); }
void NaifContext::frmnam_c(SpiceInt frcode, SpiceInt lenout, SpiceChar *frname) { return ::frmnam_c(m_naif.get(), frcode, lenout, frname); }
void NaifContext::ftncls_c(SpiceInt unit) { return ::ftncls_c(m_naif.get(), unit); }
void NaifContext::furnsh_c(ConstSpiceChar *file) { return ::furnsh_c(m_naif.get(), file); }
void NaifContext::gcpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt lenout, SpiceInt *n, void *cvals, SpiceBoolean *found) { return ::gcpool_c(m_naif.get(), name, start, room, lenout, n, cvals, found); }
void NaifContext::gdpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceDouble *values, SpiceBoolean *found) { return ::gdpool_c(m_naif.get(), name, start, room, n, values, found); }
void NaifContext::georec_c(SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble rectan[3]) { return ::georec_c(m_naif.get(), lon, lat, alt, re, f, rectan); }
void NaifContext::getcml_c(SpiceInt *argc, SpiceChar ***argv) { return ::getcml_c(m_naif.get(), argc, argv); }
void NaifContext::getelm_c(SpiceInt frstyr, SpiceInt lineln, const void *lines, SpiceDouble *epoch, SpiceDouble *elems) { return ::getelm_c(m_naif.get(), frstyr, lineln, lines, epoch, elems); }
void NaifContext::getfat_c(ConstSpiceChar *file, SpiceInt arclen, SpiceInt typlen, SpiceChar *arch, SpiceChar *type) { return ::getfat_c(m_naif.get(), file, arclen, typlen, arch, type); }
void NaifContext::getfov_c(SpiceInt instid, SpiceInt room, SpiceInt shapelen, SpiceInt framelen, SpiceChar *shape, SpiceChar *frame, SpiceDouble bsight[3], SpiceInt *n, SpiceDouble bounds[][3]) { return ::getfov_c(m_naif.get(), instid, room, shapelen, framelen, shape, frame, bsight, n, bounds); }
void NaifContext::getmsg_c(ConstSpiceChar *option, SpiceInt lenout, SpiceChar *msg) { return ::getmsg_c(m_naif.get(), option, lenout, msg); }
SpiceBoolean NaifContext::gfbail_c() { return ::gfbail_c(m_naif.get()); }
void NaifContext::gfclrh_c() { return ::gfclrh_c(m_naif.get()); }
void NaifContext::gfdist_c(ConstSpiceChar *target, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfdist_c(m_naif.get(), target, abcorr, obsrvr, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfilum_c(ConstSpiceChar *method, ConstSpiceChar *angtyp, ConstSpiceChar *target, ConstSpiceChar *illum, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfilum_c(m_naif.get(), method, angtyp, target, illum, fixref, abcorr, obsrvr, spoint, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfinth_c(int sigcode) { return ::gfinth_c(m_naif.get(), sigcode); }
void NaifContext::gfoclt_c(ConstSpiceChar *occtyp, ConstSpiceChar *front, ConstSpiceChar *fshape, ConstSpiceChar *fframe, ConstSpiceChar *back, ConstSpiceChar *bshape, ConstSpiceChar *bframe, ConstSpiceChar *obsrvr, ConstSpiceChar *abcorr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result) { return ::gfoclt_c(m_naif.get(), occtyp, front, fshape, fframe, back, bshape, bframe, obsrvr, abcorr, step, cnfine, result); }
void NaifContext::gfpa_c(ConstSpiceChar *target, ConstSpiceChar *illum, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfpa_c(m_naif.get(), target, illum, abcorr, obsrvr, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfposc_c(ConstSpiceChar *target, ConstSpiceChar *frame, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfposc_c(m_naif.get(), target, frame, abcorr, obsrvr, crdsys, coord, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfrefn_c(SpiceDouble t1, SpiceDouble t2, SpiceBoolean s1, SpiceBoolean s2, SpiceDouble *t) { return ::gfrefn_c(m_naif.get(), t1, t2, s1, s2, t); }
void NaifContext::gfrepf_c() { return ::gfrepf_c(m_naif.get()); }
void NaifContext::gfrepi_c(SpiceCell *window, ConstSpiceChar *begmss, ConstSpiceChar *endmss) { return ::gfrepi_c(m_naif.get(), window, begmss, endmss); }
void NaifContext::gfrepu_c(SpiceDouble ivbeg, SpiceDouble ivend, SpiceDouble time) { return ::gfrepu_c(m_naif.get(), ivbeg, ivend, time); }
void NaifContext::gfrfov_c(ConstSpiceChar *inst, ConstSpiceDouble raydir[3], ConstSpiceChar *rframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result) { return ::gfrfov_c(m_naif.get(), inst, raydir, rframe, abcorr, obsrvr, step, cnfine, result); }
void NaifContext::gfrr_c(ConstSpiceChar *target, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfrr_c(m_naif.get(), target, abcorr, obsrvr, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfsep_c(ConstSpiceChar *targ1, ConstSpiceChar *shape1, ConstSpiceChar *frame1, ConstSpiceChar *targ2, ConstSpiceChar *shape2, ConstSpiceChar *frame2, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfsep_c(m_naif.get(), targ1, shape1, frame1, targ2, shape2, frame2, abcorr, obsrvr, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfsntc_c(ConstSpiceChar *target, ConstSpiceChar *fixref, ConstSpiceChar *method, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfsntc_c(m_naif.get(), target, fixref, method, abcorr, obsrvr, dref, dvec, crdsys, coord, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gfsstp_c(SpiceDouble step) { return ::gfsstp_c(m_naif.get(), step); }
void NaifContext::gfstep_c(SpiceDouble time, SpiceDouble *step) { return ::gfstep_c(m_naif.get(), time, step); }
void NaifContext::gfstol_c(SpiceDouble value) { return ::gfstol_c(m_naif.get(), value); }
void NaifContext::gfsubc_c(ConstSpiceChar *target, ConstSpiceChar *fixref, ConstSpiceChar *method, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *crdsys, ConstSpiceChar *coord, ConstSpiceChar *relate, SpiceDouble refval, SpiceDouble adjust, SpiceDouble step, SpiceInt nintvls, SpiceCell *cnfine, SpiceCell *result) { return ::gfsubc_c(m_naif.get(), target, fixref, method, abcorr, obsrvr, crdsys, coord, relate, refval, adjust, step, nintvls, cnfine, result); }
void NaifContext::gftfov_c(ConstSpiceChar *inst, ConstSpiceChar *target, ConstSpiceChar *tshape, ConstSpiceChar *tframe, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble step, SpiceCell *cnfine, SpiceCell *result) { return ::gftfov_c(m_naif.get(), inst, target, tshape, tframe, abcorr, obsrvr, step, cnfine, result); }
void NaifContext::gipool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt *n, SpiceInt *ivals, SpiceBoolean *found) { return ::gipool_c(m_naif.get(), name, start, room, n, ivals, found); }
void NaifContext::gnpool_c(ConstSpiceChar *name, SpiceInt start, SpiceInt room, SpiceInt lenout, SpiceInt *n, void *kvars, SpiceBoolean *found) { return ::gnpool_c(m_naif.get(), name, start, room, lenout, n, kvars, found); }
void NaifContext::hrmint_c(SpiceInt n, ConstSpiceDouble *xvals, ConstSpiceDouble *yvals, SpiceDouble x, SpiceDouble *work, SpiceDouble *f, SpiceDouble *df) { return ::hrmint_c(m_naif.get(), n, xvals, yvals, x, work, f, df); }
SpiceDouble NaifContext::halfpi_c() { return ::halfpi_c(m_naif.get()); }
void NaifContext::hx2dp_c(ConstSpiceChar *string, SpiceInt lenout, SpiceDouble *number, SpiceBoolean *error, SpiceChar *errmsg) { return ::hx2dp_c(m_naif.get(), string, lenout, number, error, errmsg); }
void NaifContext::ident_c(SpiceDouble matrix[3][3]) { return ::ident_c(m_naif.get(), matrix); }
void NaifContext::ilumin_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn) { return ::ilumin_c(m_naif.get(), method, target, et, fixref, abcorr, obsrvr, spoint, trgepc, srfvec, phase, solar, emissn); }
void NaifContext::illum_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn) { return ::illum_c(m_naif.get(), target, et, abcorr, obsrvr, spoint, phase, solar, emissn); }
void NaifContext::illum_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn) { return ::illum_pl02(m_naif.get(), handle, dladsc, target, et, abcorr, obsrvr, spoint, phase, solar, emissn); }
void NaifContext::illum_plid_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceInt plid, SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn, SpiceBoolean *visible, SpiceBoolean *lit) { return ::illum_plid_pl02(m_naif.get(), handle, dladsc, target, et, abcorr, obsrvr, spoint, plid, trgepc, srfvec, phase, solar, emissn, visible, lit); }
void NaifContext::illumf_c(ConstSpiceChar *method, ConstSpiceChar *target, ConstSpiceChar *ilusrc, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *incdnc, SpiceDouble *emissn, SpiceBoolean *visibl, SpiceBoolean *lit) { return ::illumf_c(m_naif.get(), method, target, ilusrc, et, fixref, abcorr, obsrvr, spoint, trgepc, srfvec, phase, incdnc, emissn, visibl, lit); }
void NaifContext::illumg_c(ConstSpiceChar *method, ConstSpiceChar *target, ConstSpiceChar *illum, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceDouble *phase, SpiceDouble *solar, SpiceDouble *emissn) { return ::illumg_c(m_naif.get(), method, target, illum, et, fixref, abcorr, obsrvr, spoint, trgepc, srfvec, phase, solar, emissn); }
void NaifContext::inedpl_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpicePlane *plane, SpiceEllipse *ellipse, SpiceBoolean *found) { return ::inedpl_c(m_naif.get(), a, b, c, plane, ellipse, found); }
void NaifContext::inelpl_c(ConstSpiceEllipse *ellips, ConstSpicePlane *plane, SpiceInt *nxpts, SpiceDouble xpt1[3], SpiceDouble xpt2[3]) { return ::inelpl_c(m_naif.get(), ellips, plane, nxpts, xpt1, xpt2); }
void NaifContext::insrtc_c(ConstSpiceChar *item, SpiceCell *set) { return ::insrtc_c(m_naif.get(), item, set); }
void NaifContext::insrtd_c(SpiceDouble item, SpiceCell *set) { return ::insrtd_c(m_naif.get(), item, set); }
void NaifContext::insrti_c(SpiceInt item, SpiceCell *set) { return ::insrti_c(m_naif.get(), item, set); }
void NaifContext::inter_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::inter_c(m_naif.get(), a, b, c); }
void NaifContext::inrypl_c(ConstSpiceDouble vertex[3], ConstSpiceDouble dir[3], ConstSpicePlane *plane, SpiceInt *nxpts, SpiceDouble xpt[3]) { return ::inrypl_c(m_naif.get(), vertex, dir, plane, nxpts, xpt); }
SpiceInt NaifContext::intmax_c() { return ::intmax_c(m_naif.get()); }
SpiceInt NaifContext::intmax_() { return ::intmax_(m_naif.get()); }
SpiceInt NaifContext::intmin_c() { return ::intmin_c(m_naif.get()); }
SpiceInt NaifContext::intmin_() { return ::intmin_(m_naif.get()); }
void NaifContext::invert_c(ConstSpiceDouble m1[3][3], SpiceDouble m2[3][3]) { return ::invert_c(m_naif.get(), m1, m2); }
void NaifContext::invort_c(ConstSpiceDouble m[3][3], SpiceDouble mit[3][3]) { return ::invort_c(m_naif.get(), m, mit); }
SpiceBoolean NaifContext::isordv_c(ConstSpiceInt *array, SpiceInt n) { return ::isordv_c(m_naif.get(), array, n); }
SpiceBoolean NaifContext::isrot_c(ConstSpiceDouble m[3][3], SpiceDouble ntol, SpiceDouble dtol) { return ::isrot_c(m_naif.get(), m, ntol, dtol); }
SpiceInt NaifContext::isrchc_c(ConstSpiceChar *value, SpiceInt ndim, SpiceInt lenvals, const void *array) { return ::isrchc_c(m_naif.get(), value, ndim, lenvals, array); }
SpiceInt NaifContext::isrchd_c(SpiceDouble value, SpiceInt ndim, ConstSpiceDouble *array) { return ::isrchd_c(m_naif.get(), value, ndim, array); }
SpiceInt NaifContext::isrchi_c(SpiceInt value, SpiceInt ndim, ConstSpiceInt *array) { return ::isrchi_c(m_naif.get(), value, ndim, array); }
SpiceBoolean NaifContext::iswhsp_c(ConstSpiceChar *string) { return ::iswhsp_c(m_naif.get(), string); }
SpiceDouble NaifContext::j1900_c() { return ::j1900_c(m_naif.get()); }
SpiceDouble NaifContext::j1950_c() { return ::j1950_c(m_naif.get()); }
SpiceDouble NaifContext::j2000_c() { return ::j2000_c(m_naif.get()); }
SpiceDouble NaifContext::j2100_c() { return ::j2100_c(m_naif.get()); }
SpiceDouble NaifContext::jyear_c() { return ::jyear_c(m_naif.get()); }
void NaifContext::kclear_c() { return ::kclear_c(m_naif.get()); }
void NaifContext::kdata_c(SpiceInt which, ConstSpiceChar *kind, SpiceInt fillen, SpiceInt typlen, SpiceInt srclen, SpiceChar *file, SpiceChar *filtyp, SpiceChar *source, SpiceInt *handle, SpiceBoolean *found) { return ::kdata_c(m_naif.get(), which, kind, fillen, typlen, srclen, file, filtyp, source, handle, found); }
void NaifContext::kinfo_c(ConstSpiceChar *file, SpiceInt typlen, SpiceInt srclen, SpiceChar *filtyp, SpiceChar *source, SpiceInt *handle, SpiceBoolean *found) { return ::kinfo_c(m_naif.get(), file, typlen, srclen, filtyp, source, handle, found); }
void NaifContext::kplfrm_c(SpiceInt frmcls, SpiceCell *idset) { return ::kplfrm_c(m_naif.get(), frmcls, idset); }
void NaifContext::ktotal_c(ConstSpiceChar *kind, SpiceInt *count) { return ::ktotal_c(m_naif.get(), kind, count); }
void NaifContext::kxtrct_c(ConstSpiceChar *keywd, SpiceInt termlen, const void *terms, SpiceInt nterms, SpiceInt stringlen, SpiceInt substrlen, SpiceChar *string, SpiceBoolean *found, SpiceChar *substr) { return ::kxtrct_c(m_naif.get(), keywd, termlen, terms, nterms, stringlen, substrlen, string, found, substr); }
SpiceInt NaifContext::lastnb_c(ConstSpiceChar *string) { return ::lastnb_c(m_naif.get(), string); }
void NaifContext::latcyl_c(SpiceDouble radius, SpiceDouble lon, SpiceDouble lat, SpiceDouble *r, SpiceDouble *lonc, SpiceDouble *z) { return ::latcyl_c(m_naif.get(), radius, lon, lat, r, lonc, z); }
void NaifContext::latrec_c(SpiceDouble radius, SpiceDouble longitude, SpiceDouble latitude, SpiceDouble rectan[3]) { return ::latrec_c(m_naif.get(), radius, longitude, latitude, rectan); }
void NaifContext::latsph_c(SpiceDouble radius, SpiceDouble lon, SpiceDouble lat, SpiceDouble *rho, SpiceDouble *colat, SpiceDouble *lons) { return ::latsph_c(m_naif.get(), radius, lon, lat, rho, colat, lons); }
void NaifContext::latsrf_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, SpiceInt npts, ConstSpiceDouble lonlat[][2], SpiceDouble srfpts[][3]) { return ::latsrf_c(m_naif.get(), method, target, et, fixref, npts, lonlat, srfpts); }
void NaifContext::lcase_c(SpiceChar *in, SpiceInt lenout, SpiceChar *out) { return ::lcase_c(m_naif.get(), in, lenout, out); }
void NaifContext::ldpool_c(ConstSpiceChar *filename) { return ::ldpool_c(m_naif.get(), filename); }
void NaifContext::lgrind_c(SpiceInt n, ConstSpiceDouble *xvals, ConstSpiceDouble *yvals, SpiceDouble *work, SpiceDouble x, SpiceDouble *p, SpiceDouble *dp) { return ::lgrind_c(m_naif.get(), n, xvals, yvals, work, x, p, dp); }
void NaifContext::limb_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npoints, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble limbpts[][3], SpiceInt plateIDs[]) { return ::limb_pl02(m_naif.get(), handle, dladsc, target, et, fixref, abcorr, obsrvr, npoints, trgepc, obspos, limbpts, plateIDs); }
void NaifContext::limbpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *corloc, ConstSpiceChar *obsrvr, ConstSpiceDouble refvec[3], SpiceDouble rolstp, SpiceInt ncuts, SpiceDouble schstp, SpiceDouble soltol, SpiceInt maxn, SpiceInt npts[], SpiceDouble points[][3], SpiceDouble epochs[], SpiceDouble tangts[][3]) { return ::limbpt_c(m_naif.get(), method, target, et, fixref, abcorr, corloc, obsrvr, refvec, rolstp, ncuts, schstp, soltol, maxn, npts, points, epochs, tangts); }
void NaifContext::llgrid_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, SpiceInt npoints, ConstSpiceDouble grid[][2], SpiceDouble spoints[][3], SpiceInt plateIDs[]) { return ::llgrid_pl02(m_naif.get(), handle, dladsc, npoints, grid, spoints, plateIDs); }
void NaifContext::lmpool_c(const void *cvals, SpiceInt lenvals, SpiceInt n) { return ::lmpool_c(m_naif.get(), cvals, lenvals, n); }
void NaifContext::lparse_c(ConstSpiceChar *list, ConstSpiceChar *delim, SpiceInt nmax, SpiceInt lenout, SpiceInt *n, void *items) { return ::lparse_c(m_naif.get(), list, delim, nmax, lenout, n, items); }
void NaifContext::lparsm_c(ConstSpiceChar *list, ConstSpiceChar *delims, SpiceInt nmax, SpiceInt lenout, SpiceInt *n, void *items) { return ::lparsm_c(m_naif.get(), list, delims, nmax, lenout, n, items); }
void NaifContext::lparss_c(ConstSpiceChar *list, ConstSpiceChar *delims, SpiceCell *set) { return ::lparss_c(m_naif.get(), list, delims, set); }
SpiceDouble NaifContext::lspcn_c(ConstSpiceChar *body, SpiceDouble et, ConstSpiceChar *abcorr) { return ::lspcn_c(m_naif.get(), body, et, abcorr); }
SpiceInt NaifContext::lstlec_c(ConstSpiceChar *string, SpiceInt n, SpiceInt lenvals, const void *array) { return ::lstlec_c(m_naif.get(), string, n, lenvals, array); }
SpiceInt NaifContext::lstled_c(SpiceDouble x, SpiceInt n, ConstSpiceDouble *array) { return ::lstled_c(m_naif.get(), x, n, array); }
SpiceInt NaifContext::lstlei_c(SpiceInt x, SpiceInt n, ConstSpiceInt *array) { return ::lstlei_c(m_naif.get(), x, n, array); }
SpiceInt NaifContext::lstltc_c(ConstSpiceChar *string, SpiceInt n, SpiceInt lenvals, const void *array) { return ::lstltc_c(m_naif.get(), string, n, lenvals, array); }
SpiceInt NaifContext::lstltd_c(SpiceDouble x, SpiceInt n, ConstSpiceDouble *array) { return ::lstltd_c(m_naif.get(), x, n, array); }
SpiceInt NaifContext::lstlti_c(SpiceInt x, SpiceInt n, ConstSpiceInt *array) { return ::lstlti_c(m_naif.get(), x, n, array); }
void NaifContext::ltime_c(SpiceDouble etobs, SpiceInt obs, ConstSpiceChar *dir, SpiceInt targ, SpiceDouble *ettarg, SpiceDouble *elapsd) { return ::ltime_c(m_naif.get(), etobs, obs, dir, targ, ettarg, elapsd); }
void NaifContext::lx4dec_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar) { return ::lx4dec_c(m_naif.get(), string, first, last, nchar); }
void NaifContext::lx4num_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar) { return ::lx4num_c(m_naif.get(), string, first, last, nchar); }
void NaifContext::lx4sgn_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar) { return ::lx4sgn_c(m_naif.get(), string, first, last, nchar); }
void NaifContext::lx4uns_c(ConstSpiceChar *string, SpiceInt first, SpiceInt *last, SpiceInt *nchar) { return ::lx4uns_c(m_naif.get(), string, first, last, nchar); }
void NaifContext::lxqstr_c(ConstSpiceChar *string, SpiceChar qchar, SpiceInt first, SpiceInt *last, SpiceInt *nchar) { return ::lxqstr_c(m_naif.get(), string, qchar, first, last, nchar); }
void NaifContext::m2eul_c(ConstSpiceDouble r[3][3], SpiceInt axis3, SpiceInt axis2, SpiceInt axis1, SpiceDouble *angle3, SpiceDouble *angle2, SpiceDouble *angle1) { return ::m2eul_c(m_naif.get(), r, axis3, axis2, axis1, angle3, angle2, angle1); }
void NaifContext::m2q_c(ConstSpiceDouble r[3][3], SpiceDouble q[4]) { return ::m2q_c(m_naif.get(), r, q); }
SpiceBoolean NaifContext::matchi_c(ConstSpiceChar *string, ConstSpiceChar *templ, SpiceChar wstr, SpiceChar wchr) { return ::matchi_c(m_naif.get(), string, templ, wstr, wchr); }
SpiceBoolean NaifContext::matchw_c(ConstSpiceChar *string, ConstSpiceChar *templ, SpiceChar wstr, SpiceChar wchr) { return ::matchw_c(m_naif.get(), string, templ, wstr, wchr); }
void NaifContext::mequ_c(ConstSpiceDouble m1[3][3], SpiceDouble mout[3][3]) { return ::mequ_c(m_naif.get(), m1, mout); }
void NaifContext::mequg_c(const void *m1, SpiceInt nr, SpiceInt nc, void *mout) { return ::mequg_c(m_naif.get(), m1, nr, nc, mout); }
int NaifContext::moved_(SpiceDouble *arrfrm, SpiceInt *ndim, SpiceDouble *arrto) { return ::moved_(m_naif.get(), arrfrm, ndim, arrto); }
void NaifContext::mtxm_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]) { return ::mtxm_c(m_naif.get(), m1, m2, mout); }
void NaifContext::mtxmg_c(const void *m1, const void *m2, SpiceInt row1, SpiceInt col1, SpiceInt col2, void *mout) { return ::mtxmg_c(m_naif.get(), m1, m2, row1, col1, col2, mout); }
void NaifContext::mtxv_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble vin[3], SpiceDouble vout[3]) { return ::mtxv_c(m_naif.get(), m1, vin, vout); }
void NaifContext::mtxvg_c(const void *m1, const void *v2, SpiceInt ncol1, SpiceInt nr1r2, void *vout) { return ::mtxvg_c(m_naif.get(), m1, v2, ncol1, nr1r2, vout); }
void NaifContext::mxm_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]) { return ::mxm_c(m_naif.get(), m1, m2, mout); }
void NaifContext::mxmg_c(const void *m1, const void *m2, SpiceInt row1, SpiceInt col1, SpiceInt col2, void *mout) { return ::mxmg_c(m_naif.get(), m1, m2, row1, col1, col2, mout); }
void NaifContext::mxmt_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble m2[3][3], SpiceDouble mout[3][3]) { return ::mxmt_c(m_naif.get(), m1, m2, mout); }
void NaifContext::mxmtg_c(const void *m1, const void *m2, SpiceInt nrow1, SpiceInt nc1c2, SpiceInt nrow2, void *mout) { return ::mxmtg_c(m_naif.get(), m1, m2, nrow1, nc1c2, nrow2, mout); }
void NaifContext::mxv_c(ConstSpiceDouble m1[3][3], ConstSpiceDouble vin[3], SpiceDouble vout[3]) { return ::mxv_c(m_naif.get(), m1, vin, vout); }
void NaifContext::mxvg_c(const void *m1, const void *v2, SpiceInt nrow1, SpiceInt nc1r2, void *vout) { return ::mxvg_c(m_naif.get(), m1, v2, nrow1, nc1r2, vout); }
void NaifContext::namfrm_c(ConstSpiceChar *frname, SpiceInt *frcode) { return ::namfrm_c(m_naif.get(), frname, frcode); }
SpiceInt NaifContext::ncpos_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start) { return ::ncpos_c(m_naif.get(), str, chars, start); }
SpiceInt NaifContext::ncposr_c(ConstSpiceChar *str, ConstSpiceChar *chars, SpiceInt start) { return ::ncposr_c(m_naif.get(), str, chars, start); }
void NaifContext::nearpt_c(ConstSpiceDouble positn[3], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble npoint[3], SpiceDouble *alt) { return ::nearpt_c(m_naif.get(), positn, a, b, c, npoint, alt); }
void NaifContext::npedln_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble linept[3], ConstSpiceDouble linedr[3], SpiceDouble pnear[3], SpiceDouble *dist) { return ::npedln_c(m_naif.get(), a, b, c, linept, linedr, pnear, dist); }
void NaifContext::npelpt_c(ConstSpiceDouble point[3], ConstSpiceEllipse *ellips, SpiceDouble pnear[3], SpiceDouble *dist) { return ::npelpt_c(m_naif.get(), point, ellips, pnear, dist); }
void NaifContext::nplnpt_c(ConstSpiceDouble linpt[3], ConstSpiceDouble lindir[3], ConstSpiceDouble point[3], SpiceDouble pnear[3], SpiceDouble *dist) { return ::nplnpt_c(m_naif.get(), linpt, lindir, point, pnear, dist); }
void NaifContext::nvc2pl_c(ConstSpiceDouble normal[3], SpiceDouble constant, SpicePlane *plane) { return ::nvc2pl_c(m_naif.get(), normal, constant, plane); }
void NaifContext::nvp2pl_c(ConstSpiceDouble normal[3], ConstSpiceDouble point[3], SpicePlane *plane) { return ::nvp2pl_c(m_naif.get(), normal, point, plane); }
void NaifContext::occult_c(ConstSpiceChar *target1, ConstSpiceChar *shape1, ConstSpiceChar *frame1, ConstSpiceChar *target2, ConstSpiceChar *shape2, ConstSpiceChar *frame2, ConstSpiceChar *abcorr, ConstSpiceChar *observer, SpiceDouble time, SpiceInt *occult_code) { return ::occult_c(m_naif.get(), target1, shape1, frame1, target2, shape2, frame2, abcorr, observer, time, occult_code); }
SpiceInt NaifContext::ordc_c(ConstSpiceChar *item, SpiceCell *set) { return ::ordc_c(m_naif.get(), item, set); }
SpiceInt NaifContext::ordd_c(SpiceDouble item, SpiceCell *set) { return ::ordd_c(m_naif.get(), item, set); }
SpiceInt NaifContext::ordi_c(SpiceInt item, SpiceCell *set) { return ::ordi_c(m_naif.get(), item, set); }
void NaifContext::orderc_c(SpiceInt lenvals, const void *array, SpiceInt ndim, SpiceInt *iorder) { return ::orderc_c(m_naif.get(), lenvals, array, ndim, iorder); }
void NaifContext::orderd_c(ConstSpiceDouble *array, SpiceInt ndim, SpiceInt *iorder) { return ::orderd_c(m_naif.get(), array, ndim, iorder); }
void NaifContext::orderi_c(ConstSpiceInt *array, SpiceInt ndim, SpiceInt *iorder) { return ::orderi_c(m_naif.get(), array, ndim, iorder); }
void NaifContext::oscelt_c(ConstSpiceDouble state[6], SpiceDouble et, SpiceDouble mu, SpiceDouble elts[8]) { return ::oscelt_c(m_naif.get(), state, et, mu, elts); }
void NaifContext::oscltx_c(ConstSpiceDouble state[6], SpiceDouble et, SpiceDouble mu, SpiceDouble elts[20]) { return ::oscltx_c(m_naif.get(), state, et, mu, elts); }
void NaifContext::pckcls_c(SpiceInt handle) { return ::pckcls_c(m_naif.get(), handle); }
void NaifContext::pckcov_c(ConstSpiceChar *pck, SpiceInt idcode, SpiceCell *cover) { return ::pckcov_c(m_naif.get(), pck, idcode, cover); }
void NaifContext::pckfrm_c(ConstSpiceChar *pck, SpiceCell *ids) { return ::pckfrm_c(m_naif.get(), pck, ids); }
void NaifContext::pcklof_c(ConstSpiceChar *fname, SpiceInt *handle) { return ::pcklof_c(m_naif.get(), fname, handle); }
void NaifContext::pckopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle) { return ::pckopn_c(m_naif.get(), name, ifname, ncomch, handle); }
void NaifContext::pckuof_c(SpiceInt handle) { return ::pckuof_c(m_naif.get(), handle); }
void NaifContext::pckw02_c(SpiceInt handle, SpiceInt clssid, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, SpiceDouble cdata[], SpiceDouble btime) { return ::pckw02_c(m_naif.get(), handle, clssid, frame, first, last, segid, intlen, n, polydg, cdata, btime); }
void NaifContext::pcpool_c(ConstSpiceChar *name, SpiceInt n, SpiceInt lenvals, const void *cvals) { return ::pcpool_c(m_naif.get(), name, n, lenvals, cvals); }
void NaifContext::pdpool_c(ConstSpiceChar *name, SpiceInt n, ConstSpiceDouble *dvals) { return ::pdpool_c(m_naif.get(), name, n, dvals); }
void NaifContext::pgrrec_c(ConstSpiceChar *body, SpiceDouble lon, SpiceDouble lat, SpiceDouble alt, SpiceDouble re, SpiceDouble f, SpiceDouble rectan[3]) { return ::pgrrec_c(m_naif.get(), body, lon, lat, alt, re, f, rectan); }
SpiceDouble NaifContext::phaseq_c(SpiceDouble et, ConstSpiceChar *target, ConstSpiceChar *illumn, ConstSpiceChar *obsrvr, ConstSpiceChar *abcorr) { return ::phaseq_c(m_naif.get(), et, target, illumn, obsrvr, abcorr); }
SpiceDouble NaifContext::pi_c() { return ::pi_c(m_naif.get()); }
void NaifContext::pipool_c(ConstSpiceChar *name, SpiceInt n, ConstSpiceInt *ivals) { return ::pipool_c(m_naif.get(), name, n, ivals); }
void NaifContext::pjelpl_c(ConstSpiceEllipse *elin, ConstSpicePlane *plane, SpiceEllipse *elout) { return ::pjelpl_c(m_naif.get(), elin, plane, elout); }
void NaifContext::pl2nvc_c(ConstSpicePlane *plane, SpiceDouble normal[3], SpiceDouble *constant) { return ::pl2nvc_c(m_naif.get(), plane, normal, constant); }
void NaifContext::pl2nvp_c(ConstSpicePlane *plane, SpiceDouble normal[3], SpiceDouble point[3]) { return ::pl2nvp_c(m_naif.get(), plane, normal, point); }
void NaifContext::pl2psv_c(ConstSpicePlane *plane, SpiceDouble point[3], SpiceDouble span1[3], SpiceDouble span2[3]) { return ::pl2psv_c(m_naif.get(), plane, point, span1, span2); }
SpiceDouble NaifContext::pltar_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3]) { return ::pltar_c(m_naif.get(), nv, vrtces, np, plates); }
void NaifContext::pltexp_c(ConstSpiceDouble iverts[3][3], SpiceDouble delta, SpiceDouble overts[3][3]) { return ::pltexp_c(m_naif.get(), iverts, delta, overts); }
void NaifContext::pltnp_c(ConstSpiceDouble point[3], ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], ConstSpiceDouble v3[3], SpiceDouble pnear[3], SpiceDouble *dist) { return ::pltnp_c(m_naif.get(), point, v1, v2, v3, pnear, dist); }
void NaifContext::pltnrm_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], ConstSpiceDouble v3[3], SpiceDouble normal[3]) { return ::pltnrm_c(m_naif.get(), v1, v2, v3, normal); }
SpiceDouble NaifContext::pltvol_c(SpiceInt nv, ConstSpiceDouble vrtces[][3], SpiceInt np, ConstSpiceInt plates[][3]) { return ::pltvol_c(m_naif.get(), nv, vrtces, np, plates); }
void NaifContext::polyds_c(ConstSpiceDouble *coeffs, SpiceInt deg, SpiceInt nderiv, SpiceDouble t, SpiceDouble *p) { return ::polyds_c(m_naif.get(), coeffs, deg, nderiv, t, p); }
SpiceInt NaifContext::pos_c(ConstSpiceChar *str, ConstSpiceChar *substr, SpiceInt start) { return ::pos_c(m_naif.get(), str, substr, start); }
SpiceInt NaifContext::posr_c(ConstSpiceChar *str, ConstSpiceChar *substr, SpiceInt start) { return ::posr_c(m_naif.get(), str, substr, start); }
SpiceChar *NaifContext::prompt_c(ConstSpiceChar *prmptStr, SpiceInt lenout, SpiceChar *buffer) { return ::prompt_c(m_naif.get(), prmptStr, lenout, buffer); }
void NaifContext::prop2b_c(SpiceDouble gm, ConstSpiceDouble pvinit[6], SpiceDouble dt, SpiceDouble pvprop[6]) { return ::prop2b_c(m_naif.get(), gm, pvinit, dt, pvprop); }
void NaifContext::prsdp_c(ConstSpiceChar *string, SpiceDouble *dpval) { return ::prsdp_c(m_naif.get(), string, dpval); }
void NaifContext::prsint_c(ConstSpiceChar *string, SpiceInt *intval) { return ::prsint_c(m_naif.get(), string, intval); }
void NaifContext::psv2pl_c(ConstSpiceDouble point[3], ConstSpiceDouble span1[3], ConstSpiceDouble span2[3], SpicePlane *plane) { return ::psv2pl_c(m_naif.get(), point, span1, span2, plane); }
void NaifContext::putcml_c(SpiceInt argc, SpiceChar **argv) { return ::putcml_c(m_naif.get(), argc, argv); }
void NaifContext::pxform_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble et, SpiceDouble rotate[3][3]) { return ::pxform_c(m_naif.get(), from, to, et, rotate); }
void NaifContext::pxfrm2_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble etfrom, SpiceDouble etto, SpiceDouble rotate[3][3]) { return ::pxfrm2_c(m_naif.get(), from, to, etfrom, etto, rotate); }
void NaifContext::q2m_c(ConstSpiceDouble q[4], SpiceDouble r[3][3]) { return ::q2m_c(m_naif.get(), q, r); }
void NaifContext::qcktrc_c(SpiceInt tracelen, SpiceChar *trace) { return ::qcktrc_c(m_naif.get(), tracelen, trace); }
void NaifContext::qdq2av_c(ConstSpiceDouble q[4], ConstSpiceDouble dq[4], SpiceDouble av[3]) { return ::qdq2av_c(m_naif.get(), q, dq, av); }
void NaifContext::qxq_c(ConstSpiceDouble q1[4], ConstSpiceDouble q2[4], SpiceDouble qout[4]) { return ::qxq_c(m_naif.get(), q1, q2, qout); }
void NaifContext::radrec_c(SpiceDouble range, SpiceDouble ra, SpiceDouble dec, SpiceDouble rectan[3]) { return ::radrec_c(m_naif.get(), range, ra, dec, rectan); }
void NaifContext::rav2xf_c(ConstSpiceDouble rot[3][3], ConstSpiceDouble av[3], SpiceDouble xform[6][6]) { return ::rav2xf_c(m_naif.get(), rot, av, xform); }
void NaifContext::raxisa_c(ConstSpiceDouble matrix[3][3], SpiceDouble axis[3], SpiceDouble *angle) { return ::raxisa_c(m_naif.get(), matrix, axis, angle); }
void NaifContext::rdtext_c(ConstSpiceChar *file, SpiceInt lenout, SpiceChar *line, SpiceBoolean *eof) { return ::rdtext_c(m_naif.get(), file, lenout, line, eof); }
void NaifContext::reccyl_c(ConstSpiceDouble rectan[3], SpiceDouble *r, SpiceDouble *lon, SpiceDouble *z) { return ::reccyl_c(m_naif.get(), rectan, r, lon, z); }
void NaifContext::recgeo_c(ConstSpiceDouble rectan[3], SpiceDouble re, SpiceDouble f, SpiceDouble *lon, SpiceDouble *lat, SpiceDouble *alt) { return ::recgeo_c(m_naif.get(), rectan, re, f, lon, lat, alt); }
void NaifContext::reclat_c(ConstSpiceDouble rectan[3], SpiceDouble *radius, SpiceDouble *longitude, SpiceDouble *latitude) { return ::reclat_c(m_naif.get(), rectan, radius, longitude, latitude); }
void NaifContext::recpgr_c(ConstSpiceChar *body, SpiceDouble rectan[3], SpiceDouble re, SpiceDouble f, SpiceDouble *lon, SpiceDouble *lat, SpiceDouble *alt) { return ::recpgr_c(m_naif.get(), body, rectan, re, f, lon, lat, alt); }
void NaifContext::recrad_c(ConstSpiceDouble rectan[3], SpiceDouble *radius, SpiceDouble *ra, SpiceDouble *dec) { return ::recrad_c(m_naif.get(), rectan, radius, ra, dec); }
void NaifContext::reordc_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceInt lenvals, void *array) { return ::reordc_c(m_naif.get(), iorder, ndim, lenvals, array); }
void NaifContext::reordd_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceDouble *array) { return ::reordd_c(m_naif.get(), iorder, ndim, array); }
void NaifContext::reordi_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceInt *array) { return ::reordi_c(m_naif.get(), iorder, ndim, array); }
void NaifContext::reordl_c(ConstSpiceInt *iorder, SpiceInt ndim, SpiceBoolean *array) { return ::reordl_c(m_naif.get(), iorder, ndim, array); }
void NaifContext::removc_c(ConstSpiceChar *item, SpiceCell *set) { return ::removc_c(m_naif.get(), item, set); }
void NaifContext::removd_c(SpiceDouble item, SpiceCell *set) { return ::removd_c(m_naif.get(), item, set); }
void NaifContext::removi_c(SpiceInt item, SpiceCell *set) { return ::removi_c(m_naif.get(), item, set); }
void NaifContext::repmc_c(ConstSpiceChar *in, ConstSpiceChar *marker, ConstSpiceChar *value, SpiceInt lenout, SpiceChar *out) { return ::repmc_c(m_naif.get(), in, marker, value, lenout, out); }
void NaifContext::repmct_c(ConstSpiceChar *in, ConstSpiceChar *marker, SpiceInt value, SpiceChar strCase, SpiceInt lenout, SpiceChar *out) { return ::repmct_c(m_naif.get(), in, marker, value, strCase, lenout, out); }
void NaifContext::repmd_c(ConstSpiceChar *in, ConstSpiceChar *marker, SpiceDouble value, SpiceInt sigdig, SpiceInt lenout, SpiceChar *out) { return ::repmd_c(m_naif.get(), in, marker, value, sigdig, lenout, out); }
void NaifContext::repmf_c(ConstSpiceChar *in, ConstSpiceChar *marker, SpiceDouble value, SpiceInt sigdig, SpiceChar format, SpiceInt lenout, SpiceChar *out) { return ::repmf_c(m_naif.get(), in, marker, value, sigdig, format, lenout, out); }
void NaifContext::repmi_c(ConstSpiceChar *in, ConstSpiceChar *marker, SpiceInt value, SpiceInt lenout, SpiceChar *out) { return ::repmi_c(m_naif.get(), in, marker, value, lenout, out); }
void NaifContext::repmot_c(ConstSpiceChar *in, ConstSpiceChar *marker, SpiceInt value, SpiceChar strCase, SpiceInt lenout, SpiceChar *out) { return ::repmot_c(m_naif.get(), in, marker, value, strCase, lenout, out); }
void NaifContext::reset_c() { return ::reset_c(m_naif.get()); }
SpiceBoolean NaifContext::return_c() { return ::return_c(m_naif.get()); }
void NaifContext::recsph_c(ConstSpiceDouble rectan[3], SpiceDouble *r, SpiceDouble *colat, SpiceDouble *lon) { return ::recsph_c(m_naif.get(), rectan, r, colat, lon); }
void NaifContext::rotate_c(SpiceDouble angle, SpiceInt iaxis, SpiceDouble mout[3][3]) { return ::rotate_c(m_naif.get(), angle, iaxis, mout); }
void NaifContext::rotmat_c(ConstSpiceDouble m1[3][3], SpiceDouble angle, SpiceInt iaxis, SpiceDouble mout[3][3]) { return ::rotmat_c(m_naif.get(), m1, angle, iaxis, mout); }
void NaifContext::rotvec_c(ConstSpiceDouble v1[3], SpiceDouble angle, SpiceInt iaxis, SpiceDouble vout[3]) { return ::rotvec_c(m_naif.get(), v1, angle, iaxis, vout); }
SpiceDouble NaifContext::rpd_c() { return ::rpd_c(m_naif.get()); }
void NaifContext::rquad_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble root1[2], SpiceDouble root2[2]) { return ::rquad_c(m_naif.get(), a, b, c, root1, root2); }
void NaifContext::saelgv_c(ConstSpiceDouble vec1[3], ConstSpiceDouble vec2[3], SpiceDouble smajor[3], SpiceDouble sminor[3]) { return ::saelgv_c(m_naif.get(), vec1, vec2, smajor, sminor); }
void NaifContext::scard_c(SpiceInt card, SpiceCell *cell) { return ::scard_c(m_naif.get(), card, cell); }
void NaifContext::scdecd_c(SpiceInt sc, SpiceDouble sclkdp, SpiceInt sclklen, SpiceChar *sclkch) { return ::scdecd_c(m_naif.get(), sc, sclkdp, sclklen, sclkch); }
void NaifContext::sce2s_c(SpiceInt sc, SpiceDouble et, SpiceInt sclklen, SpiceChar *sclkch) { return ::sce2s_c(m_naif.get(), sc, et, sclklen, sclkch); }
void NaifContext::sce2c_c(SpiceInt sc, SpiceDouble et, SpiceDouble *sclkdp) { return ::sce2c_c(m_naif.get(), sc, et, sclkdp); }
void NaifContext::sce2t_c(SpiceInt sc, SpiceDouble et, SpiceDouble *sclkdp) { return ::sce2t_c(m_naif.get(), sc, et, sclkdp); }
void NaifContext::scencd_c(SpiceInt sc, ConstSpiceChar *sclkch, SpiceDouble *sclkdp) { return ::scencd_c(m_naif.get(), sc, sclkch, sclkdp); }
void NaifContext::scfmt_c(SpiceInt sc, SpiceDouble ticks, SpiceInt clkstrlen, SpiceChar *clkstr) { return ::scfmt_c(m_naif.get(), sc, ticks, clkstrlen, clkstr); }
void NaifContext::scpart_c(SpiceInt sc, SpiceInt *nparts, SpiceDouble *pstart, SpiceDouble *pstop) { return ::scpart_c(m_naif.get(), sc, nparts, pstart, pstop); }
void NaifContext::scs2e_c(SpiceInt sc, ConstSpiceChar *sclkch, SpiceDouble *et) { return ::scs2e_c(m_naif.get(), sc, sclkch, et); }
void NaifContext::sct2e_c(SpiceInt sc, SpiceDouble sclkdp, SpiceDouble *et) { return ::sct2e_c(m_naif.get(), sc, sclkdp, et); }
void NaifContext::sctiks_c(SpiceInt sc, ConstSpiceChar *clkstr, SpiceDouble *ticks) { return ::sctiks_c(m_naif.get(), sc, clkstr, ticks); }
void NaifContext::sdiff_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::sdiff_c(m_naif.get(), a, b, c); }
SpiceBoolean NaifContext::set_c(SpiceCell *a, ConstSpiceChar *op, SpiceCell *b) { return ::set_c(m_naif.get(), a, op, b); }
void NaifContext::setmsg_c(ConstSpiceChar *msg) { return ::setmsg_c(m_naif.get(), msg); }
void NaifContext::shellc_c(SpiceInt ndim, SpiceInt lenvals, void *array) { return ::shellc_c(m_naif.get(), ndim, lenvals, array); }
void NaifContext::shelld_c(SpiceInt ndim, SpiceDouble *array) { return ::shelld_c(m_naif.get(), ndim, array); }
void NaifContext::shelli_c(SpiceInt ndim, SpiceInt *array) { return ::shelli_c(m_naif.get(), ndim, array); }
void NaifContext::sigerr_c(ConstSpiceChar *message) { return ::sigerr_c(m_naif.get(), message); }
void NaifContext::sincpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3], SpiceBoolean *found) { return ::sincpt_c(m_naif.get(), method, target, et, fixref, abcorr, obsrvr, dref, dvec, spoint, trgepc, srfvec, found); }
SpiceInt NaifContext::size_c(SpiceCell *size) { return ::size_c(m_naif.get(), size); }
SpiceDouble NaifContext::spd_c() { return ::spd_c(m_naif.get()); }
void NaifContext::sphcyl_c(SpiceDouble radius, SpiceDouble colat, SpiceDouble slon, SpiceDouble *r, SpiceDouble *lon, SpiceDouble *z) { return ::sphcyl_c(m_naif.get(), radius, colat, slon, r, lon, z); }
void NaifContext::sphlat_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lons, SpiceDouble *radius, SpiceDouble *lon, SpiceDouble *lat) { return ::sphlat_c(m_naif.get(), r, colat, lons, radius, lon, lat); }
void NaifContext::sphrec_c(SpiceDouble r, SpiceDouble colat, SpiceDouble lon, SpiceDouble rectan[3]) { return ::sphrec_c(m_naif.get(), r, colat, lon, rectan); }
void NaifContext::spk14a_c(SpiceInt handle, SpiceInt ncsets, ConstSpiceDouble coeffs[], ConstSpiceDouble epochs[]) { return ::spk14a_c(m_naif.get(), handle, ncsets, coeffs, epochs); }
void NaifContext::spk14b_c(SpiceInt handle, ConstSpiceChar *segid, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, SpiceInt chbdeg) { return ::spk14b_c(m_naif.get(), handle, segid, body, center, frame, first, last, chbdeg); }
void NaifContext::spk14e_c(SpiceInt handle) { return ::spk14e_c(m_naif.get(), handle); }
void NaifContext::spkapo_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceDouble sobs[6], ConstSpiceChar *abcorr, SpiceDouble ptarg[3], SpiceDouble *lt) { return ::spkapo_c(m_naif.get(), targ, et, ref, sobs, abcorr, ptarg, lt); }
void NaifContext::spkapp_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceDouble sobs[6], ConstSpiceChar *abcorr, SpiceDouble starg[6], SpiceDouble *lt) { return ::spkapp_c(m_naif.get(), targ, et, ref, sobs, abcorr, starg, lt); }
void NaifContext::spkacs_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, SpiceInt obs, SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt) { return ::spkacs_c(m_naif.get(), targ, et, ref, abcorr, obs, starg, lt, dlt); }
void NaifContext::spkaps_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceDouble stobs[6], ConstSpiceDouble accobs[6], SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt) { return ::spkaps_c(m_naif.get(), targ, et, ref, abcorr, stobs, accobs, starg, lt, dlt); }
void NaifContext::spkcls_c(SpiceInt handle) { return ::spkcls_c(m_naif.get(), handle); }
void NaifContext::spkcov_c(ConstSpiceChar *spk, SpiceInt idcode, SpiceCell *cover) { return ::spkcov_c(m_naif.get(), spk, idcode, cover); }
void NaifContext::spkcpo_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceDouble obssta[3], ConstSpiceChar *obsctr, ConstSpiceChar *obsref, SpiceDouble state[6], SpiceDouble *lt) { return ::spkcpo_c(m_naif.get(), target, et, outref, refloc, abcorr, obssta, obsctr, obsref, state, lt); }
void NaifContext::spkcpt_c(ConstSpiceDouble trgpos[3], ConstSpiceChar *trgctr, ConstSpiceChar *trgref, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble state[6], SpiceDouble *lt) { return ::spkcpt_c(m_naif.get(), trgpos, trgctr, trgref, et, outref, refloc, abcorr, obsrvr, state, lt); }
void NaifContext::spkcvo_c(ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceDouble obssta[6], SpiceDouble obsepc, ConstSpiceChar *obsctr, ConstSpiceChar *obsref, SpiceDouble state[6], SpiceDouble *lt) { return ::spkcvo_c(m_naif.get(), target, et, outref, refloc, abcorr, obssta, obsepc, obsctr, obsref, state, lt); }
void NaifContext::spkcvt_c(ConstSpiceDouble trgsta[6], SpiceDouble trgepc, ConstSpiceChar *trgctr, ConstSpiceChar *trgref, SpiceDouble et, ConstSpiceChar *outref, ConstSpiceChar *refloc, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble state[6], SpiceDouble *lt) { return ::spkcvt_c(m_naif.get(), trgsta, trgepc, trgctr, trgref, et, outref, refloc, abcorr, obsrvr, state, lt); }
void NaifContext::spkez_c(SpiceInt target, SpiceDouble epoch, ConstSpiceChar *frame, ConstSpiceChar *abcorr, SpiceInt observer, SpiceDouble state[6], SpiceDouble *lt) { return ::spkez_c(m_naif.get(), target, epoch, frame, abcorr, observer, state, lt); }
void NaifContext::spkezp_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, SpiceInt obs, SpiceDouble ptarg[3], SpiceDouble *lt) { return ::spkezp_c(m_naif.get(), targ, et, ref, abcorr, obs, ptarg, lt); }
void NaifContext::spkezr_c(ConstSpiceChar *target, SpiceDouble epoch, ConstSpiceChar *frame, ConstSpiceChar *abcorr, ConstSpiceChar *observer, SpiceDouble state[6], SpiceDouble *lt) { return ::spkezr_c(m_naif.get(), target, epoch, frame, abcorr, observer, state, lt); }
void NaifContext::spkgeo_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceInt obs, SpiceDouble state[6], SpiceDouble *lt) { return ::spkgeo_c(m_naif.get(), targ, et, ref, obs, state, lt); }
void NaifContext::spkgps_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceInt obs, SpiceDouble pos[3], SpiceDouble *lt) { return ::spkgps_c(m_naif.get(), targ, et, ref, obs, pos, lt); }
void NaifContext::spklef_c(ConstSpiceChar *filename, SpiceInt *handle) { return ::spklef_c(m_naif.get(), filename, handle); }
void NaifContext::spkltc_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceDouble stobs[6], SpiceDouble starg[6], SpiceDouble *lt, SpiceDouble *dlt) { return ::spkltc_c(m_naif.get(), targ, et, ref, abcorr, stobs, starg, lt, dlt); }
void NaifContext::spkobj_c(ConstSpiceChar *spk, SpiceCell *ids) { return ::spkobj_c(m_naif.get(), spk, ids); }
void NaifContext::spkopa_c(ConstSpiceChar *file, SpiceInt *handle) { return ::spkopa_c(m_naif.get(), file, handle); }
void NaifContext::spkopn_c(ConstSpiceChar *name, ConstSpiceChar *ifname, SpiceInt ncomch, SpiceInt *handle) { return ::spkopn_c(m_naif.get(), name, ifname, ncomch, handle); }
void NaifContext::spkpds_c(SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceInt type, SpiceDouble first, SpiceDouble last, SpiceDouble descr[5]) { return ::spkpds_c(m_naif.get(), body, center, frame, type, first, last, descr); }
void NaifContext::spkpos_c(ConstSpiceChar *targ, SpiceDouble et, ConstSpiceChar *ref, ConstSpiceChar *abcorr, ConstSpiceChar *obs, SpiceDouble ptarg[3], SpiceDouble *lt) { return ::spkpos_c(m_naif.get(), targ, et, ref, abcorr, obs, ptarg, lt); }
void NaifContext::spkpvn_c(SpiceInt handle, ConstSpiceDouble descr[5], SpiceDouble et, SpiceInt *ref, SpiceDouble state[6], SpiceInt *center) { return ::spkpvn_c(m_naif.get(), handle, descr, et, ref, state, center); }
void NaifContext::spksfs_c(SpiceInt body, SpiceDouble et, SpiceInt idlen, SpiceInt *handle, SpiceDouble descr[5], SpiceChar *ident, SpiceBoolean *found) { return ::spksfs_c(m_naif.get(), body, et, idlen, handle, descr, ident, found); }
void NaifContext::spkssb_c(SpiceInt targ, SpiceDouble et, ConstSpiceChar *ref, SpiceDouble starg[6]) { return ::spkssb_c(m_naif.get(), targ, et, ref, starg); }
void NaifContext::spksub_c(SpiceInt handle, SpiceDouble descr[5], ConstSpiceChar *ident, SpiceDouble begin, SpiceDouble end, SpiceInt newh) { return ::spksub_c(m_naif.get(), handle, descr, ident, begin, end, newh); }
void NaifContext::spkuds_c(ConstSpiceDouble descr[5], SpiceInt *body, SpiceInt *center, SpiceInt *frame, SpiceInt *type, SpiceDouble *first, SpiceDouble *last, SpiceInt *begin, SpiceInt *end) { return ::spkuds_c(m_naif.get(), descr, body, center, frame, type, first, last, begin, end); }
void NaifContext::spkuef_c(SpiceInt handle) { return ::spkuef_c(m_naif.get(), handle); }
void NaifContext::spkw02_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble btime) { return ::spkw02_c(m_naif.get(), handle, body, center, frame, first, last, segid, intlen, n, polydg, cdata, btime); }
void NaifContext::spkw03_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble btime) { return ::spkw03_c(m_naif.get(), handle, body, center, frame, first, last, segid, intlen, n, polydg, cdata, btime); }
void NaifContext::spkw05_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble gm, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]) { return ::spkw05_c(m_naif.get(), handle, body, center, frame, first, last, segid, gm, n, states, epochs); }
void NaifContext::spkw08_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], SpiceDouble epoch1, SpiceDouble step) { return ::spkw08_c(m_naif.get(), handle, body, center, frame, first, last, segid, degree, n, states, epoch1, step); }
void NaifContext::spkw09_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]) { return ::spkw09_c(m_naif.get(), handle, body, center, frame, first, last, segid, degree, n, states, epochs); }
void NaifContext::spkw10_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, ConstSpiceDouble consts[8], SpiceInt n, ConstSpiceDouble elems[], ConstSpiceDouble epochs[]) { return ::spkw10_c(m_naif.get(), handle, body, center, frame, first, last, segid, consts, n, elems, epochs); }
void NaifContext::spkw12_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], SpiceDouble epoch0, SpiceDouble step) { return ::spkw12_c(m_naif.get(), handle, body, center, frame, first, last, segid, degree, n, states, epoch0, step); }
void NaifContext::spkw13_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, ConstSpiceDouble states[][6], ConstSpiceDouble epochs[]) { return ::spkw13_c(m_naif.get(), handle, body, center, frame, first, last, segid, degree, n, states, epochs); }
void NaifContext::spkw15_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble epoch, ConstSpiceDouble tp[3], ConstSpiceDouble pa[3], SpiceDouble p, SpiceDouble ecc, SpiceDouble j2flg, ConstSpiceDouble pv[3], SpiceDouble gm, SpiceDouble j2, SpiceDouble radius) { return ::spkw15_c(m_naif.get(), handle, body, center, frame, first, last, segid, epoch, tp, pa, p, ecc, j2flg, pv, gm, j2, radius); }
void NaifContext::spkw17_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble epoch, ConstSpiceDouble eqel[9], SpiceDouble rapol, SpiceDouble decpol) { return ::spkw17_c(m_naif.get(), handle, body, center, frame, first, last, segid, epoch, eqel, rapol, decpol); }
void NaifContext::spkw18_c(SpiceInt handle, SpiceSPK18Subtype subtyp, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceInt degree, SpiceInt n, const void *packts, ConstSpiceDouble epochs[]) { return ::spkw18_c(m_naif.get(), handle, subtyp, body, center, frame, first, last, segid, degree, n, packts, epochs); }
void NaifContext::spkw20_c(SpiceInt handle, SpiceInt body, SpiceInt center, ConstSpiceChar *frame, SpiceDouble first, SpiceDouble last, ConstSpiceChar *segid, SpiceDouble intlen, SpiceInt n, SpiceInt polydg, ConstSpiceDouble cdata[], SpiceDouble dscale, SpiceDouble tscale, SpiceDouble initjd, SpiceDouble initfr) { return ::spkw20_c(m_naif.get(), handle, body, center, frame, first, last, segid, intlen, n, polydg, cdata, dscale, tscale, initjd, initfr); }
void NaifContext::srfrec_c(SpiceInt body, SpiceDouble lon, SpiceDouble lat, SpiceDouble rectan[3]) { return ::srfrec_c(m_naif.get(), body, lon, lat, rectan); }
void NaifContext::srfc2s_c(SpiceInt code, SpiceInt bodyid, SpiceInt srflen, SpiceChar *srfstr, SpiceBoolean *isname) { return ::srfc2s_c(m_naif.get(), code, bodyid, srflen, srfstr, isname); }
void NaifContext::srfcss_c(SpiceInt code, ConstSpiceChar *bodstr, SpiceInt srflen, SpiceChar *srfstr, SpiceBoolean *isname) { return ::srfcss_c(m_naif.get(), code, bodstr, srflen, srfstr, isname); }
void NaifContext::srfnrm_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, SpiceInt npts, ConstSpiceDouble srfpts[][3], SpiceDouble normls[][3]) { return ::srfnrm_c(m_naif.get(), method, target, et, fixref, npts, srfpts, normls); }
void NaifContext::srfs2c_c(ConstSpiceChar *srfstr, ConstSpiceChar *bodstr, SpiceInt *code, SpiceBoolean *found) { return ::srfs2c_c(m_naif.get(), srfstr, bodstr, code, found); }
void NaifContext::srfscc_c(ConstSpiceChar *surfce, SpiceInt bodyid, SpiceInt *surfid, SpiceBoolean *found) { return ::srfscc_c(m_naif.get(), surfce, bodyid, surfid, found); }
void NaifContext::srfxpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, ConstSpiceChar *dref, ConstSpiceDouble dvec[3], SpiceDouble spoint[3], SpiceDouble *dist, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceBoolean *found) { return ::srfxpt_c(m_naif.get(), method, target, et, abcorr, obsrvr, dref, dvec, spoint, dist, trgepc, obspos, found); }
void NaifContext::ssize_c(SpiceInt size, SpiceCell *cell) { return ::ssize_c(m_naif.get(), size, cell); }
void NaifContext::stelab_c(ConstSpiceDouble pobj[3], ConstSpiceDouble vobs[3], SpiceDouble appobj[3]) { return ::stelab_c(m_naif.get(), pobj, vobs, appobj); }
void NaifContext::stpool_c(ConstSpiceChar *item, SpiceInt nth, ConstSpiceChar *contin, SpiceInt lenout, SpiceChar *string, SpiceInt *size, SpiceBoolean *found) { return ::stpool_c(m_naif.get(), item, nth, contin, lenout, string, size, found); }
void NaifContext::str2et_c(ConstSpiceChar *date, SpiceDouble *et) { return ::str2et_c(m_naif.get(), date, et); }
void NaifContext::subpnt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3]) { return ::subpnt_c(m_naif.get(), method, target, et, fixref, abcorr, obsrvr, spoint, trgepc, srfvec); }
void NaifContext::subpt_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *alt) { return ::subpt_c(m_naif.get(), method, target, et, abcorr, obsrvr, spoint, alt); }
void NaifContext::subpt_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *alt, SpiceInt *plateID) { return ::subpt_pl02(m_naif.get(), handle, dladsc, method, target, et, abcorr, obsrvr, spoint, alt, plateID); }
void NaifContext::subslr_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *trgepc, SpiceDouble srfvec[3]) { return ::subslr_c(m_naif.get(), method, target, et, fixref, abcorr, obsrvr, spoint, trgepc, srfvec); }
void NaifContext::subsol_c(ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3]) { return ::subsol_c(m_naif.get(), method, target, et, abcorr, obsrvr, spoint); }
void NaifContext::subsol_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *method, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceDouble spoint[3], SpiceDouble *dist, SpiceInt *plateID) { return ::subsol_pl02(m_naif.get(), handle, dladsc, method, target, et, abcorr, obsrvr, spoint, dist, plateID); }
SpiceDouble NaifContext::sumad_c(ConstSpiceDouble array[], SpiceInt n) { return ::sumad_c(m_naif.get(), array, n); }
SpiceInt NaifContext::sumai_c(ConstSpiceInt array[], SpiceInt n) { return ::sumai_c(m_naif.get(), array, n); }
void NaifContext::surfnm_c(SpiceDouble a, SpiceDouble b, SpiceDouble c, ConstSpiceDouble point[3], SpiceDouble normal[3]) { return ::surfnm_c(m_naif.get(), a, b, c, point, normal); }
void NaifContext::surfpt_c(ConstSpiceDouble positn[3], ConstSpiceDouble u[3], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble point[3], SpiceBoolean *found) { return ::surfpt_c(m_naif.get(), positn, u, a, b, c, point, found); }
void NaifContext::surfpv_c(ConstSpiceDouble stvrtx[6], ConstSpiceDouble stdir[6], SpiceDouble a, SpiceDouble b, SpiceDouble c, SpiceDouble stx[6], SpiceBoolean *found) { return ::surfpv_c(m_naif.get(), stvrtx, stdir, a, b, c, stx, found); }
void NaifContext::swpool_c(ConstSpiceChar *agent, SpiceInt nnames, SpiceInt lenvals, const void *names) { return ::swpool_c(m_naif.get(), agent, nnames, lenvals, names); }
void NaifContext::sxform_c(ConstSpiceChar *from, ConstSpiceChar *to, SpiceDouble et, SpiceDouble xform[6][6]) { return ::sxform_c(m_naif.get(), from, to, et, xform); }
void NaifContext::szpool_c(ConstSpiceChar *name, SpiceInt *n, SpiceBoolean *found) { return ::szpool_c(m_naif.get(), name, n, found); }
void NaifContext::term_pl02(SpiceInt handle, ConstSpiceDLADescr *dladsc, ConstSpiceChar *trmtyp, ConstSpiceChar *source, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *obsrvr, SpiceInt npoints, SpiceDouble *trgepc, SpiceDouble obspos[3], SpiceDouble trmpts[][3], SpiceInt plateIDs[]) { return ::term_pl02(m_naif.get(), handle, dladsc, trmtyp, source, target, et, fixref, abcorr, obsrvr, npoints, trgepc, obspos, trmpts, plateIDs); }
void NaifContext::termpt_c(ConstSpiceChar *method, ConstSpiceChar *ilusrc, ConstSpiceChar *target, SpiceDouble et, ConstSpiceChar *fixref, ConstSpiceChar *abcorr, ConstSpiceChar *corloc, ConstSpiceChar *obsrvr, ConstSpiceDouble refvec[3], SpiceDouble rolstp, SpiceInt ncuts, SpiceDouble schstp, SpiceDouble soltol, SpiceInt maxn, SpiceInt npts[], SpiceDouble points[][3], SpiceDouble epochs[], SpiceDouble tangts[][3]) { return ::termpt_c(m_naif.get(), method, ilusrc, target, et, fixref, abcorr, corloc, obsrvr, refvec, rolstp, ncuts, schstp, soltol, maxn, npts, points, epochs, tangts); }
void NaifContext::timdef_c(ConstSpiceChar *action, ConstSpiceChar *item, SpiceInt lenout, SpiceChar *value) { return ::timdef_c(m_naif.get(), action, item, lenout, value); }
void NaifContext::timout_c(SpiceDouble et, ConstSpiceChar *pictur, SpiceInt lenout, SpiceChar *output) { return ::timout_c(m_naif.get(), et, pictur, lenout, output); }
void NaifContext::tipbod_c(ConstSpiceChar *ref, SpiceInt body, SpiceDouble et, SpiceDouble tipm[3][3]) { return ::tipbod_c(m_naif.get(), ref, body, et, tipm); }
void NaifContext::tisbod_c(ConstSpiceChar *ref, SpiceInt body, SpiceDouble et, SpiceDouble tsipm[6][6]) { return ::tisbod_c(m_naif.get(), ref, body, et, tsipm); }
ConstSpiceChar *NaifContext::tkvrsn_c(ConstSpiceChar *item) { return ::tkvrsn_c(m_naif.get(), item); }
void NaifContext::tparse_c(ConstSpiceChar *string, SpiceInt lenout, SpiceDouble *sp2000, SpiceChar *errmsg) { return ::tparse_c(m_naif.get(), string, lenout, sp2000, errmsg); }
void NaifContext::tpictr_c(ConstSpiceChar *sample, SpiceInt lenpictur, SpiceInt lenerror, SpiceChar *pictur, SpiceBoolean *ok, SpiceChar *error) { return ::tpictr_c(m_naif.get(), sample, lenpictur, lenerror, pictur, ok, error); }
SpiceDouble NaifContext::trace_c(ConstSpiceDouble matrix[3][3]) { return ::trace_c(m_naif.get(), matrix); }
void NaifContext::trcdep_c(SpiceInt *depth) { return ::trcdep_c(m_naif.get(), depth); }
void NaifContext::trcnam_c(SpiceInt index, SpiceInt namelen, SpiceChar *name) { return ::trcnam_c(m_naif.get(), index, namelen, name); }
void NaifContext::trcoff_c() { return ::trcoff_c(m_naif.get()); }
void NaifContext::tsetyr_c(SpiceInt year) { return ::tsetyr_c(m_naif.get(), year); }
SpiceDouble NaifContext::twopi_c() { return ::twopi_c(m_naif.get()); }
void NaifContext::twovec_c(ConstSpiceDouble axdef[3], SpiceInt indexa, ConstSpiceDouble plndef[3], SpiceInt indexp, SpiceDouble mout[3][3]) { return ::twovec_c(m_naif.get(), axdef, indexa, plndef, indexp, mout); }
SpiceDouble NaifContext::tyear_c() { return ::tyear_c(m_naif.get()); }
void NaifContext::ucase_c(SpiceChar *in, SpiceInt lenout, SpiceChar *out) { return ::ucase_c(m_naif.get(), in, lenout, out); }
void NaifContext::ucrss_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]) { return ::ucrss_c(m_naif.get(), v1, v2, vout); }
void NaifContext::udf_c(SpiceDouble x, SpiceDouble *value) { return ::udf_c(m_naif.get(), x, value); }
void NaifContext::union_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::union_c(m_naif.get(), a, b, c); }
SpiceDouble NaifContext::unitim_c(SpiceDouble epoch, ConstSpiceChar *insys, ConstSpiceChar *outsys) { return ::unitim_c(m_naif.get(), epoch, insys, outsys); }
void NaifContext::unload_c(ConstSpiceChar *file) { return ::unload_c(m_naif.get(), file); }
void NaifContext::unorm_c(ConstSpiceDouble v1[3], SpiceDouble vout[3], SpiceDouble *vmag) { return ::unorm_c(m_naif.get(), v1, vout, vmag); }
void NaifContext::unormg_c(ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout, SpiceDouble *vmag) { return ::unormg_c(m_naif.get(), v1, ndim, vout, vmag); }
void NaifContext::utc2et_c(ConstSpiceChar *utcstr, SpiceDouble *et) { return ::utc2et_c(m_naif.get(), utcstr, et); }
void NaifContext::vadd_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]) { return ::vadd_c(m_naif.get(), v1, v2, vout); }
void NaifContext::vaddg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim, SpiceDouble *vout) { return ::vaddg_c(m_naif.get(), v1, v2, ndim, vout); }
void NaifContext::valid_c(SpiceInt size, SpiceInt n, SpiceCell *a) { return ::valid_c(m_naif.get(), size, n, a); }
void NaifContext::vcrss_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]) { return ::vcrss_c(m_naif.get(), v1, v2, vout); }
SpiceDouble NaifContext::vdist_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]) { return ::vdist_c(m_naif.get(), v1, v2); }
SpiceDouble NaifContext::vdistg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim) { return ::vdistg_c(m_naif.get(), v1, v2, ndim); }
SpiceDouble NaifContext::vdot_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]) { return ::vdot_c(m_naif.get(), v1, v2); }
SpiceDouble NaifContext::vdotg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim) { return ::vdotg_c(m_naif.get(), v1, v2, ndim); }
void NaifContext::vequ_c(ConstSpiceDouble vin[3], SpiceDouble vout[3]) { return ::vequ_c(m_naif.get(), vin, vout); }
void NaifContext::vequg_c(ConstSpiceDouble *vin, SpiceInt ndim, SpiceDouble *vout) { return ::vequg_c(m_naif.get(), vin, ndim, vout); }
void NaifContext::vhat_c(ConstSpiceDouble v1[3], SpiceDouble vout[3]) { return ::vhat_c(m_naif.get(), v1, vout); }
void NaifContext::vhatg_c(ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout) { return ::vhatg_c(m_naif.get(), v1, ndim, vout); }
void NaifContext::vlcom_c(SpiceDouble a, ConstSpiceDouble v1[3], SpiceDouble b, ConstSpiceDouble v2[3], SpiceDouble sum[3]) { return ::vlcom_c(m_naif.get(), a, v1, b, v2, sum); }
void NaifContext::vlcom3_c(SpiceDouble a, ConstSpiceDouble v1[3], SpiceDouble b, ConstSpiceDouble v2[3], SpiceDouble c, ConstSpiceDouble v3[3], SpiceDouble sum[3]) { return ::vlcom3_c(m_naif.get(), a, v1, b, v2, c, v3, sum); }
void NaifContext::vlcomg_c(SpiceInt n, SpiceDouble a, ConstSpiceDouble *v1, SpiceDouble b, ConstSpiceDouble *v2, SpiceDouble *sum) { return ::vlcomg_c(m_naif.get(), n, a, v1, b, v2, sum); }
void NaifContext::vminug_c(ConstSpiceDouble *vin, SpiceInt ndim, SpiceDouble *vout) { return ::vminug_c(m_naif.get(), vin, ndim, vout); }
void NaifContext::vminus_c(ConstSpiceDouble v1[3], SpiceDouble vout[3]) { return ::vminus_c(m_naif.get(), v1, vout); }
SpiceDouble NaifContext::vnorm_c(ConstSpiceDouble v1[3]) { return ::vnorm_c(m_naif.get(), v1); }
SpiceDouble NaifContext::vnormg_c(ConstSpiceDouble *v1, SpiceInt ndim) { return ::vnormg_c(m_naif.get(), v1, ndim); }
void NaifContext::vpack_c(SpiceDouble x, SpiceDouble y, SpiceDouble z, SpiceDouble v[3]) { return ::vpack_c(m_naif.get(), x, y, z, v); }
void NaifContext::vperp_c(ConstSpiceDouble a[3], ConstSpiceDouble b[3], SpiceDouble p[3]) { return ::vperp_c(m_naif.get(), a, b, p); }
void NaifContext::vprjp_c(ConstSpiceDouble vin[3], ConstSpicePlane *plane, SpiceDouble vout[3]) { return ::vprjp_c(m_naif.get(), vin, plane, vout); }
void NaifContext::vprjpi_c(ConstSpiceDouble vin[3], ConstSpicePlane *projpl, ConstSpicePlane *invpl, SpiceDouble vout[3], SpiceBoolean *found) { return ::vprjpi_c(m_naif.get(), vin, projpl, invpl, vout, found); }
void NaifContext::vproj_c(ConstSpiceDouble a[3], ConstSpiceDouble b[3], SpiceDouble p[3]) { return ::vproj_c(m_naif.get(), a, b, p); }
SpiceDouble NaifContext::vrel_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]) { return ::vrel_c(m_naif.get(), v1, v2); }
SpiceDouble NaifContext::vrelg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim) { return ::vrelg_c(m_naif.get(), v1, v2, ndim); }
void NaifContext::vrotv_c(ConstSpiceDouble v[3], ConstSpiceDouble axis[3], SpiceDouble theta, SpiceDouble r[3]) { return ::vrotv_c(m_naif.get(), v, axis, theta, r); }
void NaifContext::vscl_c(SpiceDouble s, ConstSpiceDouble v1[3], SpiceDouble vout[3]) { return ::vscl_c(m_naif.get(), s, v1, vout); }
void NaifContext::vsclg_c(SpiceDouble s, ConstSpiceDouble *v1, SpiceInt ndim, SpiceDouble *vout) { return ::vsclg_c(m_naif.get(), s, v1, ndim, vout); }
SpiceDouble NaifContext::vsep_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3]) { return ::vsep_c(m_naif.get(), v1, v2); }
void NaifContext::vsub_c(ConstSpiceDouble v1[3], ConstSpiceDouble v2[3], SpiceDouble vout[3]) { return ::vsub_c(m_naif.get(), v1, v2, vout); }
void NaifContext::vsubg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim, SpiceDouble *vout) { return ::vsubg_c(m_naif.get(), v1, v2, ndim, vout); }
SpiceDouble NaifContext::vsepg_c(ConstSpiceDouble *v1, ConstSpiceDouble *v2, SpiceInt ndim) { return ::vsepg_c(m_naif.get(), v1, v2, ndim); }
SpiceDouble NaifContext::vtmv_c(ConstSpiceDouble v1[3], ConstSpiceDouble matrix[3][3], ConstSpiceDouble v2[3]) { return ::vtmv_c(m_naif.get(), v1, matrix, v2); }
SpiceDouble NaifContext::vtmvg_c(const void *v1, const void *matrix, const void *v2, SpiceInt nrow, SpiceInt ncol) { return ::vtmvg_c(m_naif.get(), v1, matrix, v2, nrow, ncol); }
void NaifContext::vupack_c(ConstSpiceDouble v[3], SpiceDouble *x, SpiceDouble *y, SpiceDouble *z) { return ::vupack_c(m_naif.get(), v, x, y, z); }
SpiceBoolean NaifContext::vzero_c(ConstSpiceDouble v[3]) { return ::vzero_c(m_naif.get(), v); }
SpiceBoolean NaifContext::vzerog_c(ConstSpiceDouble *v, SpiceInt ndim) { return ::vzerog_c(m_naif.get(), v, ndim); }
SpiceInt NaifContext::wncard_c(SpiceCell *window) { return ::wncard_c(m_naif.get(), window); }
void NaifContext::wncomd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window, SpiceCell *result) { return ::wncomd_c(m_naif.get(), left, right, window, result); }
void NaifContext::wncond_c(SpiceDouble left, SpiceDouble right, SpiceCell *window) { return ::wncond_c(m_naif.get(), left, right, window); }
void NaifContext::wndifd_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::wndifd_c(m_naif.get(), a, b, c); }
SpiceBoolean NaifContext::wnelmd_c(SpiceDouble point, SpiceCell *window) { return ::wnelmd_c(m_naif.get(), point, window); }
void NaifContext::wnexpd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window) { return ::wnexpd_c(m_naif.get(), left, right, window); }
void NaifContext::wnextd_c(SpiceChar side, SpiceCell *window) { return ::wnextd_c(m_naif.get(), side, window); }
void NaifContext::wnfetd_c(SpiceCell *window, SpiceInt n, SpiceDouble *left, SpiceDouble *right) { return ::wnfetd_c(m_naif.get(), window, n, left, right); }
void NaifContext::wnfild_c(SpiceDouble sml, SpiceCell *window) { return ::wnfild_c(m_naif.get(), sml, window); }
void NaifContext::wnfltd_c(SpiceDouble sml, SpiceCell *window) { return ::wnfltd_c(m_naif.get(), sml, window); }
SpiceBoolean NaifContext::wnincd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window) { return ::wnincd_c(m_naif.get(), left, right, window); }
void NaifContext::wninsd_c(SpiceDouble left, SpiceDouble right, SpiceCell *window) { return ::wninsd_c(m_naif.get(), left, right, window); }
void NaifContext::wnintd_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::wnintd_c(m_naif.get(), a, b, c); }
SpiceBoolean NaifContext::wnreld_c(SpiceCell *a, ConstSpiceChar *op, SpiceCell *b) { return ::wnreld_c(m_naif.get(), a, op, b); }
void NaifContext::wnsumd_c(SpiceCell *window, SpiceDouble *meas, SpiceDouble *avg, SpiceDouble *stddev, SpiceInt *shortest, SpiceInt *longest) { return ::wnsumd_c(m_naif.get(), window, meas, avg, stddev, shortest, longest); }
void NaifContext::wnunid_c(SpiceCell *a, SpiceCell *b, SpiceCell *c) { return ::wnunid_c(m_naif.get(), a, b, c); }
void NaifContext::wnvald_c(SpiceInt size, SpiceInt n, SpiceCell *window) { return ::wnvald_c(m_naif.get(), size, n, window); }
void NaifContext::xf2eul_c(ConstSpiceDouble xform[6][6], SpiceInt axisa, SpiceInt axisb, SpiceInt axisc, SpiceDouble eulang[6], SpiceBoolean *unique) { return ::xf2eul_c(m_naif.get(), xform, axisa, axisb, axisc, eulang, unique); }
void NaifContext::xf2rav_c(ConstSpiceDouble xform[6][6], SpiceDouble rot[3][3], SpiceDouble av[3]) { return ::xf2rav_c(m_naif.get(), xform, rot, av); }
void NaifContext::xfmsta_c(ConstSpiceDouble input_state[6], ConstSpiceChar *input_coord_sys, ConstSpiceChar *output_coord_sys, ConstSpiceChar *body, SpiceDouble output_state[6]) { return ::xfmsta_c(m_naif.get(), input_state, input_coord_sys, output_coord_sys, body, output_state); }
void NaifContext::xpose_c(ConstSpiceDouble m1[3][3], SpiceDouble mout[3][3]) { return ::xpose_c(m_naif.get(), m1, mout); }
void NaifContext::xpose6_c(ConstSpiceDouble m1[6][6], SpiceDouble mout[6][6]) { return ::xpose6_c(m_naif.get(), m1, mout); }
void NaifContext::xposeg_c(const void *matrix, SpiceInt nrow, SpiceInt ncol, void *xposem) { return ::xposeg_c(m_naif.get(), matrix, nrow, ncol, xposem); }
void NaifContext::zzgetcml_c(SpiceInt *argc, SpiceChar ***argv, SpiceBoolean init) { return ::zzgetcml_c(m_naif.get(), argc, argv, init); }
SpiceBoolean NaifContext::zzgfgeth_c() { return ::zzgfgeth_c(m_naif.get()); }
void NaifContext::zzgfsavh_c(SpiceBoolean status) { return ::zzgfsavh_c(m_naif.get(), status); }
void NaifContext::zzsynccl_c(SpiceTransDir xdir, SpiceCell *cell) { return ::zzsynccl_c(m_naif.get(), xdir, cell); }
}

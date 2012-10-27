;*****************************************************************************
;+
; NAME:
;   QUICK_FILE_TO_PLOT_TRUE_COLOR_IMAGES
; PURPOSE:
;   Procedure to correct, display and save three-color images 
;     fron MDIS "event". 
; CALLING SEQUENCE:
;   quick_file_to_plot_true_color_images, do_pds_pre = do_pds_pre, 
;     do_pds_post = do_pds_post, do_red = do_red, do_blue = do_blue, 
;     scale_set = scale_set, help = hlp 
; INPUTS: NONE. File_search used to retrieve data. 
; OUTPUTS: NONE. Images saved to jpegs and fits files internally. 
; KEYWORDS: NONE. 
; USES: print_help.pro, readfits.pro, swindow.pro, jpegscreen.pro, 
;   writefits.pro. 
; NOTES: NONE. 
; MODIFICATION HISTORY: 
;   M.R. Keller 03 AUG 2012 Created. 
;           MRK 14 SEP 2012 Calculated correction factors applied. 
;           MRK 21 SEP 2012 /do_pds added. 
;           MRK 28 SEP 2012 /do_red, /do_blue, /scale_set and fits output 
;             added, /do_pds replaced with /do_pds_pre and /do_pds_post. 
; Copyright (C) 2012, Johns Hopkins University/Applied Physics Laboratory
; This software may be used, copied, or redistributed as long as it is not
; sold and this copyright notice is reproduced on each copy made. This
; routine is provided as is without any express or implied warranties
; whatsoever. Other limitations apply as described in the file disclaimer.txt.
;-
;*****************************************************************************

pro quick_file_to_plot_true_color_images, do_pds_pre = do_pds_pre, $ 
  do_pds_post = do_pds_post, do_red = do_red, do_blue = do_blue, $
  scale_set = scale_set, help = hlp 

!except = 2 ; Turn on maximum error messaging. 

; Echo header to screen for help, if requested.
if keyword_set(hlp) then begin
  help, calls = selfhelp
  print_help, selfhelp
  return
endif 

; Set RGB indices for three-color imagery. WAC filter outputs 
; for 430, 480, 560, 630, 750, 830, 900 and 1000 are stored (in 
; that order) in the 8-color FITS files, but only 430, 750, and 
; 900 are used for three-color displays (1000 not corrected). 
blueind = 0L 
greenind = 4L 
redind = 6L 

; Use pre-determined scaling parameters. 
have_pds = 'N' 
have_rb = 'N' 
if keyword_set(do_pds_pre) or keyword_set(do_pds_post) then have_pds = 'Y' 
if keyword_set(do_red) or keyword_set(do_blue) then have_rb = 'Y' 
if (have_pds eq 'Y') then begin 
  ; Values approximate (badly) the equivalent of 
  ; the I/F values from Brett following. 
  overminred = 100 ; min(red) 
  overmaxred = 255 ; max(red) 
  overmingreen = 100 ; min(green) 
  overmaxgreen = 255 ; max(green)
  overminblue = 100 ; min(blue) 
  overmaxblue = 255 ; max(blue) 
  overtop = 125
endif 
if (have_rb eq 'Y') then begin 
  overminred = 0.0295111 ; min(red) 
  overmaxred = 0.190991 ; max(red) 
  overmingreen = 0.193207 ; min(green) 
  overmaxgreen =  0.261358; max(green)
  overminblue = 0.00517048 ; min(blue) 
  overmaxblue = 0.0979371 ; max(blue) 
  overtop = 255
endif 
if (have_pds eq 'N') and (have_rb eq 'N') then begin 
  ; Use Brett's pre-determined I/F scaling parameters 
  ; (see E-mail from Chris Hash 20 JUL 2012). 
  overminred = 0.0295111 ; min(red) 
  overmaxred = 0.190991 ; max(red) 
  overmingreen = 0.0193207 ; min(green) 
  overmaxgreen =  0.161358; max(green)
  overminblue = 0.00517048 ; min(blue) 
  overmaxblue = 0.0979371 ; max(blue) 
  overtop = 255
endif 

; Get overall data structure and FITS filename templates for analysis. 
eventmet = 215000000L ; MET of event. 
case 1 of 
  keyword_set(do_pds_pre): begin 
    restore, 'EW0211978981E_8set.sav' 
    ststarttime = EW0211978981E_8set[3L].start_time 
    stmet = EW0211978981E_8set[3L].mess_met_exp
    fits_templ = 'EW0211978981E_8set.fits' 
  end  
  keyword_set(do_pds_post): begin 
    restore, 'EW0217310053E_8set.sav' 
    ststarttime = EW0217310053E_8set[3L].start_time 
    stmet = EW0217310053E_8set[3L].mess_met_exp
    fits_templ = 'EW0217310053E_8set.fits' 
  end  
  keyword_set(do_red): begin 
    restore, 'red_8band_pho.sav' 
    ststarttime = red_8band_pho.isiscube_obj_instrument_grp_starttime 
    stmet = red_8band_pho.isiscube_obj_archive_grp_missionelapsedtime
    fits_templ = 'red.8band.pho.fits' 
  end  
  keyword_set(do_blue): begin 
    restore, 'blue_8band_pho.sav' 
    ststarttime = blue_8band_pho.isiscube_obj_instrument_grp_starttime 
    stmet = blue_8band_pho.isiscube_obj_archive_grp_missionelapsedtime
    fits_templ = 'blue.8band.pho.fits' 
  end  
  else: begin 
    restore, 'eight_color_struct.sav'
    ststarttime = eight_color_struct.isiscube_obj_instrument_grp_starttime 
    stmet = eight_color_struct.isiscube_obj_archive_grp_missionelapsedtime 
    eight_color_struct = 0.0 ; Release memory. 
    fits_templ = 'EW0*I.8band_bp.nopho.fits' 
  end 
endcase ; End case block for setting file and structure names. 

; Set looping indices for whole-set scaling, if requested. 
scaleloop = 0L 
if keyword_set(scale_set) then begin 
  startloop = 0L
  endloop = 1L 
endif else begin 
  startloop = 1L
  endloop = 1L 
endelse 

; Get FITS file names in current directory. 
fits_to_show = file_search(fits_templ, count = numfts) 

; Set minimum "good" data threshold. 
mingood = -360.0 

; Declare strings and arrays needed to read in times 
; and coefficients from event correction table. 
numfilts = 12L 
fhstr = '0000-00-00T00:00:00.000000'
mszero = '00:00.000000'
numfakehrs = 4708L 
fhourstr = strarr(numfakehrs) + fhstr
fakecorrfphr = fltarr(numfilts, numfakehrs)
coefsfile = 'event_table_ratioed_v2.txt' 

; Begin loop over FITS 8-color image files. 
; If requested, loop twice, the first time to get scaling 
; extrema, the second time to apply corrections, display, 
; and save. Otherwise, just correct, display, and save. 
for iloop = startloop, endloop do begin 

  ; Read in correction coefficients. 
  if (iloop eq endloop) then begin 

    openr, lundat, coefsfile, /get_lun 
    for ifh = 0L, numfakehrs - 1L do begin 
      readf, lundat, fhstr, fakecorrfphr0, fakecorrfphr1, fakecorrfphr2, $ 
        fakecorrfphr3, fakecorrfphr4, fakecorrfphr5, fakecorrfphr6, $ 
        fakecorrfphr7, fakecorrfphr8, fakecorrfphr9, fakecorrfphr10, $ 
        fakecorrfphr11, format = "$(A26, 2X, 12(F11.8, 2X))" 
      fhourstr[ifh] = fhstr 
      fakecorrfphr[0L, ifh] = fakecorrfphr0 ; 430 nm. 
      fakecorrfphr[1L, ifh] = fakecorrfphr1 ; 480 nm. 
      fakecorrfphr[2L, ifh] = fakecorrfphr2 ; 560 nm. 
      fakecorrfphr[3L, ifh] = fakecorrfphr3 ; 630 nm. 
      fakecorrfphr[4L, ifh] = fakecorrfphr4 ; 750 nm. 
      fakecorrfphr[5L, ifh] = fakecorrfphr5 ; 830 nm. 
      fakecorrfphr[6L, ifh] = fakecorrfphr6 ; 900 nm. 
      fakecorrfphr[7L, ifh] = fakecorrfphr7 ; 1000 nm. 
      fakecorrfphr[8L, ifh] = fakecorrfphr8 ; Filter 1 (700 nm). 
      fakecorrfphr[9L, ifh] = fakecorrfphr9 ; Filter 2 (clear). 
      fakecorrfphr[10L, ifh] = fakecorrfphr10 ; Filter 8 (950 nm).
      fakecorrfphr[11L, ifh] = fakecorrfphr11 ; Filter 11 (1010 nm). 
    endfor ; End for loop for per-hour correction coefficient 
           ; reads (ifh = 0L, numfakehrs - 1L). 
    close, lundat 
    free_lun, lundat 

  endif ; End if block for reading in corrections (iloop eq endloop).

  ; Begin loop over fits files to process. 
  for ifts = 0L, numfts - 1L do begin 

    ; Get FITS file name only. 
    stripfits = strmid(fits_to_show[ifts], 0L, $ 
      strpos(fits_to_show[ifts], '.fits'))

    ; Read in 8-color MDIS image and create 
    ; output array for corrected imagery. 
    img = readfits(fits_to_show[ifts]) 
    corrimg = img * 0.0 ; Get output array size right and clear values. 

    ; Get array dimensions and red, green, blue planes. 
    numxpix = n_elements(img[*, 0, 0]) 
    numypix = n_elements(img[0, *, 0]) 
    imgb = img[*, *, blueind] ; 430 nm. 
    imgg = img[*, *, redind] ; 750 nm. 
    imgr = img[*, *, greenind] ; 900 nm. 

    ; Get locations of non-fill values. 
    getgoodb = where(imgb gt mingood, numgoodb)
    getgoodg = where(imgg gt mingood, numgoodg)
    getgoodr = where(imgr gt mingood, numgoodr) 

    ; Scale only non-fill values. 
    red = imgr * 0.0 
    green = imgg * 0.0 
    blue = imgb * 0.0 
    red[getgoodr] = imgr[getgoodr] 
    green[getgoodg] = imgg[getgoodg] 
    blue[getgoodb] = imgb[getgoodb] 

    ; If requested, set scaling limits for whole data set. 
    if keyword_set(scale_set) then begin 
      if (iloop eq scaleloop) then begin 
        if (ifts eq 0L) then begin 
          overminred = min(red) 
          overmaxred = max(red) 
          overmingreen = min(green) 
          overmaxgreen = max(green)
          overminblue = min(blue) 
          overmaxblue = max(blue) 
        endif else begin 
          if (min(red) lt overminred) then overminred = min(red) 
          if (max(red) gt overmaxred) then overmaxred = max(red) 
          if (min(green) lt overmingreen) then overmingreen = min(green) 
          if (max(green) gt overmaxgreen) then overmaxgreen = max(green)
          if (min(blue) lt overminblue) then overminblue = min(blue) 
          if (max(blue) gt overmaxblue) then overmaxblue = max(blue) 
        endelse 
      endif ; End if block for scaling run 
            ; through data (iloop eq scaleloop). 
    endif ; End if block to set scaling limits for whole 
          ; data set [keyword_set(scale_set)]. 

    ; Get correction factors, apply, display, and save. 
    if (iloop eq endloop) then begin 

      ; Match MET from filename to MET in header structure 
      ; to get file start time in string format that matches 
      ; format of start times in conversion coefficients file. 
      if keyword_set(do_red) or keyword_set(do_blue) then begin 
        ffmet = stmet ; For red and blue case, MET NOT in filename. 
      endif else begin 
        ffmet = long(strmid(stripfits, 3, 9)) 
      endelse 
      if (ffmet lt eventmet) then evstr = 'Pre-event data: '
      if (ffmet ge eventmet) then evstr = 'Event data: '
      getstmatch = where(abs(ffmet - stmet) eq min(abs(ffmet - stmet)), $ 
        numstmatch) 

      ; If file MET to structure MET match found, search starttimes. 
      if (numstmatch gt 0L) then begin 

        ; Set start time to on the top of the nearest hour. 
        shourtomatch = strmid(ststarttime[getstmatch[0L]], 0L, 14L) + mszero 
        getcrmatch = where(shourtomatch eq fhourstr, numcrmatch) 

        ; If nearest hour found, get correction coefficients. 
        if (numcrmatch gt 0L) then begin 

          ; For three-color images, get correction coefficients. 
          blueconv = fakecorrfphr[blueind, getcrmatch[0L]] 
          greenconv = fakecorrfphr[greenind, getcrmatch[0L]] 
          redconv = fakecorrfphr[redind, getcrmatch[0L]] 

          ; Get correction coefficients for this file. 
          corr0 = fakecorrfphr[0, getcrmatch[0L]] 
          corr1 = fakecorrfphr[1, getcrmatch[0L]] 
          corr2 = fakecorrfphr[2, getcrmatch[0L]] 
          corr3 = fakecorrfphr[3, getcrmatch[0L]] 
          corr4 = fakecorrfphr[4, getcrmatch[0L]] 
          corr5 = fakecorrfphr[5, getcrmatch[0L]] 
          corr6 = fakecorrfphr[6, getcrmatch[0L]]  
          corr7 = fakecorrfphr[7, getcrmatch[0L]] 
          corr8 = fakecorrfphr[8, getcrmatch[0L]]   ; These are
          corr9 = fakecorrfphr[9, getcrmatch[0L]]   ; set for 
          corr10 = fakecorrfphr[10, getcrmatch[0L]] ; completeness 
          corr11 = fakecorrfphr[11, getcrmatch[0L]] ; but not used. 

          ; Correct, and save corrected planes in output array. 
          corrimg[*, *, 0] = img[*, *, 0] * corr0 
          corrimg[*, *, 1] = img[*, *, 1] * corr1 
          corrimg[*, *, 2] = img[*, *, 2] * corr2 
          corrimg[*, *, 3] = img[*, *, 3] * corr3 
          corrimg[*, *, 4] = img[*, *, 4] * corr4 
          corrimg[*, *, 5] = img[*, *, 5] * corr5 
          corrimg[*, *, 6] = img[*, *, 6] * corr6 
          corrimg[*, *, 7] = img[*, *, 7] * corr7 

          ; Write corrected planes to output FITS file. 
          writefits, 'corr_' + fits_to_show[ifts], corrimg 

        endif else begin ; End if block for nearest hour found in 
                         ; correction table (numcrmatch gt 0L).

          blueconv = 1.0 
          greenconv = 1.0 
          redconv = 1.0 

        endelse ; End else block for nearest hour found in 
                ; correction table (numcrmatch gt 0L).

      endif else begin ; End if block for file MET to structure 
                       ; MET match found (numstmatch gt 0L).

        blueconv = 1.0 
        greenconv = 1.0 
        redconv = 1.0 

      endelse ; End else block for file MET to structure MET
              ; match found (numstmatch gt 0L).

      ; Update user. 
      print, evstr, fits_to_show[ifts], ' for scaling, ', 'blueconv: ', $ 
        blueconv, ' greenconv: ', greenconv, ' redconv: ', redconv

      ; Apply 630 ratioed corrections. 
      imgbw630corr = blue * blueconv 
      imggw630corr = green * greenconv 
      imgrw630corr = red * redconv 

      ; Scale uncorrected planes for display. 
      redbyt = bytscl(red, min = overminred, max = overmaxred, $
        top = overtop) 
      greenbyt = bytscl(green, min = overmingreen, max = overmaxgreen, $ 
        top = overtop) 
      bluebyt = bytscl(blue, min = overminblue, max = overmaxblue, $ 
        top = overtop) 

      ; Scale 630 corrected imagery. 
      red630corr = red * 0.0 
      green630corr = green * 0.0 
      blue630corr = blue * 0.0 
      red630corr[getgoodr] = imgrw630corr[getgoodr]
      green630corr[getgoodg] = imggw630corr[getgoodg]
      blue630corr[getgoodb] = imgbw630corr[getgoodb] 
      red630corrbyt = bytscl(red630corr, $ 
        min = overminred, max = overmaxred, top = overtop) 
      green630corrbyt = bytscl(green630corr, $ 
        min = overmingreen, max = overmaxgreen, top = overtop) 
      blue630corrbyt = bytscl(blue630corr, $ 
        min = overminblue, max = overmaxblue, top = overtop) 
  
      ; Assign scaled original and corrected 
      ; image planes to true-color arrays. 
      originalimg = bytarr(numxpix, numypix, 3) 
      corrected630img = bytarr(numxpix, numypix, 3) 
      originalimg[*, *, 0] = redbyt 
      originalimg[*, *, 1] = greenbyt 
      originalimg[*, * ,2] = bluebyt 
      corrected630img[*, *, 0] = red630corrbyt 
      corrected630img[*, *, 1] = green630corrbyt 
      corrected630img[*, * ,2] = blue630corrbyt 

      ; Display scaled original and corrected images and save. 
      swindow, xs = numxpix, ys = numypix, title = stripfits 
      tv, originalimg, true = 3, ord = 1 
      jpegscreen, 'orig_' + stripfits + '.jpg', /quiet 
      swindow, xs = numxpix, ys = numypix, title = '630 corr ' + stripfits 
      tv, corrected630img, true = 3, ord = 1 
      jpegscreen, 'corr630_' + stripfits + '.jpg', /quiet 

    endif ; End if block for scaling and displaying data (iloop eq endloop).

  endfor ; End for loop over fits images (ifts = 0L, numfts - 1L). 

endfor ; End for loop for scaling and/or correcting and 
       ; displaying the data (iloop = startloop, endloop). 

; Return control to main, end program.
return
end


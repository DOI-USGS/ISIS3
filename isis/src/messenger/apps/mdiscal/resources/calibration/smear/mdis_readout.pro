pro  mdis_readout,image_dark,exp_1,im_type,filter1,image_readout
!ORDER=1
;MSR
;UPDATED May 1, 2003
;UPDATED August 2005
;UPDATED February 24 2008 (rconstant per wavelength)

; Function to undo linear smearing blur that occurs during
; pixel read-out time in MDIS Imager
; Algorithm form Pat Murphy, APL originally for NEAR MSI

; t2 = .9 msec / nlines frame transfer per line
; T1 = exposure time

;if im_type eq 1 then begin
;print,'*************************************************************'
;print,'Warning: need confirmation that t constant is half for binned '
;print,'*************************************************************'
;endif

;FOR MESSENGER MDIS 3.84 msecs initially

;  t2 = 3.84/1024.  ORIGINAL VALUE FROM HUGO
;  readout_constant=3.4/1000.     ;converting into seconds to match exposure
  rconstant=fltarr(12)
   rindex=filter1-1
  rconstant[0]=3.4/1000.    ;1  A  700nm narrow       .
  rconstant[1]=3.4/1000.    ;2  B  700nm clear
  rconstant[2]=3.7/1000.    ;3  C  480nm
  rconstant[3]=3.5/1000.    ;4  D  560nm
  rconstant[4]=3.4/1000.    ;5  E  630nm
  rconstant[5]=4.2/1000.    ;6  F  430nm
  rconstant[6]=3.4/1000.    ;7  G  750nm
  rconstant[7]=3.4/1000.    ;8  H  950nm*
  rconstant[8]=3.4/1000.    ;9  I 1000nmS*
  rconstant[9]=3.4/1000.    ;10 J  900nm*
  rconstant[10]=3.4/1000.   ;11 K 1020nm*
  rconstant[11]=3.4/1000.   ;12 L  830nm*

  t2 = rconstant[rindex]/1024.

  if im_type eq 1 then t2=rconstant[rindex]/512.

print,'Using readout constant: ',rconstant[rindex],' Updated Feb 2008' 
print,'Using readout time of: ',t2, ' msecs'

 T1 = exp_1

 s=size(image_dark)

 image_readout = image_dark

 NX = s(1)
 NY = s(2)

 added = fltarr(NX)

if T1 gt 0.0000 then begin
 for j = 1, NY-1 do begin
      added = added + t2/T1*image_readout(*,j-1)
      image_readout(*,j) = image_dark(*,j) - added
 endfor
endif

if T1 eq 0.0000 then print, '*** INFO: Found 0 exposure, skip readout smear'

print,'       MDIS: readout smear removed'

return
end










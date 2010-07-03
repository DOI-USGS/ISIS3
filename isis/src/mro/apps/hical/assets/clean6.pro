;clean6.pro
;need the following
;restore, 'c:\hirise\data\dark4\b_matrix_25nov07.sav
; dark current matrices 28 channels of 1024/bin columns by 4 tdi
;
;restore,'c:\hirise\data\dark4\temp_profiles.sav'
; temperature profiles matrices 256 columns by 4 tdi by intercept and slope
;
;History
; 1/5/08 Alan changed image variable from zz to z to use less memory
;10 jan 08 Alan added provision for having no b_matrix
;
function clean6,z,info,b_matrix,t_intercept,t_slope
nc=info.ncol
tdi=info.tdi
case tdi of
128: itdi=0
64: itdi=1
32: itdi=2
8: itdi=3
endcase
;z=zz	; i preserved original for debugging
;
removemissing, z, bad=fmissing	; get rid of gaps
;
;drift correction
	swidth=11
  	ave_cols, z, 5, 11, b ;get row means for buffer pixels, cols[5:11]
    	iccd = info.ccd
    	ichl = info.channel
    	l0 = 20 + (20 + info.tdi) / info.bin ;image starting line
    	b = smooth(b, swidth, /edge)	; need for initial parameters
    	drift = driftcorrect(b, info, params, x, status=status) ;get drift function
;    	print,params	;for debugging and watching progress
    	drift=drift-drift(0)
    	nl=info.nline
    	for iline=long(l0),nl-1 do   z(*,iline)=z(*,iline)-drift(iline-l0)
    	;
;offset correction
ave_lines,z,1,19,offset,sd
for iline=0l,nl-1 do z[0,iline] = z[*,iline]-offset
;
;dc subtraction
 w=where(b_matrix.ccd eq info.ccd and $
            b_matrix.ch eq info.channel and $
            b_matrix.tdi eq info.tdi and $
            b_matrix.bin eq info.bin, count)	;finds right dark current matrix
if w(0) eq -1 then print,info.obs_id, ' No b_matrix available used mask'

temp=(info.fpa_py_temp+info.fpa_my_temp)/2	;use average baseplate temperature
t_prof=rebin(smooth(t_intercept(*,itdi),3,/edge)+smooth(t_slope(*,itdi),3,/edge)*temp,1024/info.bin);convert to correct number of columns
; i have left the smoothing in for the time being. after testing we can fix it
t_profile=[fltarr(12)+temp,t_prof,fltarr(16)+temp]	;i process all columns as it is useful for diagnostic tests
;
if w(0) ne -1 then dc = b_matrix(w).matrix(0:nc-1) * $ ;b[nc] = dn / px / sec
      info.linetime * 1e-6 * $  ;scale by time
      info.bin^2 * $            ;bin area
      (20*103/89. + info.tdi) * $ ;we're doing 20 masked lines of larger area + tdi area
      tempeqn(t_profile, 2, 12) / tempeqn(21, 2, 12) $ ;dc at temp=21c. is the reference
      else  ave_lines,z,20,20+20/info.bin-1,dc


    dcs = smooth(dc[12:nc-17],3,/edge)
    dc[12:nc-17] = dcs            ;replace by smoothed b

    for iline=20l,nl-1 do z[0,iline] = z[*,iline]-dc
;__________________________________________________
;stop
return,z
end

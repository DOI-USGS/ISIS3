;test_mdr_15feb08.pro
pro test_mdr, obs
;
files=file_search(obs,'*.IMG')
nf=n_elements(files)
	mode_bin=intarr(14,2)
	mode_tdi=intarr(14,2)
	n_lines=lonarr(14,2)
	w_times=fltarr(14,2)
;
t0=systime(1)
;
for j=0,nf-1 do begin
	if files(j) ne '' then begin
	rheader,files(j),info,0
	iccd=info.ccd & ich=info.channel
	bin=info.bin
	tdi=info.tdi
	start=	20+(20+tdi)/bin
	n_lines(iccd,ich)=long(info.nline-start)*bin
	w_times(iccd,ich)=time_secs(info.start_time)-time_secs(info.analog_on_time)
	mode_bin(iccd,ich)=bin
	mode_tdi(iccd,ich)=tdi
	endif
	endfor
max_lines=max(n_lines)

center0=fltarr(14,2,max_lines)
overlap=center0
imagemean= center0
restore,'c:\hirise_idl\matrices\save\a_matrix_composite_1feb08.sav'
restore,'c:\hirise_idl\matrices\save\b_matrix_2dec07.sav'
restore,'c:\hirise_idl\matrices\save\gain_15feb08.sav'
restore,'c:\hirise_idl\matrices\save\temp_profiles.sav'
restore,'c:\hirise_idl\matrices\save\line_correct_15Feb08A.sav'
ggain=gain
t0=systime(1)
print,systime(1)-t0
for i= 0,nf-1 do begin
	rchannel,files(i),z,info,0
;	if info.luntype ne 0 then print,'lutted image',info.file
	if (time_secs(info.start_time)-time_secs(info.analog_on_time)) gt 150 then begin
		start=20+(20+info.tdi)/info.bin
		nc=info.ncol
		ncol=info.ncol
		nlines = info.nline
		ccd=info.ccd & ch =info.channel & tdi=info.tdi & bin=info.bin
		w=where((a_matrix.ccd eq ccd) and (a_matrix.ch eq ch )and a_matrix.tdi eq tdi and a_matrix.bin eq bin) ;& help,w
		if w(0) ne -1 then a=1/a_matrix(w).matrix else a=fltarr(1052)+1
		z=clean6(z,info,b_matrix,t_intercept,t_slope)

		z=line_correction(z,info,line_correct)

		FPA_temp=(info.fpa_py_temp+info.fpa_my_temp)/2
		for j=0l,info.nline-1 do z(12,j) =z(12:info.ncol-16-1,j)*a(12:info.ncol-16-1)*(1+0.005*(FPA_temp-21))

;		print,i,' ccd=',ccd,' ch=',ch,' tdi=',tdi,' bin=',bin,ncol,nlines

		case bin of
			1: ibin=0
			2: ibin=1
			4: ibin=2
			8: ibin=3
			else: stop,'wrong bin number'
			endcase

		factor=ggain(info.ccd,info.channel,ibin)*128/tdi/bin^2	;normalizes gain
		nl=info.nline
		for il=0l,nl-1 do z(*,il)=z(*,il)*factor
;---------------------------------------
		wchannel_mdr,files(i),z>0,0
		start=	20+(20+info.tdi)/bin
		endline=info.nline-1
		ave_cols,z(12:12+1024/bin-1,start:endline),0,24/bin,c
		center0(info.ccd,info.channel,0:(endline-start+1)*info.bin-1) = rebin(c,max_lines)
		ave_cols,z(12:12+1024/bin-1,start:endline),(1024-24)/bin,1024/bin-1,over
		overlap(info.ccd,info.channel,0:(endline-start+1)*info.bin-1) = rebin(over,max_lines)
		ave_cols,z(12:12+1024/bin-1,start:endline),0,1024/bin-1,m

		imagemean(info.ccd,info.channel,0:(endline-start+1)*info.bin-1)=rebin(m,max_lines)
;
		endif	;
		print,files(i),systime(1)-t0
		wait,0.05
	endfor
	pos=strpos(obs,'PSP')
obsname=strmid(obs,pos,15)
fileb= 'C:\HiRISE\Data\test_mdr_images\results\'+obsname+'_join_data_A.sav'
save,filename=fileb,files,center0,overlap,imagemean,mode_bin,mode_tdi,info
;
end

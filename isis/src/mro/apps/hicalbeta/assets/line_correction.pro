;line_correction.pro
function line_correction,z,info,line_correct
l=findgen(info.nline)
t=l*info.linetime*1e-6*info.bin
i=info.ccd
k=info.channel
;
case info.bin of
1: ibin=0
2: ibin=1
4: ibin=2
endcase
;
lin_cor=line_correct(i,k,ibin,0)+line_correct(i,k,ibin,1)*t+line_correct(i,k,ibin,2)*exp(line_correct(i,k,ibin,3)*t)
start=20l+(20+info.tdi)/info.bin
nl=info.nline
;stop
for il=start,nl-1 do z(*,il)=z(*,il)/lin_cor(il-start)
return,z
end

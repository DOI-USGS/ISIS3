pro wacfocallen, newfl=newfl,iak=iak

wacfls = [ 78.218D, 78.163D, 77.987D, 78.023D, 78.109D, 78.075D, $
           78.218D, 78.449D, 78.510D, 78.390D, 78.535D, 78.308D ]

;  Filter two is the clear filter and used to construct the geometric 
;  distortion model.  Any change in this filter is directly proportional
;  to the other filters.
baseFilterIndex = 1

if (N_ELEMENTS(newfl) le 0) then $
  newfl = double(wacfls[baseFilterIndex])

fratio = double(newfl / wacfls[baseFilterIndex])

for i = 0, 11 do begin
  flen = wacfls[i] * fratio
  print, 'Filter',i+1,' :, FocalLength: ', flen
endfor

if (N_ELEMENTS(iak) gt 0) then begin

print, " "
print, " Naif Instrument Camera Kernel Focal Lengths"
wacN0 = -236801
flen = wacfls[baseFilterIndex] * fratio
print, "INS" + strcompress(string(wacN0+1),/REMOVE_ALL) + "_FOCAL_LENGTH = ", flen
for i = 0, 11 do begin
  flen = wacfls[i] * fratio
  print, "INS" + strcompress(string(wacN0-i),/REMOVE_ALL) + "_FOCAL_LENGTH = ", flen
endfor

endif

return
end




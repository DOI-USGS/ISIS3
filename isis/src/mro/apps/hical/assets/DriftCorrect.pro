;driftcorrect.pro Version 10Oct 07
PRO gauss4, x, a, f, pder
; Implement the function y = a[0] + a[1]*x + a[2] * exp(a[3] * x)
; return the value of the function as f, and the value of
; the partials at the value of x in pder[4].
; let p0 = exp(a[3]*x)
; dy/da[0] = 1.
; dy/da[1] = x
; dy/da[2] = p0
; dy/da[3] = a[2] * x * p0

nx = n_elements(x)
; Note: the next two lines of code are added/changed
maxLog = 87.0                   ;approx log of max finite single FP value
; Note, if you use double throughout, maxLog is around 709.  It
; shouldn't matter because normally the term should be < 1.
p0 = double(exp(a[3]*x < maxLog))
f = a[0] + a[1] * x + a[2] * p0
if total(finite(f)) ne nx then stop
; Partial of y with respsect to a[i]:
pder = [[ replicate(1., nx)], [x], [p0], [a[2]*x*p0]]
end

Function DriftCorrect, buffer, info, params, x, STATUS=status
; return the drift correction profile for an image.  This is obtained by
; fitting the function  y = a[0] + a[1]*x + a[2] * exp(a[3] * x)
; to the mean of the buffer pixels for each line.
;
;  Inputs:
; buffer[info.nline] = mean of buffer pixel columns [5:11] for each line of image.
; info = header structure for image
;  Outputs:
; params = parameters of function fit to the buffer area [4].
; Function value = [info.nline] vector of drift values / line.
;    Time 0 is defined as the time of the start of the 1st image line.
; x = time vector [info.nline], in seconds.
; STATUS = status of fit.  0 for OK, 1 for ChiSq increasing without
;      bounds, 2 for did not converge.
;

sWidth = 17                     ;Width of our smoother
n0 = 20 + (20 + info.tdi) / info.bin ;Image starting line
n = info.nline - n0              ;# of image lines

if info.nline lt 4000 then begin ;short image?
    x = findgen(n) * (info.bin * info.linetime * 1.0e-6) ;time in seconds [n]
    cc = poly_fit(x, buffer[n0:*], 1, yfit2) ;just use linear
    params = [cc[0], cc[1], 0.0, 1.0]
    status = 0
    return, yfit2
endif

last = info.nline - info.trim/info.bin  - 1  ;Last line to use
b1 = buffer[n0 : last]          ;Extract rows we care about
nb = n_elements(b1)
b2 = smooth(b1, sWidth, /EDGE)  ;Smooth
x = findgen(nb) * (info.bin * info.linetime * 1.0e-6) ;time in seconds [nb]
xmax = x[nb-1]                  ;Last time value

; Fit a straight line to latter half of data for initial estimates
nb2 = nb/2
cc = poly_fit(x[nb2:*], b2[nb2:*], 1) ;linear fit
resid = b2 - (cc[0] + x * cc[1]) ;residuals

tail = mean(resid[0.9*nb : *])  ;mean of tail area
head = mean(resid[0:20])        ;mean of start area

 ; fit y = a[0] + a[1]*x + a[2] * exp(a[3] * x)  GAUSS4
a = [cc[0], cc[1], head-tail, -5./xmax] ;Initial guess for params
a0 = a
yfit2 =  curvefit(x, b1, replicate(1., nb), a, $
                  FUNCTION_NAME="GAUSS4", STATUS=status, ITER=iter, ITMAX=100)
; print, 'ITER = ', iter
if status ne 0 then begin       ;Did fit work??  it might not have converged.
    print, 'Could not fit, using straight line., status = ', status
    print, info.obs_id, ', CCD/CHL ', info.ccd, info.channel
    print, 'Initial: ', a0
    print, 'Fit:     ', a
    cc = poly_fit(x, b1, 1)     ;Punt & fit a straight line
    a = [cc[0], cc[1], 0.0, -1.0] ;i.e. exp coeff = 0
endif

; recalculate x and yfit2 for entire image area
x = findgen(info.nline-n0) * (info.bin * info.linetime * 1.0e-6) ;time in seconds
yfit2 = a[0] + a[1] * x + a[2] * exp(a[3] * x)
params = a                      ;Return function params.
return, yfit2
end

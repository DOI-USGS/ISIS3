#########################################################################
# Step 1: Run HapkeHen photometric model with albedoatm normalization
# model, and an hapkeamt2 atmospheric model.  The tau value was obtained
# by collecting measurements of the darkest shadows within a set of image
# sequence having the same filter, and running the result through shadowtau 
# to get estimated atmospheric optical depth (tau) values.
#
#Note:  The entire line for each program must be continuous when the
#       script is executed.  Take out the returns before executing.
#########################################################################

photomet from=f344s47.lev1_slo.cub to=344s47_albat.cub 
frompvl=mdim2.1_hapken_albedoatm_hapkeATM2_blu_vio.pvl phtname=hapkehen 
theta=30 wh=0.16 hg1=0.145 hg2=1 hh=0 b0=0 atmname=hapkeatm2 nulneg=yes 
tau=0.398 tauref=0 hga=0.78 wha=0.76 hnorm=0.003 normname=albedoatm 
incref=0

photomet from=f344s47.lev1_albatdiv.cub to=f344s47.lev1_topat.cub 
frompvl=mdim2.1_hapken_topoatm_hapkeATM2_blue_vio.pvl phtname=hapkehen 
theta=30 wh=0.16 hg1=0.145 hg2=1 hh=0 b0=0 atmname=hapkeatm2 nulneg=no 
tau=.398 tauref=0.5 hga=0.78 wha=0.76 hnorm=0.003 normname=topoatm 
incref=30 albedo=0.15

photomet from=f807a10.lev1_slo.cub to=f807a10.lev1_albat.cub 
frompvl=mdim2.1_hapken_albedoatm_hapkeATM2_otherfilters.pvl 
phtname=hapkehen theta=30 wh=0.52 hg1=0.213 hg2=1 hh=0 b0=0 
atmname=hapkeatm2 nulneg=yes tau=.258 tauref=0 hga=0.68 wha=0.95 
hnorm=0.003 normname=albedoatm incref=0

#########################################################################
# Step2:  Run a divfilter on the output of step 1
#########################################################################

divfilter from=f344s47.lev1_albat.cub to=f344s47.lev1_albatdiv.cub 
samp=215 line=215

divfilter from=f804a29.lev1_albat.cub to=f804a29.lev1_albatdiv.cub 
samp=215 line=215

divfilter from=f807a10.lev1_albat.cub to=f807a10.lev1_albatdiv.cub 
samp=215 line=215

#########################################################################
# Step3:  Run HapkeHen photometric model with topoatm normalization
# model, and an hapkeamt2 atmospheric model.  Use the same tau values as
# in step 1, and modify the reference incidence angle value.
#########################################################################

photomet from=f804a29.lev1_slo.cub to=f804a29.lev1_albat.cub 
frompvl=mdim2.1_hapken_albedoatm_hapkeATM2_otherfilters.pvl 
phtname=hapkehen theta=30 wh=0.52 hg1=0.213 hg2=1 hh=0 b0=0 
atmname=hapkeatm2 nulneg=yes tau=.264 tauref=0 hga=0.68 wha=0.95 
hnorm=0.003 normname=albedoatm incref=0

photomet from=f804a29.lev1_albatdiv.cub to=f804a29.lev1_topat.cub 
frompvl=mdim2.1_hapken_topoatm_hapkeATM2_otherfilters.pvl 
phtname=hapkehen theta=30 wh=0.52 hg1=0.213 hg2=1 hh=0 b0=0 
atmname=hapkeatm2 nulneg=no tau=.264 tauref=0.5 hga=0.68 wha=0.95 
hnorm=0.003 normname=topoatm incref=30.0 albedo=0.15

photomet from=f807a10.lev1_albatdiv.cub to=f807a10.lev1_topat.cub 
frompvl=mdim2.1_hapken_topoatm_hapkeATM2_otherfilters.pvl 
phtname=hapkehen theta=30 wh=0.52 hg1=0.213 hg2=1 hh=0 b0=0 
atmname=hapkeatm2 nulneg=no tau=.258 tauref=0.5 hga=0.68 wha=0.95 
hnorm=0.003 normname=topoatm incref=30.0 albedo=0.15

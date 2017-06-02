# Category test for OSIRIS-REx sample cameras.
# The programs chosen for this test were copied from the OSIRIS-REx team's "testIsis" program.
#
# NOTE: this test should be updated when a better samcam image is acquired.
#
# @history 2016-09-09 Jeannie Backer - Original version.
#
include $(ISISROOT)/make/isismake.tsts

commands:
	# import first image
	ocams2isis $(TSTARGS)                                                          \
	           from=$(INPUT)/20141111T202650_SCAM_L2_V001_SCAM.fits                \
	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub        \
	           >& /dev/null;

#	# spiceinit first image
#	spiceinit $(TSTARGS)                                                           \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           ckpredict=t                                                         \
#	           spkpredict=t                                                        \
#	           extra='$$osirisrex/kernels/pck/bennu_v10.tpc'                       \
#	           >& /dev/null;
#
#	# phocube first image
#	# create backplane with dn, phase angle, emission angle, incidence angle, and pixel resolution
#	phocube $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-bp.cub               \
#	           dn=t                                                                \
#	           phase=t                                                             \
#	           emission=t                                                          \
#	           incidence=t                                                         \
#	           lat=f                                                               \
#	           lon=f                                                               \
#	           pixelres=t                                                          \
#	           >& /dev/null;
#
#	# explode first image's backplane into 5 cubes
#	explode $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-bp.cub             \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-bp-explode           \
#	           >& /dev/null;
#
#	# reunite exploded cubes. output should be identical to phocube bp output above
#	$(LS) $(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-bp-explode* > $(OUTPUT)/phocubeBandList.txt;
#	cubeit $(TSTARGS)                                                              \
#	           from=$(OUTPUT)/phocubeBandList.txt                                  \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-bp.cub               \
#	           >& /dev/null;
#
#	# for each pixel in first image, multiply by 2 then add 1 
#	algebra $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-algebra.cub          \
#	           operator=unary                                                      \
#	           a=2                                                                 \
#	           a=1                                                                 \
#	           >& /dev/null;
#
#	# get camera information for first image
#	caminfo $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-caminfo.pvl          \
#	           >& /dev/null;
#
#	# get camera statistics for first image
#	camstats $(TSTARGS)                                                            \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-camstats.pvl         \
#	           >& /dev/null;
#
#	# get DN statistics for first image
#	stats $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-dnstats.pvl          \
#	           > /dev/null;
#
#	# photrim first image
#	photrim $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-photrim.cub          \
#	           minphase=100                                                        \
#	           maxphase=101                                                        \
#	           >& /dev/null;
#
#	# photomet first image
#	photomet $(TSTARGS)                                                            \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-photomet_lunlam.cub  \
#	           phtname=lunarlambert                                                \
#	           L=0.5                                                               \
#	           normname=albedo                                                     \
#	           incref=30                                                           \
#	           incmat=0.0                                                          \
#	           thresh=10e30                                                        \
#	           albedo=1.0                                                          \
#	           maxemission=80                                                      \
#	           maxincidenc=85                                                      \
#	           >& /dev/null;
#
#	# footprint first image
#	footprintinit $(TSTARGS)                                                       \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           >& /dev/null;
#
#	#campt 4 corners and center of first image
#	campt $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-campt_upperleft.pvl  \
#	           SAMPLE=1                                                            \
#	           LINE=1                                                              \
#	           ALLOWOUTSIDE=no                                                     \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-campt_upperright.pvl \
#	           SAMPLE=1024                                                         \
#	           LINE=1                                                              \
#	           ALLOWOUTSIDE=no                                                     \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-campt_lowerright.pvl \
#	           SAMPLE=1024                                                         \
#	           LINE=1024                                                           \
#	           ALLOWOUTSIDE=no                                                     \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-campt_lowerleft.pvl  \
#	           SAMPLE=1                                                            \
#	           LINE=1024                                                           \
#	           ALLOWOUTSIDE=no                                                     \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-campt_center.pvl     \
#	           SAMPLE=512                                                          \
#	           LINE=512                                                            \
#	           ALLOWOUTSIDE=no                                                     \
#	           >& /dev/null;
#
#	# get mapping group containing Equirectangular mosaic range values for the two images
#	$(LS) $(OUTPUT)/*spiceinit.cub > $(OUTPUT)/unprojectedSpiceinitCubeList.txt;
#	mosrange $(TSTARGS)                                                            \
#	            fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                \
#	            to=$(OUTPUT)/SCAM_equi.map                                         \
#	            precision=6                                                        \
#	            projection=Equirectangular                                         \
#	           >& /dev/null;
#
#	# project the two images using the map from mosrange
#	cam2map $(TSTARGS)                                                             \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-cam2map_equi.cub     \
#	           map=$(OUTPUT)/SCAM_equi.map                                         \
#	           pixres=map                                                          \
#	           >& /dev/null;
#
#	# getsn from each image
#	echo -e "\nOutput of getsn..." > $(OUTPUT)/SCAM_getsn.txt;
#	getsn $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM-spiceinit.cub      \
#	           >> $(OUTPUT)/SCAM_getsn.txt;
#
#	# change name so that pvldiff can compare
#	$(MV) $(OUTPUT)/SCAM_equi.map $(OUTPUT)/SCAM_equi.pvl;

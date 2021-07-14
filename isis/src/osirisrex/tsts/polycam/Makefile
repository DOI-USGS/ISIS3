# Category test for OSIRIS-REx poly cameras.
# The programs chosen for this test were copied from the OSIRIS-REx team's "testIsis" program.
# The output file names in this test match the output file names from the "testIsis"
# program in order to make comparison easier.
#
# @history 2016-09-09 Jeannie Backer - Original version. 
#
2019-01-13T23_35_59.000_PCAM_L2b_V001_map.cub.TOLERANCE=1e-03
2019-01-13T23_36_05.000_PCAM_L2b_V001_map.cub.TOLERANCE=1e-03
mosaic.cub.TOLERANCE=1e-03
noseamMOS.cub.TOLERANCE=1e-03
trimmed.cub.TOLERANCE=1e-03

include $(ISISROOT)/make/isismake.tsts

commands:
	# import first image
	ocams2isis $(TSTARGS)                                                              \
	           from=$(INPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.fits                \
	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                  \
	           >& /dev/null;

	# spiceinit first image
	spiceinit $(TSTARGS)                                                               \
	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
	           SCLK='$$osirisrex/kernels/sclk/SCLK.tsc'                                \
			   extra='$$osirisrex/kernels/pck/bennu_v10.tpc'                           \
	           > /dev/null;

#	# phocube first image
#	# create backplane with dn, phase angle, emission angle, incidence angle, and pixel resolution
#	phocube $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_bp.cub               \
#	           dn=t                                                                    \
#	           phase=t                                                                 \
#	           emission=t                                                              \
#	           incidence=t                                                             \
#	           pixelres=t                                                              \
#	           >& /dev/null;
#
#	# explode first image's backplane into 5 cubes
#	explode $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_bp.cub             \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001                      \
#	           >& /dev/null;
#
#	# reunite exploded cubes. output should be identical to phocube bp output above
#	$(LS) $(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.band* > $(OUTPUT)/phocubeBandList.txt;
#	cubeit $(TSTARGS)                                                                  \
#	           from=$(OUTPUT)/phocubeBandList.txt                                      \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_cubeit.cub           \
#	           >& /dev/null;
#
	# for each pixel in first image, multiply by 2 then add 1 
	algebra $(TSTARGS)                                                                 \
	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_ALGEBRA.cub          \
	           operator=unary                                                          \
	           a=2                                                                     \
	           a=1                                                                     \
	           >& /dev/null;

#	# get camera information for first image
#	caminfo $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_caminfo.pvl          \
#	           >& /dev/null;
#
#	# get camera statistics for first image
#	camstats $(TSTARGS)                                                                \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_camstats.pvl         \
#	           >& /dev/null;
#
#	# get DN statistics for first image
#	stats $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_dnstats.pvl          \
#	           > /dev/null;
#
#	# photrim first image
#	photrim $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/photrimmed.cub                                             \
#	           minphase=100                                                            \
#	           maxphase=101                                                            \
#	           >& /dev/null;
#
#	# photomet first image
#	photomet $(TSTARGS)                                                                \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/lunlam.cub                                                 \
#	           phtname=lunarlambert                                                    \
#	           L=0.5                                                                   \
#	           normname=albedo                                                         \
#	           incref=30                                                               \
#	           incmat=0.0                                                              \
#	           thresh=10e30                                                            \
#	           albedo=1.0                                                              \
#	           maxemission=80                                                          \
#	           maxincidenc=85                                                          \
#	           >& /dev/null;
#
#	# footprint first image
#	footprintinit $(TSTARGS)                                                           \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           >& /dev/null;
#
#	#campt 4 corners and center of first image
#	campt $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_campt_upperleft.pvl  \
#	           SAMPLE=1                                                                \
#	           LINE=1                                                                  \
#	           ALLOWOUTSIDE=no                                                         \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_campt_upperright.pvl \
#	           SAMPLE=1024                                                             \
#	           LINE=1                                                                  \
#	           ALLOWOUTSIDE=no                                                         \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_campt_lowerright.pvl \
#	           SAMPLE=1024                                                             \
#	           LINE=1024                                                               \
#	           ALLOWOUTSIDE=no                                                         \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_campt_lowerleft.pvl  \
#	           SAMPLE=1                                                                \
#	           LINE=1024                                                               \
#	           ALLOWOUTSIDE=no                                                         \
#	           >& /dev/null;
#	campt $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_campt_center.pvl     \
#	           SAMPLE=512                                                              \
#	           LINE=512                                                                \
#	           ALLOWOUTSIDE=no                                                         \
#	           >& /dev/null;
#
#	# import second image
#	ocams2isis $(TSTARGS)                                                              \
#	           from=$(INPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.fits                \
#	           to=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub                  \
#	           >& /dev/null;
#
#	# spiceinit second image
#	spiceinit $(TSTARGS)                                                               \
#	           from=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub                \
#	           ckpredict=t                                                             \
#	           spkpredict=t                                                            \
#	           extra='$$osirisrex/kernels/pck/bennu_v10.tpc'                           \
#	           >& /dev/null;
#
#	# footprintinit second image
#	footprintinit $(TSTARGS)                                                           \
#	           from=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub                \
#	           >& /dev/null;
#
#	# get mapping group containing Equirectangular mosaic range values for the two images
#	$(LS) $(OUTPUT)/*PCAM_L2b_V001.cub > $(OUTPUT)/unprojectedSpiceinitCubeList.txt;
#	mosrange $(TSTARGS)                                                                \
#	            fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                    \
#	            to=$(OUTPUT)/PCAM_equi.map                                             \
#	            precision=6                                                            \
#	            projection=Equirectangular                                             \
#	           >& /dev/null;
#
#	# project the two images using the map from mosrange
#	cam2map $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001_map.cub              \
#	           map=$(OUTPUT)/PCAM_equi.map                                             \
#	           pixres=map                                                              \
#	           >& /dev/null;
#	cam2map $(TSTARGS)                                                                 \
#	           from=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub                \
#	           to=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001_map.cub              \
#	           map=$(OUTPUT)/PCAM_equi.map                                             \
#	           pixres=map                                                              \
#	           >& /dev/null;
#
#	# mosaic the two Equirectangular projected images
#	$(LS) $(OUTPUT)/*map.cub > $(OUTPUT)/projectedCubeList.txt;
#	automos $(TSTARGS)                                                                 \
#	           fromlist=$(OUTPUT)/projectedCubeList.txt                                \
#	           mosaic=$(OUTPUT)/mosaic.cub                                             \
#	           >& /dev/null;
#
#	# create a noseam mosaic
#	noseam $(TSTARGS)                                                                  \
#	           fromlist=$(OUTPUT)/projectedCubeList.txt                                \
#	           to=$(OUTPUT)/noseamMOS.cub                                              \
#	           samples=5                                                               \
#	           lines=5                                                                 \
#	           >& /dev/null;
#
#	# band trim automos mosaic
#	bandtrim $(TSTARGS)                                                                \
#	           from=$(OUTPUT)/mosaic.cub                                               \
#	           to=$(OUTPUT)/trimmed.cub                                                \
#	           >& /dev/null;
#
#	# coreg the two original (unprojected) images
#	coreg $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           match=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub               \
#	           to=$(OUTPUT)/PCAM_coreg.cub                                             \
#	           onet=$(OUTPUT)/PCAM_coreg.net                                           \
#	           >& /dev/null;
#
#	# getsn from each image
#	echo -e "\nOutput of getsn..." > $(OUTPUT)/PCAM_getsn.txt;
#	getsn $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub                \
#	           >> $(OUTPUT)/PCAM_getsn.txt;
#	getsn $(TSTARGS)                                                                   \
#	           from=$(OUTPUT)/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub                \
#	           >> $(OUTPUT)/PCAM_getsn.txt;
#
#	# find overlaps of the two images
#	findimageoverlaps $(TSTARGS)                                                       \
#	           fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                     \
#	           overlaplist=$(OUTPUT)/PCAM_findimageoverlaps.txt                        \
#	           >& /dev/null;
#
#	# report overlap stats for the two images
#	overlapstats $(TSTARGS)                                                            \
#	           fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                     \
#	           overlaplist=$(OUTPUT)/PCAM_findimageoverlaps.txt                        \
#	           detail=full                                                             \
#	           to=$(OUTPUT)/PCAM_overlapstats.csv                                      \
#	           >& /dev/null;
#
#	# autoseed the two images
#	autoseed $(TSTARGS)                                                                \
#	           fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                     \
#	           overlaplist=$(OUTPUT)/PCAM_findimageoverlaps.txt                        \
#	           onet=$(OUTPUT)/PCAM_autoseed.net                                        \
#	           deffile=$(INPUT)/autoseed.def                                           \
#	           networkid=OREx                                                          \
#	           pointid="Bennu????"                                                     \
#	           description=auto                                                        \
#	           > /dev/null;
#
#	# register the two images
#	pointreg $(TSTARGS)                                                                \
#	           fromlist=$(OUTPUT)/unprojectedSpiceinitCubeList.txt                     \
#	           cnet=$(OUTPUT)/PCAM_autoseed.net                                        \
#	           deffile=$(INPUT)/pointreg.def                                           \
#	           onet=$(OUTPUT)/PCAM_pointreg.net                                        \
#	           > /dev/null;
#
#	# change name so that pvldiff can compare
#	$(MV) $(OUTPUT)/PCAM_equi.map $(OUTPUT)/PCAM_equi.pvl;
#	# if findimageoverlaps fails, this will be reflected in the overlapstats test
#	# no need to keep this file.
#	$(RM) $(OUTPUT)/PCAM_findimageoverlaps.txt;

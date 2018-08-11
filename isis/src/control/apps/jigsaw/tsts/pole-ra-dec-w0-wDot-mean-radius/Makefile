APPNAME = jigsaw

# 2016-10-31 Tyler Wilson This test exercises the bundle adjustment of images from Enceladus 
# when the target body Enceladus when the target body parameter file is used.  This particular
# test sets the RadiusSolveOption to mean.
# 2018-07-24 Debbie A. Cook Changed the SED commands to remove digits beyond the second
# decimal place instead of the third to get the MacOS test to match.  Also changed
#                        "-0.00" to "0.00" after truncating.


# The "cat bundleout.txt" command in these tests uses perl and sed to do the following (in order):
# 1. remove cube filename paths.
# 2. remove net filename paths.
# 3. remove digits beyond the second decimal place of decimal numbers.
# 4. remove date and time.

# The "cat bundleout_images.csv/residuals.csv" commands uses perl to:
# 1. remove cube filename paths.
# 2. remove net filename paths.


include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cubes.lis cnet=$(INPUT)/enc_global_fin2-cl14_forken_extract_cl-salih-lon-constrained.net \
	onet=$(OUTPUT)/mean.net errorpropagation=yes solvetargetbody=yes camera_angles_sigma=2.0 tbparameters=$(INPUT)/EnceladusTargetBodyParameters-pole-ra-dec-w0-wDot-mean-radius.pvl \
	file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       | sed 's/\ -0.00/ 0.00/' \
	       | sed 's/\ -0.00/ 0.00/' \
	       > $(OUTPUT)/pole-ra-dec-w0-wDot-mean-radius_bundleout.txt;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/pole-ra-dec-w0-wDot-mean-radius_bundleout_images.csv;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/pole-ra-dec-w0-wDot-mean_radius_residuals.csv;
#$(MV) $(OUTPUT)/bundleout_images1.csv $(OUTPUT)/pole-ra-dec-w0-wDot-mean-radius_bundleout_images.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/pole-ra-dec-w0-wDot-mean-radius_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cubes.lis > /dev/null;
	$(MV) $(OUTPUT)/mean.net $(OUTPUT)/out-pole-ra-dec-w0-wDot-mean-radiius.net > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt > /dev/null;

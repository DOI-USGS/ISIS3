APPNAME = jigsaw
# This test excercises jigsaw's OUTLIER_REJECTION and REJECTION_MULTIPLIER parameters which
# reject control points whose residuals severaly deviate from the median residual.  This test
# uses the input data from jigsaw's apollo test.

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/apollo_rejection_out.net \
	           radius=yes \
	           outlier_rejection=yes \
	           rejection_multiplier=3.0 \
	           spsolve=position \
	           spacecraft_position_sigma=1000.0 \
	           camera_angles_sigma=2.0 > /dev/null;
	$(CAT) bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/apollo_rejection_bundleout.txt;
	$(CAT) residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/apollo_rejection_residuals.csv;
	$(MV) bundleout_points.csv $(OUTPUT)/apollo_rejection_bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt > /dev/null;
	$(RM) print.prt > /dev/null;
	$(RM) bundleout_images.csv > /dev/null;
	$(RM) residuals.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;

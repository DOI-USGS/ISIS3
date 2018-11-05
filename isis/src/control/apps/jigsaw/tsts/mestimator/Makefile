APPNAME = jigsaw
# This test exercises the bundle adjustment of images from the Apollo frame camera.
# It is solving for position, angles (with twist), and radius using a two tier M-estimator approach.

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time
#
# 2014-07-23 Jeannie Backer - Commented out references to bundleout_images.csv.  
#                Removed default parameters.
# 2016-08-11 Jeannie Backer - Updated documentation
# 2018-07-24 Debbie A. Cook - Now removing digits beyond the fourth decimal place of
#                        decimal numbers to make the mac test work.  Also changed
#                        "-0.0000" to "0.0000" after truncating.
include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	           cnet=$(INPUT)/Ames_5-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/mEstimator_out.net \
	           radius=yes \
	           model1=huber \
	           max_model1_c_quantile=0.6 \
	           model2=chen \
	           max_model2_c_quantile=.98 \
	           sigma0=1.e-3  \
	           maxits=150 \
	           spsolve=position > /dev/null;
	$(CAT) bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       | sed 's/\-0.0000/ 0.0000/' \
	       | sed 's/\-0.0000/ 0.0000/' \
	       > $(OUTPUT)/mEstimator_bundleout.txt;
	$(CAT) residuals.csv | $(SED) 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	       > $(OUTPUT)/mEstimator_residuals.csv
	$(CAT) bundleout_images.csv | $(SED) 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' \
	       > $(OUTPUT)/mEstimator_bundleout_images.csv
	$(RM) bundleout_images.csv > /dev/null;
	$(RM) residuals.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/mEstimator_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;
	$(RM) bundleout.txt > /dev/null


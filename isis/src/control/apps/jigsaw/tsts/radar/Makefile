APPNAME = jigsaw
# These tests exercise the bundle adjustment of images from the MiniRF radar instrument onboard LRO.
# Test 1: solving for position only, with error propagation
# Test 2: solving for position, velocity, acceleration, using polynomial over a constant hermite 
#         spline, with error propagation

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time
#
# 2014-07-23 Jeannie Backer - Commented out references to bundleout_images.csv.  
#                Removed default parameters.
# 2016-08-11 Jeannie Backer - Updated documentation
# 2016-10-05 Ian Humphrey - Removed no longer relevant RM command to remove inverseMatrix.dat
#                as jigsaw no longer generates this file during error propagation.
#                References #4315.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	           cnet=$(INPUT)/Cabeus_Orbit400_withSS_AprioriPts.net \
	           onet=$(OUTPUT)/radar_sparse_poh_out.net \
	           maxits=10 \
	           errorprop=yes \
	           spsolve=accelerations \
	           camsolve=no > /dev/null;
	$(CAT) bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/radar_sparse_poh_bundleout.txt;
	$(CAT) residuals.csv | $(SED) 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	       > $(OUTPUT)/radar_sparse_poh_residuals.csv;
#	$(CAT) bundleout_images.csv \
#	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
#	       | $(SED) 's/\([0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' \
#	       > $(OUTPUT)/radar_sparse_poh_bundleout_images.csv;
#	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/radar_sparse_poh_bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) residuals.csv  > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	           cnet=$(INPUT)/Cabeus_Orbit400_withSS_AprioriPts.net \
	           onet=$(OUTPUT)/radar_sparse_out.net \
	           maxits=10 \
	           errorprop=yes \
	           spsolve=position \
	           overhermite=yes \
	           camsolve=no > /dev/null;
	$(CAT) bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/radar_sparse_bundleout.txt;
	$(CAT) residuals.csv | $(SED) 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	       > $(OUTPUT)/radar_sparse_residuals.csv
	$(CAT) bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' \
	       > $(OUTPUT)/radar_sparse_bundleout_images.csv
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/radar_sparse_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) residuals.csv  > /dev/null;

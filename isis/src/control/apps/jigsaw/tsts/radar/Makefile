APPNAME = jigsaw
# These tests exercise the bundle adjustment of images from the MiniRF radar instrument onboard LRO.
# Test 1: oldsparse method, solving for position only, with error propagation
# Test 2: sparse (cholmod) method, solving for position, velocity, acceleration, using polynomial over a constant hermite spline, with error propagation

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/Cabeus_Orbit400_withSS_AprioriPts.net \
	onet=$(OUTPUT)/out.net \
	method=specialk \
	maxits=5 \
	sigma0=1.0e-5 \
	update=no \
	errorprop=yes \
	spsolve=position \
	camsolve=no | grep -v "100% Processed" | \
	grep -v "jigsaw" > radar_specialk_outLog.txt;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/radar_specialk_bundleout.txt
	cat radar_specialk_outLog.txt | grep -v "Elapsed Time:" | grep -v "TotalElapsedTime" | grep -v "ErrorPropagationElapsedTime" \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/radar_specialk_outLog.txt
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/specialk_residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/specialk_bundleout_images.csv
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) radar_specialk_outLog.txt > /dev/null;
	$(RM) residuals.csv  > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/Cabeus_Orbit400_withSS_AprioriPts.net \
	onet=$(OUTPUT)/out.net \
	method=sparse \
	maxits=10 \
	sigma0=1.0e-10 \
	update=no \
	errorprop=yes \
	spsolve=position \
	overhermite=yes \
	camsolve=no | grep -v "100% Processed" | \
	grep -v "jigsaw" > radar_sparse_outLog.txt;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/radar_sparse_bundleout.txt
	cat radar_sparse_outLog.txt | grep -v "Elapsed Time:" | grep -v "TotalElapsedTime" | grep -v "ErrorPropagationElapsedTime" \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/radar_sparse_outLog.txt
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/radar_sparse_residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/radar_sparse_bundleout_images.csv
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) radar_sparse_outLog.txt > /dev/null;
	$(RM) residuals.csv  > /dev/null;


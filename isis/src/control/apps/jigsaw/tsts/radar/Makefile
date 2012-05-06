APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/apollo_basin_qnet.net \
	onet=$(OUTPUT)/out.net \
	method=oldsparse \
	maxits=5 \
	sigma0=1.0e-5 \
	update=no \
	errorprop=yes \
	spsolve=position \
	camsolve=no | grep -v "100% Processed" | \
	grep -v "jigsaw" > radarOutLog.txt;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/bundleout.txt
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove net filename paths
	# 3. remove digits beyond the fourth decimal place of decimal numbers
	# 4. remove date and time
	cat radarOutLog.txt \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/radarOutLog.txt
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/bundleout_images.csv
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) radarOutLog.txt > /dev/null;
	$(RM) residuals.csv  > /dev/null;

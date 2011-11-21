APPNAME = jigsaw
#This test exercises a framing camera, specialk solution method, solving for radius, solving for twist,
#and spsolve=position.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	cnet=$(INPUT)/Ames_5-ImageLSTest_USGS_combined.net \
	onet=$(OUTPUT)/specialK_out.net \
	radius=yes \
	twist=yes \
	update=no \
	maxits=50 \
	method=specialk \
	errorpropagation=yes \
	outlier_rejection=no \
	sigma0=1.e-10  \
	ckdegree=yes \
	bundleout_txt=yes \
	output_csv=yes \
	residuals_csv=yes \
	ckdegree=2 \
	solvedegree=2 \
	camsolve=angles \
	spsolve=position > /dev/null;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/specialK_bundleout.txt
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove net filename paths
	# 3. remove digits beyond the fifth decimal place of decimal numbers
	# 4. remove date and time
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/specialK_residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/specialK_bundleout_images.csv
	$(RM) residuals.csv bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/specialK_bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt > /dev/null
	# 
	# Now test cholod method (default)
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	cnet=$(INPUT)/Ames_5-ImageLSTest_USGS_combined.net \
	onet=$(OUTPUT)/cholmod_out.net \
	radius=yes \
	twist=yes \
	update=no \
	maxits=50 \
	errorpropagation=yes \
	outlier_rejection=no \
	sigma0=1.e-10  \
	ckdegree=yes \
	bundleout_txt=yes \
	output_csv=yes \
	residuals_csv=yes \
	ckdegree=2 \
	solvedegree=2 \
	camsolve=angles \
	spsolve=position  > /dev/null;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/cholmod_bundleout.txt
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove net filename paths
	# 3. remove digits beyond the fifth decimal place of decimal numbers
	# 4. remove date and time
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/cholmod_residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/cholmod_bundleout_images.csv
	$(RM) residuals.csv bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/cholmod_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;
	$(RM) bundleout.txt > /dev/null

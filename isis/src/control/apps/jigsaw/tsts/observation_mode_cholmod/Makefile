APPNAME = jigsaw
# This test exercises the bundle adjustment of HiRISE images with observation mode, updating cubes.
# sparse (cholmod) method, solving for angles (3rd degree polynomial), updating cubes.

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT) > /dev/null;
	$(LS) -1 $(OUTPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/redPntreg.net \
	onet=$(OUTPUT)/obsmode_cholmod_out.net \
	observations=yes \
	method=sparse \
	cksolvedegree=3 \
	camsolve=all \
	update=yes \
	twist=no > /dev/null;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/obsmode_cholmod_bundleout.txt
	cat residuals.csv | sed 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	  > $(OUTPUT)/obsmode_cholmod_residuals.csv
	cat bundleout_images.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  > $(OUTPUT)/obsmode_cholmod_bundleout_images.csv
	$(RM) residuals.csv bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/obsmode_cholmod_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) bundleout.txt > /dev/null;
	cathist from=$(OUTPUT)/PSP_002733_1880_RED4.crop.cub > $(OUTPUT)/PSP4.pvl;
	cathist from=$(OUTPUT)/PSP_002733_1880_RED5.crop.cub > $(OUTPUT)/PSP5.pvl;

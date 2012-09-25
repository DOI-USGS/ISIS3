APPNAME = jigsaw

# This test exercises the bundle adjustment of images from the HiRISE camera onboard MRO.
# oldsparse method with observation mode; solving for angles (3rd degree polynomial).

# The "cat t_bundleout.txt" command in these tests uses sed to do the following (in order):
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
	onet=$(OUTPUT)/case01OutNet.net \
	observations=yes \
	method=oldsparse \
	cksolvedegree=3 \
	camsolve=all \
	update=yes \
	file_pre=t \
	twist=no | grep -v "100% Processed" | \
	grep -v "jigsaw" > case01OutLog.txt;
	cat t_bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/bundleout.txt
	cat case01OutLog.txt \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/case01OutLog.txt
	cat t_residuals.csv | sed 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	  > $(OUTPUT)/residuals.csv
	cat t_bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/bundleout_images.csv
	$(RM) t_bundleout_images.csv > /dev/null;
	$(MV) t_bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) t_bundleout.txt > /dev/null;
	$(RM) case01OutLog.txt > /dev/null;
	$(RM) t_residuals.csv print.prt > /dev/null;
	cathist from=$(OUTPUT)/PSP_002733_1880_RED4.crop.cub > $(OUTPUT)/PSP4.pvl;				    
	cathist from=$(OUTPUT)/PSP_002733_1880_RED5.crop.cub > $(OUTPUT)/PSP5.pvl;

APPNAME = jigsaw
# This test exercises the bundle adjustment of one image each from the MOC-WA camera and Viking Orbiter-1 B Camera.
# oldsparse method with held image option; solving for position.

# The "cat t_bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT) > /dev/null;
	$(LS) -1 $(OUTPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(LS) -1 $(OUTPUT)/e0400710.lev1.cub > $(OUTPUT)/held.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cub.lis  \
	heldlist=$(OUTPUT)/held.lis  \
	cnet=$(INPUT)/mocVik.net \
	onet=$(OUTPUT)/case2OutNet.net \
	maxits=10 \
	method=oldsparse \
	sigma0=1.e-5  \
	update=yes \
	spsolve=position | grep -v "100% Processed" | \
	grep -v "jigsaw" > case2OutLog.txt;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/bundleout.txt
	cat case2OutLog.txt \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/case2OutLog.txt
	cat residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/residuals.csv
	cat bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/bundleout_images.csv
	$(RM) residuals.csv bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cub.lis $(OUTPUT)/held.lis $(OUTPUT)/case2OutNet.bin print.prt > /dev/null;
	$(RM) bundleout.txt case2OutLog.txt print.prt > /dev/null
	cathist from=$(OUTPUT)/vik.cub > $(OUTPUT)/vik.pvl;
	cathist from=$(OUTPUT)/e0400710.lev1.cub > $(OUTPUT)/e0400710.lev1.cub.pvl;

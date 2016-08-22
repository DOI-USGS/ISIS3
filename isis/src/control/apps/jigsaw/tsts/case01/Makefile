APPNAME = jigsaw

# This test exercises the bundle adjustment of images from the HiRISE camera onboard MRO
# with observation mode, solving for angles (3rd degree polynomial).

# The "cat t_bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time
# 5. remove the number of iterations that occurred
#
# 2014-07-23 Jeannie Backer - Changed method from oldsparse to default (sparse).
#                Commented out references to bundleout_images.csv.
#                Removed default parameters.
# 2015-07-30 Jeannie Backer - Changed file_prefix from "t" to "t_" since BundleAdjust
#                no longer adds the underscore. 
# 2016-07-27 Ian Humphrey -  Added the SED to remove iterations. This fixed the issue with macs
#                having slightly different iteration values in jigsaw observation mode tests
# 2016-08-11 Jeannie Backer - Updated documentation

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT) > /dev/null;
	$(LS) -1 $(OUTPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	           cnet=$(INPUT)/redPntreg.net \
	           onet=$(OUTPUT)/case01OutNet.net \
	           observations=yes \
	           update=yes \
	           cksolvedegree=3 \
	           camsolve=all \
	           file_pre=t_ \
	           twist=no > /dev/null;
	$(CAT) t_bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       | $(SED) 's/\(Iterations:\).*/\1/' \
	       > $(OUTPUT)/bundleout.txt
	$(CAT) t_residuals.csv | $(SED) 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	       > $(OUTPUT)/residuals.csv
#	$(CAT) t_bundleout_images.csv \
#	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
#	       | $(SED) 's/\([0-9]*\.[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' \
#	       > $(OUTPUT)/bundleout_images.csv
#	$(RM) t_bundleout_images.csv > /dev/null;
	$(MV) t_bundleout_points.csv $(OUTPUT)/bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) t_bundleout.txt > /dev/null;
	$(RM) t_residuals.csv print.prt > /dev/null;
	cathist from=$(OUTPUT)/PSP_002733_1880_RED4.crop.cub > $(OUTPUT)/PSP4.pvl;				    
	cathist from=$(OUTPUT)/PSP_002733_1880_RED5.crop.cub > $(OUTPUT)/PSP5.pvl;

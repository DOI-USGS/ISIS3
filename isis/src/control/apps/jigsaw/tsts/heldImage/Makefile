# This tests the HELDLIST paramater with a single image. We hold a green-filter image,
# N1597182807_2.filtered.cub, effectively setting any control points that intersect
# this image to FIXED, and setting these control points' a priori surface points to the
# corresponding measures' surface points in the held image. See the history of
# N1597182807_2.filtered.cub, as qtie was used before running the jigsaw.
#
# @history 2016-10-13 Ian Humphrey - Original version, adapted from test data provided by
#                         Tammy Becker. References #4293.
APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*cub > $(OUTPUT)/cubes.lis;
	$(LS) $(INPUT)/N1597182807_2.filtered.cub > $(OUTPUT)/hold-green.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cubes.lis \
	  heldlist=$(OUTPUT)/hold-green.lis \
	  cnet=$(INPUT)/color-tlb.net \
	  onet=$(OUTPUT)/color-tlb-out.net \
	  camera_angles_sigma=10 \
	  > /dev/null;
	$(CAT) bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/hold_bundleout.txt;
	$(CAT) residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/hold_residuals.csv;
	$(CAT) bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > bundleout_images1.csv;  
	$(MV) bundleout_points.csv $(OUTPUT)/hold_bundleout_points.csv;
	$(MV) bundleout_images1.csv $(OUTPUT)/hold_bundleout_images.csv;
	$(RM) bundleout.txt bundleout_images.csv residuals.csv $(OUTPUT)/cubes.lis $(OUTPUT)/hold-green.lis;

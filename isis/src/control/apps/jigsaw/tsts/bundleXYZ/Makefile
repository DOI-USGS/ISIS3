APPNAME = jigsaw
# These tests exercise the bundle adjustment of Apollo images using the bundleXYZ options
# These tests should probably be replaced with Bennu data or other more appropriate data.
# Solving for position, angles (with twist), and radius with error propagation.
#
# 2018-10-12 Debbie A. Cook - original test

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
#  TODO Change truncation to at least 7th decimal place for free tests
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
# test rect bundle with lat output some points constrained
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/rect-latConstrained_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           spsolve=position \
	           spacecraft_position_sigma=1000.0 \
	           camera_angles_sigma=2. \
	           control_point_coordinate_type_bundle=RECT \
	           control_point_coordinate_type_reports=LAT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/rect-latConstrained_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rect-latConstrained_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rect-latConstrained_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/rect-latConstrained_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;

# test rect bundle and rect output with a free network (all points free) - (sigma 0 should match lat bundle with rect output
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_free.net \
	           onet=$(OUTPUT)/rectFree_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           control_point_coordinate_type_bundle=RECT \
	           control_point_coordinate_type_reports=RECT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/rectFree_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rectFree_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rectFree_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/rectFree_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;

# test lat bundle and rect output with a free network (all points free) - (sigma 0 should match rect bundle with rect outpu
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_free.net \
	           onet=$(OUTPUT)/lat-rectFree_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           control_point_coordinate_type_bundle=LAT \
	           control_point_coordinate_type_reports=RECT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/lat-rectFree_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/lat-rectFree_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/lat-rectFree_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/lat-rectFree_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;

# test lat bundle and rect output with some constrained points
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/lat-rectConstrained_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           spsolve=position \
	           spacecraft_position_sigma=1000.0 \
	           camera_angles_sigma=2. \
	           control_point_coordinate_type_bundle=LAT \
	           control_point_coordinate_type_reports=RECT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/lat-rectConstrained_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/lat-rectConstrained_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/lat-rectConstrained_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/lat-rectConstrained_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;

# test rect bundle and rect output with some constrained points
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/rectConstrained_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           spsolve=position \
	           spacecraft_position_sigma=1000.0 \
	           camera_angles_sigma=2. \
	           control_point_coordinate_type_bundle=RECT \
	           control_point_coordinate_type_reports=RECT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/rectConstrained_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rectConstrained_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rectConstrained_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/rectConstrained_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;

# test rect bundle and lat output with a free network (all points free) - (sigma 0 should match lat bundle with lat output
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_free.net \
	           onet=$(OUTPUT)/rect-latFree_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           control_point_coordinate_type_bundle=RECT \
	           control_point_coordinate_type_reports=LAT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/rect-latFree_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rect-latFree_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/rect-latFree_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/rect-latFree_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;
#	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;

# test lat bundle and lat output with a free network (all points free) - (sigma 0 should match lat bundle with lat output
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_free.net \
	           onet=$(OUTPUT)/latFree_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           control_point_coordinate_type_bundle=LAT \
	           control_point_coordinate_type_reports=LAT \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/latFree_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/latFree_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/latFree_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/latFree_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;

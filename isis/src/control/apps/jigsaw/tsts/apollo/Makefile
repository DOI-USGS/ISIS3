APPNAME = jigsaw
# These tests exercise the bundle adjustment of images from the Apollo frame camera.
# Solving for position, angles (with twist), and radius with error propagation.
#
# 2013-04-02 Ken Edmundson - added weights for spacecraft position and angles to test. This reduced 
#                the precision of the adjusted point covariance matrices in the output netfiles.
#                To compensate I reduced the tolerance in the corresponding DIFF files to 0.00003.
#                We should find out why this is happening. Without these weights, a tolerance of
#                0.000000003 was sufficient. 
# 2013-04-02 Debbie Cook - The values in bundleout.txt were working with 5 places across all
#                systems.  With the above change they are only good to 4 places now.  Also added
#                tolerance for AprioriX in the control nets and changed the covariance tolerance 
#                to 0.00006 for the specialK test.
# 2013-11-12 Ken Edmundson - Modified apollo test input Net "Ames_7-ImageLSTest_USGS_combined.net"
#                by changing point AS15_SocetPAN_45 from "Constrained" to "Free" but leaving 
#                a priori constraints on coordinates. This is to test toggling a point between
#                "Constrained" and "Free" while leaving constraints in place.  
# 2014-07-23 Jeannie Backer - Commented out specialk test and references to bundleout_images.csv
# 2015-07-30 Jeannie Backer - Added command to remove inverseMatrix.dat file since error propagation
#                is turned on and we do not have a way to compare this file type.
# 2016-08-11 Jeannie Backer - Removed specialk test since we have removed this solve method from 
#                the bundle adjustment. Updated documentation
# 2016-10-05 Ian Humphrey - Removed no longer relevant RM command for inverseMatrix.dat, as it is
#                no longer generated from jigsaw during error propagation. References #4315.

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	           cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	           onet=$(OUTPUT)/apollo_out.net \
	           radius=yes \
	           errorpropagation=yes \
	           spsolve=position \
	           spacecraft_position_sigma=1000.0 \
	           camera_angles_sigma=2. \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(CAT) $(OUTPUT)/bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       | $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	       | $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	       > $(OUTPUT)/apollo_bundleout.txt;
	$(CAT) $(OUTPUT)/residuals.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/apollo_residuals.csv;
	$(CAT) $(OUTPUT)/bundleout_images.csv \
	       | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	       > $(OUTPUT)/apollo_bundleout_images.csv;
	$(RM) $(OUTPUT)/bundleout_images.csv > /dev/null;
	$(MV) $(OUTPUT)/bundleout_points.csv $(OUTPUT)/apollo_bundleout_points.csv > /dev/null;
	$(RM) $(OUTPUT)/bundleout.txt print.prt > /dev/null;
	$(RM) $(OUTPUT)/residuals.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;

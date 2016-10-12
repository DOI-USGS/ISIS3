APPNAME = jigsaw
# This tests the function which outputs bundleout_images.csv.  It
#
# # 2016-10-11 Tyler Wilson - Original version. Fixes #4314.

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
			   bundleout_txt=no \
			   output_csv=no \
			   residuals_csv=no \
	           file_prefix=$(OUTPUT)/ > /dev/null;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	$(RM) $(OUTPUT)/*.net > /dev/null;

APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

# Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test TGO CaSSIS
	$(APPNAME) from=$(INPUT)/stitched-spice-2016-11-26T22-50-27.381_crop.cub \
	  to=$(OUTPUT)/CaSSIS_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/CaSSIS_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_CaSSIS_settings.txt
	$(RM) $(OUTPUT)/CaSSIS_settings.txt

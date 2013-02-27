APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test Galileo SSI, lens cover off
	$(APPNAME) from=$(INPUT)/6478r_lens_cover_off.cub \
	  to=$(OUTPUT)/6478r_lens_cover_off_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/6478r_lens_cover_off_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_6478r_lens_cover_off_settings.txt
	$(RM) $(OUTPUT)/6478r_lens_cover_off_settings.txt

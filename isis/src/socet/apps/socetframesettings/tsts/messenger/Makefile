APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test Messenger NAC, no summation
	$(APPNAME) from=$(INPUT)/EN0220460248M.lev1.cub \
	  to=$(OUTPUT)/EN0220460248M_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/EN0220460248M_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_EN0220460248M_settings.txt
	$(RM) $(OUTPUT)/EN0220460248M_settings.txt

#Test Messenger WAC, no summation
	$(APPNAME) from=$(INPUT)/EW0250939219G.lev1.cub \
	  to=$(OUTPUT)/EW0250939219G_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/EW0250939219G_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_EW0250939219G_settings.txt
	$(RM) $(OUTPUT)/EW0250939219G_settings.txt

#Test Messenger WAC, summation = 2
	$(APPNAME) from=$(INPUT)/EW0252783389G.summed.lev1.cub \
	  to=$(OUTPUT)/EW0252783389G_summed_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/EW0252783389G_summed_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_EW0252783389G_summed_settings.txt
	$(RM) $(OUTPUT)/EW0252783389G_summed_settings.txt


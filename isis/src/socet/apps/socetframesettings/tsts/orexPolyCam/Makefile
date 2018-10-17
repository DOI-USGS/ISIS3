APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test Celemintine, UVVIS
	$(APPNAME) from=$(INPUT)/20190329T183101S0400_pol_iofL2pan.cub \
	  to=$(OUTPUT)/20190329T183101S0400_pol_iofL2pan.set \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/20190329T183101S0400_pol_iofL2pan.set | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_20190329T183101S0400_pol_iofL2pan_settings.txt
	$(RM) $(OUTPUT)/20190329T183101S0400_pol_iofL2pan.set

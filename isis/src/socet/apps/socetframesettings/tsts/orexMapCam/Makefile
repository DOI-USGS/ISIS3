APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test OSIRIS-REx, MapCam
	$(APPNAME) from=$(INPUT)/20190223T023051S790_map_iofL2pan.cub \
	  to=$(OUTPUT)/20190223T023051S790_map_iofL2pan.set \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/20190223T023051S790_map_iofL2pan.set | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_20190223T023051S790_map_iofL2pan_settings.txt
	$(RM) $(OUTPUT)/20190223T023051S790_map_iofL2pan.set

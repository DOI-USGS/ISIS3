APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

# The sed removes the [<path>/<cube>] string from output error messages
commands:
	$(APPNAME) from=$(INPUT)/N1612266665_1.noblobs.cub \
	  to=$(OUTPUT)/N1612266665_1.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ >& \
	  $(OUTPUT)/error.txt || true;
	cat $(OUTPUT)/error.txt | \
	 sed 's/\[\([^"]*\)\]//g' > $(OUTPUT)/sed_error.txt
	$(RM) $(OUTPUT)/error.txt


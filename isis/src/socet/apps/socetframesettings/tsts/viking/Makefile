APPNAME = socetframesettings

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test Viking 1A, noproj'ed image
	$(APPNAME) from=$(INPUT)/f519a03.noproj.8bit.cub \
	  to=$(OUTPUT)/f519a03_noproj_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/f519a03_noproj_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_f519a03_noproj_settings.txt
	$(RM) $(OUTPUT)/f519a03_noproj_settings.txt

#Test Viking 1B, noproj'ed image
	$(APPNAME) from=$(INPUT)/f519a04.noproj.8bit.cub \
	  to=$(OUTPUT)/f519a04_noproj_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/f519a04_noproj_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_f519a04_noproj_settings.txt
	$(RM) $(OUTPUT)/f519a04_noproj_settings.txt

#Test Viking 2A, noproj'ed image
	$(APPNAME) from=$(INPUT)/f046b09.noproj.8bit.cub \
	  to=$(OUTPUT)/f046b09_noproj_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/f046b09_noproj_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_f046b09_noproj_settings.txt
	$(RM) $(OUTPUT)/f046b09_noproj_settings.txt

#Test Viking 2B, noproj'ed image
	$(APPNAME) from=$(INPUT)/f046b08.noproj.8bit.cub \
	  to=$(OUTPUT)/f046b08_noproj_settings.txt \
	  ss_project=D:\\data\\TEST \
	  ss_img_loc=TEST \
	  ss_input_path=E:\\IMAGES\\TEST\\ \
	  ss_cam_calib_path=C:\\SOCET_SET_5.6.0\\internal_dbs\\CAM\\ > /dev/null;
	cat $(OUTPUT)/f046b08_noproj_settings.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_f046b08_noproj_settings.txt
	$(RM) $(OUTPUT)/f046b08_noproj_settings.txt


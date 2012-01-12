APPNAME = socetnet2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MRF_NAC_xyz_noZadj_Yvel20_4ISIS.atf \
	to=$(OUTPUT)/jcrater.net \
	sigmas=override \
	point_type=all \
	slat=2 slon=2 srad=0.1 \
	editlock=measures \
	point_id_prefix=SS_ > /dev/null;

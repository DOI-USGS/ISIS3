APPNAME = soc2isisnet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MRF_NAC_xyz_noZadj_Yvel20_4ISIS.atf \
	to=$(OUTPUT)/jcrater.net \
	measurement=adjusted \
	sigmas=override \
	point_type=all \
	slat=2 slon=2 srad=0.1 \
	editlock=points \
	point_id_prefix=SS_ > /dev/null;

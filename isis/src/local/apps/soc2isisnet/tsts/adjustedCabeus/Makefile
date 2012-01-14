APPNAME = soc2isisnet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/absolute.atf \
	to=$(OUTPUT)/truth.net \
	measurements=adjusted \
	point_id_prefix=SS_ > /dev/null;
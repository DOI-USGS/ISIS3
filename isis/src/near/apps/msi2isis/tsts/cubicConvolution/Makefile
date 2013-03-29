APPNAME = msi2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/m0126865998f4_2p_iof.lbl \
	to=$(OUTPUT)/truth.cub > /dev/null;
	catlab from=$(OUTPUT)/truth.cub to=$(OUTPUT)/truth.pvl > /dev/null;

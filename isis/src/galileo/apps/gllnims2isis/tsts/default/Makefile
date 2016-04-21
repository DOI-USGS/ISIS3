APPNAME = gllnims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/25i005tr.qub core=$(OUTPUT)/25i005tr_core.cub suffix=$(OUTPUT)/25i005tr_suffix.cub > /dev/null;
	catlab from=$(OUTPUT)/25i005tr_core.cub to=$(OUTPUT)/25i005tr_core.pvl > /dev/null;
	catlab from=$(OUTPUT)/25i005tr_suffix.cub to=$(OUTPUT)/25i005tr_suffix.pvl > /dev/null;

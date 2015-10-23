APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	# default to dark=model, keepdark=no, flat=yes, radiometric=yes, ecfactor=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	to=$(OUTPUT)/default.cub \
	iof=no > /dev/null;
	catlab \
	FROM=$(OUTPUT)/default.cub \
	to=$(OUTPUT)/default_label.pvl > /dev/null;
	# default to keepdark=no, flat=yes, radiometric=yes, ecfactor=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	to=$(OUTPUT)/nodark.cub \
	DARKCURRENT=none \
	iof=no > /dev/null;
	catlab \
	FROM=$(OUTPUT)/nodark.cub \
	to=$(OUTPUT)/nodark_label.pvl > /dev/null;
	# default to keepdark=no, flat=yes, radiometric=yes, ecfactor=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	to=$(OUTPUT)/lineardark.cub \
	DARKCURRENT=linear \
	iof=no > /dev/null;
	catlab \
	FROM=$(OUTPUT)/lineardark.cub \
	to=$(OUTPUT)/lineardark_label.pvl > /dev/null;
	# default to keepdark=no, flat=yes, radiometric=yes, ecfactor=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	to=$(OUTPUT)/standarddark.cub \
	DARKCURRENT=standard \
	iof=no > /dev/null;
	catlab \
	FROM=$(OUTPUT)/standarddark.cub \
	to=$(OUTPUT)/standarddark_label.pvl > /dev/null;
	# default to dark=model, flat=yes, radiometric=yes, ecfactor=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	KEEPDARK=true \
	to=$(OUTPUT)/keepdark.cub \
	iof=no > /dev/null;
	catlab \
	FROM=$(OUTPUT)/keepdark.cub \
	to=$(OUTPUT)/keepdark_label.pvl > /dev/null;
	# default to dark=model, keepdark=no, flat=yes, radiometric=yes, ecfactor=yes, iof=yes
	$(APPNAME) \
	FROM=$(INPUT)/EW0089565626A.cub \
	to=$(OUTPUT)/iof.cub > /dev/null;
	catlab \
	FROM=$(OUTPUT)/iof.cub \
	to=$(OUTPUT)/iof_label.pvl > /dev/null;

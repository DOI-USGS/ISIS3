APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub to=$(OUTPUT)/default.cub iof=no > /dev/null;
	catlab FROM=$(OUTPUT)/default.cub > $(OUTPUT)/default_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub to=$(OUTPUT)/nodark.cub DARKCURRENT=none iof=no > /dev/null;
	catlab FROM=$(OUTPUT)/nodark.cub > $(OUTPUT)/nodark_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub to=$(OUTPUT)/lineardark.cub DARKCURRENT=linear iof=no > /dev/null;
	catlab FROM=$(OUTPUT)/lineardark.cub > $(OUTPUT)/lineardark_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub to=$(OUTPUT)/standarddark.cub DARKCURRENT=standard iof=no > /dev/null;
	catlab FROM=$(OUTPUT)/standarddark.cub > $(OUTPUT)/standarddark_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub KEEPDARK=true to=$(OUTPUT)/keepdark.cub iof=no > /dev/null;
	catlab FROM=$(OUTPUT)/keepdark.cub > $(OUTPUT)/keepdark_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0089565626A.cub to=$(OUTPUT)/iof.cub > /dev/null;
	catlab FROM=$(OUTPUT)/iof.cub > $(OUTPUT)/iof_label.pvl;


APPNAME = crop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) PROPSPICE=false FROM=$(INPUT)/h1235_0001_bl2.cub TO=$(OUTPUT)/h1235_0001_bl2.cub > /dev/null;
	catlab FROM=$(OUTPUT)/h1235_0001_bl2.cub > $(OUTPUT)/nospice.pvl;
	$(APPNAME) FROM=$(INPUT)/h1235_0001_bl2.cub TO=$(OUTPUT)/h1235_0001_bl2.cub  \
	  PROPSPICE=TRUE > /dev/null;
	catlab FROM=$(OUTPUT)/h1235_0001_bl2.cub > $(OUTPUT)/spice.pvl;
	$(RM) $(OUTPUT)/h1235_0001_bl2.cub;
	$(APPNAME) FROM=$(INPUT)/PSP_001365_2720_RED4_0.cub LINE=500 NLINES=30 \
	TO=$(OUTPUT)/hirise.cub LINC=3 > /dev/null;
	tabledump FROM=$(OUTPUT)/hirise.cub NAME="HiRISE Ancillary" \
	to=$(OUTPUT)/lineAssocTbl.txt > /dev/null;
	$(RM) $(OUTPUT)/hirise.cub;

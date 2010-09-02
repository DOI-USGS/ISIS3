APPNAME = voycal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/c1639118.imq.cub \
	  TO=$(OUTPUT)/noCal.cub LINEAR=FALSE > /dev/null;
	catlab FROM=$(OUTPUT)/noCal.cub \
	  TO=$(OUTPUT)/noCal.lbl.pvl > /dev/null;

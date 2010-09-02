APPNAME = voycal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/c1639118.imq.cub \
	  TO=$(OUTPUT)/linearCal.cub LINEAR=TRUE  > /dev/null;
	catlab FROM=$(OUTPUT)/linearCal.cub \
	  TO=$(OUTPUT)/linearCal.lbl.pvl > /dev/null;

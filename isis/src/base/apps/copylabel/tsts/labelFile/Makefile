APPNAME = copylabel

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/stripped.cub $(OUTPUT)
	$(APPNAME) FROM=$(OUTPUT)/stripped.cub \
  	  SOURCE=$(INPUT)/justLabels.pvl > /dev/null;
	catlab FROM=$(OUTPUT)/stripped.cub \
	  TO=$(OUTPUT)/stripped.pvl > /dev/null;	


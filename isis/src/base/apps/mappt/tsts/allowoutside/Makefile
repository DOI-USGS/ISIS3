APPNAME = mappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mola_clon0.cub \
	  to=$(OUTPUT)/mappt.pvl \
	  append=false \
          type=ground \
	  latitude=17 \
          longitude=10 \
          > /dev/null;

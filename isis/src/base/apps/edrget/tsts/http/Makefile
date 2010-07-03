APPNAME = edrget

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) URL=http://pds-imaging.jpl.nasa.gov/data/mro/mars_reconnaissance_orbiter/marci/mrom_0209/calib/vis1flat.ddd TOPATH=$(OUTPUT) > /dev/null;
	ddd2isis from=$(OUTPUT)/vis1flat.ddd to=$(OUTPUT)/vis1flat.cub > /dev/null;
	$(RM) $(OUTPUT)/vis1flat.ddd > /dev/null;

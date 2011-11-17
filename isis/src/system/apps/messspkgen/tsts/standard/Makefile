APPNAME = messspkgen

include $(ISISROOT)/make/isismake.tsts

CUTOFF = `commnt -r $(INPUT)/msgr_20040803_20120401_od233sc_0.bsp \
				 | grep -A 1 "Orbit" | sed -n "2p"`

commands:
	$(APPNAME) from=$(INPUT)/kernels.0063.db to=$(OUTPUT)/kernels.0063.pvl \
	time="$(CUTOFF)" \
	> /dev/null;

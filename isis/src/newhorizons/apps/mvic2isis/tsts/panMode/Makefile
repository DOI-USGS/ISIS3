APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mp1_0042515645_0x530_sci_1.fit \
          to=$(OUTPUT)/mp1_0042515645_0x530_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mp1_0042515645_0x530_sci_1_Truth.cub > \
          $(OUTPUT)/mp1_0042515645_0x530_sci_1_Truth.pvl;


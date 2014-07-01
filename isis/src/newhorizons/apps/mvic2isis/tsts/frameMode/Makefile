APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mpf_0035126517_0x539_sci_1.fit \
          to=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.cub > \
          $(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.pvl;


APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mpf_0035126517_0x539_sci_1.fit \
          to=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.cub > \
          $(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/mpf_0035126517_0x539_sci_1.fit \
	  to=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth2.cub \
	  undistorted=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Undistorted \
	  error=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Error \
	  quality=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Quality \
	  > /dev/null;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth2.cub > \
	  $(OUTPUT)/mpf_0035126517_0x539_sci_1_Truth2.pvl;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Undistorted.cub > \
	  $(OUTPUT)/mpf_0035126517_0x539_sci_1_Undistorted.pvl;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Error.cub > \
	  $(OUTPUT)/mpf_0035126517_0x539_sci_1_Error.pvl;
	catlab from=$(OUTPUT)/mpf_0035126517_0x539_sci_1_Quality.cub > \
	  $(OUTPUT)/mpf_0035126517_0x539_sci_1_Quality.pvl;


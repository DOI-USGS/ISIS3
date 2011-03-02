APPNAME = enlarge

peaks2.cub.TOLERANCE = 16

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/peaks.cub \
	  to=$(OUTPUT)/peaks.cub \
	  sscale=1.0 \
	  lscale=1.5 \
	  interp=bilinear > /dev/null;
	lowpass from=$(OUTPUT)/peaks.cub samples=1 lines=5 \
	  to=$(OUTPUT)/peaks2.cub > /dev/null;
	$(RM) $(OUTPUT)/peaks.cub

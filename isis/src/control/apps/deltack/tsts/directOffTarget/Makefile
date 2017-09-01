APPNAME  = deltack

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/2778_2017Jul30.cub $(OUTPUT);
	$(APPNAME) from=$(OUTPUT)/2778_2017Jul30.cub \
	  method=direct                   \
	  samp1=647.0                     \
	  line1=251.0                     \
	  lat1=0.039005173732125          \
	  lon1=77.187005417140938 > /dev/null;         
	cathist from=$(OUTPUT)/2778_2017Jul30.cub > $(OUTPUT)/pvlTruth.pvl;
	campt from=$(OUTPUT)/2778_2017Jul30.cub \
	  sample=647.0 line=251 \
          to=$(OUTPUT)/adjusted_campt.pvl > /dev/null;
	$(RM) $(OUTPUT)/2778_2017Jul30.cub;

	  

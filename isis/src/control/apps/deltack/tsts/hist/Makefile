APPNAME  = deltack

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT);              \
	$(APPNAME) from=$(OUTPUT)/vik.cub            \
	  twist=false                                \
	  maxit=10                                   \
	  samp1=731.875                              \
	  line1=669.375                              \
	  lat1=-26.439008370134                      \
	  lon1= 352.19745020451 > /dev/null;         \
	cathist from=$(OUTPUT)/vik.cub > $(OUTPUT)/pvlTruth.pvl;
	$(RM) bundleout.txt > /dev/null; 
	$(RM) residuals.csv > /dev/null; 
	$(RM) $(OUTPUT)/vik.cub;

	  

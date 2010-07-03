APPNAME = kuwahara

include $(ISISROOT)/make/isismake.tsts

#Separate ostruthdata for samson needed due to rounding errors
#The rounding errors are in variances which are equal, which can
#then lead to significantly different DNs. Read code for further detail 

commands:
	$(APPNAME) from=$(INPUT)/peaks.cub \
	to=$(OUTPUT)/filtered.cub > /dev/null;

APPNAME = enlarge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/total.cub \
	  mode=total \
	  ons=126 \
	  onl=189 \
	  interp=nearestneighbor > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/scale.cub \
	  sscale=1.0 \
	  lscale=1.5 \
	  interp=nearestneighbor > /dev/null;
	cubediff from=$(OUTPUT)/total.cub \
	  from2=$(OUTPUT)/scale.cub \
	  to=$(OUTPUT)/comparisonTruth.txt > /dev/null;
	rm -f $(OUTPUT)/total.cub;
	rm -f $(OUTPUT)/scale.cub;

APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/large.lbl \
	  to=$(OUTPUT)/isis2stdTruth1.png \
	  mode=grayscale \
	  format=png \
	  stretch=linear \
	   >& $(OUTPUT)/error.txt` ]; \
	then \
	  true; \
	fi;

APPNAME = isis2std

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/large.lbl \
	  to=$(OUTPUT)/isis2stdTruth1.png \
	  mode=grayscale \
	  format=png \
	  stretch=linear \
	  -preference=$(ISISROOT)/src/base/objs/Preference/TestPreferences \
	   >& $(OUTPUT)/error.txt` ]; \
	then \
	  true; \
	fi; \


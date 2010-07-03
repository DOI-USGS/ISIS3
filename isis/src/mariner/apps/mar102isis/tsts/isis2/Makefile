APPNAME = mar102isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/0027438.cub to=$(OUTPUT)/0027438_isis3.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/0027438_isis3.cub to=$(OUTPUT)/0027438_isis3.pvl \
	> /dev/null;
	rm $(OUTPUT)/0027438_isis3.cub;

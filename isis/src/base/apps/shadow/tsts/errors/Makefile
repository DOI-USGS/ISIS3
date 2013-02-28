APPNAME = shadow

include $(ISISROOT)/make/isismake.tsts

# The seds erase cube names from []'s in the output error messages
commands:
	$(APPNAME) from=$(INPUT)/ab102401.dem.cub match=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/error1.cub 2>&1 | sed 's/\[.*cub/[/g' > $(OUTPUT)/error1.txt || true;
	$(APPNAME) from=$(INPUT)/ab102401.almostdem.cub match=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/error1.cub 2>&1 | sed 's/\[.*cub/[/g' > $(OUTPUT)/error2.txt || true;
	$(APPNAME) from=$(INPUT)/isisTruth.cub match=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/error2.cub 2>&1 | sed 's/\[.*cub/[/g' > $(OUTPUT)/error3.txt || true;

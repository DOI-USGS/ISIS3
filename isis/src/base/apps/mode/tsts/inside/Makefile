APPNAME = mode

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/modeTruth2.cub \
	  samples=13 \
	  lines=13 \
	  pixels=INSIDE \
	  low=60 \
	  high=150 \
	  minimum=1 > /dev/null;

APPNAME = grid

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth1.cub \
	  mode=ground linewidth=3 > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth2.cub \
	  mode=ground ticks=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth3.cub \
	  mode=ground ticks=true diagonal=true \
	  linewidth=3 ticksize=7 > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.map.cub \
	  to=$(OUTPUT)/groundTruth4.cub \
	  mode=ground ticks=true diagonal=false \
	  linewidth=5 ticksize=11 > /dev/null;

APPNAME = mocproc

mocprocTruth3.cub.TOLERANCE = 0.0009

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102402.img ingestion=true calibration=false mapping=false \
	  to=$(OUTPUT)/mocprocTruth1.cub > /dev/null;

	$(APPNAME) from=$(OUTPUT)/mocprocTruth1.cub ingestion=false calibration=true mapping=false \
	  to=$(OUTPUT)/mocprocTruth2.cub > /dev/null;

	$(APPNAME) from=$(OUTPUT)/mocprocTruth2.cub ingestion=false calibration=false mapping=true \
	  to=$(OUTPUT)/mocprocTruth3.cub > /dev/null;

APPNAME = spicefit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/moc2isisTruth1.cub $(OUTPUT)/moc2isisTruth.cub;
	$(APPNAME) FROM= $(OUTPUT)/moc2isisTruth.cub > /dev/null;
	tabledump FROM= $(OUTPUT)/moc2isisTruth.cub \
	TO= $(OUTPUT)/instramentpointing.txt NAME=InstrumentPointing > /dev/null;
	tabledump FROM= $(OUTPUT)/moc2isisTruth.cub \
	TO= $(OUTPUT)/instramentposition.txt NAME=InstrumentPosition > /dev/null;
	tabledump FROM= $(OUTPUT)/moc2isisTruth.cub \
	TO= $(OUTPUT)/botdrotation.txt NAME=BodyRotation > /dev/null;
	tabledump FROM= $(OUTPUT)/moc2isisTruth.cub \
	TO= $(OUTPUT)/sunposition.txt NAME=SunPosition > /dev/null;
	$(RM) $(OUTPUT)/moc2isisTruth.cub;

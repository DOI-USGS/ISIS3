APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) algospecfile=$(INPUT)/RepeatedAlgo.list \
	           match=$(INPUT)/EW0211981114G.lev1.cub  \
	           from=$(INPUT)/EW0242463603G.lev1.cub \
	           epitolerance=1.0 \
	           ratio=0.650 \
	           hmgtolerance=1.0\
	           networkid="EW0211981114G_EW0242463603G" \
	           pointid="EW0211981114G_?????" \
	           onet=$(OUTPUT)/EW0211981114G.net \
	           description="Test MESSENGER pair" \
	           debug=true \
	           debuglog=$(OUTPUT)/tempDebug.log \
	           >& /dev/null;   \
	grep "Total Query keypoints" $(OUTPUT)/tempDebug.log \
	           > $(OUTPUT)/keypoints.txt; \
	$(RM) $(OUTPUT)/tempDebug.log;


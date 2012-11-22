# This test will print errors thrown by the hideal2pds application.
#
#   TEST A: 
#     if (AlphaCube group: beta lines != alphastart - alphaend)
#        This happens when the reduce app modifies lines
#   TEST B: 
#     if (AlphaCube group: beta samp != alphastart - alphaend)
#        This happens when the reduce app modifies samples
#   TEST C: 
#     if (AlphaCube group: alpha lines > beta lines)
#        This happens when the enlarge app modifies lines
#   TEST D: 
#     if (AlphaCube group: alpha samps > beta samps)
#        This happens when the enlarge app modifies samples
#   TEST E: 
#         "Unsupported target."
#   TEST F: 
#         "Input cube must be IdealCamera."
#   TEST G: 
#         "Input cube must be from a HiRISE image. Unsupported instrument."
#   TEST H: 
#     if (no rationale desc given and none in original instrument)
#   TEST I: 
#     if (no rationale desc given and original instrument rationale desc is null)
.IGNORE:
APPNAME = hideal2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/input.cub $(OUTPUT)/input.cub;
#   TEST A: edit label to simulate using reduce program on number of lines
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=AlphaCube key=BetaLines val=9 > /dev/null;
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST B: edit label to simulate using reduce program on number of samples
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=AlphaCube key=BetaSamples val=9 > /dev/null;
	echo -e "\nError Test B:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST C: edit label to simulate using enlarge program on number of lines
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=AlphaCube key=BetaLines val=11 > /dev/null;
	echo -e "\nError Test C:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST D: use edit lab to simulate using enlarge program on number of samples
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=AlphaCube key=BetaSamples val=20001 > /dev/null;
	echo -e "\nError Test D:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST E: target is not mars
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=Instrument key=TargetName val=Invalid > /dev/null;
	echo -e "\nError Test E:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST F: instrument id is not ideal
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=Instrument key=InstrumentId val=HIRISE > /dev/null;
	echo -e "\nError Test F:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST G: Original instrument is not hirise
	editlab from=$(OUTPUT)/input.cub \
	  op=modkey grp=OriginalInstrument key=InstrumentId val=Invalid > /dev/null;
	echo -e "\nError Test G:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	  to=$(OUTPUT)/output.img 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST H: 
#   TEST I:

	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error_temp.txt;
	rm -f $(OUTPUT)/input.cub;

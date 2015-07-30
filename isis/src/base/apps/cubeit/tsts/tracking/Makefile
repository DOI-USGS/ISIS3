#tests to see that tracking bands are being stripped correctly by cubeit
APPNAME = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
# Test without input attributes
	touch $(OUTPUT)/input.lis;  
	ls $(INPUT)/*.cub >> $(OUTPUT)/input.lis; \
	$(APPNAME) fromlist=$(OUTPUT)/input.lis	\
	to=$(OUTPUT)/trackingOutput.cub > /dev/null;
	catlab from=$(OUTPUT)/trackingOutput.cub	\
	to=$(OUTPUT)/labels.pvl > /dev/null; 
	$(RM) $(OUTPUT)/input.lis;  

#Test with input attributes
	cp $(INPUT)/trackingAndVirtual.lis $(OUTPUT);
	cp $(INPUT)/lroMosaicBeneath.cub .;
	ls $(INPUT)/*.cub >> $(OUTPUT)/trackingAndVirtual.lis; 
	$(APPNAME) fromlist=$(OUTPUT)/trackingAndVirtual.lis \
	to=$(OUTPUT)/trackingAndVirtual.cub > /dev/null; 
	catlab from=$(OUTPUT)/trackingAndVirtual.cub \
	to=$(OUTPUT)/trackingAndVirtualLabels.pvl > /dev/null;

# Cleanup
	$(RM) ./lroMosaicBeneath.cub; 
	$(RM) $(OUTPUT)/trackingAndVirtual.lis; 

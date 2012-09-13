include $(ISISROOT)/make/isismake.tsts

.ONESHELL:

SQUISHBIN=/opt/Squish/bin

commands:
	if [ -f "$$HOME/.squish/ver1/server.ini" ]; then
	  $(RM) "$$HOME/.squish/ver1/server.ini"
	fi
	$(SQUISHBIN)/squishserver >& /dev/null & 
	$(SQUISHBIN)/squishserver --config addAppPath '$(ISISROOT)/bin' >& /dev/null
	$(SQUISHBIN)/squishrunner --config setCursorAnimation off >& /dev/null
	cd $(OUTPUT) >& /dev/null
	ABSOUTPUT=`pwd`
	cd - >& /dev/null
	DELETEINPUT=false
	if [ ! -d input ]; then
	  ln -s "$(INPUT)"
	  DELETEINPUT=true
	fi
	for SUITE in `ls -1 suites/`; do
	  if [ -d "suites/$$SUITE" ] && [ -f "suites/$$SUITE/suite.conf" ]; then
	    touch $$ABSOUTPUT/$$SUITE.xml
	    $(SQUISHBIN)/squishrunner --testsuite "suites/$$SUITE" \
	      --reportgen xml2.1,$$ABSOUTPUT/$$SUITE.xml 2>&1 >& /dev/null
	    if [ -f "$$ABSOUTPUT/$$SUITE.xml" ]; then
	      $(CAT) $$ABSOUTPUT/$$SUITE.xml | sed 's/time="[^"]*"/time="..."/g' | \
	        sed 's/file="[^"]*"/file="..."/g' > $$ABSOUTPUT/$$SUITE.tmp.xml
	      $(MV) $$ABSOUTPUT/$$SUITE.tmp.xml $$ABSOUTPUT/$$SUITE.xml
	      $(MV) $$ABSOUTPUT/$$SUITE.xml $$ABSOUTPUT/$$SUITE.xml.txt
	    else
	      echo "File $$ABSOUTPUT/$$SUITE.xml not created by squishrunner"
	    fi
	  fi
	done
	$(SQUISHBIN)/squishserver --stop >& /dev/null
	if [ -L input ]; then
	  $(RM) input
	fi

configsquish:
	if [ ! -d input ]; then \
	  ln -s "$(INPUT)";     \
	fi                      
	$(SQUISHBIN)/squishserver >& /dev/null & 
	$(SQUISHBIN)/squishserver --config addAppPath '$(ISISROOT)/bin' >& /dev/null
	$(SQUISHBIN)/squishrunner --config setCursorAnimation off >& /dev/null
	$(SQUISHBIN)/squishserver --stop >& /dev/null

suite_%: configsquish
	$(SQUISHBIN)/squishserver >& /dev/null & 
	
	if [ ! -d output ]; then
	  $(MKDIR) output;
	fi
	
	cd $(OUTPUT) >& /dev/null
	ABSOUTPUT=`pwd`
	cd - >& /dev/null
	
	if [ -d "suites/$@" ] && [ -f "suites/$@/suite.conf" ]; then
	  touch $$ABSOUTPUT/$@.xml
	  $(SQUISHBIN)/squishrunner --testsuite "suites/$@" \
	    --reportgen xml2.1,$$ABSOUTPUT/$@.xml 2>&1 >& /dev/null
	  if [ -f "$$ABSOUTPUT/$@.xml" ]; then
	    $(CAT) $$ABSOUTPUT/$@.xml | sed 's/time="[^"]*"/time="..."/g' | \
	      sed 's/file="[^"]*"/file="..."/g' > $$ABSOUTPUT/$@.tmp.xml
	    $(MV) $$ABSOUTPUT/$@.tmp.xml $$ABSOUTPUT/$@.xml
	    $(MV) $$ABSOUTPUT/$@.xml $$ABSOUTPUT/$@.xml.txt
	    grep -Hn -A 1 '<result type="FAIL" time="...">' $(OUTPUT)/$@.xml.txt 2>>/dev/null
	    grep -Hn -A 2 '<message .*>' $(OUTPUT)/$@.xml.txt 2>>/dev/null
	  else
	    echo "File $$ABSOUTPUT/$@.xml not created by squishrunner"
	  fi
	fi
	$(SQUISHBIN)/squishserver --stop >& /dev/null
	if [ -L input ]; then
	  $(RM) input
	fi


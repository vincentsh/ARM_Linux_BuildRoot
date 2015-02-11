
all: test_script

test_script:
	@echo $(SHELL)
	@echo "#!/bin/sh\necho \"This is a test app (script)\"" > $@
	@chmod -c 1777 $@

clean:
	@rm -rf test_script
	

all:
	##########		src			##########
	+ $(MAKE) -C src

	##########		module	##########
	+ $(MAKE) -C module

clean: 
	##########		src			##########
	+ $(MAKE) clean -C src

	##########		module	##########
	+ $(MAKE) clean -C module

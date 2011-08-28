SUBDIRS= front_end back_end

all:
	wdir=`pwd`;\
	list='$(SUBDIRS)'; \
	for subdir in $$list; do echo $$subdir; cd $$subdir; $(MAKE) || exit 1; cd $$wdir; done;

clean:
	wdir=`pwd`;\
	list='$(SUBDIRS)'; \
	for subdir in $$list; do echo $$subdir; cd $$subdir; $(MAKE) clean || exit 1; cd $$wdir; done;


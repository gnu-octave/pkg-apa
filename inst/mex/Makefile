# Source code formatting

UNCRUSTIFY_FILES = $(wildcard *.c) $(wildcard *.h)

format:
	for i in ${UNCRUSTIFY_FILES}; do \
	  uncrustify -c uncrustify.cfg --no-backup $$i; \
	done

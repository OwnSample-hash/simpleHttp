M4_FILE := template.m4
DEF_SRC := $(shell ls defs/*.def)
HEADERS := $(DEF_SRC:defs/%.def=%.h)

M4 := m4

M4_U = ${shell echo ${M4} | sed 's/.*/\U&/'}

.PHONY: all clean
all: $(HEADERS)
	@printf "  Done generating headers.\n"

%.h: defs/%.def
	@${M4} -DHEADER_GUARD=__HTTP_$*_H__ \
		-DDESC="$(shell cat $<)" \
		-DSC=$* \
		-DSR_SC=SR_"$*(res, cfd)" \
		-DSC_payload=SR_$*_payload \
		-DHTTP_SC_PAGE=HTTP_$*_page \
		$(M4_FILE) > $@
	@printf "  %s %s\n" "$(M4_U)" "$@"

clean:
	@printf "  %-9s %s\n" "RM" "$(HEADERS)"
	@rm *.h

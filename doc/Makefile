BIBTEX_SOURCES =

FIG_FILES = \

PSTEXT_FILES = $(FIG_FILES:.fig=.pstex_t)

all: $(PSTEXT_FILES) main.pdf

main.pdf: main.tex $(BIBTEX_SOURCES)

.PHONY: clean

clean:
	@echo "Warning: This target blows away all postscript files"
	rm -f *.dvi *.lot *.lof *.blg *.log *.aux *.fff *.bbl *.bak *.ttt *.toc
	rm -f *.idx *.ind *.ilg *.alg *.spl
	rm -f *.nca *.out
	rm -f main.ps main.pdf

.PHONY: veryclean

veryclean: clean
	rm -f *.pstex_t *.pstex

################################################################################
# Do not modify anything below this line.
################################################################################

# Note: The default paper size for dvips appears to be A4 on Red Hat Linux.
#DVIPS = dvips -t letter -f -Ppdf -G0
DVIPS = dvips -t letter -f -Ppdf
PS2PDF = ps2pdf
LATEX = latex
BIBTEX = bibtex
MAKEINDEX = makeindex
MAKEINDEXFLAGS = -s myhead.ist

.ps.pdf:
	$(PS2PDF) $< > $@

.tex.dvi:
	$(LATEX) $*
	-$(BIBTEX) $*
	-$(MAKEINDEX) $(MAKEINDEXFLAGS) $*
	for i in 1 2 3; do \
		$(LATEX) $* ;\
	done
	-$(BIBTEX) $*
	-$(MAKEINDEX) $(MAKEINDEXFLAGS) $*
	@echo "************************************************************"
	$(LATEX) $*
	@echo "************************************************************"

.dvi.ps:
	$(DVIPS) -o $@ $<

.fig.pstex_t:
	fig2dev -L pstex -b 0 -n $< $< > $*.pstex
	fig2dev -L pstex_t -E 1 -p $*.pstex $< > $*.pstex_t

.SUFFIXES: .tex .dvi .pdf .ps .fig .pstex_t


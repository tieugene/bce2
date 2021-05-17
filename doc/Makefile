DST = bce2.1 btcbklocs.1 btclocview.1 bce2.cfg.5

.PHONY: all clean

all: $(DST)

%.1: %.1.adoc
	asciidoctor -b manpage $<

%.5: %.5.adoc
	asciidoctor -b manpage $<

clean:
	rm *.1 *.5

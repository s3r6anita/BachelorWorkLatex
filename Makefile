.DEFAULT_GOAL := all

build:
	pdflatex -synctex=1 -interaction=nonstopmode main.tex -output-directory=build
	biber build/main
	pdflatex -synctex=1 -interaction=nonstopmode main.tex -output-directory=build
	pdflatex -synctex=1 -interaction=nonstopmode main.tex -output-directory=build

clean:
	rm -rf $(shell pwd)/build
	mkdir $(shell pwd)/build

view: build
	evince build/main.pdf

all: view

.PHONY: build

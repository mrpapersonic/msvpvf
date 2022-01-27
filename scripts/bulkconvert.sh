#!/bin/bash
for i in *.veg; do
    [ -f "$i" ] || break
	python3 msvpvf.py -i "$i" --version 13 --type veg
done

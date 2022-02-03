#!/bin/bash

if [[ ! -f "libbee8086.a" ]]; then
	echo "Run this script from the directory where you built the Bee8086 engine."
	exit 1
fi

mkdir -p dist

if [ -d "Bee8086-SDL2" ]; then
	for lib in $(ldd Bee8086-SDL2/Bee8086-SDL2.exe | grep mingw | sed "s/.*=> //" | sed "s/(.*)//"); do
		cp "${lib}" dist
	done
	cp Bee8086-SDL2/Bee8086-SDL2.exe dist
fi

if [[ -f "kujobios.bin" ]]; then
	cp kujobios.bin dist
fi


default: flash

.PHONY: clean flash monitor build

PROJECT = $(notdir $(CURDIR))

# force fresh build
build:
	cmake -G Ninja -B build
	cmake --build build

flash: build
	cmake --build build
	picotool load -x build/src/$(PROJECT).elf

clean:
	rm -rf build

monitor:
	minicom -o -D /dev/ttyACM0 -b 115200

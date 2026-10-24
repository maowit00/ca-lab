#!/bin/sh

set -xe

sudo apt update

sudo apt install -y \
  build-essential \
  gcc-riscv64-linux-gnu \
  binutils-riscv64-linux-gnu \
  qemu-user \
  gdb-multiarch

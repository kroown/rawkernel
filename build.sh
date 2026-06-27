#!/usr/bin/env bash
set -euo pipefail

PROJ_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJ_DIR/build"
SRC_DIR="$PROJ_DIR/src"
TOOLCHAIN="$PROJ_DIR/toolchain/bin"
LIMINE_DIR="$PROJ_DIR/limine-binary"

CC="$TOOLCHAIN/x86_64-elf-gcc"
LD="$TOOLCHAIN/x86_64-elf-ld"

CFLAGS=(
    -ffreestanding -m64 -mcmodel=kernel
    -mno-red-zone -mno-mmx -mno-sse -mno-sse2
    -Wall -Wextra -nostdlib -I "$SRC_DIR"
    -O2 -fno-stack-protector
)

echo "==> Building kernel..."
mkdir -p "$BUILD_DIR"

objs=()
for src in isr_stubs.S; do
    o="$BUILD_DIR/${src%.S}.o"
    "$CC" "${CFLAGS[@]}" -c "$SRC_DIR/$src" -o "$o"
    objs+=("$o")
done
for src in kernel.c idt.c pic.c ps2.c compositor.c; do
    o="$BUILD_DIR/${src%.c}.o"
    "$CC" "${CFLAGS[@]}" -c "$SRC_DIR/$src" -o "$o"
    objs+=("$o")
done

echo "==> Linking kernel..."
"$LD" -n -T "$SRC_DIR/linker.ld" "${objs[@]}" -o "$BUILD_DIR/kernel.elf"

echo "==> Creating partition image..."
PART_FILE="$BUILD_DIR/partition.img"
qemu-img create -f raw "$PART_FILE" 63M >/dev/null
mkfs.fat -F32 -n "RAWKERNELY" "$PART_FILE" >/dev/null

echo "==> Copying files..."
mcopy -i "$PART_FILE" "$BUILD_DIR/kernel.elf" "::kernel.elf"
mcopy -i "$PART_FILE" "$PROJ_DIR/limine.conf" "::limine.conf"
mcopy -i "$PART_FILE" "$LIMINE_DIR/limine-bios.sys" "::limine-bios.sys"

echo "==> Assembling disk image..."
IMG_FILE="$BUILD_DIR/limine.img"
qemu-img create -f raw "$IMG_FILE" 64M >/dev/null

mbr=$(mktemp)
dd if=/dev/zero bs=512 count=1 of="$mbr" 2>/dev/null
printf '\xEB\x3E\x90' | dd of="$mbr" bs=1 seek=0 count=3 conv=notrunc 2>/dev/null
pofs=446
part_data=$(dd if="$PART_FILE" bs=512 count=1 2>/dev/null | xxd -p)
sectors=$((63 * 1024 * 1024 / 512))
printf '\x80\x00\x02\x00\x0C\xFF\xFF\xFF' | dd of="$mbr" bs=1 seek=$pofs count=8 conv=notrunc 2>/dev/null
printf '\x00\x08\x00\x00' | dd of="$mbr" bs=1 seek=$((pofs + 8)) count=4 conv=notrunc 2>/dev/null
python3 -c "import struct; import sys; sys.stdout.buffer.write(struct.pack('<I', $sectors))" | dd of="$mbr" bs=1 seek=$((pofs + 12)) count=4 conv=notrunc 2>/dev/null
printf '\x55\xAA' | dd of="$mbr" bs=1 seek=510 count=2 conv=notrunc 2>/dev/null

dd if="$mbr" of="$IMG_FILE" bs=512 count=1 conv=notrunc 2>/dev/null
dd if="$PART_FILE" of="$IMG_FILE" bs=512 seek=2048 conv=notrunc 2>/dev/null
rm -f "$mbr" "$PART_FILE"

echo "==> Installing Limine bootloader..."
"$LIMINE_DIR/limine" bios-install "$IMG_FILE"

echo "==> Image created: $IMG_FILE"

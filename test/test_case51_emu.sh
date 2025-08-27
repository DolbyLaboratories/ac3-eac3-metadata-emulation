#!/bin/bash
echo File: $1 Acmod: $2 Dialnorm: $3 Profile: $4 Compression Mode: $5
export stem=`basename $1 .wav`

build_release/MdEmu -p0 -s0 -a$2 -dn$3 -k$4 -c$5 $1 test/output/$stem.emu.wav

export pwt=pwr_vs_time

$pwt -c 0 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.0.res
$pwt -c 1 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.1.res
$pwt -c 2 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.2.res
$pwt -c 4 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.4.res
$pwt -c 5 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.5.res

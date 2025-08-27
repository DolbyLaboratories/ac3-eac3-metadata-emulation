#!/bin/bash
echo File: $1 MdEmu Config: $2
export stem=`basename $1 .wav`

build_debug/MdEmu $2 $1 test/output/$stem.emu.wav

export pwt=./tools/pwr_vs_time

$pwt -c 0 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.0.res
$pwt -c 1 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.1.res
$pwt -c 2 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.2.res
$pwt -c 4 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.3.res
$pwt -c 5 -s -i test/output/$stem.emu.wav -to test/output/$stem.emu.4.res

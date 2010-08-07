#!/bin/bash
( echo "class Commands:";
  cat ../arduino/SHETSource_commands.h |
    grep -Eo '[A-Z_]+\s*=\s*0x[0-9a-f]{2}' |
    awk '{printf "\t" $0 "\n"}'
) > command_nos.py

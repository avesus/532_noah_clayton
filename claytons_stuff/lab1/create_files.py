#! /usr/bin/env python3

import sys

if len(sys.argv) != 3:
    print("Usage: prg <scene_file> <output dir>")
    sys.exit(0)

play_file = open(sys.argv[1])
out_dir = sys.argv[2]

char_dict = {}

new_char = True
cur_char = ""
lnum = 1
line_format = "{} {}"
for lines in play_file:
    if new_char is True:
        lines = lines.split('.')
        if lines[0] not in char_dict:
            char_dict[lines[0]] = []
        new_char = False
        cur_char = lines[0]
    elif not lines.strip():
        new_char = True
    else:
        char_dict[cur_char].append(line_format.format(lnum, lines))
        lnum += 1

    
for k in char_dict.keys():
    char_file = open(out_dir + "/" + k + ".txt", "w")
    for l in char_dict[k]:
        char_file.write(l)
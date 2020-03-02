#! /usr/bin/env python3

import argparse
import random
parser = argparse.ArgumentParser(description='lab 1 EC')
parser.add_argument("-o", default=".", help="output directory for name files")
parser.add_argument("-f", default="", help="play file")
parser.add_argument("-SCRAMBLE", action='store_true', default=False, help="do scramble")




flags = parser.parse_args()
play_file = open(flags.f)
out_dir = flags.o
scamble = flags.SCRAMBLE
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
    swap_arr = []
    for l in char_dict[k]:
        swap_arr.append(l)
    if scamble is True:
        for l in range(0, 2*len(swap_arr)):
            idx_0 = random.randint(0, len(swap_arr)-1)
            idx_1 = random.randint(0, len(swap_arr)-1)
            tmp = swap_arr[idx_0]
            swap_arr[idx_0] = swap_arr[idx_1]
            swap_arr[idx_1] = tmp;
    for l in swap_arr:
        char_file.write(l)


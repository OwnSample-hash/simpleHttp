#!/usr/bin/env python3

import os
import re
import sys
from time import strftime

OUT_HEADER = """#ifndef __HTTP_PLUGIN_PARAMS_H__
#define __HTTP_PLUGIN_PARAMS_H__
#include "plugin.h"
static const param_type_t params[%d][%d] = {

"""
OUT_FOOTER = """};
#endif
"""

enum_start = "typedef enum plugin_event {"
enum_end = "} plugin_event_t;"
enum_vars = []
define_start = r"#define EVENTS[ ]+\\"
define_end = "// EVENTS ENDS"
define_vars = []
define_funcs: list[list[tuple[str]]] = []

fmt_arr = "    {\n%s\n    },"
fmt_arg = "        {.type = TYPE_%s, .size = sizeof(%s), .name = \"%s\"},\n"
fmt_last_arg = "        {.type = TYPE_last, .size = sizeof(void), .name = \"last\"},"


def check_and_fix_def_line(it: iter, line: str) -> str:
    """
    Check and fix the define line if it is in multiline format.
    """
    if '(' in line and ')' in line:
        return line
    while line.endswith('\\\n'):
        next_line = next(it, None)
        if next_line is None:
            print("Error: Unexpected end of file while processing define line.")
            sys.exit(1)
        line = line[:-2].strip() + next_line.strip()
    print(f"Fixed define line: {line}")
    return line


def check_plugin_sync(lines: list[str]) -> bool:
    """
    Check if the plugin events and defines are in sync.
    """
    it = iter(lines)
    for line in it:
        if line.startswith(enum_start):
            in_enum = True
            while in_enum:
                line = next(it, None)
                if line is None or line.strip() == enum_end:
                    in_enum = False
                elif line.strip() and not line.startswith("//"):
                    enum_vars.append(line.split('=')[0].strip())
        elif re.match(define_start, line):
            in_define = True
            while in_define:
                args = []
                line = next(it, None)
                if line is None or line.strip() == define_end:
                    in_define = False
                elif line.strip() and not line.startswith("//"):
                    line = check_and_fix_def_line(it, line)
                    define_vars.append(line.split(',')[1].strip().upper())
                    args = [tuple(arg.replace(")", "").replace("\\", "")
                                  .strip().split(' ')) for arg in line
                            .split(',')[2:]]
                if args:
                    define_funcs.append(args)
    if not enum_vars or not define_vars:
        print("Error: No plugin events or defines found.")
        return False
    if len(enum_vars) != len(define_vars):
        print("Error: The number of plugin events and defines do not match.")
        print(f"Events: {len(enum_vars)}, Defines: {len(define_vars)}")
        return False
    if len(define_funcs) != len(enum_vars):
        print("Error: The number of plugin event arguments does not match the number of events.")
        print("Functions that take no arguments should be take a void argument.")
        print(f"Events: {len(enum_vars)}, Args: {len(define_funcs)}")
        __import__('pprint').pprint(define_funcs)
        __import__('pprint').pprint(enum_vars)
        return False
    for eevent, devens in zip(enum_vars, define_vars):
        if eevent != devens:
            print(f"Error: Plugin event '{
                  eevent}' does not match define '{devens}'.")
            return False
    return True


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python for_each.py <path_to_plugin.h> <out>")
        sys.exit(1)

    plugin_header_path = sys.argv[1]
    if not os.path.isfile(plugin_header_path):
        print(f"Error: The file {
              plugin_header_path} does not exist or is not a file.")
        sys.exit(1)

    if len(sys.argv) > 2:
        out_file_path = sys.argv[2]
        if not out_file_path.endswith(".h"):
            print("Error: Output file must have a .h extension.")
            sys.exit(1)

    with open(plugin_header_path, 'r') as f:
        lines = f.readlines()

    if not check_plugin_sync(lines):
        print("Plugin events and defines are not in sync. Exiting.")
        sys.exit(1)

    with open(out_file_path, "w") as out_file:
        max_args = max(len(args) for args in define_funcs)
        out_file.write(OUT_HEADER % (len(enum_vars), max_args + 1))
        for func in define_funcs:
            args = []
            for arg in func:
                tp, name = arg
                args.append(fmt_arg % (tp, tp, name))
            args.append(fmt_last_arg)
            out_file.write(fmt_arr % ''.join(args) + '\n')

        out_file.write(OUT_FOOTER)
        out_file.write(
            f"// Created On: {strftime('%Y-%m-%d %H:%M:%S')}\n")
        out_file.write(f"// Source: {plugin_header_path}\n")
        out_file.write(
            "// This file is auto-generated. Do not edit manually.\n")
# Vim: set expandtab tabstop=4 shiftwidth=4:

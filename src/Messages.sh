#!/bin/sh

# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: CC0-1.0

# Extract strings from all source files.
# EXTRACT_TR_STRINGS extracts strings with lupdate and convert them to .pot with
# lconvert.
$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h` -o $podir/kaccountautoconfig6_qt.pot
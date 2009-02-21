#!/bin/sh

indent --gnu-style --no-tabs --braces-on-if-line --no-space-after-function-call-names -i4 \
       --no-space-after-casts --indent-label0 --cuddle-else  --case-indentation4\
       -T Uint8 -T FILE -T xmlChar \
       -T Archive -T Palette -T Pixmap -T Shape -T Character -T CommandSpot -T CartoonDesc -T CartoonFont \
       $*

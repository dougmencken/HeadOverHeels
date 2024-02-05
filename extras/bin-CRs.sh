#!/bin/sh

find . -type f -iname "*.h" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.c" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.hpp" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.cpp" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.xml" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.xsd" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.ac" -print0 | xargs -0 sed -i 's/\r//g'
find . -type f -iname "*.am" -print0 | xargs -0 sed -i 's/\r//g'

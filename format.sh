#!/usr/bin/env bash

shopt -s globstar
for F in **/*.{cc,hh}
do
  echo "${F}"
  clang-format -i --style=file "${F}"
done

#!/usr/bin/env bash

for F in */*.{cc,hh}
do
  echo "${F}"
  clang-format -i --style=file "${F}"
done

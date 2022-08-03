#!/bin/bash

source scripts/logging.sh

function search_and_replace_bazel()
{
  FILES=$(find $1 -name "*.bazel")

  for FILE in ${FILES}; do
    sed -i "s/\/\/src\//\/\/gfx\//" ${FILE}
  done
}

function search_and_replace_cmake()
{
  FILES=$(find $1 -name "*.cmake")

  for FILE in ${FILES}; do
    sed -i "s/src/gfx/" ${FILE}
  done
}

search_and_replace_cmake test/

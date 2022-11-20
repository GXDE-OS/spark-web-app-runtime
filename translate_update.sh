#!/bin/bash
# this file is used to auto-update .ts file.

cd $(dirname $0)

lupdate ./spark-webapp-runtime/spark-webapp-runtime.pro -no-obsolete

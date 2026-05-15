#!/bin/bash

rm -rf build

MESSAGE=${1:-"Auto-commit: $(date +'%Y-%m-%d %H:%M:%S')"}
git add .

git commit -m "$MESSAGE"

git push

#!/bin/bash

rm -rf build
rm my_app.exe 2>/dev/null
rm my_app 2>/dev/null

MESSAGE=${1:-"Auto-commit: $(date +'%Y-%m-%d %H:%M:%S')"}
git add .

git commit -m "$MESSAGE"

git push

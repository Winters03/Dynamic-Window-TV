#!/bin/bash

# Get the commit message from the first argument
# Use current date/time if no message is provided
MESSAGE=${1:-"Auto-commit: $(date +'%Y-%m-%d %H:%M:%S')"}

# Stage all changes
git add .

# Commit with the message
git commit -m "$MESSAGE"

# Push to the current branch
# Note: This assumes your upstream is already set
git push

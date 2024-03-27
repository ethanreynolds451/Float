#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PYTHON_SCRIPT_NAME="FloatUI.py"
python3 "$DIR/$PYTHON_SCRIPT_NAME" >/dev/null 2>&1 &
osascript -e 'tell application "Terminal" to close (first window whose name contains "bash")' &>/dev/null

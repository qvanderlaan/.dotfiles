#!/usr/bin/env bash

ED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
CYAN="\033[36m"
BOLD="\033[1m"
RESET="\033[0m"

info()		{ echo -e "${CYAN}$*${RESET}"; }
success()	{ echo -e "${GREEN}✅ $*${RESET}"; }
warning()	{ echo -e "${YELLOW}⚠️  $*${RESET}"; }
error()		{ echo -e "${RED}❌ $*${RESET}"; }

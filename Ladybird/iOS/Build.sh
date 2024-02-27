#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SERENITY_ROOT="$(realpath "${DIR}"/../..)"

# shellcheck source=/dev/null
. "${SERENITY_ROOT}/Meta/shell_include.sh"

cmake -S .. -B Build -GNinja \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator \
    -DLagomTools_DIR="$SERENITY_ROOT/Build/lagom-tools-install/share/LagomTools" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DCMAKE_OSX_ARCHITECTURES=arm64
ninja -C Build

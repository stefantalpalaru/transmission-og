#!/bin/bash

# Build base Docker images for making distributable binaries.
# Should be used from "build-*" Make targets, passing the target architecture's
# name and Docker image tag as parameters.

set -e

cd "$(dirname "${BASH_SOURCE[0]}")"

if [[ -z "${2}" ]]; then
  echo "Usage: $(basename ${0}) ARCH DOCKER_TAG"
  exit 1
fi
ARCH="${1}"
DOCKER_TAG="${2}"

DOCKER_BUILDKIT=1 \
  docker build \
  -t ${DOCKER_TAG} \
  --progress=plain \
  -f Dockerfile.${ARCH} .


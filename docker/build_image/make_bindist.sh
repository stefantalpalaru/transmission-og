#!/usr/bin/env bash

# Build release binaries fit for public distribution, using Docker.
# Should be used from "bindist-*" Make targets, passing the target architecture's name as a parameter.

set -e

cd "$(dirname "${BASH_SOURCE[0]}")"/../..
REPO_DIR="${PWD}"

ARCH="${1:-amd64}"
DOCKER_TAG="transmission-og-dist-${ARCH}"

docker rm ${DOCKER_TAG} &>/dev/null || true

cd docker/build_image

DOCKER_BUILDKIT=1 \
  docker build \
  -t ${DOCKER_TAG} \
  --progress=plain \
  --build-arg USER_ID=$(id -u) \
  --build-arg GROUP_ID=$(id -g) \
  -f Dockerfile.${ARCH} .

# seccomp can have some serious overhead, so we disable it with "--privileged" - https://pythonspeed.com/articles/docker-performance-overhead/
docker run --privileged --rm --name ${DOCKER_TAG} -v ${REPO_DIR}:/home/user/transmission-og ${DOCKER_TAG}

cd - &>/dev/null

# We rebuild everything inside the container, so we need to clean up afterwards.
${MAKE} --no-print-directory distclean


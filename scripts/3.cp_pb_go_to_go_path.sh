#!/bin/bash

echo "1. cp *.go to gopath"

mkdir -p $GOROOT/src/gateway_rp/proto/
cp ../proto/*.go $GOROOT/src/gateway_rp/proto/

echo "finish"

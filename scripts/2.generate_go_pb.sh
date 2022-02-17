#!/bin/bash

echo "1. generate proto.pb.go && proto_grpc.pb.go"

cd ../proto/ && protoc -I . \
	-I $GOPATH/src/ --go_out=. \
	--go_opt=paths=source_relative \
	--go-grpc_out=. \
	--go-grpc_opt=paths=source_relative \
	./hello.proto

echo "2. generate proto.pb.gw.go"

protoc -I . -I $GOPATH/src/ \
	--grpc-gateway_out . \
	--grpc-gateway_opt logtostderr=true \
    --grpc-gateway_opt paths=source_relative \
    ./hello.proto

cd -

echo "finish"

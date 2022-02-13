## demo

Provide a sample project for those people: 
- use Linux
- need C++ project (support gcc)
- need bazel construction
- need gRPC and protobuf
- need openssl (version 1.1.1+, or the better option is boringssl)
- need {fmt} for formatting.
- need sqlite3.
- use Visual Studio Code (not have to)

---

## How to prepare env

### bazel

In macOS, homebrew can only install the latest version, please use 3.7.1 for this repo.

```bash
export BAZEL_VERSION=3.7.1
mkdir -p ~/tmp
cd ~/tmp
curl -fLO "https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh"
chmod +x "bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh"
./bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh --user
echo 'export PATH="$PATH:$HOME/bin"' >> ~/.bashrc
source ~/.bashrc
```

### Visual studio code

Install the extension Bazel.

Install the extension vscode-proto3.

## How to build it

### Import this repo

```bash
git clone https://github.com/lty2226262/demo.git
```

Open your VS Code, Click `File->Open` then choose this repo.
### Build All

Open terminal in VS Code and run

```bash
bazel build --cxxopt='-std=c++17' //...
```

### Test All

Open terminal in VS Code and run

```bash
bazel test //...
```

### Usage

#### Server

After built, run

```bash
cd bazel-bin/src
./server
```

to launch server.

#### Client

run 

```bash
cd bazel-bin/src
./client
```

for client test

#### CUrl

install grpcurl first

```bash
mkdir -p ~/tmp
cd ~/tmp
wget https://github.com/fullstorydev/grpcurl/releases/download/v1.8.0/grpcurl_1.8.0_linux_x86_64.tar.gz
mkdir -p ~/bin
tar -zxvf grpcurl_1.8.0_linux_x86_64.tar.gz -C ~/bin
echo 'export PATH="$PATH:$HOME/bin"' >> ~/.bashrc
source ~/.bashrc
```

grpcurl format

```
grpcurl -d '{<request data>}' -import-path {<proto path>} -proto {<proto filename>} {gRPC-Server:port} {proto package name}.{service name}/{method name}
```

grpcurl example

```
grpcurl -d '{"name":"Hello,Elaine"}' -import-path ~/workspace/demo/proto -proto hello.proto 49.235.109.193:50001/helloworld helloworld.Hello/GetKey
```

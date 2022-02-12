## bazel-sample

Provide a sample project for those people: 
- use Linux
- need C++ project (support clang and gcc)
- need bazel construction
- need gRPC and protobuf
- need openssl (version 1.1.1+, or the better option is boringssl)
- use Visual Studio Code (not have to)

---

## How to prepare env

### bazel

In macOS, homebrew can only install the latest version, please use 3.7.1 for this repo.

```bash
export BAZEL_VERSION=3.7.1
mkdir -p ~/tmp
cd ~/tmp
curl -fLO "https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-darwin-x86_64.sh"
chmod +x "bazel-${BAZEL_VERSION}-installer-darwin-x86_64.sh"
./bazel-${BAZEL_VERSION}-installer-darwin-x86_64.sh --user
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

Open terminal in CLion and run

```bash
bazel build //...
```

If you want to build and run a spec binary, just go to a `BUILD` file or `BUILD.bazel` file, click a green triangle, then this binary will show in your top-right corner.
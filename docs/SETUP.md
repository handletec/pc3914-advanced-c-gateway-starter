# PC3914 Starter Repository Setup

This repository is designed to be built and tested inside an Ubuntu/Linux environment.

## Ubuntu Linux

```sh
sudo apt update
sudo apt install -y build-essential gdb valgrind strace binutils coreutils \
  libc-bin tar gzip grep sed gawk diffutils git
```

## Windows 10/11

Use WSL2 with Ubuntu. Microsoft installation guide: `https://learn.microsoft.com/windows/wsl/install`. In Administrator PowerShell:

```powershell
wsl --install -d Ubuntu
```

Restart if Windows requests it, launch Ubuntu, create your Linux user account, then verify WSL2 from PowerShell:

```powershell
wsl --list --verbose
```

Inside Ubuntu/WSL:

```sh
sudo apt update
sudo apt install -y build-essential gdb valgrind strace binutils coreutils \
  libc-bin tar gzip grep sed gawk diffutils git
mkdir -p ~/training/pc3914
```

Keep the repository inside the Linux filesystem, such as `~/training/pc3914`, rather than working directly under `/mnt/c`.

## macOS

Use an Ubuntu VM for the course environment. The recommended route is Canonical Multipass.

Install Multipass using Canonical's current macOS installer: `https://documentation.ubuntu.com/multipass/stable/how-to-guides/install-multipass/`. Then create an Ubuntu 24.04 instance:

```sh
multipass launch 24.04 --name pc3914 --cpus 2 --memory 4G --disk 20G
multipass shell pc3914
```

Inside the Ubuntu VM:

```sh
sudo apt update
sudo apt install -y build-essential gdb valgrind strace binutils coreutils \
  libc-bin tar gzip grep sed gawk diffutils git
mkdir -p ~/training/pc3914
```

Do not use native macOS/Xcode tools as a substitute for the governed Linux lab environment.

## Readiness check

Run inside the Ubuntu/Linux environment:

```sh
chmod +x tools/pc3914_readiness_check_v2.17.sh
./tools/pc3914_readiness_check_v2.17.sh
```

Required final line:

```text
PC3914 READINESS: PASS
```

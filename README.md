ReactiveEffector
===

## ソースコードについて

このリポジトリには一部のソースコードしかコミットしていない。  
不足しているコードは `CubeMX(6.6.1)` や `STM32CubeIDE(1.10.1)` などの自動生成機能で生成すること。

## ビルド手順

[STM32CubeIDE(1.10.1)](https://www.st.com/ja/development-tools/stm32cubeide.html)もしくは `MacOS + CMake + ARMコンパイラ` でビルドできます。

## MacOS + CMake + ARMコンパイラ

`cmake` インストール

```sh
$ brew install cmake
$ cmake --version
cmake version 3.20.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

`arm-none-eabi-gcc` インストール

```sh
$ brew tap ArmMbed/homebrew-formulae
$ brew install arm-none-eabi-gcc
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

`stlink` インストール

```sh
$ brew install stlink
$ st-flash --version
v1.7.0
```

ビルドコマンド

```sh
$ bash build.sh
```

書き込みコマンド

```sh
$ st-flash --format ihex write build/ReactiveEffector.hex
```

## ディレクトリ構成

```
（ディレクトリ）
├── cmake（CMake関連）
├── Core（自動生成・アプリケーションコード）
├── Drivers（自動生成・ドライバなど）
├── Middlewares（自動生成・RTOS, USBなど）
├── USB_DEVICE（自動生成・USBなど）
├── User（編集用・アプリケーションコード）

（ファイル）
├── README.md（このファイル）
├── Doxyfile（doxygen設定）
├── ReactiveEffector.ioc（CubeMXファイル）
├── STM32F745VGTX_FLASH.ld（自動生成・FLASH設定）
├── build.sh（CMakeビルド実行スクリプト）
├── .clang-format（clang-format設定）
└── format.sh（clang-format実行スクリプト）
```

CubeMXで自動生成したファイルのうち、以下は一部手動で編集した。
* USB_DEVICE/App/usbd_cdc_if.c

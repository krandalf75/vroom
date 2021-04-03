# Building with CMake

It's also possible build this project with cmake.

## Windows with MSVC 2019 64 bits

### Install dependencies

- Download and install openSSL client and headers from [Shining Light Providers](https://slproweb.com/products/Win32OpenSSL.html) (Win64OpenSSL-1_1_1k.msi) or choose another [provider](https://wiki.openssl.org/index.php/Binaries)
- Download [libasio](https://sourceforge.net/projects/asio/files/asio/1.18.1%20%28Stable%29/asio-1.18.1.zip/download) and uncompress on a new folder.

### Building

- Download sources of [VROOM](https://github.com/VROOM-Project/vroom) or git clone.
- Open "x64 Native Tools Command Prompt for VS 2019" and go to the folder where you installed the vroom sources.
- Execute: mkdir build
- Execute: cd build
- Execute (check paths before): cmake -G Ninja .. -DAsio_DIR="C:/projectes/asio-1.18.1" -DASIO_INCLUDE_DIR="C:/projectes/asio-1.18.1/include" -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64" -DCMAKE_BUILD_TYPE=Release  
- Execute: ninja -j 4

## MacOS

### Install dependencies

- Check Xcode already installed.
- Open terminal
- Install OpenSSL. Execute: brew install OpenSSL
- Install asio. Execute: brew install asio

### Building

Execute following sentences:
- git clone https://github.com/VROOM-Project/vroom
- cd vroom
- mkdir build
- cd build 
- cmake .. -DOPENSSL_ROOT_DIR=/usr/local/Cellar/openssl\@1.1/1.1.1k/
- make

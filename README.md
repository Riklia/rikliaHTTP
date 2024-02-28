# Riklia HTTP Server

## Description

Riklia HTTP Server is an implementation of an HTTP server designed to handle various aspects of web server functionality. It includes features for serving static files, proxying requests to other servers, and basic HTTP request handling.

## Features

- **HTTP Server**: HTTP server capable of handling GET requests.
- **Static File Serving**: Support for serving static files from a specified directory.
- **Proxy Server**: Capabilities to act as a proxy server, forwarding requests to designated destinations.
- **Configurability**: Configuration files in the `configs/riklia.conf` enable customization of server behavior.
- **Logging**: Customizable logging with support for different log levels (Trace, Debug, Info, Warn, Error, Fatal). Compressing files after the defined maxsize.
- **Range Requests**: Support for handling HTTP Range requests, allowing clients to request specific portions of a file.
- **Continue Requests**: Implementation of the HTTP/1.1 "Expect: 100-continue" feature for efficient data transmission.

## Prerequisites

- C++ compiler

## Getting Started

1. Clone the repository: `git clone https://github.com/Riklia/rikliaHTTP.git` (or use SSH as an alternative)
2. Install dependencies:
```
sudo apt-get update
sudo apt-get install zlib1g-dev
```
3. Build the project:
```
cd rikliaHTTP
mkdir build
cmake -S . -B build
cmake --build build
cp build/rikliaHTTP ./rikliaHTTP
```
4. Run the server with `sudo ./riklkiaHTTP`

## Configuration

Customize server behavior by modifying configuration files found in the `configs` directory.

## Usage

Riklia HTTP Server provides a simple interface for serving content and proxying requests. Check the examples for usage.

## Examples

The riklia.conf provided in demo directory configures two servers, each has two location paths - proxy and static. 
Each server has their own logfile with configured maximal size and logging level. 

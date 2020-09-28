# `b`

A build system

## How It Works

`b` is a stripped-down version of `make`, a utility to maintain groups of
programs. It looks for a file `b.txt` and runs the commands in the specified
order. `b` is compiled using `b`.

## Sample Configuration File

Sample `b.txt` file (taken from this repository).

```
run:
	clang++ -Wall -std=c++17 src/b.cpp -o src/b

all: run clean

clean:
	rm src/b
```

## Running Locally

```sh
git clone https://github.com/oniani/b
./b all
```

`b` can be copied into the directory that is in the `PATH` and then can be used
just like `make`.

## License

[GNU General Public License v3.0](LICENSE)

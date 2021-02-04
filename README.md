# telegram_categorization

Code for Telegram categorization library

## Code Structure

```text
telegram_categorization
- resources
  - fasttext            (dependency)
  - libtgcat            (existing code)
  - libtgcat-tester     (existing code - used for testing)
  - models              (make sure that the model you use is in this directory)
- test-data             (input txt files)
- src
  - libtgcat            (updated libtgcat source - to be tested with libtgcat-tester)
```

Make sure you download and copy `lid.176.bin` in the `resources/models` directory.
Also, you can put your test files in `test-data` directory for easy access.

## Build

1. Build [fasttext](./resources/fasttext/) library:

```shell
cd resources/fasttext
mkdir build
mkdir build && cd build && cmake ..
make
```

The library `libfasttext.so` will be generated in the `build` folder.

2. Build [src/libtgcat](./src/libtgcat/) - the updated library:

```shell
cd src/libtgcat
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The `libtgcat` library loads the model i.e. `./resources/models/lid.176.bin`
and is linked with `/resources/fasttext/build/libfasttext.so`.

3. Build [resources/libtgcat-tester](./resources/libtgcat-tester/):

Make sure that the `libtgcat` is already built and its `libtgcat.so` file is
located in its `build` folder.

```shell
cd resources/libtgcat-tester
ln -s ../../src/libtgcat/build/libtgcat.so .
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Test

Make sure `libtgcat-tester` has been built and its executable is under its
`build` folder.

```shell
cd resources/libtgcat-tester/build
```

Using `d1k.txt` file from `test-data` folder:

```shell
$ ./tgcat-tester language ./../../test-data/d1k.txt ./o1k.txt
Processed 1000 queries in 0.699325 seconds
```

You can create symlinks of test files in this directory as well:

```shell
$ ln -s ./../../test-data/d1k.txt .
$ ./tgcat-tester language ./d1k.txt ./o1k.txt
Processed 1000 queries in 0.699325 seconds
```

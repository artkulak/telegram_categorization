# Data Clustering Contest 2021
The task in this contest was to create a C/C++ library that can determine the language and topic of a Telegram channel.
**Team: Tanned Gull**

**4th place solution** <br>
[See model predictions here](https://contest.com/dc2021-r1/entry1495)


* Team Members:
	- https://github.com/artkulak
	- https://github.com/iamAzeem

## The task
Determine channel language. The algorithm must use the channel's name, description and the text of several posts to determine its language and return the language's 2-letter ISO code (or “other” if the language doesn’t have a two-letter code).

Determine channel topic. For channels in English and Russian, the algorithm must determine the relative weight for each of the topics identified in the channel. List of possible topics:

- Art & Design
- Bets & Gambling
- Books
- Business & Entrepreneurship
- Cars & Other Vehicles
- Celebrities & Lifestyle
- Cryptocurrencies
- Culture & Events
- Curious Facts
- Directories of Channels & Bots
- Economy & Finance
- Education
- Erotic Content
- Fashion & Beauty
- Fitness
- Food & Cooking
- Foreign Languages
- Health & Medicine
- History
- Hobbies & Activities
- Home & Architecture
- Humor & Memes
- Investments
- Job Listings
- Kids & Parenting
- Marketing & PR
- Motivation & Self-Development
- Movies
- Music
- Offers & Promotions
- Pets
- Politics & Incidents
- Psychology & Relationships
- Real Estate
- Recreation & Entertainment
- Religion & Spirituality
- Science
- Sports
- Technology & Internet
- Travel & Tourism
- Video Games
- Other

## Code Structure

```text
telegram_categorization
- notebooks
  - training            (contains training jupyter notebooks for the contest)
  - helper              (contains helper jupyter notebooks for the contest)
- resources
  - fasttext            (dependency)
  - libtgcat            (existing code)
  - libtgcat-tester     (existing code - used for testing)
  - models              (make sure that the model you use is in this directory)
- test-data             (input txt files)
- src
  - libtgcat            (updated libtgcat source - to be tested with libtgcat-tester)
```

Make sure you download and place `lid.176.bin` in the `resources/models` directory.  
The `models/language` is a symlink to `lid.176.bin` and is used inside `libtgcat` to
load the language model for categorization. You can create your own symlink `language`
under `models` directory to any arbitrary location. This removes the need to compile 
the library everytime a different model is used.

Also, you can put your test files in `test-data` directory and use its symlink in the
`libtgcat-tester/build` for testing purposes.


## Models

All trained models are in `submission.zip` branch, the only model that is not there is the fasttext language model, it can be downloaded [here](https://fasttext.cc/docs/en/language-identification.html)

## Build

1. Build [fasttext](./resources/fasttext/) library:

```shell
cd resources/fasttext
mkdir build && cd build && cmake ..
make
```

The library `libfasttext.so` will be generated in the `build` folder.

2. Build [src/libtgcat](./src/libtgcat/) - the updated library:

```shell
cd src/libtgcat
mkdir build && cd build
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
mkdir build && cd build
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

You can create symlink of `test-data` in this directory for a better workflow:

```shell
$ ln -s ./../../test-data/ .
$ ./tgcat-tester language ./test-data/d1k.txt ./test-data/o1k.txt
Processed 1000 queries in 0.699325 seconds
```

Now, all the inputs and output will be in the `test-data` folder.

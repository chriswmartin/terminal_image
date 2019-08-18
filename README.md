# Terminal Image

![Example output using an image of Mona Lisa](docs/images/example_mona_lisa.png)


## Requirements

### [ImageMagick](https://github.com/ImageMagick/ImageMagick)

OSX
```ShellSession
$ brew install imagemagick
```
Ubuntu
```ShellSession
$ apt install imagemagick
```
Arch Linux
```ShellSession
$ pacman -S imagemagick
```

## Building

```ShellSession
$ cd terminal_image
$ make
```

## Usage

Run terminal_image with any number of images as arguments. Additionally you may provide width and height values to alter the output dimensions using the -w and -h flags. 

```ShellSession
$ ./terminal_image -w [width] -h [height] img1 img2 img2 ...
```

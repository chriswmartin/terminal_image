# Terminal Image

![Example output using an image of Mona Lisa](docs/images/example_mona_lisa.png)


## Requirements

### [ImageMagick](https://github.com/ImageMagick/ImageMagick)

OSX
```console
$ brew install imagemagick
```
Ubuntu
```console
$ apt install imagemagick
```
Arch Linux
```console
$ pacman -S imagemagick
```

## Building

```console
$ cd terminal_image
$ make
```

## usage

Run terminal_image with any number of images as arguments. Additionally you may provide width and height values to alter the output dimensions using the -w and -h flags. 

```console
$ ./terminal_image -w [width] -h [height] img1 img2 img2 ...
```

# Terminal Image

![Example output using a color table image](docs/images/example_color-table.png)
![Example output using an image of Mona Lisa](docs/images/example_mona-lisa.png)
![Example output using an image of Starry Night](docs/images/example_starry-night.png)


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

Run terminal_image with any number of images as arguments. Additionally you may enable monochrome mode with the -m flag and provide width and height values to alter the output dimensions using the -w and -h flags. By default terminal_image will display input images in color at a size scaled to fit your currrent terminal window dimensions.

```bash
terminal_image img                                        # display an image in color scaled to fit your terminal window
terminal_image -w [width] -h [height] img1 img2 img2      # display multiple images in color at a specific scale
terminal_image -m -w [width] -h [height] img              # display an image in monochrome mode at a specific scale
```

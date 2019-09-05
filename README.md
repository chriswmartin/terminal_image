# Terminal Image

![Example output using a color table image](docs/images/example_color-table.png)
![Example output using an image of Mona Lisa](docs/images/example_mona-lisa.png)
![Example output using an image of Starry Night](docs/images/example_starry-night.png)


## Requirements

### [ImageMagick](https://github.com/ImageMagick/ImageMagick)

OSX
```bash
brew install imagemagick
```
Ubuntu
```bash
apt install imagemagick
```
Arch Linux
```bash
pacman -S imagemagick
```

## Building

```bash
cd terminal_image
make
```

## Usage

Run terminal_image with any number of images as arguments.  
You may specify 'color', 'limit', 'monochrome', or 'plain-text' output using the --colorspace flag and can manually set output dimensions by passing values to the --width and --height flags.  
By default terminal_image will display images in [24-bit color](https://gist.github.com/XVilka/8346728) at a size scaled to fit your currrent terminal window dimensions.  

```bash
USAGE:
terminal_image [options] [image]

--width, -w <number>
     set width of output, must be used in conjunction with --height

--height -h <number>
     set height of output, must be used in conjunction with --width

--colorspace, -c <color|limit|monochrome|plain-text>
     set colorspace of output
     possible values:
          color: 24-bit True Color (default)
          limit: displays the image using a limited color palette
          monochrome: represents image using red '0's for dark areas and green '1's for light areas
          plain-text: the same as monochrome but the '0's and '1's are not colored

terminal_image [image]                                                                   # display an image in color scaled to fit your terminal window
terminal_image --colorspace=color --width=10 --height=10 [image 1] [image 2] [image 3]   # display multiple images in color at 10x10 scale
terminal_image --colorspace=monochrome --width=50 --height=50 [image]                    # display an image in monochrome mode at 50x50 scale
```

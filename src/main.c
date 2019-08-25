#include <stdio.h>
#include <MagickWand/MagickWand.h>
#include <sys/ioctl.h>
#include <magic.h>
#include <string.h>
#include <getopt.h>

// create a struct to hold our resized image dimensions
struct resized_image_dimensions{int width, height;};

struct resized_image_dimensions resize_image(int width, int height, char *image, char *output_name);
int process_image(char *colorspace, char *image, char *output_name);
unsigned char * get_image_pixels(int width, int height, char *colorspace, char *image);
int display_image(int width, int height, char *colorspace, unsigned char *buffer);
int check_file(char *file);
int print_usage_and_exit(char *program_name);

int main (int argc, char *argv[]){
  // default width & height values if --width & --height flags aren't used
  int width = 0, height = 0;
  
  // default colorspace value if --colorspace flag isn't used
  char *colorspace = "color";

  // initialize a struct to hold our resized image dimensions
  struct resized_image_dimensions dimensions;

  // temporary file for imagemagick operations
  char *tmp_file = "/tmp/terminal_image_tmp.jpg";
  
  // if there is not at least one argument exit
  if(argc < 2){
    print_usage_and_exit(argv[0]);
  }

  // parse width, height, & colorspace  options using getopt()
  struct option longopts[] = {
    { "width", required_argument, NULL, 'w' },
    { "height", required_argument, NULL, 'h' },
    { "colorspace", required_argument, NULL, 'c' },
    { 0, 0, 0, 0 }
  };

  int c;
  while ((c = getopt_long(argc, argv, "w:h:c:W;", longopts, NULL)) != -1) {
    switch (c) {
    case 'w':
      if (optarg != NULL) {
        if (atoi(optarg) >= 1){
          width = atoi(optarg) * 2;
        } else {
            printf("width needs to be equal to or greater than 1\n");
            exit(1);
        }
      }
      break;
    case 'h':
      if (optarg != NULL) {
        if (atoi(optarg) >= 1){
          height = atoi(optarg);
        } else {
            printf("height needs to be equal to or greater than 1\n");
            exit(1);
        }
      }
      break;
    case 'c':
      if (optarg != NULL) {
        if (strncmp(optarg, "color", 5) == 0 || strncmp(optarg, "monochrome", 10) == 0){
          colorspace = optarg;
        } else {
            printf("please specify either 'color' or 'monochrome'\n");  
            exit(1);
        }
      } 
      break;
    case 0:
      break; 
    #if 0
      case 1:
      break;
    #endif
    case ':':
      fprintf(stderr, "%s: option `-%c' requires an argument\n", argv[0], optopt);
      break;
    case '?':
    default:
      fprintf(stderr, "%s: option `-%c' is invalid: ignored\n",
      argv[0], optopt);
      break;
    }
  }

  // if there is not at least one image provided in addition to width and height options - exit
  if (optind >= argc){
    print_usage_and_exit(argv[0]);
  }

  // iterate through non width, height, colorspace arguments provided
  for (int i=optind; i < argc; i++){
    // check if the argument is an image file
    check_file(argv[i]);
    
    // resize image
    // if both --width & --height flags are used resize to chosen size
    // otherwise proportionally resize to fit terminal window
    dimensions = resize_image(width, height, argv[i], tmp_file);

    // color mode: set depth to 8 bit
    // monochrome mode: convert to grayscale -> increase contrast -> set depth to 8 bit
    process_image(colorspace, tmp_file, tmp_file);
    
    // create an array of all pixels values in the image
    unsigned char *buffer = get_image_pixels(dimensions.width, dimensions.height, colorspace, tmp_file);
    
    // format the pixel values then display them
    display_image(dimensions.width, dimensions.height, colorspace, buffer);

    // free memory allocated for the pixel array
    free(buffer);

    // remove temporary file
    remove(tmp_file);
  }

  return 0;
}

struct resized_image_dimensions resize_image(int width, int height, char *image, char *output_name){
  struct resized_image_dimensions dimensions;

  // initialize MagickWand
  MagickWand *m_wand = NULL;
  MagickWandGenesis();
  m_wand = NewMagickWand();

  // read the original image
  MagickReadImage(m_wand,image);
  
  // if --width & --height flags are used and their values are >= 1
  // resize to those values
  if (width >= 1 && height >= 1){
    dimensions.width = width;
    dimensions.height = height;

    // resize the image
    MagickResizeImage(m_wand, width, height, LanczosFilter);
  } else {
      // if --width & --height flags are not used resize to fit current terminal window
      
      struct winsize w;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
      int terminal_width = w.ws_col;
      int terminal_height = w.ws_row;
  
      int image_width = MagickGetImageWidth(m_wand) * 2;
      int image_height = MagickGetImageHeight(m_wand);

      double ratio = 0; 
      double ratioX = (double) terminal_width / (double) image_width;
      double ratioY = (double) terminal_height / (double) image_height;

      if (ratioX > ratioY){
        ratio = ratioY;
      } else {
          ratio = ratioX;
      }

      int width = image_width * ratio;
      int height = image_height * ratio;

      dimensions.width = width;
      dimensions.height = height;

      // resize the image
      MagickResizeImage(m_wand, width, height, LanczosFilter);
    }
  
  // save our edited image
  MagickWriteImage(m_wand,output_name);

  // destroy MagickWand
  if(m_wand)m_wand = DestroyMagickWand(m_wand);
  MagickWandTerminus();

  return dimensions; 
}

int process_image(char *colorspace, char *image, char *output_name){
  // initialize MagickWand
  MagickWand *m_wand = NULL;
  MagickWandGenesis();
  m_wand = NewMagickWand();

  // read the original image
  MagickReadImage(m_wand,image);

  if (strncmp(colorspace, "monochrome", 10) == 0){
    int number_of_colors = 16;
    int tree_depth = 1;
    int brightness = 0;
    int contrast = 50;
      
    // convert the image to Grayscale
    MagickQuantizeImage(m_wand, number_of_colors, GRAYColorspace, tree_depth, NoDitherMethod, MagickFalse);

    // increate the contrast
    MagickBrightnessContrastImage(m_wand, brightness, contrast);
  }

  // set image depth to 8 after other transformations - mostly to help get correct RGB pixel values
  MagickSetImageDepth(m_wand, 8);
	
  // save our edited image
  MagickWriteImage(m_wand,output_name);

  // destroy MagickWand
  if(m_wand)m_wand = DestroyMagickWand(m_wand);
  MagickWandTerminus();

  return 0;
}

unsigned char * get_image_pixels(int width, int height, char *colorspace, char *image){
  // initialize MagickWand
  MagickWand *m_wand = NULL;
  MagickWandGenesis();
  m_wand = NewMagickWand();

  // initialize *buffer to NULL as we won't know how much space to allocate to it
  // until after reading 'colorspace' but we must always return it
  unsigned char *buffer = NULL;

  // read out processed image
  MagickReadImage(m_wand, image);

  if (strncmp(colorspace, "color", 10) == 0){
    // allocate memory
    buffer = calloc(width * height * 3,1);
  
    // export the color ("RGB") values of all pixels into the array we just allocated space for
    MagickExportImagePixels(m_wand, 0,0,width,height, "RGB", CharPixel, buffer);
  } else {
      // allocate memory
      buffer = calloc(width * height,1);
      
      // export the grayscale ("I") value of all pixels into the array we just allocated space for
      MagickExportImagePixels(m_wand, 0,0,width,height, "I", CharPixel, buffer);
  }

  // destroy MagickWand
  if(m_wand)m_wand = DestroyMagickWand(m_wand);	
  MagickWandTerminus();

  // return the pixel array so that we can use it when displaying the image later
  return buffer;
}

int display_image(int width, int height, char *colorspace, unsigned char *buffer){
  if (strncmp(colorspace, "color", 5) == 0){
      // truecolor ANSI code
      int red = 0;
      int green = 0;
      int blue = 0;
      char *text = NULL;
  
      int pixels = width * height * 3;
      
      int x = 0;
       
      // iterate through all pixels
      for(int i=1; i<pixels+1; i=i+3){
        red = buffer[i-1];
        green = buffer[i];
        blue = buffer[i+1];
        text = "\u2580";
        //text = " ";

        // print truecolor string with the current pixel's RGB value
        // '\x1b[48;2;%d;%d;%dm' sets the background color 
        // '\x1b[38;2;%d;%d;%dm%s' sets the foreground color
        // '\x1b[0m' resets all colors
        printf("\x1b[48;2;%d;%d;%dm\x1b[38;2;%d;%d;%dm%s\x1b[0m", red, green, blue, red, green, blue, text);
         
        x++; 
        if(x%width == 0 && x!=pixels){
          // break the block of pixels onto separate lines
          printf("\n");
        }

      }
    } else {
      char *red="\033[1;31m";
      char *green="\033[1;32m";
      char *reset="\033[0m";

      // define a darkness/lightness threshold
      int threshold = 127;
    
      int pixels = width * height;
    
      // iterate through all pixels
      for(int i=1; i<pixels+1; i++){
        if(buffer[i-1] <= threshold ){
          // if the current pixel is darker than the threshold replace it with a red '0'
          buffer[i-1] = 0;
          printf("%s", red);
          printf("%d", buffer[i-1]);
          printf("%s", reset);
        } else {
          // if the current pixel is lighter than the threshold replace it with a green '1'
          buffer[i-1] = 1;
          printf("%s", green);
          printf("%d", buffer[i-1]);
          printf("%s", reset);
        }
        if(i%width == 0 && i!=pixels){
          // break the block of pixels onto separate lines
          printf("\n");
        }
      }
    }

  printf("\n\n");

  return 0;
}

int check_file(char *file){
  struct magic_set *magic = magic_open(MAGIC_MIME_TYPE|MAGIC_CHECK);
  magic_load(magic,NULL);

  // check the mime type of the input file using libmagic to determine whether of not it is an image
  const char *result = magic_file(magic, file);

  // we only need the first section of the mime type output so delete the rest
  char *result_pointer = strchr(result, '/');
  if (result_pointer != NULL) {
    *result_pointer = '\0';
  }

  if (strncmp(result, "image", 5) == 0){
    // if the provided file is an image print it's name and continue
    printf("%s\n", file);
    return 0;
  } else {
    // if provided with a non-image file exit
    printf("%s is not a valid image file\n", file);
    exit(1);
  }
}

// print usage text
int print_usage_and_exit(char *program_name){
  printf("please provide at least one image to analyze\nadditionally you may specify an output width and height using --width & --height flags as well as a colorspace value with --colorspace\n\nusage: %s --width=[number] --height=[number] --colorspace=[color or monochrome] img1 img2 img3...\n", program_name);

  exit(1);
}

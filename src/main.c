#include <stdio.h>
#include <MagickWand/MagickWand.h>
#include <magic.h>
#include <string.h>
#include <unistd.h>

int process_image(int resize_width, int resize_height, char *image, char *output_name);
unsigned char * get_image_pixels(char *image);
int display_image(int width, int height, unsigned char *buffer);
int check_file(char *file);
int print_usage_and_exit(char *program_name);

int main (int argc, char *argv[]){
  // default width & height values to use if none are provided as arguments
  int width = 16;
  int height = 16;

  // if there is not at least one argument exit
  if(argc < 2){
    print_usage_and_exit(argv[0]);
  }

  // parse width and height options using getopt()
  int i;
  while ((i = getopt (argc, argv, ":w:h:")) != -1){
    switch (i){
      case 'w':
        width = atoi(optarg);
        break;
      case 'h':
        height = atoi(optarg);
        break;
      default:
        abort();
    }
  }

  // if there is not at least one image provided in addition to width and height options - exit
  if (optind >= argc){
    print_usage_and_exit(argv[0]);
  }

  // iterate through non width & height arguments provided
  for (int i=optind; i < argc; i++){
    // check if argument is an image file
    check_file(argv[i]);
    
    // scale -> convert to grayscale -> increase contrast
    process_image(width, height, argv[i], "processed.jpg");
    
    // create an array of all grayscale pixel values in the image
    unsigned char *buffer = get_image_pixels("processed.jpg");
    
    // format the pixel values then display them
    display_image(width, height, buffer);

    // free memory allocated for the pixel array
    free(buffer);
  }

  return 0;
}

int process_image(int resize_width, int resize_height, char *image, char *output_name){
  int number_of_colors = 16;
  int tree_depth = 1;
  int brightness = 0;
  int contrast = 50;

  MagickWand *m_wand = NULL;	
	MagickWandGenesis();
	m_wand = NewMagickWand();
	
  // read the original image
  MagickReadImage(m_wand,image);
	
  // resize the image
  MagickResizeImage(m_wand, resize_width, resize_height, LanczosFilter);	

  // convert the image to grayscale
	MagickQuantizeImage(m_wand, number_of_colors, GRAYColorspace, tree_depth, NoDitherMethod, MagickFalse);

  // increate the contrast
  MagickBrightnessContrastImage(m_wand, brightness, contrast);
	
  // save our edited image
	MagickWriteImage(m_wand,output_name);
	
	if(m_wand)m_wand = DestroyMagickWand(m_wand);	
	MagickWandTerminus();

  return 0;
}

unsigned char * get_image_pixels(char *image){
  MagickWand *m_wand = NULL;	
	MagickWandGenesis();
	m_wand = NewMagickWand();
	
  MagickReadImage(m_wand,image);

  // determine how much memory will need to be allocated for all of our pixels
  int width = MagickGetImageWidth(m_wand);
  int height = MagickGetImageHeight(m_wand);

  // allocate memory
  unsigned char *buffer = calloc(width * height,1);

  // export the grayscale ("I") value of all pixels into the array we just allocated space for
  MagickExportImagePixels(m_wand, 0,0,width,height, "I", CharPixel, buffer);
	
	if(m_wand)m_wand = DestroyMagickWand(m_wand);	
	MagickWandTerminus();

  // return the pixel array so that we can use it when displaying the image later
  return buffer;
}

int display_image(int width, int height, unsigned char *buffer){
  // define a darkness/lightness threshold
  int threshold = 127;

  char *red="\033[1;31m";
  char *green="\033[1;32m";
  //char *yellow="\033[1;33m";
  //char *blue="\033[1;34m";
  //char *magenta="\033[1;35m";
  char *reset="\033[0m";

  int pixels = width * height;

  // iterate through all pixels
  for(int i=1; i<pixels+1; i++){
    if(buffer[i-1] <= threshold ){
      // if the current pixel is darker than the threshold replace it with a red '0'
      buffer[i-1] = 0;
      printf("%s", red);
      printf("%d ", buffer[i-1]);
      printf("%s", reset);
    } else {
      // if the current pixel is lighter than the threshold replace it with a green '1'
      buffer[i-1] = 1;
      printf("%s", green);
      printf("%d ", buffer[i-1]);
      printf("%s", reset);
    }
    if(i%width == 0 && i!=pixels){
      // break the block of pixels onto separate lines
      printf("\n");
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

  if (strcmp(result, "image") == 0){
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
    printf("please provide at least one image to analyze\nadditionally you may specify an output width and height\n\nusage: %s -w [number] -h [number] img1 img2 img3...\n", program_name);

  exit(1);
}

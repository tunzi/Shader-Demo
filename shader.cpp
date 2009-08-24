#include <iostream>
#include <cmath>
#include <SDL/SDL.h>
#include "vector.h";

#define CAP_TO_1(x) (fabs(x)<1?fabs(x):1.0)

bool redraw;

// Screen coordinate boundaries
const int LENGTH = 512;
const int HEIGHT = 512;

// World coordinate boundaries
double X1 = -1.0;
double X2 = 1.0;
double Y1 = -1.0;
double Y2 = 1.0;


struct point
{
  point() : x(0), y(0) {}
  point(double nx, double ny) : x(nx), y(ny) {}

  double x;
  double y;
};

std::ostream& operator<<(std::ostream& out, const point& p)
{
  out << "(" << p.x << "," << p.y << ")"; 
  return out;
}




struct light_source
{
  light_source() : loc(vector(0,0,3)), b(1)  {}
  light_source(vector nloc, double nb) : loc(nloc), b(nb) {}

  vector loc; // location
  double b; //intensity
};


// World coordinate map
vector world_coord_map[LENGTH][HEIGHT];
 
// Primary Height map
double map[LENGTH][HEIGHT];

// Normal Map
vector n_map[LENGTH][HEIGHT];

// Shaded surface
double shade_map[LENGTH][HEIGHT];

 
bool do_input(point& p_mouse, light_source &light);
void render_array(double map[LENGTH][HEIGHT]);
void render_array(vector map[LENGTH][HEIGHT]);
void normalize(vector map[LENGTH][HEIGHT]);
void shade(light_source light);
void init_height_map();
void init_height_map_from_bmp(char* map);
double f(point p);
SDL_Color getColor(Uint32 pix, SDL_PixelFormat *fmt);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
point to_world(point p);
void height_to_normal_map();

// convert p from Screen coordinates (LENGTH X WIDTH) to World coordinates (X1 x Y2)
point to_world(point p) {
  double x, y;
  x = p.x*(X2-X1)/LENGTH + X1;
  y = -(p.y*(Y2-Y1)/HEIGHT + Y1);
  return point(x,y);
}

#undef main() //for mingw32
int main(int argc, char* argv[])
{ 
  point p_mouse;
  light_source light;
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      std::cerr << "Error initializing sdl" << std::endl;
      return 1;
    }
 
  SDL_Surface* screen = SDL_SetVideoMode(LENGTH, HEIGHT, 32, 0);
  redraw = true;
  if(argc == 2)
  	init_height_map_from_bmp(argv[1]);
  else
  	init_height_map();

  height_to_normal_map();
 
  //MAIN RENDER/INPUT LOOP
  while (do_input(p_mouse, light))
    {
      if (redraw)
        {
	  // Rerender
	  shade(light);
	  
	  // Draw to screen
	  render_array(shade_map);
	  
          redraw = false;
        }
      SDL_Flip(screen);
    }
 
  return 0;
}

//Method: getpixel
//gets the pixel at the given coords, irrelevent of filetype
//Requires:
//	*surface - pointer to initlized sdl surface
//	x - x location of the pixel
//	y - y location of the pixel
//returns:

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	
	switch(bpp)
	{
		case 1:
			return *pixel;
		case 2:
			return *(Uint16 *)pixel;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
			else 
				return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
		case 4:
			return *(Uint32 *)pixel;
		default:
			return 0;
	}
} 

//getColor
//Returns an SDL_Color of the pixels color with a given pixel format
//Requires:
//	pix - the pixel;
//	fmt - the format;
SDL_Color getColor(Uint32 pix, SDL_PixelFormat *fmt)
{
      int bpp = fmt->BytesPerPixel;
      Uint32 temp;
      Uint8 red, green, blue, alpha;
      SDL_Color clr;

      if(bpp == 1)
      {	
	      Uint8 index = (Uint8)((Uint32)pix);
	      return fmt->palette->colors[index];
	      //return pix;
      } else {
	      temp = (Uint32)pix & fmt->Rmask;
	      temp = temp >> fmt->Rshift;
	      temp = temp << fmt->Rloss;
	      clr.r = (Uint8)temp;

	      temp = (Uint32)pix & fmt->Gmask;
	      temp = temp >> fmt->Gshift;
	      temp = temp << fmt->Gloss;
	      clr.g = (Uint8)temp;

	      temp = (Uint32)pix & fmt->Bmask;
	      temp = temp >> fmt->Bshift;
	      temp = temp << fmt->Bloss;
	      clr.b = (Uint8)temp;

	      return clr;
	      
      }
}

void init_height_map_from_bmp(char *mapfile)
{
	point p;
	int i, j;
	SDL_Surface *img;
	img = SDL_LoadBMP(mapfile);
	SDL_LockSurface(img);
	for(i=0; i<LENGTH; i++)
	{
		for(j=0; j<HEIGHT; j++)
		{
			Uint8 *red, *green, *blue;
			p = to_world(point(i,j));
			world_coord_map[i][j] = vector(p.x, p.y, 0);
			int img_x = img->h / HEIGHT;
			int img_y = img->w / LENGTH;
			Uint32 pix = getpixel(img, i, j);
			SDL_Color color = getColor(pix, img->format);
			//			std::cout<<"test"<<color.r;
			//SDL_GetRGB(pix, img->format, red, green, blue);
			//double value = (red + green + blue) / 768;
			//std::cout<<"Value ="<<value<<std::endl;
			map[i][j] = (double)pix / (img->format->BytesPerPixel * 256);
		}
	}
	SDL_UnlockSurface(img);
	SDL_FreeSurface(img);
}
void init_height_map()
{
  int i, j;
  point p;
  for (i=0; i<LENGTH; i++)
    for (j=0; j<HEIGHT; j++) {
      p = to_world(point(i,j));
      world_coord_map[i][j] = vector(p.x, p.y, 0);
      map[i][j] = f(p);
    }
}

// Returns a value between 0 and 1 given any point (x,y)
double f(point p)
{
  double s = 2.0;  //scale factor
  return 0.5/(1+s*p.x*p.x) + 0.5/(1+s*p.y*p.y);
}

// Here is where the magick happens
void height_to_normal_map()
{
  int i,j;
  double dx = (X2-X1)/LENGTH;
  double dy = (Y2-Y1)/HEIGHT;
  double z_scale = 50*HEIGHT;

  // Ignoring edge cases for now...
  for (i=1; i<LENGTH-1; i++)
    for (j=1; j<HEIGHT-1; j++) {
      n_map[i][j].x = -(map[i+1][j] - map[i-1][j])/dx;
      n_map[i][j].y = (map[i][j+1] - map[i][j-1])/dy;
      n_map[i][j].z = 1/(dy*dx)/z_scale;
      n_map[i][j] = n_map[i][j].normalize();
    }

}

void shade(light_source light)
{
  int i,j;
  for (i=0; i<LENGTH; i++)
    for (j=0; j<HEIGHT; j++) {
      // Calculate light source unit vector
      vector to_light = light.loc-world_coord_map[i][j];
      vector unit_light = to_light.normalize();
      shade_map[i][j] = light.b * (unit_light * n_map[i][j])/pow(to_light.magnitude(), 2);
    }
  //  std::cout << "shaded";
}

void render_array(double map[LENGTH][HEIGHT])
{
  SDL_Surface* dest = SDL_GetVideoSurface();
  int i, j; 
  for (i = 0; i < HEIGHT; i++)
    for (j = 0; j < LENGTH; j++)
      {
        SDL_Rect pix = {i, j, 1, 1};
        int darkness = CAP_TO_1(map[i][j]) * 0xff;
        SDL_FillRect(dest, &pix,
          SDL_MapRGB(dest->format, darkness, darkness, darkness));
      }
  return;
}

void render_array(vector n_map[LENGTH][HEIGHT])
{
  SDL_Surface* dest = SDL_GetVideoSurface();
  int i, j;
  for (i = 0; i < HEIGHT; i++)
    for (j = 0; j < LENGTH; j++)
      {
        SDL_Rect pix = {i, j, 1, 1};
        vector v = (n_map[i][j] + vector(1,1,0))*128.0;
	// double (-1, 0, 1) converts to RGB (0, 128, 255) colored bitmap
        SDL_FillRect(dest, &pix,
		     SDL_MapRGB(dest->format, v.x, v.y, v.z));
      }
  return;
}

void normalize(vector map[LENGTH][HEIGHT])
{
  
}
 
bool do_input(point& p_mouse, light_source &light)
{
  SDL_Event d;
  while (SDL_PollEvent(&d))
    {
      if (d.type == SDL_QUIT)
        return false;
      
      if (d.type == SDL_MOUSEMOTION)
	{
	  int x,y;       // Mouse position in screen coordinates
	  SDL_GetMouseState(&x, &y);
	  //	  p_mouse = point(x/(float)LENGTH*(X2-X1)+X1, (float)y/HEIGHT*(Y2-Y1)+Y1);
	  p_mouse = to_world(point(x,y));
	  light.loc.x = p_mouse.x;
	  light.loc.y = p_mouse.y;
	  redraw = true;
	}	

      if (d.type == SDL_MOUSEBUTTONDOWN)
	{
	  switch(d.button.button)
	    {
	    case 4:
	      if (light.loc.z > .3)
		light.loc.z -= .2;
	      break;
	    case 5:
	      light.loc.z += .2;
	      break;
	    }
	  std::cout << "Light location changed to: " << light.loc << std::endl;
	}

      if (d.type == SDL_KEYDOWN)
        {
          switch (d.key.keysym.sym)
            {
	    case (SDLK_q):
	      {
		return false;
		break;
	      } 
            }
        }
    }
  return true;
 }


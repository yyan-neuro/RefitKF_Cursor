/*This source code was modified from tutorials written by Lazy Foo's Productions 
("https://lazyfoo.net/tutorials/SDL/26_motion/index.php")
*/

//Using SDL, SDL_image, standard IO, and strings
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>  
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <ctime>
#include<winsock2.h>
using namespace std;


#define PI 3.14159265
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define BUFLEN 512  //Max length of buffer
#define PORT 8051   //The port on which to listen for incoming data


//Screen dimension constants
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
double CONTACT_RADIUS = 0;
int CONTACT_TIME = 500;
int MAX_TIME = 20000;
int WAIT_TIME = 0;
float MAX_VEL = 5;
float ASSIS_RATIO = 0;
float ASSIS_VRATIO = 0.5;


//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//The application time based timer
class LTimer
{
public:
	//Initializes variables
	LTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};

//The dot that will move around on the screen
class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 2;

	//Initializes the variables
	Dot();

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	//Moves the dot
	void move();


	//shift position
	void shift(Dot chaser);

	//shift in a circle
	void shift(double r);

	//Shows the dot on the screen
	void render();
	void render(LTexture *pic);

	//getters and setters
	void setX(int nx);
	void setY(int ny);
	void setVX(float vx);
	void setVY(float vy);
	float getVX();
	float getVY();
	int getX();
	int getY();

	static double dist(Dot a, Dot b);


private:
	//The X and Y offsets of the dot
	int mPosX, mPosY;

	//The velocity of the dot
	float mVelX, mVelY;
};

//Starts up SDL and creates window
 bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gTarTexture;
LTexture gWaitTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x - mWidth / 2, y - mHeight / 2, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


Dot::Dot()
{
	//Initialize the offsets
	mPosX = SCREEN_WIDTH/2;
	mPosY = SCREEN_HEIGHT/2;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move()
{

	int tvx = (int)round(mVelX);
	int tvy = (int)round(mVelY);
	//Move the dot left or right
	mPosX += tvx;

	//If the dot went too far to the left or right
	if ((mPosX < 0 + DOT_WIDTH / 2) || (mPosX + DOT_WIDTH / 2 > SCREEN_WIDTH))
	{
		//Move back
		mPosX -= tvx;
	}

	//Move the dot up or down
	mPosY += tvy;

	//If the dot went too far up or down
	if ((mPosY < 0 + DOT_HEIGHT / 2) || (mPosY + DOT_HEIGHT / 2 > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= tvy;
	}
}

/*
void Dot::move2()
{
	const char* address = "localhost:5000";

	double xc, yc, xv, yv, temp;

	receiving_udpsocket socket(address);
	sockstream connection(socket);
	string line;
	// Print each line of BCI2000 input to stdout.
	while (getline(connection, line)) {
		sscanf(line, "%f %f %f %f %f", xc, yc, xv, yv, temp);
		mPosX = (int)xc+0.5;
		mPosY = (int)yc + 0.5;
		mVelX = (int)xv + 0.5;
		mVelY = (int)yv + 0.5;
	}





}
*/

void Dot::shift(Dot chaser) {



	while (Dot::dist(*this, chaser) <= CONTACT_RADIUS) {
		mPosX = gTarTexture.getWidth() / 2 + rand() % (SCREEN_WIDTH - gTarTexture.getWidth());
		mPosY = gTarTexture.getHeight() / 2 + rand() % (SCREEN_HEIGHT - gTarTexture.getHeight());
	}
}

void Dot::shift(double r) {
	int ang = rand() % 360;
	mPosX = (int)round(SCREEN_WIDTH/2 + r*cos(ang*PI / 180));
	mPosY = (int)round(SCREEN_HEIGHT / 2 +r*sin(ang*PI / 180));
}

void Dot::render()
{
	//Show the dot
	gDotTexture.render(mPosX, mPosY);
}

void Dot::render(LTexture *pic) {
	pic->render(mPosX, mPosY);
}


void Dot::setX(int nx) {
	mPosX = nx;
}

void Dot::setY(int ny) {
	mPosY = ny;
}

void Dot::setVX(float vx) {
	mVelX = vx;
}
void Dot::setVY(float vy) {
	mVelY = vy;
}
int Dot::getX() {
	return mPosX;
}
int Dot::getY() {
	return mPosY;
}
float Dot::getVX() {
	return mVelX;
}
float Dot::getVY() {
	return mVelY;
}


double Dot::dist(Dot a, Dot b) {
	return sqrt(pow((a.getX() - b.getX()), 2) + pow((a.getY() - b.getY()), 2));
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		SCREEN_WIDTH = DM.w / 2;
		SCREEN_HEIGHT = DM.h / 2;


		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}




			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if (!gDotTexture.loadFromFile("dot.bmp") || !gTarTexture.loadFromFile("TAR.png") || !gWaitTexture.loadFromFile("WAIT.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	CONTACT_RADIUS = gTarTexture.getWidth() / 2;
	printf("radius is %f\n",CONTACT_RADIUS);


	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();
	gTarTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

string get_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", &timeinfo);
	string ttt(buffer);
	return ttt;


}

float limit_velo(float v) {
	if (v > MAX_VEL) {
		return(MAX_VEL);
	}
	else if (v < -MAX_VEL) {
		return (-MAX_VEL);
	}
	else {
		return(v);
	}


}


void limit_velo2(float xv, float yv, float *save, float max_vel) {

	float base = max_vel / sqrt(xv*xv + yv*yv);
	if (sqrt(xv*xv + yv*yv)>max_vel) {
		save[0] = xv*base;
		save[1] = yv*base;
	}
	else {
		save[0] = xv;
		save[1] = yv;
	}
}

int main(int argc, char* args[])
{

	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	float ud1, ud2, ud3, ud4, ud5;
	float txDiff, tyDiff, desVX, desVY, tBase;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done++");














	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			float xyv[2] = { 0,0 };

			//The dot that will be moving around on the screen
			Dot dot;
			Dot target, wait;
			Uint32 start_time = 0, current = 0, contact_mark = 0, wait_start = 0;
			int trialnum = 0;
			bool is_waiting = true;
			ofstream record;
			FILE* aratio;

			//Set the position of the rest sign
			wait.setX(SCREEN_WIDTH / 2);
			wait.setY(SCREEN_HEIGHT / 2);


			record.open("record_con.txt", ios::out | ios::trunc);
			aratio = fopen("aratio.txt", "r");
			if (!record.is_open() || aratio == NULL) {
				cout << "file did not open!!!!\n";
			}

			fscanf(aratio, "%f", &ASSIS_RATIO);
			fclose(aratio);

			//record << "New Experiment! " << get_time() << '\n';

			target.shift(150);
			//record << "Trial: " << trialnum << '\n';
			record << "Target: " + to_string(target.getX()) + " " + to_string(target.getY()) << '\n';
			record.close();




			wait_start = SDL_GetTicks();
			
			while (!quit)
			{
				current = SDL_GetTicks();

			
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent(e);
				}




			
				SDL_PollEvent(&e);

				fflush(stdout);


				/* Neural Control

				//clear the buffer by filling null, it might have previously received data
				memset(buf, '\0', BUFLEN);
				//try to receive some data, this is a blocking call
				if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
				{
					printf("recvfrom() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
				}


				aratio = fopen("aratio.txt", "r");
				fscanf(aratio, "%f", &ASSIS_RATIO);
				fclose(aratio);

				sscanf_s(buf, "%f %f %f %f %f", &ud1, &ud2, &ud3, &ud4, &ud5);
				printf("ud3 is %f ud4 is %f \n\n", ud3, ud4);
				limit_velo2(ud3, ud4, xyv, MAX_VEL);
				ud3 = xyv[0];
				ud4 = xyv[1];

				printf("now ud3 is %f ud4 is %f \n\n", ud3, ud4);

				//Assistive Control

				txDiff = target.getX() - dot.getX();
				tyDiff = target.getY() - dot.getY();

				if (sqrt(txDiff*txDiff + tyDiff*tyDiff) < 0.5 * CONTACT_RADIUS) {
					desVX = 0;
					desVY = 0;
				}
				else {
					tBase = sqrt(txDiff*txDiff + tyDiff*tyDiff);
					desVX = ASSIS_VRATIO *MAX_VEL * txDiff / tBase;
					desVY = ASSIS_VRATIO *MAX_VEL * tyDiff / tBase;
				}

				printf("desVX is %f desVY is %f \n\n", desVX, desVY);

				dot.setVX(ud3*ASSIS_RATIO + desVX * (1 - ASSIS_RATIO));
				dot.setVY(ud4*ASSIS_RATIO + desVY * (1 - ASSIS_RATIO));

				printf("dot VX is %f dot VY is %f \n\n", dot.getVX(), dot.getVY());

				*/
			

				


				if (!is_waiting) {
					//Move the dot
					dot.move();

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//Render objects
					target.render(&gTarTexture);
					dot.render();

					record.open("record_con.txt", ios::out | ios::app);
					//record the current time / location
					record << dot.getX();
					record << " " << dot.getY();
					record << " " << (current - start_time) << '\n';



					if (Dot::dist(target, dot) > CONTACT_RADIUS) {
						contact_mark = current;

					}

					if (current - start_time > MAX_TIME || current - contact_mark >= CONTACT_TIME) {


						trialnum++;
						if (Dot::dist(target, dot) <= CONTACT_RADIUS) {
							//record << "Completed! " << '\n';
						}
						else {
							//record << "Failed! " << '\n';
						}

						target.shift(150);
						//record << "Trial: " << trialnum << '\n';
						record << "Target: " + to_string(target.getX()) + " " + to_string(target.getY()) << '\n';



						dot.setX(SCREEN_WIDTH / 2);
						dot.setY(SCREEN_HEIGHT / 2);
						wait_start = current;
						is_waiting = true;

					}
				}
				else {
					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					wait.render(&gWaitTexture);

					record.open("record_con.txt", ios::out | ios::app);
					record << "W\n";

					if (current - wait_start > WAIT_TIME) {
						is_waiting = false;
						start_time = current;
						contact_mark = current;
					}
				}

				//Update screen
				SDL_RenderPresent(gRenderer);
				record.close();
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
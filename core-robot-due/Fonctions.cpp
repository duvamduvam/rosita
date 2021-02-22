#pragma once
#include <Wire.h>
#include <DS3231.h>
#include "ArduinoLog.h"
#include "Fonctions.h"


//######### HORLOGE RTC ##########
//--------------------------

/*void ExecuteRtc(){
  
}

void StartRtc(){
    //REAL TIME
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);
}*/

//######### MATHS ##########
//--------------------------


float MapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  if ((in_max - in_min + out_min) == 0)
    return 0;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



//######### TO ROBOT ##########
//--------------------------
//Transformation depuis une coordonée cartésienne d'un joystick vers des commandes de vitesse pour un robot de type char d'assault



SpeedVector ToRobot(int bX, int bY)
{
          //Fractionaire (protocol -> maths)
          float x = MapFloat(bX, 0, 255, -1, 1);
          float y = MapFloat(bY, 0, 255, -1, 1);


          //Recherche du cadran trigonométrique
          char cadran = 0;
          if (x < 0 && y > 0)
            cadran = 1;
          else if (x < 0 && y < 0)
            cadran = 2;
          else if (x > 0 && y < 0)
            cadran = 3;
          
          //  Log.notice("PARSE JOY - C=%d - M=%d \n",cadran,dm);

          int dx = (int)(x * 1000);
          int dy = (int)(y * 1000);

            // Log.notice("PARSE JOY - X=%d - Y=%d \n",dx,dy);

          float angle = atan(y / x);
          float module = sqrt(x * x + y * y);
          if (module > 1)
            module = 1;

         int sousCadran;
          //Configuration des moteurs en fonction de l'angle
          float l, r;
          switch (cadran)
          {
            case 0: //Avance Droite
              l = 1;
              r = MapFloat(angle, 0, PI / 2, -1, 1);
              break;

            case 1: //Avance Gauche
              l = MapFloat(angle, -PI/2, 0, 1, -1);
              r = 1;
              break;

            case 2: //Recul Gauche

              sousCadran=0;
              if (angle > PI / 6 && angle < PI /3)
                sousCadran = 1;
              if (angle > PI / 3)
                sousCadran = 2;

              switch (sousCadran)
              {
                case 0:
                  l = MapFloat(angle, 0, PI / 6, -1, 0);
                  r = MapFloat(angle, 0, PI / 6, 1, 0);
                  break;
                  
                case 1:
                  l = 0;
                  r = MapFloat(angle, PI / 6, PI / 3, 0, -1);
                  break;
                  
                case 2:
                  l = MapFloat(angle, PI / 3, PI / 2, 0, -1);
                  r = -1;
                  break;
              }

              
              break;

            case 3: //Recul Droite

              sousCadran=5;
              if (angle < -PI / 6 && angle > -PI /3)
                sousCadran = 4;
              if (angle < -PI / 3)
                sousCadran = 3;

              switch (sousCadran)
              {
                case 3:
                  l = -1;
                  r = MapFloat(angle, -PI / 2, -PI / 3, -1, 0);
                  break;
                  
                case 4:
                  l = MapFloat(angle, -PI / 3, -PI / 6, -1, 0);
                  r = 0;
                  break;
                  
                case 5:
                  l = MapFloat(angle, -PI / 6, 0, 0, 1);
                  r = MapFloat(angle, -PI / 6, 0, 0, -1);
                  break;
              }
              break;
          }

          int da = (int)(angle * 1000);
          int dm = (int)(module * 1000);

             Log.notice("PARSE JOY - A=%d - M=%d | SC=%d \n",da,dm,sousCadran);


  SpeedVector result;
  result.left = l * module;
  result.right = r * module;

          int dl = (int)(result.left * 1000);
          int dr = (int)(result.right * 1000);


         // Log.notice("PARSE JOY - L=%d - R=%d \n", dl, dr);


  return result;
}


//######### COLORS ##########
//--------------------------
CRGB Brightness(CRGB Base, float intens)
{
  CRGB result;
  if (intens < 0)
    intens = 0;
  if (intens > 2)
    intens = 2;

  if (intens <= 1)
  { //Dimmer
    result.red = Base.red * intens;
    result.green = Base.green * intens;
    result.blue = Base.blue * intens;
  }
  else
  { //brightnass
    int gain = (intens - 1) * 255;
    int r = Base.red + gain;
    int g = Base.green + gain;
    int b = Base.blue + gain;
    if (r > 255)
      r = 255;
    if (g > 255)
      g = 255;
    if (b > 255)
      b = 255;

    result.red = r;
    result.green = g;
    result.blue = b;
  }
  return result;
}

CRGB ColorTintByte(byte tint)
{
  CRGB result;

  tint = 255 - tint;
  if (tint < 85)
  {
    result.red = 255 - tint * 3;
    result.green = 0;
    result.blue = tint * 3;
  }
  else if (tint < 170)
  {
    tint -= 85;
    result.red = 0;
    result.green = tint * 3;
    result.blue = 255 - tint * 3;
  }
  else
  {
    tint -= 170;
    result.red = tint * 3;
    result.green = 255 - tint * 3;
    result.blue = 0;
  }

  return result;
}

CRGB ColorTint(int angle)
{
  return ColorTintByte(map(angle, 0, 360, 0, 255));
}

CRGB RandomColor()
{
  CRGB result;
  result.red = random(255);
  result.blue = random(255);
  result.green = random(255);
  return result;
}

CRGB RandomColorTint()
{
  byte tint = random(255);
  return ColorTint(tint);
}


CRGB Gradient(int startAngle, int stopAngle, float ratio)
{
  float angle = MapFloat(ratio, 0, 1, startAngle, stopAngle);
  return ColorTint((int)angle);
}


//######### UTILS ##########
//--------------------------
char* strtrim_safe( char* input ) {

  int size = sizeof(input);

  if (input[size] == ' ') {
    char copy[size - 1];
    for ( int i = 0; i < size - 1; i++ ) {
      copy[i] = input[i];
    }
  }
  return input;
}

/*void moveServo(Servo servo, int pos)
{ if (0 >= pos <= 180)
  {
    servo.write(pos);
  }
}*/

char* nextToken(char* str)
{
  return strtok(str, "|");
}

// str input string, s start index, e end index return long
long extractTime(char* input, int s, int m) {
  //-32 before that ascii are not easy to but in string
  int seconds = input[s] - 32;
  int minutes = input[m] - 32;
  long total = (minutes * 60 + seconds) * 1000;
  Log.notice("extract time from %s:[%i, %i] %i seconds %i minutes total:%i * 1000 %l\n", input, s, m, seconds, minutes, total);
  return total;
}

// str input string, s start index, e end index return char*
char* extractChar(char* str, int s, int e) {

  char result[e - s];

  int i = 0;
  for (int x = s; x <= e; x++) {
    result[i] = str[x];
    i++;
  }
  Log.notice("extractChar input:\"%s\" int s:%i int e:%i result:\"%s\"\n", str, s, e, result);
  return result;
}


byte arraySize(char array[])
{
  byte cont = 0;
  for (byte i = 0; array[i] != 0; i++)
    cont++;
  return cont;
}

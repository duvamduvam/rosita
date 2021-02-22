#include <Arduino.h>
#include "ArduinoLog.h"
#include "Commands.h"


#define SPEED_MAX  100
#define SPEED_RATIO  1
#define SPEED_ACCELERATION  20 //2ms
#define INCREMENT_ACCELERATION  0.05f //2ms

/*
//left
#define pwm_left 4
#define dir_left 5

//right
#define pwm_right 3
#define dir_right 2
*/


//left
#define pwm_left 3
#define dir_left 2

//right
#define pwm_right 4
#define dir_right 5


class Move
{
  private :

    Command command;
    int speedleft;
    int speedright;
    
    float cmdLeft,cmdRight;
    float spLeft,spRight;

    long lastSpeedTc; //dernier timecode pour tempo acceleration


    // These maintain the current state
    unsigned int previousMillis;  
    unsigned int TimeoutTimer;    

    char* action;
    

  public:


    Move()
    {
      pinMode(pwm_left, OUTPUT);
      pinMode(dir_left, OUTPUT);
      pinMode(pwm_right, OUTPUT);
      pinMode(dir_right, OUTPUT);
    }

    void direct(char* input ) {
      action = input;
    }

    void process(Commands cmd) {

      char* action = cmd.wheel;
      if (strcmp((char*)action, "") != 0){
        
        Log.notice("Move.h 57 move process %s\n", action);
  
        if (strcmp(action, "WF") == 0){
          cmdLeft = SPEED_MAX;
          cmdRight = SPEED_MAX;
        }
        else if (strcmp(action, "WB") == 0){
          cmdLeft = -SPEED_MAX;
          cmdRight = -SPEED_MAX;
        }
        else if (strcmp(action, "WL") == 0){
          cmdLeft = -SPEED_MAX;
          cmdRight = SPEED_MAX;
        }
        else if (strcmp(action, "WR") == 0){
          cmdLeft = SPEED_MAX;
          cmdRight = -SPEED_MAX;
        }
        else if (strcmp(action, "WS") == 0){
          cmdLeft = 0;
          cmdRight = 0;
        } 
      }
      else {
        cmdLeft = cmd.leftSpeed;
        cmdRight = cmd.rightSpeed;
      }


      
      // TODO finish multi action move
      //command.set(action);
      //next();
    }

    void stop()
    {
      cmdLeft = 0;
      cmdRight = 0;
    }

    
    //input structure char[0,1] -> attack, char[2,3,4] -> music, char[5,6] ex : "+ 123? "
    void next() {
      strcpy(action, command.nextCommand(0, 1, 4, 5, 2, 3));
    }


    void execute()
    {

      
            
      if (millis() - lastSpeedTc > SPEED_ACCELERATION)
      {
        lastSpeedTc = millis();


        float deltaL = cmdLeft - spLeft;
        float deltaR = cmdRight - spRight;


        int decCmdL = (int)(cmdLeft * 1000);
        int decCmdR = (int)(cmdRight * 1000);
        
       // Log.notice("JOY - L=%d - R=%d \n", decCmdL,decCmdR);

        if (abs(deltaL)>0)
          if (deltaL>0)
            spLeft+=INCREMENT_ACCELERATION;
          else
            spLeft-=INCREMENT_ACCELERATION;
            
        if (abs(deltaR)>0)
          if (deltaR>0)
            spRight+=INCREMENT_ACCELERATION;
          else
            spRight-=INCREMENT_ACCELERATION;

        int dL=0;
        int dR=0;
        if (spLeft>0)
          dL=1;
         if (spRight>0)
          dR=1;


        int decL = (int)(spLeft * 1000);
        int decR = (int)(spRight * 1000);
        
       // Log.notice("JOY - L=%d - R=%d \n", decL,decR);
          
        speedleft =  abs((int)(spLeft * SPEED_MAX));
        speedright = abs((int)(spRight * SPEED_MAX));

        if (speedleft<10)
          speedleft=0;
        if (speedright<10)
          speedright=0;

       // Log.notice("JOY - L=%d - R=%d \n", speedleft,speedright);

      //  Log.notice("JOY - CMD L=%d - R=%d | CUR L=%d - R=%d | DIR L=%d - R=%d \n", decCmdL,decCmdR, decL,decR,dL,dR);

        digitalWrite(dir_left, dL);
        digitalWrite(dir_right, dR);

        analogWrite(pwm_left, speedleft);
        analogWrite(pwm_right, speedright);
      }

      
      if (millis() - TimeoutTimer > 1000)
      {
        TimeoutTimer = millis();
        stop();
      }
    }

          


    

};

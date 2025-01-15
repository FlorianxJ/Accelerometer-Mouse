#include <Wire.h>
#include <BMI160Gen.h>
#include <Mouse.h>
#include <math.h>


void setup() {
  int range = 2, rate = 1600; // variables for sensor reading range and speed
  bool accelerationsensoractive = false; // variable used to initialy wait for teh sensor to work, and after that reset the mouse if drift was detected
  Wire.begin();  // Initilize Wire communication
  BMI160.begin(BMI160GenClass::I2C_MODE, 0x68); //connect to the BMI 160 using Wire Libary
  BMI160.setAccelerometerRange(range);  //set measuring range to meke the sensor just sensetive enough
  BMI160.setAccelerometerRate(rate);  // set speed at which the sensor updates it´s data, balance it to prevent drift and proccesing required
  Serial.begin(9600); // Serial communication for debugging purposes
  Mouse.begin(); // Start the Mouse library
  // Setup mouse buttons, pullup inverts the signal but prevents unpridictable behaviour
  pinMode(3, INPUT_PULLUP); // left mouse button
  pinMode(4, INPUT_PULLUP); // right mouse button
  pinMode(5, INPUT_PULLUP); // mouse wheel down
  pinMode(6, INPUT_PULLUP); // mouse wheel up
}

void loop() {
    // Wait until the BMI sensor provides non-zero acceleration data
    while(!accelerationsensoractive){
      if (ax != 0 && ay != 0) {
        //save first values to later center the sensor values
        ax_initial=ax;
        ay_initial=ay;
        accelerationsensoractive = true; //make the if statment false to prefent resetting of initial values
      } else {
        double ax, ay, az, speedX, speedY, ax_centerd, pspeedX, pspeedY, ay_centerd, ax_initial, ay_initial;
        int conditionDuration = 5000; // Gewünschte Dauer in Millisekunden
        long currentMillis, previousMillis, timeresetconditionwasmeet; //values to keep track of time
        BMI160.readAccelerometer(ax, ay, az);
        delay(10);
      }
    }
    //Read sensor data to get values for current loop
    BMI160.readAccelerometer(ax, ay, az);
    //Keep track of time
    currentMillis = millis();
    //Subtract the initial values each loop to center the values for calculation
    ax_centerd = ax - ax_initial;
    ay_centerd = ay - ay_initial;
    //Calculate time step for claculating chnage of speed
    double dt = (currentMillis - previousMillis) / 1000;
    //Save time for next loop to be able to calculate the time step for the next loop
    previousMillis=currentMillis
    //Integrate the speed calculated into the current speed
    speedX = pspeedX + (ax_centerd*dt);
    speedY = pspeedY + (ay_centerd*dt);
    //save values for next loop to keep integrating 
    pspeedX = speedX;
    pspeedY = speedY;
    //move the mouse if speed outside of defined values is detected or in other words deadzone to prevent drift
    if(abs(speedX) > 10 || abs(speedY) > 10){
      //Move mouse according to values, caution if sensor is placed in new orientation the sign infront speedX and speedY may need to be changed
      Mouse.move((speedX),0,0);
      Mouse.move(0,(-speedY),0);
    }
  
  //execute button controls
  controls();
  //run quick check for possible drift (if the centerd acceleration data deviates less than a certain value for more than conditionduration milliseconds)
  resetcheck();
  //check for loop time for debug purposes
  Serial.println(previousMillis-millis());
}

void resetcheck(){
  //check if threshold of defiation is met
  if(abs(ax_centerd) < 10 && abs(ay_centerd) < 10){
   timeresetconditionwasmeet += currentMillis - previousMillis;
  }
  // Condition met for defined time trigger reset of mouse
  if (timeresetconditionwasmeet >= conditionDuration) {
  accelerationsensoractive = false;
  } 
}

void controls(){
  // Left key control
  if(digitalRead(3) == 0 && !leftButtonPressed){
  Mouse.press(MOUSE_LEFT);
  }else(){
  Mouse.realese(Mouse_LEFT);
  }

  // Right key control
  if(digitalRead(4) == 0 && !rightButtonPressed){
  Mouse.press(MOUSE_RIGHT);
  } else() {
  Mouse.realese(Mouse_RIGHT);
  }

  // Scroll control
  if (digitalRead(5) == 0){
  Mouse.move(0, 0, -1);
  }
  if (digitalRead(6) == 0){
  Mouse.move(0, 0, 1);
  }
}

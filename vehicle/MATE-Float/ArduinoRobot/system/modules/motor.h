#ifndef MOTOR_H
#define MOTOR_H

class Motor {
    public:
        Motor(byte pin){
            servo.attach(pin);
        }
        void setSpeed(uint8_t& newSpeed){
            speed = newSpeed;
        }
        void setDirection(bool& newDirection){
            direction = newDirection;
        }
        void move(bool defDirection, uint8_t defSpeed){
            servo.writeMicroseconds(pwmGet(defDirection, defSpeed));
        }
        void move(){    // Overloaded version
            servo.writeMicroseconds(pwmGet(direction, speed));
        }
        void maxF(){
            move(0, 255);
        }
        void maxR(){
            move(1, 255);
        }
        void stop(){
            move(0,0);
        }
    private:
        Servo servo;    // The Servo object can only be directly accessed in the class
        uint8_t speed = 255;   // Create value to control speed
        bool direction = 0;
        uint16_t pwmRange[2] = {500, 2500};
        uint16_t pwmStop = 1500;
        uint16_t pwmGet(bool& defDirection, uint8_t& defSpeed){
            if(defSpeed == 0){
                return pwmStop;
            } else if(defDirection == 0){
                return map(defSpeed, 255, 0, pwmRange[0], pwmStop);
            } else if(defDirection == 1){
                return map(defSpeed, 0, 255, pwmStop, pwmRange[1]);
            }    
        }
};

#endif
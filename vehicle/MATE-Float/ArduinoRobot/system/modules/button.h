#ifndef BUTTON_h
#define BUTTON_h

class Button{
    public:
        Button(byte p){
            buttonPin = p;
        }
        bool pressed(){
            return digitalRead(buttonPin);
        }
    private:
      byte buttonPin;
};

#endif
#include "SD.h" //Lib to read SD card
#include "TMRpcm.h" //Lib to play audio
#include "SPI.h" //SPI lib for SD card

#define SD_ChipSelectPin 4 //Chip select is pin number 4

TMRpcm music; //lib object
int num;
int count;
double sum;
bool p;
String audioclip;

void setup() {
  music.speakerPin = 9; //Audio out on pin 9
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }
  music.setVolume(5);    //5 is max
  music.quality(1);    
  music.loop(1); 
  num = 1000;
  sum = 0;
  count = 0;
  audioclip = getRandomFile();
  p = false;
}

void loop() { //reads the voltage and finds the average over a certain looping period
  int input = analogRead(A0); 
  double volts = input * (5.0 / 1023.0);
  sum += volts;
  count++;
  if(count == num) { //concept of debouncing 
      sum /= count;
      if (sum > 1.0 && !p) {
        play(audioclip);
        p = true;
      } else if (sum < 1.0 && p) {
        stopplaying();
        p = false;
      }
      //Serial.println(sum);
      sum = 0;
      count = 0;
  }  
}

String getRandomFile() { //runs through the sd card directory twice Once finding total wavs contained and the other picking the random one
  File dir = SD.open("/");
  int filenums = 0;
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    if (entry.isDirectory()) {
      continue;
    } 
    String name(entry.name());
    name.toUpperCase();
    Serial.println(name);
    if(name.indexOf(".WAV") > 0); {
      filenums++;
    }
    entry.close();
  }
  randomSeed(analogRead(1)); //needed seed for random 
  int ran = random(filenums); //random number out of total wav files 
  dir.rewindDirectory(); //you have to rewind the directory to start from the beginning 
  int i = 0;
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    if (entry.isDirectory()) {
      continue;
    }
    String name(entry.name());
    name.toUpperCase();
    if(name.indexOf(".WAV") > 0) {
      if (i == ran) {
        Serial.println("Playing");
        Serial.println(entry.name());
        return String(entry.name());
      }
      i++;
    }
    entry.close();
  }
  dir.close();
}

void play(String playstr) { //wrapper function to play 
  Serial.println("In Play Function");
  music.play((char*) playstr.c_str(),0);
}

void stopplaying() { //wrapper function to stop playing
  Serial.println("Snooze Button Pressed");
  music.stopPlayback();
}

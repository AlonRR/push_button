#include <Arduino.h>

#define BUTTON_DEBOUNCE 50

struct input23_s
{
  bool state;
  bool last_state;
  unsigned long current_time;
  unsigned long last_time;
  unsigned long debounce_time;
  unsigned count_blinks;
} input23;

struct output21_s
{
  bool state;
  unsigned long last_change_time;
  unsigned long change_delay;
} output21;

// put function declarations here:
void change_state21();
bool track_state23();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(23, INPUT);
  input23.count_blinks = 0;
}

void loop()
{
  // put your main code here, to run repeatedly:
  bool state23 = track_state23();
  if (!state23)
    return;
  change_state21();
  if (input23.debounce_time > 5000)
    repeat_pulses21();

  // delay(100);
}

// toggle for output 21
void change_state21()
{
  static int state = LOW;
  digitalWrite(21, state);
  state = !state;
}

// tracks the operation of input 23; returns true for change
bool track_state23()
{
  unsigned long current_time = millis();
  int current_state = digitalRead(23);
  if (input23.state == current_state)
    return false;
  if (input23.last_time + BUTTON_DEBOUNCE > current_time)
    return false;
  Serial.println(current_time);
  input23.state = current_state;
  input23.debounce_time = current_time - input23.last_time;
  return true;
}



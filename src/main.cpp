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

struct phase_s
{
  bool state23;
  bool repeater21;
} phase;

// put function declarations here:
void change_state21();
void track_change23();
void monitor_pulse21();
void repeat_pulses21();

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(23, INPUT);
  input23.count_blinks = 0;
  phase.repeater21 = false;
  phase.state23 = false;
}

void loop()
{
  if (phase.repeater21)
    repeat_pulses21();
  track_change23();
  if (!phase.state23)
    return;
  change_state21();
  monitor_pulse21();
  // delay(100);
}

// toggle switch for output 21
void change_state21()
{
  static int state = LOW;
  digitalWrite(21, state);
  state = !state;
}

// tracks the operation of input 23; returns true for change
void track_change23()
{
  unsigned long current_time = millis();
  int current_state = digitalRead(23);
  if (input23.state == current_state)
    return;
  if (input23.last_time + BUTTON_DEBOUNCE > current_time)
    return;
  Serial.println(current_time);
  input23.state = current_state;
  input23.debounce_time = current_time - input23.last_time;
  phase.state23 = true;
  return;
}

// monitors the on, off, and time of the switch
void monitor_pulse21()
{
  static int last_state = -1;
  if (last_state == -1)
  {
    input23.count_blinks = 0;
    last_state = input23.state;
    return;
  }
  if (output21.state != LOW)
    return;
  if (input23.debounce_time < 1000)
  {
    input23.count_blinks++;
    return;
  }
  if (input23.debounce_time > 5000)
  {
    phase.repeater21 = true;
    return;
  }
  if (input23.debounce_time > 1000)
  {
    input23.count_blinks = 0;
    return;
  }
}

// runs a full pulse on then off
void repeat_pulses21()
{
  //runs 2 times
  for (size_t i = 0; i < 2; i++)
  {
    change_state21();
    delay(250);
  }
}

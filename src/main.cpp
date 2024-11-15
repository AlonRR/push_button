#include <Arduino.h>

#define BUTTON_DEBOUNCE 50

struct input23_s
{
  bool state;
  bool last_state;
  unsigned long current_time;
  unsigned long last_time;
  unsigned long debounce_time;
  int count_blinks;
} input23{false, false, 0, 0, 0, 0};

struct output21_s
{
  bool state;
  unsigned long last_change_time;
  unsigned long change_delay;
} output21{false, 0, 0};

struct phase_s
{
  bool state23;
  bool repeater21;
} phase{false, false};

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
  digitalWrite(21, LOW);
  input23.state = digitalRead(23);
  input23.last_state = input23.state;
  input23.last_time = millis();

  printf("Setup complete\n");
}

void loop()
{
  // delay(1000);
  if (phase.repeater21 == true)
    repeat_pulses21();
  track_change23();
  if (phase.state23 == false)
    return;
  change_state21();
  monitor_pulse21();
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
  bool current_state = (bool)digitalRead(23);
  if (input23.state == current_state)
    return;
  if (input23.last_time + BUTTON_DEBOUNCE >= current_time)
    return;
  Serial.println(current_time);
  input23.state = current_state;
  input23.debounce_time = current_time - input23.last_time;
  input23.last_time = current_time;
  phase.state23 = true;
  return;
}

// monitors the on, off, and time of the switch
void monitor_pulse21()
{
  phase.state23 = false;
  printf("Monitor pulse: %d\n", input23.debounce_time);
  if (output21.state != LOW)
    return;
  if (input23.debounce_time > 1000)
  {
    input23.count_blinks = 0;
    return;
  }
  input23.count_blinks++;
}

// runs a full pulse on -> delay -> off
void repeat_pulses21()
{
  if (input23.count_blinks == 0)
    return;
  if (input23.state == HIGH)
  {
    digitalWrite(21, LOW);
    delay(1000);
  }
  for (size_t i = 0; i < input23.count_blinks; i++)
  {
    change_state21();
    delay(1000);
  }
  if (input23.state == HIGH)
    digitalWrite(21, HIGH);
  phase.repeater21 = false;
  input23.count_blinks = 0;
}

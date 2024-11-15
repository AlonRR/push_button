#include <Arduino.h>

#define BUTTON_DEBOUNCE 50

struct loop_time_t
{
  unsigned long now;
  unsigned long last_click;
  unsigned long difference;
  void update_now()
  {
    now = millis();
  }
  void reset_last()
  {
    last_click = now;
    difference = 0;
  }
  void update()
  {
    update_now();
    difference = now - last_click;
  }
} loop_time{0, 0, 0};

struct io_state_t
{
  bool light21;
  bool button23;
  bool button23_last;
  int count_blinks;
  void change_light21()
  {
    light21 = !light21;
    digitalWrite(21, light21);
  }
  void add_count_blinks()
  {
    count_blinks++;
  }
  void reset_count_blinks()
  {
    count_blinks = 0;
  }
  void get_button23_read()
  {
    button23 = digitalRead(23);
  }
  void reset_button23_last()
  {
    button23_last = button23;
  }
  void repeater21()
  {
    if (count_blinks == 0)
      return;
    if (count_blinks == 1)
    {
      change_light21();
      reset_count_blinks();
      return;
    }
    if (light21 == HIGH) // if light is on, turn off, delay and adjust for uneven count
    {
      count_blinks--;
      change_light21();
      delay(1000);
    }
    for (size_t i = 0; i < count_blinks; i++)
    {
      change_light21();
      delay(250);
    }
    printf("light21: %d, count_blinks: %d\n", light21, count_blinks);
    printf("Repeater21 complete\n");
    reset_count_blinks();
  }
} io_state{LOW, LOW, LOW, 0};

void check_button23()
{
  io_state.get_button23_read();
  if (io_state.button23 == io_state.button23_last)
    return;
  io_state.reset_button23_last();
  io_state.change_light21();
  if (loop_time.difference > 1000)
    io_state.reset_count_blinks();
  io_state.add_count_blinks();
  loop_time.reset_last();
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(23, INPUT);
  digitalWrite(21, LOW);
  io_state.light21 = LOW;
  io_state.button23 = digitalRead(23);
  loop_time.update_now();
  loop_time.reset_last();
  printf("Setup complete\n");
}

void loop()
{
  loop_time.update();
  if (loop_time.difference < BUTTON_DEBOUNCE)
    return;
  check_button23();
  if (loop_time.difference > 5000)
    io_state.repeater21();
}
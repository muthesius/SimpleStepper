/*
  Die Pins:
  */
const int motor_enable = 12;
const int motor_dir = 11;
const int motor_step = 5;

const int _motor_ms_1 = 8;
const int _motor_ms_2 = 9;
const int _motor_ms_3 = 10;


void goStundenInMinuten(int stunden, float minuten) {
  go( stunden * 5, minuten * 60);
}

void loop()
{
  switch (Serial.read()) {

      case 'm':
        go(6 * 5, 30);
        break;

      case 'l':
        // 6 stunden auf dem blatt in 2 minuten
        goStundenInMinuten(6, 2);
        break;


      case 'h':
        goStundenInMinuten(12, 30);
        break;
  }
}


/* ------------------- Stepper Code ---------------------- */

#define STEPMIN 800
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

int _torque;
int step_time;

void enable_motors() {
  digitalWrite(motor_enable, LOW);
}

void disable_motors() {
  digitalWrite(motor_enable, HIGH);
}

void setup_motors() {
  pinMode(motor_step, OUTPUT);
  pinMode(motor_dir, OUTPUT);
  pinMode(motor_enable, OUTPUT);
  pinMode(_motor_ms_1, OUTPUT);
  pinMode(_motor_ms_2, OUTPUT);
  pinMode(_motor_ms_3, OUTPUT);
  disable_motors();
}

void current_control(int step) {
  if(step == 1)
  {
    step_time = STEPMIN + (_torque * 260);
  }
  else if(step == 2)
  {
    step_time = (STEPMIN + (_torque * 260)) / 2;
  }
  else if(step == 4)
  {
    step_time = (STEPMIN + (_torque * 260)) / 4;
  }
  else if(step == 8)
  {
    step_time = (STEPMIN + (_torque * 260)) / 8;
  }
  else if(step == 16)
  {
    step_time = (STEPMIN + (_torque * 260)) / 16;
  }
}

void set_step_mode(int step_size) {
  switch(step_size) {
    case 1:
      digitalWrite(_motor_ms_1, LOW);
      digitalWrite(_motor_ms_2, LOW);
      digitalWrite(_motor_ms_3, LOW);
      break;
    case 2:
      digitalWrite(_motor_ms_1, HIGH);
      digitalWrite(_motor_ms_2, LOW);
      digitalWrite(_motor_ms_3, LOW);
      break;
    case 4:
      digitalWrite(_motor_ms_1, LOW);
      digitalWrite(_motor_ms_2, HIGH);
      digitalWrite(_motor_ms_3, LOW);
      break;
    case 8:
      digitalWrite(_motor_ms_1, HIGH);
      digitalWrite(_motor_ms_2, HIGH);
      digitalWrite(_motor_ms_3, LOW);
      break;
    case 16:
      digitalWrite(_motor_ms_1, HIGH);
      digitalWrite(_motor_ms_2, HIGH);
      digitalWrite(_motor_ms_3, HIGH);
      break;
  }
}

void step(int step_size, int number_of_steps, int torque) {
  _torque = torque;

  int dir = (number_of_steps > 0) ? HIGH : LOW;
  number_of_steps = abs(number_of_steps);

  digitalWrite(motor_dir, dir);

  set_step_mode(step_size);
  current_control(step_size);

  // enable_motors();
  for(int i=1;i<=number_of_steps;i++)
  {
    //low to high transition moves one step
    sbi(PORTD, 5);
    delayMicroseconds(step_time); //high time
    cbi(PORTD, 5);
    delayMicroseconds(step_time); // low time
  }
  cbi(PORTD, 5);
  // disable_motors();
}

void setup()
{
  setup_motors();
  Serial.begin(9600);
}

// 90/(1,8/16)
void step_degrees(float degrees, int step_size) {
  int num_steps = degrees / (1.8 / (float) step_size);
  Serial.println(num_steps);
  step(step_size, num_steps, 0);
}

void go(int minute_parts, float in_sekunden) {
  // 360/(12*5) => 6°
  int degrees = 6 * minute_parts;
  int step_size = 16;
  int num_steps = degrees / (1.8 / (float) step_size);

  enable_motors();
  if (in_sekunden <= 0) {
    step(step_size, num_steps, 0);
  } else {

    for(int i=0; i<num_steps; i++){
        step(step_size, 1, 0);
        delay( (1000 * in_sekunden) / num_steps);
    }

  }
  disable_motors();
}


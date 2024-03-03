#define lMotPin      8
#define rMotPin      7
#define motFreq      500
#define resolution   8 // # of bits
#define lMotChannel  0
#define rMotChannel  1

#define neutralValue 189


class Drive_Motors {
  public:
    Drive_Motors() {}
    void init_motors();
    void l_motor_write(int value);
    void r_motor_write(int value);
    void set_both_motors(int value);
    bool isNeutral();
    bool flip_motors = 0;
  private:
    int l_motor_value = 0;
    int r_motor_value = 0;
};


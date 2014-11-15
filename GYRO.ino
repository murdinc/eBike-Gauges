#define FREQ 30.0 // sample freq in Hz
#define MPU 0x68 // I2C address of the MPU-6050

//int8_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
//static int16_t accX = 0, accY = 0;
static int ampX = 64, ampY=32;


void initGyro(){
  int error;
  uint8_t c;
  uint8_t sample_div;
  
   // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();
  
  // PWR_MGMT_1: // wake up 
  i2c_write_reg (MPU, 0x6b, 0x00);
  
  // CONFIG: // Low pass filter samples, 1khz sample rate
  //i2c_write_reg (MPU, 0x1a, 0x01);
  
  // GYRO_CONFIG: // 500 deg/s, FS_SEL=1 // This means 65.5 LSBs/deg/s
  //i2c_write_reg(MPU, 0x1b, 0x08);
  
  // CONFIG:
  // set sample rate
  // sample rate FREQ = Gyro sample rate / (sample_div + 1)
  // 1kHz / (div + 1) = FREQ  
  // reg_value = 1khz/FREQ - 1
  sample_div = 100 / FREQ - 1;
  i2c_write_reg (MPU, 0x19, sample_div);

}
void readAccel(){
  uint8_t i2cData[6];
  uint8_t error;
  static uint16_t accX, accY;
  
  error = i2c_read(MPU, 0x3b, i2cData, 4);
  if(error!=0)
  return;

  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  
  amplitude(accX, 128, &ampX);
  amplitude(accY, 64, &ampY);
}

void amplitude(int val, int width, int *amp) {
  //*amp = ( val * width) / width;
  //if (*amp == width) *amp = width - 1; 
  *amp = ((float)val*16.0/15000.0);
}

int i2c_read(int addr, int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);
  if (n != 1)
  return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
  return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(addr, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
  return (-11);

  return (0);  // return : no error
}

int i2c_write(int addr, int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);        // write the start address
  if (n != 1)
  return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
  return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
  return (error);

  return (0);         // return : no error
}

int i2c_write_reg(int addr, int reg, uint8_t data)
{
  int error;
  
  error = i2c_write(addr, reg, &data, 1);
  return (error);
}

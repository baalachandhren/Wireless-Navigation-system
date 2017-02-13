//
#define MOSI   11       // SPI master data out pin
#define MISO   12       // SPI master data in pin
#define SCK    13       // SPI clock pin
#define CS     10       // SPI slave select pin

int pt = 0;             // Text buffer pointer
unsigned char data[4]; // Data buffer
unsigned char res;
int pd = 0;             // Data buffer pointer
unsigned char c;
int count, tempC, avgtempC=0.0;


int dispch;  

char logo[3];
unsigned char config[] = // List of modified registers
{
  0x00, 0x29,           // IOCFG2   GDO2 Output Pin Configuration
  0x01, 0x2E,           // IOCFG1   GD01 Output Pin Configuration
  0x02, 0x06,           // IOCFG0   GDO0 Output Pin Configuration
  0x03, 0x07,           // FIFOTHR  RX FIFO and TX FIFO Thresholds
  0x07, 0x01,           // PKTCTRL0 Adress Check enabled
  0x09, 0x01,           // ADDR     Device Address Address for this node is 1
  0x0A, 0x03,           // CHANNR   Channel Number
  0x0B, 0x06,           // FSCTRL1  Frequency Synthesizer Control
  0x0D, 0x5D,           // FREQ2    Frequency Control Word, High Byte
  0x0E, 0x93,           // FREQ1    Frequency Control Word, Middle Byte
  0x0F, 0xB1,           // FREQ0    Frequency Control Word, Low Byte
  0x10, 0x7A,           // MDMCFG4  Modem Configuration
  0x11, 0x2E,           // MDMCFG3  Modem Configuration
  0x12, 0x03,           // MDMCFG2  Modem Configuration
  0x15, 0x44,           // DEVIATN  Modem Deviation Setting
 // 0x17, 0x03,           // MCSM0    Main Radio Control State Machine Configuration
  0x18, 0x18,           // MCSM0    Main Radio Control State Machine Configuration
  0x19, 0x16,           // FOCCFG   Frequency Offset Compensation Configuration
  0x1A, 0x6C,           // BSCFG    Bit Synchronization Configuration
  0x1B, 0x43,           // AGCCTRL2 AGC Control
  0x1C, 0x00,           // AGCCTRL1 AGC Control
  0x1D, 0xB2,           // AGCCTRL0 AGC Control
  0x21, 0x56,           // FREND1   Front End RX Configuration
  0x22, 0x10,           // FREND0   Front End RX Configuration
  0x23, 0xA9,           // FSCAL3   Frequency Synthesizer Calibration
  0x24, 0x0A,           // FSCAL2   Frequency Synthesizer Calibration
  0x25, 0x00,           // FSCAL1   Frequency Synthesizer Calibration
  0x26, 0x11,           // FSCAL0   Frequency Synthesizer Calibration
  0x3E, 0xFF            // PATABLE  Power setting to PATABLE memory using single access write. See table 31 on page 47 where 0xA9 = 16.2 mA
};

#define spiEnable() digitalWrite(CS, LOW); while(digitalRead(MISO) == HIGH);
#define spiDisable() digitalWrite(CS, HIGH);

unsigned char spiTransfer(volatile unsigned char data)
{
  SPDR = data;                              // Start transmission
  while(!(SPSR & (1 << SPIF)));             // Wait for end of transmission
  return SPDR;                              // Return received byte
}

unsigned char sendByte(unsigned char data)
{
  spiEnable();                              // Enable device
  unsigned char result = spiTransfer(data); // Send byte
  spiDisable();                             // Disable device
  return result;
}

#define sendStrobeCommand(command) sendByte(command)

unsigned char sendCommand(unsigned char command, unsigned char data)
{
  spiEnable();                              // Enable device
  spiTransfer(command);                     // Send command byte
  unsigned char result = spiTransfer(data); // Send data byte
  spiDisable();                             // Disable device
  return result;
}

unsigned char sendBurstCommand(unsigned char command, unsigned char* data, unsigned char length)
{
  spiEnable();                              // Enable device
  spiTransfer(command);                     // Send command byte
  unsigned char result = 0;
  for(int i = 0; i < length; i++)           // Send data bytes
  {
    result = spiTransfer(data[i]);
    data[i] = result;
  }
  spiDisable();                             // Disable device
  return result;
}

void reset_device()
{
    digitalWrite(CS, LOW);                  // Enable device
    delayMicroseconds(1);
    digitalWrite(CS, HIGH);                 // Disable device and wait at least 40 microseconds
    delayMicroseconds(41);
    spiEnable();                            // Enable device
    spiTransfer(0x30);                      // SRES: reset device
    spiDisable();                           // Disable device
}

void init_registers()
{
  for(char i = 0; i < sizeof(config); i += 2)
  {
    sendCommand(config[i], config[i + 1]);  // Initialize device registers
  }
}

void setup()
{
  Serial.begin(9600);                     // Setup serial port

  // Initialize the CC2500
  pinMode(MOSI, OUTPUT);                    // Setup SPI pins mode
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);                   // Disable device
  SPCR = (1 << SPE) | (1 << MSTR);          // Setup SPI control register: SPCR = 01010000 (interrupt disabled, SPI enabled, msb 1st, master, clk low when idle, sample on rising edge of clk, system clock rate fastest)
  unsigned char clr = SPSR;                 // Clear data registers
  clr = SPDR;                               // Clear data registers
  
  reset_device();                           // Reset device twice
  reset_device();

  init_registers();                         // Initialize device registers
  sendStrobeCommand(0x36);                  // SIDLE: exit RX/TX
  sendStrobeCommand(0x36);                  // SIDLE: exit RX/TX
  sendStrobeCommand(0x39);                  // SPWD: enter power down mode when CSn goes high
  sendStrobeCommand(0x36);                  // SIDLE: exit RX/TX
  Serial.println("CC2500 initialized");
   sendStrobeCommand(0x34);  // RX Mode
   
   

}

void loop()
{
 
    unsigned char channel = 0x03;
 
  unsigned char result;
unsigned char res;  // Number of bytes received
  char rssi,c;                                // RSSI value
  int k=0;                                  // # of RSSI samples

  unsigned int i = 0;


 if(Serial.available())
  {
    
    data[0] = 0x03;           // Set length to 3 having 1 byte len, 1 byte addr, 1 byte data 
    data[1] = 0x02;           // Set Rx device address as 2

    c = (char)Serial.read();
   // delay(100); 
       data[2] = c;       
//  sendStrobeCommand(0x36);                  // SIDLE: exit RX/TX       
 
  //  Serial.print(c);
     sendStrobeCommand(0x35);  // Change to TX
     res = sendBurstCommand(0x7F, data, sizeof(data));      // Fill the TX FIFO
    //res = sendCommand(0x3F, c);    
    // sendStrobeCommand(0x31); 
    do
    {
      delay(10);
      result =  sendStrobeCommand(0x3D); 
       result=result&0x70;
    }while(result==0x20);
 
  }
  
  sendStrobeCommand(0x34); //Return Back to RX
  if((result = sendCommand(0xFB, 0x00)) >2) 
  {
  
  for(int i = 0; i < result; i++)           // Read the RX FIFO
  {
     char c = sendCommand(0xBF, 0x00);       // Read one byte from the RX FIFO
   if(i == 2)
  {
   
    Serial.write(c);              // Send the byte to the serial monitor
  }   
  }
     
 
    // rssi = sendCommand(0xF4, 0x00);

  sendStrobeCommand(0x36);                  // SIDLE: exit RX/TX
 // sendStrobeCommand(0x3A);                  // SFRX: flush the RX FIFO buffer
 //  sendStrobeCommand(0x34);                  // SRX: enable RX
  }

}



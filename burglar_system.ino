// nokia frame layout
struct Message{
     char id;
     char destination;
     char source;
     char type;
     char unknown;
     char data_size;
     char data[25];// most frames are under 25 bytes
     char checksum[2];
};
// for recording frames
Message buffer[8];
int message_led = 13;
int blue_led = 12;
int mode = 0;
int datpt = 0;
int bufpt = 0;
volatile bool received = false;
volatile bool got = false;

// for motion
uint8_t kitchen_sensor = 7;
uint8_t livingroom_sensor = 6;
uint8_t bedroom_sensor = 5;
uint8_t sisterroom_sensor = 4;
uint8_t compare_times = 50;
uint8_t sensibility = 4;
bool motion;

// sequence number of acknowledge frames
int ack_seq = 0;
// sequence number for sending command frames
int com_seq = 0;

// print formats
char hexFormat[7] = "%.02x ";
char charFormat[4] = "%c ";

// oct format phone numbers
// family, number for burglar alarm
char silbo_simpel[4] = {0x15, 0x39, 0x74, 0x26}; //519 34762
char silbo_super[4] = {0x75, 0x04, 0x27, 0x96};  //574 07269

// frames 0xd1,0x05,0x00,0x01,0x00,0x03,0x00
unsigned char init_frame[7] = { 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x40 };
unsigned char command_frame[14] = { 0x00, 0x01, 0x00, 0x10, 0x07, 0x01, 0x02, 0x06, 0x0A, 0x14, 0x17, 0x39, 0x01, 0x40 };
unsigned char call_frame[31] = { 0x00, 0x01, 0x00, 0x01, 0x08, 0x00, 0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x37, 0x00, 0x35, 0x00, 0x36, 0x00, 0x33, 0x00, 0x35, 0x05, 0x01, 0x05, 0x00, 0x02, 0x00, 0x00, 0x01, 0x01, 0x40 };
//unsigned char call_frame[23] = { 0x00, 0x01, 0x00, 0x01, 0x04, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x35, 0x05, 0x01, 0x05, 0x00, 0x02, 0x00, 0x00, 0x01, 0x01, 0x40 };
//unsigned char call_frame2[26] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x07, 0x04, 0x01, 0x00, 0x03, 0x0E, 0x00, 0x00, 0x00, 0x04, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x35, 0x01, 0x40 };
unsigned char call_frame2[34] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x07, 0x04, 0x01, 0x00, 0x03, 0x16, 0x00, 0x00, 0x00, 0x08, 0x00, 0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x37, 0x00, 0x35, 0x00, 0x36, 0x00, 0x33, 0x00, 0x35, 0x01, 0x40 };
//unsigned char call_frame[31] = { 0x00, 0x01, 0x00, 0x01, 0x08, 0x00,  '5', 0x00,  '1', 0x00,  '9', 0x00, '3', 0x00, '4', 0x00, '7', 0x00, '6', 0x00, '2', 0x05, 0x01, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x40 };
//unsigned char call_frame2[34] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x07, 0x04, 0x01, 0x00, 0x03, 0x16, 0x00, 0x00, 0x00, 0x08, 0x00, '5', 0x00, '1', 0x00, '9', 0x00, '3', 0x00, '4', 0x00, '7', 0x00, '6', 0x00, '2', 0x01, 0x40 };
// sms frame with message "silbo"
unsigned char sms_frame_silbo[64] = { 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x55, 0x55, 0x01, 0x02, 0x34, 0x11, 0x00, 0x00, 0x00, 0x00, 0x04, 0x82, 0x0c, 0x01, 0x06, 0x08, 0x81, 0x75, 0x04, 0x27, 0x96, 0x00, 0x00, 0x82, 0x0c, 0x02, 0x07, 0x06, 0x91, 0x73, 0x52, 0x90, 0x09, 0x00, 0x00, 0x80, 0x10, 0x05, 0x05, 0xf3, 0x34, 0x5b, 0xfc, 0x06, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x08, 0x04, 0x01, 0xa9, 0x01, 0x40 };
// sms frame with message "alarm"
unsigned char sms_frame_alarm[64] = { 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x55, 0x55, 0x01, 0x02, 0x34, 0x11, 0x00, 0x00, 0x00, 0x00, 0x04, 0x82, 0x0c, 0x01, 0x06, 0x08, 0x81, 0x35, 0x39, 0x71, 0x22, 0x00, 0x00, 0x82, 0x0c, 0x02, 0x07, 0x06, 0x91, 0x73, 0x52, 0x90, 0x09, 0x00, 0x00, 0x80, 0x10, 0x05, 0x05, 0xe8, 0x70, 0x5a, 0x5e, 0x06, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x08, 0x04, 0x01, 0xa9, 0x01, 0x40 };
unsigned char torch_frame[14] = { 0x00, 0x01, 0x00, 0x10, 0x07, 0x01, 0x02, 0x06, 0x0A, 0x14, 0x17, 0x39, 0x01, 0x40 };
unsigned char torch_frame_on[16] = { 0x00, 0x01, 0x00, 0x05, 0x10, 0x01, 0x00, 0x00, 0x00, 0x01, 0x05, 0x04, 0x02, 0x00, 0x01, 0x40 };
unsigned char torch_frame_off[16] = { 0x00, 0x01, 0x00, 0x05, 0x10, 0x02, 0x00, 0x00, 0x00, 0x01, 0x05, 0x04, 0x02, 0x00, 0x01, 0x40 };

void changePhonenumber(char *number, unsigned char *data, int len)
{
    // replace info
    data[22] = len; //number lengths
    data[23] = 0x81; // national (0x81) or international (0x91) number
    // replace number
    memcpy( data+24, number, 4 );
}

int waitForAnswer()
{
    // TODO: implement retry message sending
    long time = millis() + 2000;
    // wait for replay with timeout 2 seconds
    while ( !received ) {
        phoneReceive();
        if ( millis() > time ) {
            //switchTorch( HIGH );
            return 1;
        }
    }
    received = false;
    return 0;
}

int getCommandSequence()
{
    com_seq = ( ++com_seq ) & 0x07;
    return com_seq;
}

int sendFrame(int type, int data_size, unsigned char *data, int sequence)
{
    unsigned char buf[256];
    int at, n, check, len;
    unsigned short *p;

    at = 0;

    // build header
    buf[at++] = 0x1e;        // fbus frame id for cable
    buf[at++] = 0x00;        // phone device ID (destination)
    buf[at++] = 0x0c;        // pc or mcu device ID (source)
    buf[at++] = type;        // command type
    buf[at++] = 0x00;        // unknown (multiframed command?)
    buf[at++] = data_size;   // message length in bytes

    // add data
    for ( n = 0; n < data_size; ++n ) {
        buf[at++] = data[n];
    }
    
    // clean last 3 bits of data and add 3 bit sequence
    buf[at-1] = buf[at-1] & 0xF8;
    buf[at-1] = buf[at-1] | sequence;
    
    // if odd numbered, add filler byte
    if ( data_size % 2 ) {
        buf[at++] = 0x00;
    }

    // calculate checksums
    check = 0;
    p = ( unsigned short * )buf;
    len = at / 2;
    for ( n = 0; n < len; ++n )
        check ^= p[n];
    p[n] = check;
    at += 2;

    // send the message!
    for ( n = 0; n < at; ++n ) {
        Serial.write( buf[n] );
    }
}

void sendAcknowledge(int type, int sequence)
{
    // TODO: iplement auto ack
    unsigned char buf[2];

    buf[0] = type;
    // TODO: implement sequence increment
    // will be done in sendFrame()
    //buf[1] = sequence;

    // send the acknowledge frame
    sendFrame( 0x7f, 2, buf, sequence );
}

int syncPhone()
{
    int n;
    // send syncronizing bytes
    for ( n = 0; n < 250; n++ ) {
        Serial.write( 0x55 );
    }
    return 0;
}

int initPhone()
{
    // send initalization frame, until it is acknowledged
    do sendFrame( 0xD1, 7, init_frame, 0 );
    while ( waitForAnswer() );
    
    int sequence = getCommandSequence();
    // send initalization frame again
    do sendFrame( 0xD1, 7, init_frame, sequence );
    while ( waitForAnswer() );
    
    // if no response for init frame
    if ( waitForAnswer() ) return 1;
    // init frame was successfull, acknowledge it
    sendAcknowledge( 0xD2, ack_seq );
    
    return 0;
}

int sendSms(char *number, int number_length, unsigned char *sms_frame, int sms_length)
{
    // change to right phone number
    changePhonenumber( number, sms_frame, number_length );
    // send sms frame
    do sendFrame( 0x02, sms_length, sms_frame, getCommandSequence() );
    while ( waitForAnswer() );
    // wait for response
    waitForAnswer();
    
    sendAcknowledge( 0x02, ack_seq );
    return 0;
}

int makeCall()
{
    sendFrame( 0x10, 17, command_frame, getCommandSequence() );
    sendFrame( 0x01, 31, call_frame, getCommandSequence() );
    // wait acknowledge for both sent frames
    waitForAnswer();
    waitForAnswer();
    // response
    waitForAnswer();
    sendAcknowledge( 0x01, ack_seq );
    
    // verify call
    sendFrame( 0x01, 34, call_frame2, getCommandSequence() );
    waitForAnswer();
    sendAcknowledge( 0x01, ack_seq );
    waitForAnswer();
    sendAcknowledge( 0x01, ack_seq );
    waitForAnswer();
    sendAcknowledge( 0x01, ack_seq );
    
    return 0;
}

int switchTorch( boolean state )
{
    sendFrame( 0x10, 14, torch_frame, getCommandSequence() );
    if ( state == HIGH )
      sendFrame( 0x3A, 16, torch_frame_on, getCommandSequence() );
    else
      sendFrame( 0x3A, 16, torch_frame_off, getCommandSequence() );
    
    // wait for both ack
    waitForAnswer();
    waitForAnswer();
    
    sendAcknowledge( 0x3A, ack_seq );
    return 0;
}

bool isMotion( uint8_t sensor, uint8_t times, uint8_t sensibility )
{
  int i, value, temp;
  // read first value to compare motion
  value = analogRead( sensor );
  // check x times for motion
  for ( i = 0; i < times; i++ ) {
    temp = value - analogRead( sensor );
    // if the absolute of temp is out of sensibility range, motion has occured
    if ( abs( temp ) > sensibility ) return true;
    delay(10);
  }
  return false;
}

void setup() 
{
    // assign indicator led
    pinMode( message_led, OUTPUT );
    pinMode( blue_led, OUTPUT );
    // init motion sensor pins
    digitalWrite( sisterroom_sensor, LOW );
    digitalWrite( beroom_sensor, LOW );
    digitalWrite( livingroom_sensor, LOW );
    digitalWrite( kitchen_sensor, LOW );
    // TODO: figure out where to send debug message
    // FBUS runs on 115200 baudrate
    Serial.begin( 115200 );
    // wait for 2 seconds before we start the system
    delay( 2000 );
    
    // send the syncronization frame
    syncPhone();
    // send initialization frame
    initPhone();
    
    // startup init for user, blink flashlight
    // so user knows that everything is working
    switchTorch( HIGH );
    delay(1000);
    switchTorch( LOW );
    
    // send sms to specific number with specific text
    //sendSms(silbo_simpel, 8, sms_frame_silbo, 64);
    //sendSms(silbo_super, 8, sms_frame_alarm, 64);
    
    //makeCall();
    
}
bool torch = false;
void loop()
{
    while ( 0 ) {
      if ( digitalRead( egle_sensor ) ) { digitalWrite( message_led, HIGH ); Serial.println( "sisterroom" ); }
      else if ( digitalRead( koogi_sensor ) ) { digitalWrite( message_led, HIGH ); Serial.println( "kitchen" ); }
      else { digitalWrite( message_led, LOW ); }
      if ( digitalRead( magamis_sensor ) ) { digitalWrite( blue_led, HIGH ); Serial.println( "bedroom" ); }
      else if ( digitalRead( teleka_sensor ) ) { digitalWrite( blue_led, HIGH ); Serial.println( "livingroom" ); }
      else { digitalWrite( blue_led, LOW ); }
    }
    // check if motion is occuring, x times and with y sensibility and within z compare times
    motion = isMotion( egle_sensor, compare_times, sensibility );
    // switch LED ON, if there is motion and else OFF
    if ( motion && !torch ) {
      switchTorch( HIGH );
      torch = HIGH;
    } else if ( !motion && torch ) {
      switchTorch( LOW );
      torch = LOW;
    }
    /*if ( motion ) { 
      makeCall();
      delay( 25000 );
    }*/
    //delay( 100 );
}

void phoneReceive()
{
  // as long as there is received data
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    digitalWrite( message_led, HIGH );
    switch ( mode ) {
          case 0: if ( c == 0x1e ) { buffer[bufpt].id = c; mode = 1; } break;
          case 1: if ( c == 0x0c ) { buffer[bufpt].destination = c; mode = 2; } break;
          case 2: buffer[bufpt].source = c; mode = 3; break;
          case 3: buffer[bufpt].type = c; mode = 4; break;
          case 4: buffer[bufpt].unknown = c; mode = 5; break;
          case 5: if ( c > 255 ) { /*OVERFLOW*/ mode = 0; break; } buffer[bufpt].data_size = c; mode = 6; break;
          case 6: if ( datpt == buffer[bufpt].data_size ) { ack_seq = buffer[bufpt].data[datpt-1] & 0x07; datpt = 0; mode = 7; } else { buffer[bufpt].data[datpt] = c; ++datpt; } break;
          case 7: buffer[bufpt].checksum[0] = c; mode = 8; break;
          case 8: buffer[bufpt].checksum[1] = c; mode = 0; bufpt = (++bufpt) & 0x07; received = true; break;
    }
    digitalWrite( message_led, LOW );
  }
}

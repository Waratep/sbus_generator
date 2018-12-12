unsigned int SBUS_Channel_Data[18];
byte SBUS_Current_Channel = 0;
byte SBUS_Current_Channel_Bit = 0;
byte SBUS_Current_Packet_Bit = 0;
byte SBUS_Packet_Data[25];
byte SBUS_Packet_Position = 0;

byte SBUS_Failsafe_Active = 0;
byte SBUS_Lost_Frame = 0;

void setup()
{
  Serial.begin(115200);
  SBUS_Channel_Data[0] = 348;
  SBUS_Channel_Data[1] = 1234;
  SBUS_Channel_Data[2] = 1244;
  SBUS_Channel_Data[3] = 575;
  SBUS_Channel_Data[4] = 1706;
  SBUS_Channel_Data[5] = 342;
  SBUS_Channel_Data[6] = 1024;
  SBUS_Channel_Data[7] = 1024;
  SBUS_Channel_Data[8] = 1024;
  SBUS_Channel_Data[9] = 1024;
  SBUS_Channel_Data[10] = 1024;
  SBUS_Channel_Data[11] = 1024;
  SBUS_Channel_Data[12] = 1792;
  SBUS_Channel_Data[13] = 1024;
  SBUS_Channel_Data[14] = 1024;
  SBUS_Channel_Data[15] = 1024;
  SBUS_Channel_Data[16] = 0;
  SBUS_Channel_Data[17] = 0;
  SBUS_Failsafe_Active = 1;
  SBUS_Lost_Frame = 1;
}

void SBUS_Build_Packet(void)
{
  for(SBUS_Packet_Position = 0; SBUS_Packet_Position < 25; SBUS_Packet_Position++) SBUS_Packet_Data[SBUS_Packet_Position] = 0x00;  //Zero out packet data
  
  SBUS_Current_Packet_Bit = 0;
  SBUS_Packet_Position = 0;
  SBUS_Packet_Data[SBUS_Packet_Position] = 0x0F;  //Start Byte
  SBUS_Packet_Position++;
  
  for(SBUS_Current_Channel = 0; SBUS_Current_Channel < 16; SBUS_Current_Channel++)
  {
    for(SBUS_Current_Channel_Bit = 0; SBUS_Current_Channel_Bit < 11; SBUS_Current_Channel_Bit++)
    {
      if(SBUS_Current_Packet_Bit > 7)
      {
        SBUS_Current_Packet_Bit = 0;  //If we just set bit 7 in a previous step, reset the packet bit to 0 and
        SBUS_Packet_Position++;       //Move to the next packet byte
      }
      SBUS_Packet_Data[SBUS_Packet_Position] |= (((SBUS_Channel_Data[SBUS_Current_Channel]>>SBUS_Current_Channel_Bit) & 0x01)<<SBUS_Current_Packet_Bit);  //Downshift the channel data bit, then upshift it to set the packet data byte
      SBUS_Current_Packet_Bit++;
    }
  }
  if(SBUS_Channel_Data[16] > 1023) SBUS_Packet_Data[23] |= (1<<0);  //Any number above 1023 will set the digital servo bit
  if(SBUS_Channel_Data[17] > 1023) SBUS_Packet_Data[23] |= (1<<1);
  if(SBUS_Lost_Frame != 0) SBUS_Packet_Data[23] |= (1<<2);          //Any number above 0 will set the lost frame and failsafe bits
  if(SBUS_Failsafe_Active != 0) SBUS_Packet_Data[23] |= (1<<3);
  SBUS_Packet_Data[24] = 0x00;  //End byte
}

void loop()
{
  SBUS_Build_Packet();
  Serial.write(SBUS_Packet_Data, 25);
}

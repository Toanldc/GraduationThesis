#include "FirebaseESP8266.h"  
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "test-datn-2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "kLbCVLIyjianqu05Re2sAk6Kl9NBTlQjr6z5Fo9w"

#define WIFI_SSID "abcd" 
#define WIFI_PASSWORD "12345677"

FirebaseData UIDadd;
FirebaseData NUTadd;
FirebaseData NUTchedo;

//-------------------------------------
#include <Eeprom24Cxx.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN  4   //d2
#define RST_PIN 5   //d1

#define in_pin  2   //d4
#define lock   0  //d3
#define caution 15 //d8
 
MFRC522 mfrc522(SS_PIN, RST_PIN);
//------------------------------------FIND-INDEX---------------------------------------------------
int last_index;
void fInd_last()
{
  int p = 0; 
  for (int i = 511; i > 0; i--) 
  {
      p =  EEPROM.read(i);
      if (p != 0)
      {
        last_index = i + 1;
        EEPROM.write(0, last_index);
        break;
      }
  }
}
int n;
int zero_index;
int zero_value;
void fInd_zero()
{
  n = 1;
  while (n < last_index)
  {
      zero_value = EEPROM.read(n);
      if (zero_value == 0)
      {
        zero_index = n;
        break;
      }      
     n++;
  }
  
  if(zero_index == 0)
  {
    zero_index = last_index;             
    n=1;
  } 
}
//---------------------------------COMPARE-----------------------------------------------------
int agree;
int m;
int moi;
int o;
void cOmpareEEP(int UIDin[4])
{
  int UID_EEP[4];
  int value_EEP = 0;
  m = 1;   
  o = 0;
  agree = 0;
  
    while(m < last_index)
    {
        moi = m + 4;
        for (m; m < moi; m++)
        {
            value_EEP = EEPROM.read(m);
            if (o <= 3) {UID_EEP[o]=value_EEP;}
            o++;
        }
        if(UIDin[0] == UID_EEP[0] && UIDin[1] == UID_EEP[1] && UIDin[2] == UID_EEP[2] && UIDin[3] == UID_EEP[3])
        {
            agree = 1;
            break;
        }
        else 
        {
            agree = 2;
        }
       o = 0;
       m = moi;
    }
}
//--------------------------------------SCAN_UID_ALLOW-----------------------------------------------------
int UID_scan[4];
unsigned long UIDs[4];
void sCancheck()
{
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {
      digitalWrite(caution, HIGH);           
      UIDs[i] = mfrc522.uid.uidByte[i];
      UID_scan[i] = UIDs[i];
      if (UIDs[i] < 0x10) {Serial.print("0");}
      Serial.print(UIDs[i],HEX);
    }
    Serial.println();
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();
    delay(200); digitalWrite(caution, LOW);
    cOmpareEEP(UID_scan);
    if (agree == 1)
    {
       Serial.println("Welcome");
       digitalWrite(lock, HIGH); delay(1000); digitalWrite(lock, LOW);
    }
    else if (agree == 2)
    {
       Serial.println("Wrong Tag");  
    }
    else 
    {
      Serial.println("No cards have been saved yet");
    }
}
//---------------------------------------SCAN_ADD_NEW_TAG-----------------------------------
String stringUID = "";
unsigned long UID[4];
int UID_add[4];
int tamadd;
int tamchedo;
void sCannewtag()
{
  last_index = EEPROM.read(0);
  if (last_index == 0)
  {
    int index = 1;
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    { 
      digitalWrite(caution, HIGH);         
      UID[i] = mfrc522.uid.uidByte[i];
      UID_add[i] = UID[i];
      if (UID[i] < 0x10) {stringUID += "0";}
      stringUID += String(UID[i], HEX);
    }
    Serial.println(stringUID);
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();
    //Firebase.setString(UIDadd, "RFID/UIDadd","");
    Firebase.setString(UIDadd, "RFID/UIDadd",stringUID);
    stringUID = "";
    digitalWrite(caution, LOW);
  }
  else 
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return;} 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {
      digitalWrite(caution, HIGH);          
      UID[i] = mfrc522.uid.uidByte[i];
      UID_add[i] = UID[i];
      if (UID[i] < 0x10) {stringUID += "0";}
      stringUID += String(UID[i], HEX);
    }
    Serial.println(stringUID);
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();
    //Firebase.setString(UIDadd, "RFID/UIDadd","");
    Firebase.setString(UIDadd, "RFID/UIDadd",stringUID);
    stringUID = "";
    digitalWrite(caution, LOW);
/*
    cOmpareEEP(UID_add);
    if (agree == 1) 
    {
       Serial.println("tagExisted");
       //Firebase.setString(UIDadd, "RFID/UIDadd","tagExisted");
    }
*/
  }
}

void aDdnewtag()
{
   sCannewtag();
   if (last_index == 0)
   {
         if (Firebase.getString(NUTadd, "/RFID/NUTadd"))
         {
            if (NUTadd.stringData() == "1")  tamadd = 1;
            else return; 
            while (tamadd == 1)
            {
              int index = 1;
              for (byte i = 0; i < 4; i++)
              {
                EEPROM.write(index, UID_add[i]);
                index = index + 1;
                last_index = index;
              }
              EEPROM.write(0, last_index);
              Serial.println("First tag saved");
              //Firebase.setString(NUTadd, "RFID/NUTadd","0"); dat timer
              tamadd = 0;
              goto conclude;
            }
         }
   }
   else 
   {
         if (Firebase.getString(NUTadd, "/RFID/NUTadd"))
         {
            if (NUTadd.stringData() == "1")  tamadd = 1;
            else return;  
            while (tamadd == 1)
            {
              if (agree == 2)
              {
                fInd_zero();
                if (zero_index == last_index)
                {
                  for(int i = 0; i < 4; i++)
                  {
                    EEPROM.write(zero_index, UID_add[i]);
                    zero_index = zero_index + 1;
                    last_index = zero_index;
                   }
                   EEPROM.write(0, last_index);
                   Serial.println("Saved");
                   //Firebase.setString(NUTadd, "RFID/NUTadd","0"); dat timer
                   //Firebase.setString(UIDadd, "RFID/UIDadd","");
                }
                else if (zero_index == n)
                {
                  for(int i=0; i<4; i++)
                  {
                    EEPROM.write(zero_index, UID_add[i]);
                    zero_index = zero_index + 1;
                  }
                  Serial.println("Saved");
                  //Firebase.setString(NUTadd, "RFID/NUTadd","0"); dat timer
                  //Firebase.setString(UIDadd, "RFID/UIDadd","");
                }
               o = 0; m=1; moi=0;
             } 
             agree = 0;
             tamadd = 0;
             goto conclude;
           }
         } 
   }
   conclude:
   EEPROM.commit(); 
} 
//---------------HEXTODEC----------------beginDELETE----------------------------
unsigned int decValue = 0;
unsigned int hexToDec(String hexString) 
{
  int nextInt;
  decValue = 0;
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
//----------------StringSplit-------------------
int UID_delete[4];
void stringSplit(String stringIn)
{
  char stringUID[12] = "";
  stringIn.toCharArray(stringUID, 12);  // string to char
  char idgetFBase[] = "";
  strncpy(idgetFBase, stringUID  + 2, 10); 
  idgetFBase[8] = '\0'; 
  //--
  char idgetFBase0[2] = "";
  char idgetFBase1[2] = "";
  char idgetFBase2[2] = "";
  char idgetFBase3[2] = "";
  strncpy(idgetFBase0, idgetFBase  + 0, 2);
  idgetFBase0[2] = '\0'; 
  hexToDec(idgetFBase0); UID_delete[0] = decValue;
  
  strncpy(idgetFBase1, idgetFBase  + 2, 4);
  idgetFBase1[2] = '\0'; 
  hexToDec(idgetFBase1); UID_delete[1] = decValue;

  strncpy(idgetFBase2, idgetFBase  + 4, 6);
  idgetFBase2[2] = '\0'; 
  hexToDec(idgetFBase2); UID_delete[2] = decValue;
  
  strncpy(idgetFBase3, idgetFBase  + 6, 8);
  idgetFBase3[2] = '\0'; 
  hexToDec(idgetFBase3); UID_delete[3] = decValue;
}
//-------------DELETE-TAG--------------------------
String getUID_delete="";
void dEletetag()
{
  stringSplit(getUID_delete);
  if(last_index == 0)
  {
    Serial.println("No card can't be deleted");
  }
  else if (last_index != 0)
  {
    cOmpareEEP(UID_delete);  // agree = 1 or 2 
    if  (agree == 1)
    {
      Serial.println("Deleted");
      fInd_zero();
      m= m -4;
      for (int i=0; i<4; i++)
      {
        EEPROM.write(m, 0);
        m = m + 1; 
      }
      o = 0; m=1; moi = 0;  
    }
    agree = 0;
    fInd_last();
    fInd_zero();
  }
  EEPROM.commit();
}
//--------------end_DELETE_TAG-------------------
//---------------------------------------------PrintEEP_DeleteEEP----------------------------------------
void printEEP()
{
  for(int g = 0; g < 30; g++)
  {
    Serial.print(g);Serial.print("="); Serial.print(EEPROM.read(g)); Serial.print("  ");
  }
  Serial.println(" ");
  Serial.print("zero_index:"); Serial.println(zero_index); 
}
void deleteEEP()
{
  for (int i = 0; i < 512; i++) 
  {
    EEPROM.write(i, 0);
    delay(5);
  }
  zero_index=0;
  printEEP();
  EEPROM.commit(); 
}
//--
void setup()
{
  EEPROM.begin(512);
  Serial.begin(9600);
  SPI.begin();  
  mfrc522.PCD_Init();  

  pinMode(in_pin, INPUT_PULLUP);
  pinMode(lock, OUTPUT); 
  pinMode(caution, OUTPUT); 
  fInd_last();
  fInd_zero();
  last_index = EEPROM.read(0);
  //---------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true); 
}


unsigned long t2 = 0;
unsigned long t1 = 0;
void loop()
{
  t1 = millis();
  if (digitalRead(in_pin)==LOW)  deleteEEP(); 
  sCancheck();
  

if (t1 - t2 > 1000)
{  
  if (Firebase.getString(NUTchedo, "/RFID/NUTchedo"))
  {
     getUID_delete = NUTchedo.stringData();  
     if (getUID_delete == "0" || getUID_delete == "") 
     {
      t2 =millis();
      return;
     }  
     else if (getUID_delete == "1")
     {
       tamchedo = 1;
     }
     else if (getUID_delete == "3")
     {
       //Firebase.setString(NUTchedo, "RFID/NUTchedo","");
       Serial.println("Welcome");
       digitalWrite(lock, HIGH); delay(1000); digitalWrite(lock, LOW); 
     }
     else 
     { 
       dEletetag();
       Firebase.setString(NUTchedo, "RFID/NUTchedo",""); 
       return;
     }

     while (tamchedo == 1)
     {
        aDdnewtag();
        if (Firebase.getString(NUTchedo, "/RFID/NUTchedo"))
        {  
            if (NUTchedo.stringData() == "0" || NUTchedo.stringData() == "") 
            {
              tamchedo = 0;
              break;
            }
        }
     }
   }

   t2 =millis();
}
}

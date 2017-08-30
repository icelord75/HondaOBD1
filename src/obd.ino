/*
   //      _______ __  _________ _________
   //      \_     |  |/  .  \   |   \ ___/
   //        / :  |   \  /   \  |   / _>_
   //       /      \ | \ |   /  |  /     \
   //      /   |___/___/____/ \___/_     /
   //      \___/--------TECH--------\___/
   //       ==== ABOVE SCIENCE 1994 ====
   //
   //   Ab0VE TECH - ISO9141 K-Line controller
 */

#define DEBUG

/* Controller connections
   //            +-----------+
   //            • TX    Vin •
   //            • RX  A Gnd •  <- GND
   //            • RST R RST •
   //     GND -> • GND D  +5 •  <- +5V Reg. LM2596HV
   //            • 2   U  A7 •
   //            • 3   I  A6 •
   //            • 4   N  A5 •
   //            • 5   O  A4 •
   //            • 6      A3 •
   //            • 7   N  A2 •
   //            • 8   A  A1 •
   //            • 9   N  A0 •
   // MC33290 -> • 10  O Arf •
   // MC33290 <- • 11    3V3 •
   //  Button -> • 12 ||| 13 • <- Dim
   //            +-----------+
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "errors.h"
#include "lcd.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
SoftwareSerial mySerial(10, 11); // RX, TX

uint8_t OBD1[256]; // OBD1 buffer


bool starter;
bool ac_btn;
bool ps;
bool brakes;
bool atpos;
bool vtec_press;
bool scs;
bool vtec;
bool mail_relay;
bool ac_clutch;
bool lz1_heater;
bool CE;
bool lz2_heater;
bool alt_c;
bool fan;
bool iab;
bool vtec_e;
bool econo;
bool at_mount;
bool closed_loop;
int rpm;
int vss;
int temp_cool;
int temp_air;
float abs_pres;
float atm_pres;
int thr_pos;
float volt_lz1;
float volt_lz2;
float volt_net;
float load_gen;
float load_eld;
float pos_egr;
int st_cor;
int lt_cor;
float time_inj;
int ign;
int limit_ign;
float valv_idle;
float valve_egr;
float pos_valve_egr;
int fuelc;
float purge_valve;
float knock_sensor;
int engine_load;
uint64_t ECU_ID;


byte check_data24(int nbyte)
{
        int i;
        if (nbyte!=24) {
                for(i=0; i<nbyte; i++) mySerial.read();
                return 0;
        }
        else {       //preamble
                for(i=0; i<5; i++) mySerial.read();
                return 1;
        }
}

void honda_read_data(uint8_t offset)
{
        int i;
        for(i=0; i<19; i++)
                OBD1[i+offset]=mySerial.read();
}

// функция записи в порт
void honda_write_data(byte x, byte y)
{
        mySerial.write(0x20); // command byte (read mem)
        mySerial.write(0x05); // length in bytes
        mySerial.write(x); // offset
        mySerial.write(y); // Nr bytes to retun
        mySerial.write(0x0100-(0x20+0x05+x+y)); //crc
}

void setup()
{
        Serial.begin(115200); //Console
        Serial.begin(9600); //скорость порта для связи с OBD-1
// Регистрируем собственный символы с кодами 1-5 для линейной шкалы
        lcd.createChar(1, S_1);
        lcd.createChar(2, S_2);
        lcd.createChar(3, S_3);
        lcd.createChar(4, S_4);
        lcd.createChar(5, S_5);
        lcd.clear();
}
int temp(float x)
{
     float result = 155.04149-x*3.0414878+x*x*0.03952185-x*x*x*0.00029383913+x*x*x*x*0.0000010792568-x*x*x*x*x*0.0000000015618437;
     return (int)result;
}


void loop()
{
        uint16_t nbyte;
        uint8_t OBD_POS;

// Clear
        nbyte=Serial.available();
        check_data24(nbyte);

        for (OBD_POS=0; OBD_POS<0xf0; OBD_POS+=10) // Read 00-FF blocks
        {
                honda_write_data(OBD_POS,0x10);
                delay(50);
                nbyte=Serial.available();
                if (nbyte>0)
                {
                        if (check_data24(nbyte))
                                honda_read_data(OBD_POS);
                }
        }
        // PROCESS ALL VALUES

        int srpm = (OBD1[0x00] << 8 | OBD1[0x01]);
        if (srpm > 0)
                rpm = 1875000/srpm;
        else
                rpm = 0;
        vss           = (int)OBD1[0x02];

        starter       = OBD1[0x08] & 1;
        ac_btn        = (OBD1[0x08] >> 1) & 1;
        ps            = (OBD1[0x08] >> 2) & 1;;
        brakes        = (OBD1[0x08] >> 3) & 1;;
        atpos         = (OBD1[0x08] >> 4) & 1;;
        vtec_press    = (OBD1[0x08] >> 7) & 1;;
        scs           = (OBD1[0x09] >> 3) & 1;;
        vtec          = (OBD1[0x0a] >> 2) & 1;;
        mail_relay    = OBD1[0x0B] & 1;
        ac_clutch     = (OBD1[0x0B] >> 1) & 1;
        lz1_heater    = (OBD1[0x0B] >> 2) & 1;
        CE            = (OBD1[0x0B] >> 5) & 1;
        lz2_heater    = (OBD1[0x0B] >> 7) & 1;
        alt_c         = OBD1[0x0C] & 1;
        fan           = (OBD1[0x0C] >> 1) & 1;
        iab           = (OBD1[0x0C] >> 2) & 1;
        vtec_e        = (OBD1[0x0C] >> 3) & 1;
        econo         = (OBD1[0x0C] >> 7) & 1;
        at_mount      = (OBD1[0x0D] >> 3) & 1;
        closed_loop   = OBD1[0x0F] & 1;
        temp_cool     = temp((float)OBD1[0x10]);
        temp_air      = temp((float)OBD1[0x11]);
        abs_pres      = (float)OBD1[0x12]*0.716-5.0;
        atm_pres      = (float)OBD1[0x13]*0.716-5.0;
        thr_pos       = (int)(((float)OBD1[0x14]-24.0)/2.0);
        volt_lz1      = (float)OBD1[0x15]/51.3;
        volt_net      = (float)OBD1[0x17]/10.45;
        load_gen      = (float)OBD1[0x18]/2.55;
        load_eld      = 77.06-(float)OBD1[0x19]/2.5371;
        pos_egr       = (float)OBD1[0x1B]/51.3;
        st_cor        = ((int)OBD1[0x20]/128-1)*100;
        lt_cor        = ((int)OBD1[0x22]/128-1)*100;
        time_inj      = ((float)((OBD1[0x24] << 8) | OBD1[0x25]))/250.0;
        ign           = ((int)OBD1[0x26]-128)/2;
        limit_ign     = ((int)OBD1[0x27]-24)/4;
        valv_idle     = (float)OBD1[0x28]/2.55;
        valve_egr     = (float)OBD1[0x2B]/2.55;
        pos_valve_egr = (float)OBD1[0x29]/2.55;
        purge_valve   = (float)OBD1[0x2F]/2.55;
        knock_sensor  = (float)OBD1[0x3C]/55;
        ECU_ID        = (uint64_t)OBD1[0x78] << 32 | (uint64_t)OBD1[0x79] << 24 | (uint64_t)OBD1[0x7A] << 16 | (uint64_t)OBD1[0x7B] << 8 | (uint64_t)OBD1[0x7C];
        engine_load   = (int)OBD1[0x9C];

#ifdef DEBUG
Serial.print("TEMP: ");Serial.println(temp_cool);
Serial.print("TEMP AIR: ");Serial.println(temp_air);
Serial.print("ABS PRESSURE: ");Serial.println(abs_pres);
Serial.print("ATM PRESSURE: ");Serial.println(atm_pres);
Serial.print("THROTTLE: ");Serial.println(thr_pos);
Serial.print("LZ1: ");Serial.println(volt_lz1);
Serial.print("VOLTAGE: ");Serial.println(volt_net);
Serial.print("ALTERNATOR LOAD: ");Serial.println(load_gen);
Serial.print("ELD LOAD: ");Serial.println(load_eld);
Serial.print("EGR POSITION: ");Serial.println(pos_egr);
Serial.print("SHORT-TERM CORR: ");Serial.println(st_cor);
Serial.print("LOGN-TERM CORR: ");Serial.println(lt_cor);
Serial.print("INJECTION TIME: ");Serial.println(time_inj);
Serial.print("IGNITION ADVANCE: ");Serial.println(ign);
Serial.print("LIMIT IGNTION: ");Serial.println(limit_ign);
Serial.print("IDLE VALVE: ");Serial.println(valv_idle);
Serial.print("EGR VALVE: ");Serial.println(valve_egr);
Serial.print("EGR VALVE POS: ");Serial.println(pos_valve_egr);
Serial.print("PURGE VALVE: ");Serial.println(purge_valve);
Serial.print("KNOCK SENSOR: ");Serial.println(knock_sensor);
Serial.print("ENGINE LOAD: ");Serial.println(engine_load);
Serial.print(": ");Serial.println();



#endif


}

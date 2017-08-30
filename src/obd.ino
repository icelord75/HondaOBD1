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
   // MC33290 -> • 2   U  A7 •
   // MC33290 <- • 3   I  A6 •
   //            • 4   N  A5 •
   //            • 5   O  A4 •
   //            • 6      A3 •
   //            • 7   N  A2 •
   //            • 8   A  A1 •
   //            • 9   N  A0 •
   //            • 10  O Arf •
   //            • 11    3V3 •
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
SoftwareSerial mySerial(2, 3); // RX, TX on interruptable pins

uint8_t OBD1[256]; // OBD1 buffer
bool starter, ac_btn, ps, brakes,atpos, vtec_press, scs, vtec, mail_relay, ac_clutch;
bool lz1_heater, CE, lz2_heater, alt_c,fan, iab, vtec_e, econo, at_mount, closed_loop;
int rpm, vss, temp_cool, temp_air, thr_pos;
float abs_pres, atm_pres, volt_lz1, volt_lz2, volt_net, load_gen, load_eld, pos_egr;
int st_cor, lt_cor, ign, limit_ign, engine_load;
float time_inj, valv_idle, valve_egr, pos_valve_egr, purge_valve, knock_sensor;


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

void honda_write_data(byte x, byte y)
{
        mySerial.write(0x20);                   // command byte (read mem)
        mySerial.write(0x05);                   // length in bytes
        mySerial.write(x);                      // offset
        mySerial.write(y);                      // Nr bytes to retun
        mySerial.write(0x0100-(0x20+0x05+x+y)); //crc
}

int temp(float x)
{
        float result = 155.04149-x*3.0414878+x*x*0.03952185-x*x*x*0.00029383913+x*x*x*x*0.0000010792568-x*x*x*x*x*0.0000000015618437;
        return (int)result;
}

void setup()
{
        Serial.begin(115200); //Console
        Serial.begin(9600);   //9600 for OBD-1
        lcd.createChar(1, (uint8_t *)S_1);
        lcd.createChar(2, (uint8_t *)S_2);
        lcd.createChar(3, (uint8_t *)S_3);
        lcd.createChar(4, (uint8_t *)S_4);
        lcd.createChar(5, (uint8_t *)S_5);
        lcd.clear();
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
                        if (check_data24(nbyte))
                                honda_read_data(OBD_POS);
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
        engine_load   = (int)OBD1[0x9C];

#ifdef DEBUG
        Serial.print("ECU ID: "); Serial.print(OBD1[0x78],HEX); Serial.print(OBD1[0x79],HEX); Serial.print(OBD1[0x7A],HEX); Serial.print(OBD1[0x7B],HEX); Serial.println(OBD1[0x7C],HEX);
        Serial.print("RPM: "); Serial.println(rpm);
        Serial.print("SPEED: "); Serial.println(vss);
        Serial.print("TEMP AIR: "); Serial.println(temp_air);
        Serial.print("ABS PRESSURE: "); Serial.println(abs_pres);
        Serial.print("ATM PRESSURE: "); Serial.println(atm_pres);
        Serial.print("THROTTLE: "); Serial.println(thr_pos);
        Serial.print("LZ1: "); Serial.println(volt_lz1);
        Serial.print("VOLTAGE: "); Serial.println(volt_net);
        Serial.print("ALTERNATOR LOAD: "); Serial.println(load_gen);
        Serial.print("ELD LOAD: "); Serial.println(load_eld);
        Serial.print("EGR POSITION: "); Serial.println(pos_egr);
        Serial.print("SHORT-TERM CORR: "); Serial.println(st_cor);
        Serial.print("LOGN-TERM CORR: "); Serial.println(lt_cor);
        Serial.print("INJECTION TIME: "); Serial.println(time_inj);
        Serial.print("IGNITION ADVANCE: "); Serial.println(ign);
        Serial.print("LIMIT IGNTION: "); Serial.println(limit_ign);
        Serial.print("IDLE VALVE: "); Serial.println(valv_idle);
        Serial.print("EGR VALVE: "); Serial.println(valve_egr);
        Serial.print("EGR VALVE POS: "); Serial.println(pos_valve_egr);
        Serial.print("PURGE VALVE: "); Serial.println(purge_valve);
        Serial.print("KNOCK SENSOR: "); Serial.println(knock_sensor);
        Serial.print("ENGINE LOAD: "); Serial.println(engine_load);
        Serial.print(": "); Serial.println();

        if (mail_relay)   Serial.print("MAIN ");   else Serial.print("main ");
        if (starter)      Serial.print("ST ");     else Serial.print("st ");
        if (ac_btn)       Serial.print("AC ");     else Serial.print("ac ");
        if (ac_clutch)    Serial.print("AC_C ");   else Serial.print("ac_c ");
        if (ps)           Serial.print("PS ");     else Serial.print("ps ");
        if (brakes)       Serial.print("BRAKE ");  else Serial.print("brake ");
        if (atpos)        Serial.print("AT ");     else Serial.print("at ");
        if (at_mount)     Serial.print("AT_M ");   else Serial.print("at_m ");
        if (scs)          Serial.print("scs ");    else Serial.print("scs ");
        if (CE)           Serial.print("CE ");     else Serial.print("ce ");
        if (vtec_press)   Serial.print("VTEC_P "); else Serial.print("vtec_p ");
        if (vtec)         Serial.print("VTEC ");   else Serial.print("vtec ");
        if (vtec_e)       Serial.print("VTEC_E "); else Serial.print("vtec_e ");
        if (econo)        Serial.print("ECONO ");  else Serial.print("econo ");
        if (lz1_heater)   Serial.print("LZ1 ");    else Serial.print("lz1 ");
        if (lz2_heater)   Serial.print("LZ2 ");    else Serial.print("lz2 ");
        if (closed_loop)  Serial.print("CLOOP ");  else Serial.print("cloop ");
        if (alt_c)        Serial.print("ALT_C ");  else Serial.print("alt_c ");
        if (fan)          Serial.print("FAN ");    else Serial.print("fan ");
        if (iab)          Serial.print("IAB ");    else Serial.print("iab ");
        Serial.print("\n");

#endif


}

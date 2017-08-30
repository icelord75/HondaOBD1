/*

   OBD1 Errors

 */
#include <avr/pgmspace.h>

const char Error_Un[] PROGMEM = "UNKNOWN";
const char Error_00[] PROGMEM = "ECU";
const char Error_01[] PROGMEM = "Oxygen sensor";
const char Error_02[] PROGMEM = "Oxygen sensor2";
const char Error_03[] PROGMEM = "MAP sensor";
const char Error_04[] PROGMEM = "Crank angle";
const char Error_05[] PROGMEM = "MAP sensor";
const char Error_06[] PROGMEM = "Coolant Temp sensor";
const char Error_07[] PROGMEM = "Throttle Position sensor";
const char Error_08[] PROGMEM = "Top Dead Center sensor";
const char Error_09[] PROGMEM = "Cylinder Position sensor";
const char Error_10[] PROGMEM = "Intake Air Temperature sensor";
const char Error_11[] PROGMEM = "IMA Sensor";
const char Error_12[] PROGMEM = "EGR Valve Open";
const char Error_13[] PROGMEM = "Barometric Pressure sensor";
const char Error_14[] PROGMEM = "IAC valve defective";
const char Error_15[] PROGMEM = "Ignition Output Signal";
const char Error_16[] PROGMEM = "Fuel Injector System Defective";
const char Error_17[] PROGMEM = "Vehicle Speed sensor";
const char Error_19[] PROGMEM = "Lock-up Control Solenoid";
const char Error_20[] PROGMEM = "Electrical Load Detector";
const char Error_21[] PROGMEM = "VTEC Solenoid Valve";
const char Error_22[] PROGMEM = "VTEC Oil Pressure Switch";
const char Error_23[] PROGMEM = "Knock sensor";
const char Error_30[] PROGMEM = "A/T FI Signal A";
const char Error_31[] PROGMEM = "A/T FI Signal B";
const char Error_41[] PROGMEM = "O2 sensor1 heater";
const char Error_42[] PROGMEM = "O2 sensor2 heater";
const char Error_43[] PROGMEM = "Fuel Supply System Defective";
const char Error_44[] PROGMEM = "Fuel Supply System (Slave)";
const char Error_45[] PROGMEM = "System Too Lean/Rich (Primary)";
const char Error_46[] PROGMEM = "System Too Lean/Rich (Slave)";
const char Error_48[] PROGMEM = "Lean Air Fuel sensor";
const char Error_53[] PROGMEM = "Knock sensor (second)";
const char Error_54[] PROGMEM = "Crankshaft Speed Fluctuation sensor";
const char Error_58[] PROGMEM = "Top Dead Center sensor2";
const char Error_59[] PROGMEM = "CYP Sensor2 (Cylinder)";
const char Error_61[] PROGMEM = "Primary O2 sensor Slow response";
const char Error_63[] PROGMEM = "Secondary O2 sensor Slow response";
const char Error_65[] PROGMEM = "Secondary O2 sensor circuit";
const char Error_67[] PROGMEM = "Catalyst System Efficiency";
const char Error_70[] PROGMEM = "A/T FI data line TCM ECM";
const char Error_71[] PROGMEM = "Random Misfire (Cylinder 1)";
const char Error_72[] PROGMEM = "Random Misfire (Cylinder 2)";
const char Error_73[] PROGMEM = "Random Misfire (Cylinder 3)";
const char Error_74[] PROGMEM = "Random Misfire (Cylinder 4)";
const char Error_75[] PROGMEM = "Random Misfire (Cylinder 5)";
const char Error_76[] PROGMEM = "Random Misfire (Cylinder 6)";
const char Error_80[] PROGMEM = "Exhaust Gas Re-circulation";
const char Error_86[] PROGMEM = "Engine Coolant Temp circuit";
const char Error_90[] PROGMEM = "Evaporative Emission Control System";
const char Error_91[] PROGMEM = "Fuel Tank Pressure sensor";
const char Error_92[] PROGMEM = "Evaporative Emission Control System";

const char* const Errors[] PROGMEM = {
        Error_00, Error_01, Error_02, Error_03, Error_04, Error_05, Error_06, Error_07, Error_08, Error_09,
        Error_10, Error_11, Error_12, Error_13, Error_14, Error_15, Error_16, Error_17, Error_Un, Error_19,
        Error_20, Error_21, Error_22, Error_23, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un,
        Error_30, Error_31, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un,
        Error_Un, Error_41, Error_42, Error_43, Error_44, Error_45, Error_46, Error_Un, Error_48, Error_Un,
        Error_Un, Error_Un, Error_Un, Error_53, Error_54, Error_Un, Error_Un, Error_Un, Error_58, Error_59,
        Error_Un, Error_61, Error_Un, Error_63, Error_Un, Error_65, Error_Un, Error_67, Error_Un, Error_Un,
        Error_70, Error_71, Error_72, Error_73, Error_74, Error_75, Error_76, Error_Un, Error_Un, Error_Un,
        Error_80, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_86, Error_Un, Error_Un, Error_Un,
        Error_90, Error_91, Error_92, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un, Error_Un
};

#ifndef HARDWARE_FUNCTIONS_H
#define HARDWARE_FUNCTIONS_H

#include "mbed.h"

// Globale Steuer-Variablen, die den Status der Hauptaufgabe steuern
extern bool do_execute_main_task;
extern bool do_reset_all_once;

// Funktionsprototypen:

/**
 * Initialisiert die Hardware.
 * Bindet u.a. den Button-Interrupt an die zugehörige Callback-Funktion.
 */
void initHardware(void);

/**
 * Führt die Hauptschleife aus.
 * Liest Sensoren, steuert LEDs und sorgt für den zyklischen Ablauf.
 */
void runMainLoopTask(void);

/**
 * Callback-Funktion, die beim Drücken des Benutzertasters (blue button) aufgerufen wird.
 * Sie toggelt den Zustand der Hauptaufgabe.
 */
void toggle_do_execute_main_fcn(void);

/**
 * Berechnet die kompensierte Distanz des IR-Sensors.
 *
 * @param ir_distance_mV Gemessener IR-Sensorwert in mV.
 * @return Kompensierter Wert.
 */
float ir_sensor_compensation(float ir_distance_mV);

#endif // HARDWARE_FUNCTIONS_H
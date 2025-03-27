#include "distanceSensor.h"
#include "PESBoardPinMap.h"  // Legende für die Pinbelegung
#include "DebounceIn.h"      // Treiber für den Debounce-Taster
#include "Servo.h"           // Treiber für die Servos
#include <cmath>             // Für ceilf

// Globale Steuer-Variablen
bool do_execute_main_task = false;
bool do_reset_all_once = false;

// Hardware-Objekte:

// Servos (z.B. "Futuba" und "Reely")
Servo servo_D0(PB_D0);
Servo servo_D1(PB_D1);

// Taster (blauer Button) mit Debounce
DebounceIn user_button(BUTTON1);

// Benutzer-LED (Nucleo Board)
DigitalOut user_led(LED1);

// Zusätzliche LED (extern angeschlossene LED, z. B. an PB_9)
DigitalOut led1(PB_9);

// IR-Distanzsensor (Analog-Eingang an PC_2)
AnalogIn ir_analog_in(PC_2);

// Callback-Funktion für den Taster: wird bei fallender Flanke aufgerufen
void toggle_do_execute_main_fcn() 
{
    do_execute_main_task = !do_execute_main_task;
    // Setze den Reset-Flag, wenn die Hauptaufgabe aktiviert wurde
    if (do_execute_main_task)
        do_reset_all_once = true;
}

// Berechnet die IR-Sensorkompensation basierend auf Kalibrierungskonstanten.
//paarameter is the imput of the distanceSensor itself
float ir_sensor_compensation(float ir_distance_mV) 
{
    static const float a = 2.574e+04f;
    static const float b = -29.37f;

    // Vermeidung von Division durch Null
    if (ir_distance_mV + b == 0.0f)
        ir_distance_mV -= 0.001f;

    return a / (ir_distance_mV + b);
}

// Initialisiert die Hardware, z.B. Bindung des Taster-Interrupts.
void initHardware(void) 
{
    // Binde die Fall-Interrupt-Funktion an den Benutzertaster
    user_button.fall(&toggle_do_execute_main_fcn);
    
    // Weitere Initialisierungen (z.B. für Servos oder LEDs) können hier ergänzt werden.
}

// Führt den periodischen Hauptzyklus aus.
void runMainLoopTask(void) 
{
    // Zykluszeit in Millisekunden: z. B. 200 ms (5 mal pro Sekunde)
    const int main_task_period_ms = 200;
    Timer main_task_timer;
    main_task_timer.start();

    float servo_input = 0.0f;  // Beispielwert, der z.B. als Servoeingang genutzt werden könnte
    int servo_counter = 0;     // Zusätzlicher Zähler für Servosteuerungen (optional)

    float ir_distance_mV = 0.0f; // Variable zum Speichern des IR-Sensorwerts in mV

    // Endlosschleife für die Hauptaufgabe
    while (true) {
        main_task_timer.reset();

        // Debug-Ausgabe des Servowerts
        printf("Pulse width: %f \n", servo_input);

        if (do_execute_main_task) {
            // Lese den analogen IR-Sensorwert ein, skaliere von 0...1 auf 0...3.3V und konvertiere in mV
            ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;
            
            // Schalte die zusätzliche LED ein als Indikator, dass die Hauptaufgabe aktiv ist
            led1 = 1;
        } else {
            // Dieser Block wird nur einmal ausgeführt, wenn die Hauptaufgabe deaktiviert wird
            if (do_reset_all_once) {
                do_reset_all_once = false;
                // Setze LED und IR-Sensorwert zurück
                led1 = 0;
                ir_distance_mV = 0.0f;
            }
        }
        // Debug-Ausgabe des IR-Sensorwerts
        printf("IR distance mV: %f \n", ir_distance_mV);

        // Umschalten der Benutzer-LED als Zyklusanzeige
        user_led = !user_led;

        // Ermittle die verstrichene Zeit des aktuellen Zyklus
        int main_task_elapsed_time_ms = duration_cast<milliseconds>(main_task_timer.elapsed_time()).count();

        // Falls der Zyklus länger als die geplante Zeit gedauert hat, gib eine Warnung aus;
        // ansonsten, warte die restliche Zeit bis zum nächsten Zyklus.
        if (main_task_period_ms - main_task_elapsed_time_ms < 0)
            printf("Warning: Main task took longer than main_task_period_ms\n");
        else
            thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}
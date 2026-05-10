#include "Filters.h"
#include <math.h>

// Implementarea unui filtru digital de tip "sare și piper" pentru eliminarea zgomotului impulsiv.
// Implementarea unui filtru digital de mediere ponderată pentru netezirea suplimentară a semnalului achiziționat.
// Conversia ADC-to-Voltage și Voltage-to-Parametru fizic conform specificațiilor senzorului ales.
// Aplicarea saturării pentru a limita valorile prelucrate în intervale valide.

float filter_saltAndPepperFilter(float newValue) {
    static float window[5]; // Dimensiunea ferestrei (5 eșantioane)
    static int index = 0;
    static int filled = 0;

    window[index] = newValue;
    index = (index + 1) % 5;

    if (filled < 5) {
        for (int i = filled; i < 5; i++) {
            window[i] = newValue;
        }
        filled++;
    }

    float sorted[5];
    for (int i = 0; i < 5; i++) {
        sorted[i] = window[i];
    }

    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (sorted[i] > sorted[j]) {
                float temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    return sorted[2]; // Valoarea mediană
}

float filter_weightedAverageFilter(float currentVal, float alpha) {
    static float previousFiltered = 0.0f;
    static bool initialized = false;

    if (!initialized) {
        previousFiltered = currentVal;
        initialized = true;
    }

    if (alpha <= 0.0f) {
        alpha = 0.2f;
    } else if (alpha > 1.0f) {
        alpha = 1.0f;
    }

    float filtered = (alpha * currentVal) + (1.0f - alpha) * previousFiltered;
    previousFiltered = filtered;

    return filtered;
}

float filter_applySaturation(float value, float minVal, float maxVal) {
    if (value > maxVal) return maxVal;
    if (value < minVal) return minVal;
    return value;
}

float filter_adcToVoltage(float rawValue, float vRef, float adcResolution) {
    if (adcResolution <= 0.0f) {
        return 0.0f;
    }
    return (rawValue * vRef) / adcResolution;
}

float filter_linearSensorConversion(float voltage, float slope, float offset) {
    return (slope * voltage) + offset;
}

float filter_adc_ToPhysical(float rawValue, float vRef, float adcResolution, float slope, float offset) {
    float voltage = filter_adcToVoltage(rawValue, vRef, adcResolution);
    return filter_linearSensorConversion(voltage, slope, offset);
}

float filter_adc_VoltageToTemperature(float voltage, float vRef, float seriesResistor, float r25, float beta) {
    if (voltage <= 0.0f || voltage >= vRef || seriesResistor <= 0.0f || r25 <= 0.0f || beta <= 0.0f) {
        return -273.15f;
    }

    float resistance = seriesResistor * voltage / (vRef - voltage);
    float temperatureKelvin = 1.0f / ((1.0f / (273.15f + 25.0f)) + (1.0f / beta) * logf(resistance / r25));
    return temperatureKelvin - 273.15f;
}


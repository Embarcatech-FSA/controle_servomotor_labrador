#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Define macros para os diretórios pwm_chip e pwm0
#define PWM_CHIP "/sys/class/pwm/pwmchip0"
#define PWM0 PWM_CHIP "/pwm0"
#define GPIO_LED1 "/sys/class/gpio/gpio131" // HEADER 3 da Labrador GPIOE3 (128 + 3)
#define GPIO_LED2 "/sys/class/gpio/gpio130" // HEADER 5 da Labrador GPIOE2 (128 + 2)

// Função que escreve uma string em um arquivo
void writeToFile(const char *path, const char *value) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    fprintf(fp, "%s", value);
    fclose(fp);
}

// Exporta um GPIO se ainda não estiver exportado
void exportGPIO(const char *gpio) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/export");
    if (access(gpio, F_OK) == -1) {
        writeToFile(path, gpio + strlen("/sys/class/gpio/gpio"));
        usleep(100000); // Aguarda exportação
    }
}

// Define a direção do GPIO (in/out)
void setGPIODirection(const char *gpio, const char *direction) {
    char path[128];
    snprintf(path, sizeof(path), "%s/direction", gpio);
    writeToFile(path, direction);
}

// Define o valor do GPIO (0 ou 1)
void setGPIOValue(const char *gpio, int value) {
    char path[128];
    snprintf(path, sizeof(path), "%s/value", gpio);
    writeToFile(path, value ? "1" : "0");
}

// Função principal
int main() {
    // Inicializa PWM
    writeToFile(PWM_CHIP "/export", "0");
    usleep(100000); // Aguarda exportação
    writeToFile(PWM0 "/period", "20000000"); // 20ms = 50Hz

    // Inicializa GPIOs dos LEDs
    exportGPIO(GPIO_LED1);
    exportGPIO(GPIO_LED2);
    setGPIODirection(GPIO_LED1, "out");
    setGPIODirection(GPIO_LED2, "out");

    // Ativa PWM
    writeToFile(PWM0 "/enable", "1");

    // Duty cycle para servo: de 0.5ms (0°) a 2.5ms (180°)
    int min_duty = 300000;   // 0°
    int max_duty = 2500000;  // 180°
    int step = 1500;        // Passo para suavidade
    int duty;
    int angle;

    char duty_str[16];

    while (1) {
        // 0° a 180°
        for (duty = min_duty; duty <= max_duty; duty += step) {
            snprintf(duty_str, sizeof(duty_str), "%d", duty);
            writeToFile(PWM0 "/duty_cycle", duty_str);

            angle = (duty - min_duty) * 180 / (max_duty - min_duty);
            if (angle <= 90) {
                setGPIOValue(GPIO_LED1, 1);
                setGPIOValue(GPIO_LED2, 0);
            } else {
                setGPIOValue(GPIO_LED1, 0);
                setGPIOValue(GPIO_LED2, 1);
            }
            printf("Angulo atual: %i - duty cycle atual: %i\n", angle, duty);
            usleep(10); // 10 microssegundos para suavidade

        // 180° a 0°
        for (duty = max_duty; duty >= min_duty; duty -= step) {
            snprintf(duty_str, sizeof(duty_str), "%d", duty);
            writeToFile(PWM0 "/duty_cycle", duty_str);

            angle = (duty - min_duty) * 180 / (max_duty - min_duty);
            if (angle <= 90) {
                setGPIOValue(GPIO_LED1, 1);
                setGPIOValue(GPIO_LED2, 0);
            } else {
                setGPIOValue(GPIO_LED1, 0);
                setGPIOValue(GPIO_LED2, 1);
            }
            printf("Angulo atual: %i - duty cycle atual: %i\n", angle, duty);
            usleep(10); // 10 microssegundos para suavidade
        }
    }

    return 0;
}

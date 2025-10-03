#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Define as macros para os diretórios pwm_chip e pwm0
#define PWM_CHIP "/sys/class/pwm/pwmchip0"
#define PWM0 PWM_CHIP "/pwm0"
#define GPIO_LED1 "/sys/class/gpio/gpio20"
#define GPIO_LED2 "/sys/class/gpio/gpio21"

// Função que recebe um caminho de arquivo (path) e um valor em string - abre o arquivo no modo escrita.
void writeToFile(const char *path, const char *value){
    FILE *fp = fopen(path, "w");
    if(fp == NULL){
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    fprintf(fp, "%s", value);
    fclose(fp);
}

void exportGPIO(const char *gpio) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/export");
    if (access(gpio, F_OK) == -1) {
        writeToFile(path, gpio + strlen("/sys/class/gpio/gpio"));
        usleep(100000); // Aguarda exportação
    }
}

void setGPIODirection(const char *gpio, const char *direction) {
    char path[128];
    snprintf(path, sizeof(path), "%s/direction", gpio);
    writeToFile(path, direction);
}

void setGPIOValue(const char *gpio, int value) {
    char path[128];
    snprintf(path, sizeof(path), "%s/value", gpio);
    writeToFile(path, value ? "1" : "0");
}

// Função principal
int main()
{
    // Inicializa PWM
    writeToFile(PWM_CHIP "/export", "0");
    writeToFile(PWM0 "/period", "20000000"); // 20ms = 50Hz

    // Inicializa GPIOs dos LEDs
    exportGPIO(GPIO_LED1);
    exportGPIO(GPIO_LED2);
    setGPIODirection(GPIO_LED1, "out");
    setGPIODirection(GPIO_LED2, "out");

    // Ativa PWM
    writeToFile(PWM0 "/enable", "1");

    // Duty cycle para servo: considerando 0.5ms (0°) a 2.5ms (180°), mudar se necessário
    int min_duty = 500000;   // 0°
    int max_duty = 2500000;  // 180°
    int step = 20000;        // Passo para suavidade
    int duty;
    int angle;

    while (1) {
        // 0° a 180°
        for (duty = min_duty; duty <= max_duty; duty += step) {
            writeToFile(PWM0 "/duty_cycle", (char[16]){0});
            snprintf((char *)PWM0 "/duty_cycle", 16, "%d", duty);
            writeToFile(PWM0 "/duty_cycle", (char *)PWM0 "/duty_cycle");
            angle = (duty - min_duty) * 180 / (max_duty - min_duty);
            if (angle <= 90) {
                setGPIOValue(GPIO_LED1, 1);
                setGPIOValue(GPIO_LED2, 0);
            } else {
                setGPIOValue(GPIO_LED1, 0);
                setGPIOValue(GPIO_LED2, 1);
            }
            usleep(30000); // Suavidade de 0,03 segundos
        }
        // 180° a 0°
        for (duty = max_duty; duty >= min_duty; duty -= step) {
            writeToFile(PWM0 "/duty_cycle", (char[16]){0});
            snprintf((char *)PWM0 "/duty_cycle", 16, "%d", duty);
            writeToFile(PWM0 "/duty_cycle", (char *)PWM0 "/duty_cycle");
            angle = (duty - min_duty) * 180 / (max_duty - min_duty);
            if (angle <= 90) {
                setGPIOValue(GPIO_LED1, 1);
                setGPIOValue(GPIO_LED2, 0);
            } else {
                setGPIOValue(GPIO_LED1, 0);
                setGPIOValue(GPIO_LED2, 1);
            }
            usleep(30000);
        }
    }

    return 0;
}

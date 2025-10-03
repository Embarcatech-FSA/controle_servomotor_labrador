# Controle de Servomotor na SBC Labrador

Este projeto controla um servomotor usando PWM na SBC Labrador e acende dois LEDs conforme o ângulo do servo.

## Configuração dos pinos

- **PWM:** O PWM está configurado para o canal `pwmchip0/pwm0`, que corresponde ao pino GPIO 18 na Labrador.
- **LED 1:** Configurado no GPIO 20.
- **LED 2:** Configurado no GPIO 21.

Se precisar alterar os pinos dos LEDs ou do PWM:
- Edite o arquivo `servo_motor_control.c` e altere as macros:
  - `#define GPIO_LED1 "/sys/class/gpio/gpioXX"` (substitua XX pelo número desejado)
  - `#define GPIO_LED2 "/sys/class/gpio/gpioYY"`
  - Para o PWM, ajuste `PWM_CHIP` e `PWM0` conforme necessário.

## Compilação

Para compilar o código:
```bash
gcc -o servomotor servo_motor_control.c 
```

## Execução

Execute o programa com:
```bash
sudo ./servomotor
```

O uso de `sudo` é necessário para acessar os arquivos do sistema relacionados ao GPIO e PWM.

## Funcionamento
- O servo varia suavemente de 0° a 180° e volta, indefinidamente.
- LED 1 acende quando o ângulo ≤ 90°.
- LED 2 acende quando o ângulo > 90°.

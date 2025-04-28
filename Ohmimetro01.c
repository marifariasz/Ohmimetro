/*
 * Por: Wilton Lacerda Silva
 *    Ohmímetro utilizando o ADC da BitDogLab
 *
 * Mede a resistência de um resistor desconhecido entre 560 ohms e 100k ohms
 * e exibe as cores das bandas de um resistor de 4 bandas com base na série E24.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h"
 #include "hardware/i2c.h"
 #include "lib/ssd1306.h"
 #include "lib/font.h"
 
 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define endereco 0x3C
 #define ADC_PIN 28 // GPIO para o voltímetro
 #define Botao_A 5  // GPIO para botão A
 
 int R_conhecido = 10010;   // Resistor de 10k ohm
 float R_x = 0.0;           // Resistor desconhecido
 float ADC_VREF = 3.32;     // Tensão de referência do ADC
 int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
 
 // Trecho para modo BOOTSEL com botão B
 #include "pico/bootrom.h"
 #define botaoB 6
 
 // Cores das bandas do resistor
 const char* cores[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", "Verde", "Azul", "Violeta", "Cinza", "Branco"};
 
 // Função para determinar as cores do resistor com base na série E24 (560Ω a 100kΩ)
 void calcular_cores_resistor(float resistencia, char* banda1, char* banda2, char* multiplicador) {
     int digito1 = 0, digito2 = 0, mult = 0;
 
     // Faixas baseadas na resistência calculada (série E24, ±10% tolerância, 510Ω a 100kΩ)
     if (resistencia < 540){ // ~510Ω
         digito1 = 5; digito2 = 1; mult = 1;
     }else if (resistencia < 620) { // ~560Ω
         digito1 = 5; digito2 = 6; mult = 1;
     } else if (resistencia < 680) { // ~620Ω
         digito1 = 6; digito2 = 2; mult = 1;
     } else if (resistencia < 750) { // ~680Ω
         digito1 = 6; digito2 = 8; mult = 1;
     } else if (resistencia < 820) { // ~750Ω
         digito1 = 7; digito2 = 5; mult = 1;
     } else if (resistencia < 910) { // ~820Ω
         digito1 = 8; digito2 = 2; mult = 1;
     } else if (resistencia < 980) { // ~910Ω
         digito1 = 9; digito2 = 1; mult = 1;
     } else if (resistencia < 1050) { // ~1.0kΩ
         digito1 = 1; digito2 = 0; mult = 2;
     } else if (resistencia < 1200) { // ~1.1kΩ
         digito1 = 1; digito2 = 1; mult = 2;
     } else if (resistencia < 1250) { // ~1.2kΩ
         digito1 = 1; digito2 = 2; mult = 2;
     } else if (resistencia < 1400) { // ~1.3kΩ
         digito1 = 1; digito2 = 3; mult = 2;
     } else if (resistencia < 1550) { // ~1.5kΩ
         digito1 = 1; digito2 = 5; mult = 2;
     } else if (resistencia < 1700) { // ~1.6kΩ
         digito1 = 1; digito2 = 6; mult = 2;
     } else if (resistencia < 1860) { // ~1.8kΩ
         digito1 = 1; digito2 = 8; mult = 2;
     } else if (resistencia < 2100) { // ~2.0kΩ
         digito1 = 2; digito2 = 0; mult = 2;
     } else if (resistencia < 2300) { // ~2.2kΩ
         digito1 = 2; digito2 = 2; mult = 2;
     } else if (resistencia < 2500) { // ~2.4kΩ
         digito1 = 2; digito2 = 4; mult = 2;
     } else if (resistencia < 2800) { // ~2.7kΩ
         digito1 = 2; digito2 = 7; mult = 2;
     } else if (resistencia < 3100) { // ~3.0kΩ
         digito1 = 3; digito2 = 0; mult = 2;
     } else if (resistencia < 3400) { // ~3.3kΩ
         digito1 = 3; digito2 = 3; mult = 2;
     } else if (resistencia < 3700) { // ~3.6kΩ
         digito1 = 3; digito2 = 6; mult = 2;
     } else if (resistencia < 4000) { // ~3.9kΩ
         digito1 = 3; digito2 = 9; mult = 2;
     } else if (resistencia < 4400) { // ~4.3kΩ
         digito1 = 4; digito2 = 3; mult = 2;
     } else if (resistencia < 4800) { // ~4.7kΩ
         digito1 = 4; digito2 = 7; mult = 2;
     } else if (resistencia < 5250) { // ~5.1kΩ
         digito1 = 5; digito2 = 1; mult = 2;
     } else if (resistencia < 5800) { // ~5.6kΩ
         digito1 = 5; digito2 = 6; mult = 2;
     } else if (resistencia < 6300) { // ~6.2kΩ
         digito1 = 6; digito2 = 2; mult = 2;
     } else if (resistencia < 6900) { // ~6.8kΩ
         digito1 = 6; digito2 = 8; mult = 2;
     } else if (resistencia < 7600) { // ~7.5kΩ
         digito1 = 7; digito2 = 5; mult = 2;
     } else if (resistencia < 8300) { // ~8.2kΩ
         digito1 = 8; digito2 = 2; mult = 2;
     } else if (resistencia < 9200) { // ~9.1kΩ
         digito1 = 9; digito2 = 1; mult = 2;
     } else if (resistencia < 10300) { // ~10kΩ
         digito1 = 1; digito2 = 0; mult = 3;
     } else if (resistencia < 11700) { // ~11kΩ
         digito1 = 1; digito2 = 1; mult = 3;
     } else if (resistencia < 12500) { // ~12kΩ
         digito1 = 1; digito2 = 2; mult = 3;
     } else if (resistencia < 13800) { // ~13kΩ
         digito1 = 1; digito2 = 3; mult = 3;
     } else if (resistencia < 15500) { // ~15kΩ
         digito1 = 1; digito2 = 5; mult = 3;
     } else if (resistencia < 16500) { // ~16kΩ
         digito1 = 1; digito2 = 6; mult = 3;
     } else if (resistencia < 18500) { // ~18kΩ
         digito1 = 1; digito2 = 8; mult = 3;
     } else if (resistencia < 20000) { // ~20kΩ
         digito1 = 2; digito2 = 0; mult = 3;
     } else if (resistencia < 22500) { // ~22kΩ
         digito1 = 2; digito2 = 2; mult = 3;
     } else if (resistencia < 24500) { // ~24kΩ
         digito1 = 2; digito2 = 4; mult = 3;
     } else if (resistencia < 27500) { // ~27kΩ
         digito1 = 2; digito2 = 7; mult = 3;
     } else if (resistencia < 30500) { // ~30kΩ
         digito1 = 3; digito2 = 0; mult = 3;
     } else if (resistencia < 32500) { // ~33kΩ
         digito1 = 3; digito2 = 3; mult = 3;
     } else if (resistencia < 34500) { // ~36kΩ
         digito1 = 3; digito2 = 6; mult = 3;
     } else if (resistencia < 39000) { // ~39kΩ
         digito1 = 3; digito2 = 9; mult = 3;
     } else if (resistencia < 43500) { // ~43kΩ
         digito1 = 4; digito2 = 3; mult = 3;
     } else if (resistencia < 45000) { // ~47kΩ
         digito1 = 4; digito2 = 7; mult = 3;
     } else if (resistencia < 50000) { // ~51kΩ
         digito1 = 5; digito2 = 1; mult = 3;
     } else if (resistencia < 52000) { // ~56kΩ
         digito1 = 5; digito2 = 6; mult = 3;
     } else if (resistencia < 57000) { // ~62kΩ
         digito1 = 6; digito2 = 2; mult = 3;
     } else if (resistencia < 63000) { // ~68kΩ
         digito1 = 6; digito2 = 8; mult = 3;
     } else if (resistencia < 70000) { // ~75kΩ
         digito1 = 7; digito2 = 5; mult = 3;
     } else if (resistencia < 73000) { // ~82kΩ
         digito1 = 8; digito2 = 2; mult = 3;
     } else if (resistencia < 83000) { // ~91kΩ
         digito1 = 9; digito2 = 1; mult = 3;
     } else { // ~100kΩ ou acima
         digito1 = 1; digito2 = 0; mult = 4;
     }
 
     // Atribui as cores
     sprintf(banda1, "%s", cores[digito1]);
     sprintf(banda2, "%s", cores[digito2]);
     sprintf(multiplicador, "%s", cores[mult]);
 }
 
 void gpio_irq_handler(uint gpio, uint32_t events) {
     reset_usb_boot(0, 0);
 }
 
 int main() {
     // Configuração do modo BOOTSEL com botão B
     gpio_init(botaoB);
     gpio_set_dir(botaoB, GPIO_IN);
     gpio_pull_up(botaoB);
     gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
 
     gpio_init(Botao_A);
     gpio_set_dir(Botao_A, GPIO_IN);
     gpio_pull_up(Botao_A);
 
     // Inicialização do I2C
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C_SDA);
     gpio_pull_up(I2C_SCL);
 
     // Inicialização do display SSD1306
     ssd1306_t ssd;
     ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
     ssd1306_config(&ssd);
     ssd1306_send_data(&ssd);
     ssd1306_fill(&ssd, false);
     ssd1306_send_data(&ssd);
 
     // Inicialização do ADC
     adc_init();
     adc_gpio_init(ADC_PIN);
 
     char str_x[10]; // Buffer para ADC
     char str_y[10]; // Buffer para resistência
     bool cor = true;
 
     while (true) {
         adc_select_input(2); // ADC2 (GPIO 28)
 
         // Média de 1000 leituras para maior estabilidade
         float soma = 0.0f;
         for (int i = 0; i < 1000; i++) {
             soma += adc_read();
             sleep_ms(1);
         }
         float media = soma / 1000.0f;
 
         // Calcula resistência com proteção contra divisão por zero
         if (media < ADC_RESOLUTION - 10) { // Evita valores próximos ao máximo
             R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);
         } else {
             R_x = 100000.0f; // Valor alto para resistências muito grandes
         }
 
         // Determina as cores do resistor com base na resistência
         char banda1[10], banda2[10], multiplicador[10];
         calcular_cores_resistor(R_x, banda1, banda2, multiplicador);
 
         // Formata strings para exibição
         sprintf(str_x, "%.0f", media);
         sprintf(str_y, "%.0f", R_x);
 
         // Atualiza o display
         ssd1306_fill(&ssd, !cor);
         ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
         ssd1306_draw_string(&ssd, banda1, 5, 0); // Primeira cor
         ssd1306_draw_string(&ssd, banda2, 5, 10); // Segunda cor
         ssd1306_draw_string(&ssd, multiplicador, 5, 20); // Multiplicador
         ssd1306_line(&ssd, 3, 37, 123, 37, cor);
         ssd1306_draw_string(&ssd, "ADC: ", 5, 40);
         ssd1306_draw_string(&ssd, str_x, 35, 40); // Valor ADC
         ssd1306_draw_string(&ssd, "Res: ", 5, 50);
         ssd1306_draw_string(&ssd, str_y, 35, 50); // Valor resistência
         ssd1306_send_data(&ssd);
 
         cor = !cor; // Alterna o fundo
         sleep_ms(700);
     }
 }

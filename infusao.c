#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "mqtt_client.h"

// Definições de pinos
#define LED_PIN 15
#define BOTAO_PIN 14

// Configuração Wi-Fi e MQTT
#define WIFI_SSID "SEU_WIFI"
#define WIFI_PASS "SUA_SENHA"
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "infusao/status"

mqtt_client_t client;
ip_addr_t broker_ip;

void publicar_mensagem(const char *mensagem) {
    err_t err = mqtt_publish(&client, MQTT_TOPIC, mensagem, strlen(mensagem), 1, 0, NULL, NULL);
    if (err == ERR_OK) {
        printf("📡 Mensagem enviada: %s\n", mensagem);
    } else {
        printf("❌ Erro ao enviar MQTT\n");
    }
}

void setup() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BOTAO_PIN);
    gpio_set_dir(BOTAO_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_PIN);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    printf("🔗 Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK, 10000) != 0) {
        printf("❌ Falha ao conectar!\n");
        return;
    }
    printf("✅ Conectado ao Wi-Fi!\n");

    ipaddr_aton(MQTT_BROKER, &broker_ip);
    mqtt_client_connect(&client, &broker_ip, MQTT_PORT, NULL, NULL);
    printf("📡 Conectado ao MQTT!\n");
}

void loop() {
    if (gpio_get(BOTAO_PIN) == 0) {
        gpio_put(LED_PIN, 1);
        publicar_mensagem("Infusão em andamento");
    } else {
        gpio_put(LED_PIN, 0);
        publicar_mensagem("Parado");
    }
    sleep_ms(5000);
}

int main() {
    setup();
    while (1) {
        loop();
    }
}

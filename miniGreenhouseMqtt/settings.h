/*
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0
 *
 * SPDX-License-Identifier: EPL-2.0
 */
#ifndef SETTINGS_H
#define SETTING_H

// ---- WiFi configuration ----
#define WIFI_SSID "ssid"         // The SSID of the WiFi you want your octopus board to connect to
#define WIFI_PASSWORD "password"     // The password of the WiFi you want your octopus board to connect to

// ---- Hono MQTT configuration ----
#define MQTT_BROKER "192.168.0.29"
#define MQTT_ID "plantnode1"
#define MQTT_USECREDENTIALS true
#define MQTT_USERNAME "mqtt"
#define MQTT_PASSWORD "greg"
#define MQTT_PUBTOPIC "account-name/PD1/W1/A1/PlantData"
uint16_t MQTT_PORT = 1834;
/* SHA-1 fingerprint of the server certificate of the MQTT broker, UPPERCASE and spacing */
/* openssl s_client -connect hono.eclipse.org:8883 < /dev/null 2>/dev/null | openssl x509 -fingerprint -noout -in /dev/stdin */
#define MQTT_SERVER_FINGERPRINT "8E 64 5C A0 4C C8 8B E7 86 F0 32 64 1D 9A 9E A3 F3 6A 4A DF"

#endif

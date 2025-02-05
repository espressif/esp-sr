#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_process_sdkconfig.h"
#include "esp_mn_speech_commands.h"

static char *TAG = "SR_SYS";

void check_chip_config(void)
{
#ifdef CONFIG_IDF_TARGET_ESP32S3
#ifndef CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240
    ESP_LOGW(TAG, "CPU freq should be 240MHz");
#endif

#if (! defined CONFIG_ESPTOOLPY_FLASHFREQ_80M) && (! defined CONFIG_ESPTOOLPY_FLASHFREQ_120M)
    ESP_LOGW(TAG, "Flash freq should be not less than 80MHz");
#endif

#if (! defined CONFIG_SPIRAM_SPEED_80M) && (! defined CONFIG_SPIRAM_SPEED_120M)
    ESP_LOGW(TAG, "PSRAM freq should be not less than 80MHz");
#endif

#ifndef CONFIG_ESP32S3_DATA_CACHE_64KB
    ESP_LOGW(TAG, "Data cache recommends 64KB");
#endif

#ifndef CONFIG_ESP32S3_DATA_CACHE_LINE_64B
    ESP_LOGW(TAG, "Data cache line recommends 64B");
#endif

#elif CONFIG_IDF_TARGET_ESP32
#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_240
    ESP_LOGW(TAG, "CPU freq should be 240MHz");
#endif

#ifndef CONFIG_SPIRAM_SPEED_80M
    ESP_LOGW(TAG, "PSRAM freq should be 80MHz");
#endif

#ifndef CONFIG_ESPTOOLPY_FLASHFREQ_80M
    ESP_LOGW(TAG, "Flash freq should be 80MHz");
#endif

#ifndef CONFIG_ESPTOOLPY_FLASHMODE_QIO
    ESP_LOGW(TAG, "Flash mode should be QIO");
#endif

#elif CONFIG_IDF_TARGET_ESP32P4
#if (! defined CONFIG_ESPTOOLPY_FLASHFREQ_80M)
    ESP_LOGW(TAG, "Flash freq should be 80MHz");
#endif

#ifndef CONFIG_SPIRAM_SPEED_200M
    ESP_LOGW(TAG, "PSRAM freq should be 200MHz");
#endif

#ifdef CONFIG_ESP32P4_DATA_CACHE_128KB
    ESP_LOGW(TAG, "Recommend data cache larger than 128KB");
#endif

#ifdef CONFIG_ESP32P4_DATA_CACHE_LINE_64B
    ESP_LOGW(TAG, "Recommend data cache line larger than 64B");
#endif

#else
    ESP_LOGW(TAG, "ESP-SR-AFE only support ESP32/ESP32S3/ESP32P4");
#endif
}


char *get_id_name_cn(int i)
{
#if defined CONFIG_SR_MN_CN_MULTINET2_SINGLE_RECOGNITION || defined CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION || defined CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION_QUANT8 || defined CONFIG_SR_MN_CN_MULTINET5_RECOGNITION_QUANT8
    if (i == 0) {
        return CONFIG_CN_SPEECH_COMMAND_ID0;
    } else if (i == 1) {
        return CONFIG_CN_SPEECH_COMMAND_ID1;
    } else if (i == 2) {
        return CONFIG_CN_SPEECH_COMMAND_ID2;
    } else if (i == 3) {
        return CONFIG_CN_SPEECH_COMMAND_ID3;
    } else if (i == 4) {
        return CONFIG_CN_SPEECH_COMMAND_ID4;
    } else if (i == 5) {
        return CONFIG_CN_SPEECH_COMMAND_ID5;
    } else if (i == 6) {
        return CONFIG_CN_SPEECH_COMMAND_ID6;
    } else if (i == 7) {
        return CONFIG_CN_SPEECH_COMMAND_ID7;
    } else if (i == 8) {
        return CONFIG_CN_SPEECH_COMMAND_ID8;
    } else if (i == 9) {
        return CONFIG_CN_SPEECH_COMMAND_ID9;
    } else if (i == 10) {
        return CONFIG_CN_SPEECH_COMMAND_ID10;
    } else if (i == 11) {
        return CONFIG_CN_SPEECH_COMMAND_ID11;
    } else if (i == 12) {
        return CONFIG_CN_SPEECH_COMMAND_ID12;
    } else if (i == 13) {
        return CONFIG_CN_SPEECH_COMMAND_ID13;
    } else if (i == 14) {
        return CONFIG_CN_SPEECH_COMMAND_ID14;
    } else if (i == 15) {
        return CONFIG_CN_SPEECH_COMMAND_ID15;
    } else if (i == 16) {
        return CONFIG_CN_SPEECH_COMMAND_ID16;
    } else if (i == 17) {
        return CONFIG_CN_SPEECH_COMMAND_ID17;
    } else if (i == 18) {
        return CONFIG_CN_SPEECH_COMMAND_ID18;
    } else if (i == 19) {
        return CONFIG_CN_SPEECH_COMMAND_ID19;
    } else if (i == 20) {
        return CONFIG_CN_SPEECH_COMMAND_ID20;
    } else if (i == 21) {
        return CONFIG_CN_SPEECH_COMMAND_ID21;
    } else if (i == 22) {
        return CONFIG_CN_SPEECH_COMMAND_ID22;
    } else if (i == 23) {
        return CONFIG_CN_SPEECH_COMMAND_ID23;
    } else if (i == 24) {
        return CONFIG_CN_SPEECH_COMMAND_ID24;
    } else if (i == 25) {
        return CONFIG_CN_SPEECH_COMMAND_ID25;
    } else if (i == 26) {
        return CONFIG_CN_SPEECH_COMMAND_ID26;
    } else if (i == 27) {
        return CONFIG_CN_SPEECH_COMMAND_ID27;
    } else if (i == 28) {
        return CONFIG_CN_SPEECH_COMMAND_ID28;
    } else if (i == 29) {
        return CONFIG_CN_SPEECH_COMMAND_ID29;
    } else if (i == 30) {
        return CONFIG_CN_SPEECH_COMMAND_ID30;
    } else if (i == 31) {
        return CONFIG_CN_SPEECH_COMMAND_ID31;
    } else if (i == 32) {
        return CONFIG_CN_SPEECH_COMMAND_ID32;
    } else if (i == 33) {
        return CONFIG_CN_SPEECH_COMMAND_ID33;
    } else if (i == 34) {
        return CONFIG_CN_SPEECH_COMMAND_ID34;
    } else if (i == 35) {
        return CONFIG_CN_SPEECH_COMMAND_ID35;
    } else if (i == 36) {
        return CONFIG_CN_SPEECH_COMMAND_ID36;
    } else if (i == 37) {
        return CONFIG_CN_SPEECH_COMMAND_ID37;
    } else if (i == 38) {
        return CONFIG_CN_SPEECH_COMMAND_ID38;
    } else if (i == 39) {
        return CONFIG_CN_SPEECH_COMMAND_ID39;
    } else if (i == 40) {
        return CONFIG_CN_SPEECH_COMMAND_ID40;
    } else if (i == 41) {
        return CONFIG_CN_SPEECH_COMMAND_ID41;
    } else if (i == 42) {
        return CONFIG_CN_SPEECH_COMMAND_ID42;
    } else if (i == 43) {
        return CONFIG_CN_SPEECH_COMMAND_ID43;
    } else if (i == 44) {
        return CONFIG_CN_SPEECH_COMMAND_ID44;
    } else if (i == 45) {
        return CONFIG_CN_SPEECH_COMMAND_ID45;
    } else if (i == 46) {
        return CONFIG_CN_SPEECH_COMMAND_ID46;
    } else if (i == 47) {
        return CONFIG_CN_SPEECH_COMMAND_ID47;
    } else if (i == 48) {
        return CONFIG_CN_SPEECH_COMMAND_ID48;
    } else if (i == 49) {
        return CONFIG_CN_SPEECH_COMMAND_ID49;
    } else if (i == 50) {
        return CONFIG_CN_SPEECH_COMMAND_ID50;
    } else if (i == 51) {
        return CONFIG_CN_SPEECH_COMMAND_ID51;
    } else if (i == 52) {
        return CONFIG_CN_SPEECH_COMMAND_ID52;
    } else if (i == 53) {
        return CONFIG_CN_SPEECH_COMMAND_ID53;
    } else if (i == 54) {
        return CONFIG_CN_SPEECH_COMMAND_ID54;
    } else if (i == 55) {
        return CONFIG_CN_SPEECH_COMMAND_ID55;
    } else if (i == 56) {
        return CONFIG_CN_SPEECH_COMMAND_ID56;
    } else if (i == 57) {
        return CONFIG_CN_SPEECH_COMMAND_ID57;
    } else if (i == 58) {
        return CONFIG_CN_SPEECH_COMMAND_ID58;
    } else if (i == 59) {
        return CONFIG_CN_SPEECH_COMMAND_ID59;
    } else if (i == 60) {
        return CONFIG_CN_SPEECH_COMMAND_ID60;
    } else if (i == 61) {
        return CONFIG_CN_SPEECH_COMMAND_ID61;
    } else if (i == 62) {
        return CONFIG_CN_SPEECH_COMMAND_ID62;
    } else if (i == 63) {
        return CONFIG_CN_SPEECH_COMMAND_ID63;
    } else if (i == 64) {
        return CONFIG_CN_SPEECH_COMMAND_ID64;
    } else if (i == 65) {
        return CONFIG_CN_SPEECH_COMMAND_ID65;
    } else if (i == 66) {
        return CONFIG_CN_SPEECH_COMMAND_ID66;
    } else if (i == 67) {
        return CONFIG_CN_SPEECH_COMMAND_ID67;
    } else if (i == 68) {
        return CONFIG_CN_SPEECH_COMMAND_ID68;
    } else if (i == 69) {
        return CONFIG_CN_SPEECH_COMMAND_ID69;
    } else if (i == 70) {
        return CONFIG_CN_SPEECH_COMMAND_ID70;
    } else if (i == 71) {
        return CONFIG_CN_SPEECH_COMMAND_ID71;
    } else if (i == 72) {
        return CONFIG_CN_SPEECH_COMMAND_ID72;
    } else if (i == 73) {
        return CONFIG_CN_SPEECH_COMMAND_ID73;
    } else if (i == 74) {
        return CONFIG_CN_SPEECH_COMMAND_ID74;
    } else if (i == 75) {
        return CONFIG_CN_SPEECH_COMMAND_ID75;
    } else if (i == 76) {
        return CONFIG_CN_SPEECH_COMMAND_ID76;
    } else if (i == 77) {
        return CONFIG_CN_SPEECH_COMMAND_ID77;
    } else if (i == 78) {
        return CONFIG_CN_SPEECH_COMMAND_ID78;
    } else if (i == 79) {
        return CONFIG_CN_SPEECH_COMMAND_ID79;
    } else if (i == 80) {
        return CONFIG_CN_SPEECH_COMMAND_ID80;
    } else if (i == 81) {
        return CONFIG_CN_SPEECH_COMMAND_ID81;
    } else if (i == 82) {
        return CONFIG_CN_SPEECH_COMMAND_ID82;
    } else if (i == 83) {
        return CONFIG_CN_SPEECH_COMMAND_ID83;
    } else if (i == 84) {
        return CONFIG_CN_SPEECH_COMMAND_ID84;
    } else if (i == 85) {
        return CONFIG_CN_SPEECH_COMMAND_ID85;
    } else if (i == 86) {
        return CONFIG_CN_SPEECH_COMMAND_ID86;
    } else if (i == 87) {
        return CONFIG_CN_SPEECH_COMMAND_ID87;
    } else if (i == 88) {
        return CONFIG_CN_SPEECH_COMMAND_ID88;
    } else if (i == 89) {
        return CONFIG_CN_SPEECH_COMMAND_ID89;
    } else if (i == 90) {
        return CONFIG_CN_SPEECH_COMMAND_ID90;
    } else if (i == 91) {
        return CONFIG_CN_SPEECH_COMMAND_ID91;
    } else if (i == 92) {
        return CONFIG_CN_SPEECH_COMMAND_ID92;
    } else if (i == 93) {
        return CONFIG_CN_SPEECH_COMMAND_ID93;
    } else if (i == 94) {
        return CONFIG_CN_SPEECH_COMMAND_ID94;
    } else if (i == 95) {
        return CONFIG_CN_SPEECH_COMMAND_ID95;
    } else if (i == 96) {
        return CONFIG_CN_SPEECH_COMMAND_ID96;
    } else if (i == 97) {
        return CONFIG_CN_SPEECH_COMMAND_ID97;
    } else if (i == 98) {
        return CONFIG_CN_SPEECH_COMMAND_ID98;
    } else if (i == 99) {
        return CONFIG_CN_SPEECH_COMMAND_ID99;
    } else if (i == 100) {
        return CONFIG_CN_SPEECH_COMMAND_ID100;
    } else if (i == 101) {
        return CONFIG_CN_SPEECH_COMMAND_ID101;
    } else if (i == 102) {
        return CONFIG_CN_SPEECH_COMMAND_ID102;
    } else if (i == 103) {
        return CONFIG_CN_SPEECH_COMMAND_ID103;
    } else if (i == 104) {
        return CONFIG_CN_SPEECH_COMMAND_ID104;
    } else if (i == 105) {
        return CONFIG_CN_SPEECH_COMMAND_ID105;
    } else if (i == 106) {
        return CONFIG_CN_SPEECH_COMMAND_ID106;
    } else if (i == 107) {
        return CONFIG_CN_SPEECH_COMMAND_ID107;
    } else if (i == 108) {
        return CONFIG_CN_SPEECH_COMMAND_ID108;
    } else if (i == 109) {
        return CONFIG_CN_SPEECH_COMMAND_ID109;
    } else if (i == 110) {
        return CONFIG_CN_SPEECH_COMMAND_ID110;
    } else if (i == 111) {
        return CONFIG_CN_SPEECH_COMMAND_ID111;
    } else if (i == 112) {
        return CONFIG_CN_SPEECH_COMMAND_ID112;
    } else if (i == 113) {
        return CONFIG_CN_SPEECH_COMMAND_ID113;
    } else if (i == 114) {
        return CONFIG_CN_SPEECH_COMMAND_ID114;
    } else if (i == 115) {
        return CONFIG_CN_SPEECH_COMMAND_ID115;
    } else if (i == 116) {
        return CONFIG_CN_SPEECH_COMMAND_ID116;
    } else if (i == 117) {
        return CONFIG_CN_SPEECH_COMMAND_ID117;
    } else if (i == 118) {
        return CONFIG_CN_SPEECH_COMMAND_ID118;
    } else if (i == 119) {
        return CONFIG_CN_SPEECH_COMMAND_ID119;
    } else if (i == 120) {
        return CONFIG_CN_SPEECH_COMMAND_ID120;
    } else if (i == 121) {
        return CONFIG_CN_SPEECH_COMMAND_ID121;
    } else if (i == 122) {
        return CONFIG_CN_SPEECH_COMMAND_ID122;
    } else if (i == 123) {
        return CONFIG_CN_SPEECH_COMMAND_ID123;
    } else if (i == 124) {
        return CONFIG_CN_SPEECH_COMMAND_ID124;
    } else if (i == 125) {
        return CONFIG_CN_SPEECH_COMMAND_ID125;
    } else if (i == 126) {
        return CONFIG_CN_SPEECH_COMMAND_ID126;
    } else if (i == 127) {
        return CONFIG_CN_SPEECH_COMMAND_ID127;
    } else if (i == 128) {
        return CONFIG_CN_SPEECH_COMMAND_ID128;
    } else if (i == 129) {
        return CONFIG_CN_SPEECH_COMMAND_ID129;
    } else if (i == 130) {
        return CONFIG_CN_SPEECH_COMMAND_ID130;
    } else if (i == 131) {
        return CONFIG_CN_SPEECH_COMMAND_ID131;
    } else if (i == 132) {
        return CONFIG_CN_SPEECH_COMMAND_ID132;
    } else if (i == 133) {
        return CONFIG_CN_SPEECH_COMMAND_ID133;
    } else if (i == 134) {
        return CONFIG_CN_SPEECH_COMMAND_ID134;
    } else if (i == 135) {
        return CONFIG_CN_SPEECH_COMMAND_ID135;
    } else if (i == 136) {
        return CONFIG_CN_SPEECH_COMMAND_ID136;
    } else if (i == 137) {
        return CONFIG_CN_SPEECH_COMMAND_ID137;
    } else if (i == 138) {
        return CONFIG_CN_SPEECH_COMMAND_ID138;
    } else if (i == 139) {
        return CONFIG_CN_SPEECH_COMMAND_ID139;
    } else if (i == 140) {
        return CONFIG_CN_SPEECH_COMMAND_ID140;
    } else if (i == 141) {
        return CONFIG_CN_SPEECH_COMMAND_ID141;
    } else if (i == 142) {
        return CONFIG_CN_SPEECH_COMMAND_ID142;
    } else if (i == 143) {
        return CONFIG_CN_SPEECH_COMMAND_ID143;
    } else if (i == 144) {
        return CONFIG_CN_SPEECH_COMMAND_ID144;
    } else if (i == 145) {
        return CONFIG_CN_SPEECH_COMMAND_ID145;
    } else if (i == 146) {
        return CONFIG_CN_SPEECH_COMMAND_ID146;
    } else if (i == 147) {
        return CONFIG_CN_SPEECH_COMMAND_ID147;
    } else if (i == 148) {
        return CONFIG_CN_SPEECH_COMMAND_ID148;
    } else if (i == 149) {
        return CONFIG_CN_SPEECH_COMMAND_ID149;
    } else if (i == 150) {
        return CONFIG_CN_SPEECH_COMMAND_ID150;
    } else if (i == 151) {
        return CONFIG_CN_SPEECH_COMMAND_ID151;
    } else if (i == 152) {
        return CONFIG_CN_SPEECH_COMMAND_ID152;
    } else if (i == 153) {
        return CONFIG_CN_SPEECH_COMMAND_ID153;
    } else if (i == 154) {
        return CONFIG_CN_SPEECH_COMMAND_ID154;
    } else if (i == 155) {
        return CONFIG_CN_SPEECH_COMMAND_ID155;
    } else if (i == 156) {
        return CONFIG_CN_SPEECH_COMMAND_ID156;
    } else if (i == 157) {
        return CONFIG_CN_SPEECH_COMMAND_ID157;
    } else if (i == 158) {
        return CONFIG_CN_SPEECH_COMMAND_ID158;
    } else if (i == 159) {
        return CONFIG_CN_SPEECH_COMMAND_ID159;
    } else if (i == 160) {
        return CONFIG_CN_SPEECH_COMMAND_ID160;
    } else if (i == 161) {
        return CONFIG_CN_SPEECH_COMMAND_ID161;
    } else if (i == 162) {
        return CONFIG_CN_SPEECH_COMMAND_ID162;
    } else if (i == 163) {
        return CONFIG_CN_SPEECH_COMMAND_ID163;
    } else if (i == 164) {
        return CONFIG_CN_SPEECH_COMMAND_ID164;
    } else if (i == 165) {
        return CONFIG_CN_SPEECH_COMMAND_ID165;
    } else if (i == 166) {
        return CONFIG_CN_SPEECH_COMMAND_ID166;
    } else if (i == 167) {
        return CONFIG_CN_SPEECH_COMMAND_ID167;
    } else if (i == 168) {
        return CONFIG_CN_SPEECH_COMMAND_ID168;
    } else if (i == 169) {
        return CONFIG_CN_SPEECH_COMMAND_ID169;
    } else if (i == 170) {
        return CONFIG_CN_SPEECH_COMMAND_ID170;
    } else if (i == 171) {
        return CONFIG_CN_SPEECH_COMMAND_ID171;
    } else if (i == 172) {
        return CONFIG_CN_SPEECH_COMMAND_ID172;
    } else if (i == 173) {
        return CONFIG_CN_SPEECH_COMMAND_ID173;
    } else if (i == 174) {
        return CONFIG_CN_SPEECH_COMMAND_ID174;
    } else if (i == 175) {
        return CONFIG_CN_SPEECH_COMMAND_ID175;
    } else if (i == 176) {
        return CONFIG_CN_SPEECH_COMMAND_ID176;
    } else if (i == 177) {
        return CONFIG_CN_SPEECH_COMMAND_ID177;
    } else if (i == 178) {
        return CONFIG_CN_SPEECH_COMMAND_ID178;
    } else if (i == 179) {
        return CONFIG_CN_SPEECH_COMMAND_ID179;
    } else if (i == 180) {
        return CONFIG_CN_SPEECH_COMMAND_ID180;
    } else if (i == 181) {
        return CONFIG_CN_SPEECH_COMMAND_ID181;
    } else if (i == 182) {
        return CONFIG_CN_SPEECH_COMMAND_ID182;
    } else if (i == 183) {
        return CONFIG_CN_SPEECH_COMMAND_ID183;
    } else if (i == 184) {
        return CONFIG_CN_SPEECH_COMMAND_ID184;
    } else if (i == 185) {
        return CONFIG_CN_SPEECH_COMMAND_ID185;
    } else if (i == 186) {
        return CONFIG_CN_SPEECH_COMMAND_ID186;
    } else if (i == 187) {
        return CONFIG_CN_SPEECH_COMMAND_ID187;
    } else if (i == 188) {
        return CONFIG_CN_SPEECH_COMMAND_ID188;
    } else if (i == 189) {
        return CONFIG_CN_SPEECH_COMMAND_ID189;
    } else if (i == 190) {
        return CONFIG_CN_SPEECH_COMMAND_ID190;
    } else if (i == 191) {
        return CONFIG_CN_SPEECH_COMMAND_ID191;
    } else if (i == 192) {
        return CONFIG_CN_SPEECH_COMMAND_ID192;
    } else if (i == 193) {
        return CONFIG_CN_SPEECH_COMMAND_ID193;
    } else if (i == 194) {
        return CONFIG_CN_SPEECH_COMMAND_ID194;
    } else if (i == 195) {
        return CONFIG_CN_SPEECH_COMMAND_ID195;
    } else if (i == 196) {
        return CONFIG_CN_SPEECH_COMMAND_ID196;
    } else if (i == 197) {
        return CONFIG_CN_SPEECH_COMMAND_ID197;
    } else if (i == 198) {
        return CONFIG_CN_SPEECH_COMMAND_ID198;
    } else if (i == 199) {
        return CONFIG_CN_SPEECH_COMMAND_ID199;
    } else {
        return NULL;
    }
#else
    return NULL;
#endif
}

char *get_id_name_en(int i)
{
#if CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION_QUANT8
    if (i == 0) {
        return CONFIG_EN_SPEECH_COMMAND_ID0;
    } else if (i == 1) {
        return CONFIG_EN_SPEECH_COMMAND_ID1;
    } else if (i == 2) {
        return CONFIG_EN_SPEECH_COMMAND_ID2;
    } else if (i == 3) {
        return CONFIG_EN_SPEECH_COMMAND_ID3;
    } else if (i == 4) {
        return CONFIG_EN_SPEECH_COMMAND_ID4;
    } else if (i == 5) {
        return CONFIG_EN_SPEECH_COMMAND_ID5;
    } else if (i == 6) {
        return CONFIG_EN_SPEECH_COMMAND_ID6;
    } else if (i == 7) {
        return CONFIG_EN_SPEECH_COMMAND_ID7;
    } else if (i == 8) {
        return CONFIG_EN_SPEECH_COMMAND_ID8;
    } else if (i == 9) {
        return CONFIG_EN_SPEECH_COMMAND_ID9;
    } else if (i == 10) {
        return CONFIG_EN_SPEECH_COMMAND_ID10;
    } else if (i == 11) {
        return CONFIG_EN_SPEECH_COMMAND_ID11;
    } else if (i == 12) {
        return CONFIG_EN_SPEECH_COMMAND_ID12;
    } else if (i == 13) {
        return CONFIG_EN_SPEECH_COMMAND_ID13;
    } else if (i == 14) {
        return CONFIG_EN_SPEECH_COMMAND_ID14;
    } else if (i == 15) {
        return CONFIG_EN_SPEECH_COMMAND_ID15;
    } else if (i == 16) {
        return CONFIG_EN_SPEECH_COMMAND_ID16;
    } else if (i == 17) {
        return CONFIG_EN_SPEECH_COMMAND_ID17;
    } else if (i == 18) {
        return CONFIG_EN_SPEECH_COMMAND_ID18;
    } else if (i == 19) {
        return CONFIG_EN_SPEECH_COMMAND_ID19;
    } else if (i == 20) {
        return CONFIG_EN_SPEECH_COMMAND_ID20;
    } else if (i == 21) {
        return CONFIG_EN_SPEECH_COMMAND_ID21;
    } else if (i == 22) {
        return CONFIG_EN_SPEECH_COMMAND_ID22;
    } else if (i == 23) {
        return CONFIG_EN_SPEECH_COMMAND_ID23;
    } else if (i == 24) {
        return CONFIG_EN_SPEECH_COMMAND_ID24;
    } else if (i == 25) {
        return CONFIG_EN_SPEECH_COMMAND_ID25;
    } else if (i == 26) {
        return CONFIG_EN_SPEECH_COMMAND_ID26;
    } else if (i == 27) {
        return CONFIG_EN_SPEECH_COMMAND_ID27;
    } else if (i == 28) {
        return CONFIG_EN_SPEECH_COMMAND_ID28;
    } else if (i == 29) {
        return CONFIG_EN_SPEECH_COMMAND_ID29;
    } else if (i == 30) {
        return CONFIG_EN_SPEECH_COMMAND_ID30;
    } else if (i == 31) {
        return CONFIG_EN_SPEECH_COMMAND_ID31;
    } else if (i == 32) {
        return CONFIG_EN_SPEECH_COMMAND_ID32;
    } else if (i == 33) {
        return CONFIG_EN_SPEECH_COMMAND_ID33;
    } else if (i == 34) {
        return CONFIG_EN_SPEECH_COMMAND_ID34;
    } else if (i == 35) {
        return CONFIG_EN_SPEECH_COMMAND_ID35;
    } else if (i == 36) {
        return CONFIG_EN_SPEECH_COMMAND_ID36;
    } else if (i == 37) {
        return CONFIG_EN_SPEECH_COMMAND_ID37;
    } else if (i == 38) {
        return CONFIG_EN_SPEECH_COMMAND_ID38;
    } else if (i == 39) {
        return CONFIG_EN_SPEECH_COMMAND_ID39;
    } else if (i == 40) {
        return CONFIG_EN_SPEECH_COMMAND_ID40;
    } else if (i == 41) {
        return CONFIG_EN_SPEECH_COMMAND_ID41;
    } else if (i == 42) {
        return CONFIG_EN_SPEECH_COMMAND_ID42;
    } else if (i == 43) {
        return CONFIG_EN_SPEECH_COMMAND_ID43;
    } else if (i == 44) {
        return CONFIG_EN_SPEECH_COMMAND_ID44;
    } else if (i == 45) {
        return CONFIG_EN_SPEECH_COMMAND_ID45;
    } else if (i == 46) {
        return CONFIG_EN_SPEECH_COMMAND_ID46;
    } else if (i == 47) {
        return CONFIG_EN_SPEECH_COMMAND_ID47;
    } else if (i == 48) {
        return CONFIG_EN_SPEECH_COMMAND_ID48;
    } else if (i == 49) {
        return CONFIG_EN_SPEECH_COMMAND_ID49;
    } else if (i == 50) {
        return CONFIG_EN_SPEECH_COMMAND_ID50;
    } else if (i == 51) {
        return CONFIG_EN_SPEECH_COMMAND_ID51;
    } else if (i == 52) {
        return CONFIG_EN_SPEECH_COMMAND_ID52;
    } else if (i == 53) {
        return CONFIG_EN_SPEECH_COMMAND_ID53;
    } else if (i == 54) {
        return CONFIG_EN_SPEECH_COMMAND_ID54;
    } else if (i == 55) {
        return CONFIG_EN_SPEECH_COMMAND_ID55;
    } else if (i == 56) {
        return CONFIG_EN_SPEECH_COMMAND_ID56;
    } else if (i == 57) {
        return CONFIG_EN_SPEECH_COMMAND_ID57;
    } else if (i == 58) {
        return CONFIG_EN_SPEECH_COMMAND_ID58;
    } else if (i == 59) {
        return CONFIG_EN_SPEECH_COMMAND_ID59;
    } else if (i == 60) {
        return CONFIG_EN_SPEECH_COMMAND_ID60;
    } else if (i == 61) {
        return CONFIG_EN_SPEECH_COMMAND_ID61;
    } else if (i == 62) {
        return CONFIG_EN_SPEECH_COMMAND_ID62;
    } else if (i == 63) {
        return CONFIG_EN_SPEECH_COMMAND_ID63;
    } else if (i == 64) {
        return CONFIG_EN_SPEECH_COMMAND_ID64;
    } else if (i == 65) {
        return CONFIG_EN_SPEECH_COMMAND_ID65;
    } else if (i == 66) {
        return CONFIG_EN_SPEECH_COMMAND_ID66;
    } else if (i == 67) {
        return CONFIG_EN_SPEECH_COMMAND_ID67;
    } else if (i == 68) {
        return CONFIG_EN_SPEECH_COMMAND_ID68;
    } else if (i == 69) {
        return CONFIG_EN_SPEECH_COMMAND_ID69;
    } else if (i == 70) {
        return CONFIG_EN_SPEECH_COMMAND_ID70;
    } else if (i == 71) {
        return CONFIG_EN_SPEECH_COMMAND_ID71;
    } else if (i == 72) {
        return CONFIG_EN_SPEECH_COMMAND_ID72;
    } else if (i == 73) {
        return CONFIG_EN_SPEECH_COMMAND_ID73;
    } else if (i == 74) {
        return CONFIG_EN_SPEECH_COMMAND_ID74;
    } else if (i == 75) {
        return CONFIG_EN_SPEECH_COMMAND_ID75;
    } else if (i == 76) {
        return CONFIG_EN_SPEECH_COMMAND_ID76;
    } else if (i == 77) {
        return CONFIG_EN_SPEECH_COMMAND_ID77;
    } else if (i == 78) {
        return CONFIG_EN_SPEECH_COMMAND_ID78;
    } else if (i == 79) {
        return CONFIG_EN_SPEECH_COMMAND_ID79;
    } else if (i == 80) {
        return CONFIG_EN_SPEECH_COMMAND_ID80;
    } else if (i == 81) {
        return CONFIG_EN_SPEECH_COMMAND_ID81;
    } else if (i == 82) {
        return CONFIG_EN_SPEECH_COMMAND_ID82;
    } else if (i == 83) {
        return CONFIG_EN_SPEECH_COMMAND_ID83;
    } else if (i == 84) {
        return CONFIG_EN_SPEECH_COMMAND_ID84;
    } else if (i == 85) {
        return CONFIG_EN_SPEECH_COMMAND_ID85;
    } else if (i == 86) {
        return CONFIG_EN_SPEECH_COMMAND_ID86;
    } else if (i == 87) {
        return CONFIG_EN_SPEECH_COMMAND_ID87;
    } else if (i == 88) {
        return CONFIG_EN_SPEECH_COMMAND_ID88;
    } else if (i == 89) {
        return CONFIG_EN_SPEECH_COMMAND_ID89;
    } else if (i == 90) {
        return CONFIG_EN_SPEECH_COMMAND_ID90;
    } else if (i == 91) {
        return CONFIG_EN_SPEECH_COMMAND_ID91;
    } else if (i == 92) {
        return CONFIG_EN_SPEECH_COMMAND_ID92;
    } else if (i == 93) {
        return CONFIG_EN_SPEECH_COMMAND_ID93;
    } else if (i == 94) {
        return CONFIG_EN_SPEECH_COMMAND_ID94;
    } else if (i == 95) {
        return CONFIG_EN_SPEECH_COMMAND_ID95;
    } else if (i == 96) {
        return CONFIG_EN_SPEECH_COMMAND_ID96;
    } else if (i == 97) {
        return CONFIG_EN_SPEECH_COMMAND_ID97;
    } else if (i == 98) {
        return CONFIG_EN_SPEECH_COMMAND_ID98;
    } else if (i == 99) {
        return CONFIG_EN_SPEECH_COMMAND_ID99;
    } else if (i == 100) {
        return CONFIG_EN_SPEECH_COMMAND_ID100;
    } else if (i == 101) {
        return CONFIG_EN_SPEECH_COMMAND_ID101;
    } else if (i == 102) {
        return CONFIG_EN_SPEECH_COMMAND_ID102;
    } else if (i == 103) {
        return CONFIG_EN_SPEECH_COMMAND_ID103;
    } else if (i == 104) {
        return CONFIG_EN_SPEECH_COMMAND_ID104;
    } else if (i == 105) {
        return CONFIG_EN_SPEECH_COMMAND_ID105;
    } else if (i == 106) {
        return CONFIG_EN_SPEECH_COMMAND_ID106;
    } else if (i == 107) {
        return CONFIG_EN_SPEECH_COMMAND_ID107;
    } else if (i == 108) {
        return CONFIG_EN_SPEECH_COMMAND_ID108;
    } else if (i == 109) {
        return CONFIG_EN_SPEECH_COMMAND_ID109;
    } else if (i == 110) {
        return CONFIG_EN_SPEECH_COMMAND_ID110;
    } else if (i == 111) {
        return CONFIG_EN_SPEECH_COMMAND_ID111;
    } else if (i == 112) {
        return CONFIG_EN_SPEECH_COMMAND_ID112;
    } else if (i == 113) {
        return CONFIG_EN_SPEECH_COMMAND_ID113;
    } else if (i == 114) {
        return CONFIG_EN_SPEECH_COMMAND_ID114;
    } else if (i == 115) {
        return CONFIG_EN_SPEECH_COMMAND_ID115;
    } else if (i == 116) {
        return CONFIG_EN_SPEECH_COMMAND_ID116;
    } else if (i == 117) {
        return CONFIG_EN_SPEECH_COMMAND_ID117;
    } else if (i == 118) {
        return CONFIG_EN_SPEECH_COMMAND_ID118;
    } else if (i == 119) {
        return CONFIG_EN_SPEECH_COMMAND_ID119;
    } else if (i == 120) {
        return CONFIG_EN_SPEECH_COMMAND_ID120;
    } else if (i == 121) {
        return CONFIG_EN_SPEECH_COMMAND_ID121;
    } else if (i == 122) {
        return CONFIG_EN_SPEECH_COMMAND_ID122;
    } else if (i == 123) {
        return CONFIG_EN_SPEECH_COMMAND_ID123;
    } else if (i == 124) {
        return CONFIG_EN_SPEECH_COMMAND_ID124;
    } else if (i == 125) {
        return CONFIG_EN_SPEECH_COMMAND_ID125;
    } else if (i == 126) {
        return CONFIG_EN_SPEECH_COMMAND_ID126;
    } else if (i == 127) {
        return CONFIG_EN_SPEECH_COMMAND_ID127;
    } else if (i == 128) {
        return CONFIG_EN_SPEECH_COMMAND_ID128;
    } else if (i == 129) {
        return CONFIG_EN_SPEECH_COMMAND_ID129;
    } else if (i == 130) {
        return CONFIG_EN_SPEECH_COMMAND_ID130;
    } else if (i == 131) {
        return CONFIG_EN_SPEECH_COMMAND_ID131;
    } else if (i == 132) {
        return CONFIG_EN_SPEECH_COMMAND_ID132;
    } else if (i == 133) {
        return CONFIG_EN_SPEECH_COMMAND_ID133;
    } else if (i == 134) {
        return CONFIG_EN_SPEECH_COMMAND_ID134;
    } else if (i == 135) {
        return CONFIG_EN_SPEECH_COMMAND_ID135;
    } else if (i == 136) {
        return CONFIG_EN_SPEECH_COMMAND_ID136;
    } else if (i == 137) {
        return CONFIG_EN_SPEECH_COMMAND_ID137;
    } else if (i == 138) {
        return CONFIG_EN_SPEECH_COMMAND_ID138;
    } else if (i == 139) {
        return CONFIG_EN_SPEECH_COMMAND_ID139;
    } else if (i == 140) {
        return CONFIG_EN_SPEECH_COMMAND_ID140;
    } else if (i == 141) {
        return CONFIG_EN_SPEECH_COMMAND_ID141;
    } else if (i == 142) {
        return CONFIG_EN_SPEECH_COMMAND_ID142;
    } else if (i == 143) {
        return CONFIG_EN_SPEECH_COMMAND_ID143;
    } else if (i == 144) {
        return CONFIG_EN_SPEECH_COMMAND_ID144;
    } else if (i == 145) {
        return CONFIG_EN_SPEECH_COMMAND_ID145;
    } else if (i == 146) {
        return CONFIG_EN_SPEECH_COMMAND_ID146;
    } else if (i == 147) {
        return CONFIG_EN_SPEECH_COMMAND_ID147;
    } else if (i == 148) {
        return CONFIG_EN_SPEECH_COMMAND_ID148;
    } else if (i == 149) {
        return CONFIG_EN_SPEECH_COMMAND_ID149;
    } else if (i == 150) {
        return CONFIG_EN_SPEECH_COMMAND_ID150;
    } else if (i == 151) {
        return CONFIG_EN_SPEECH_COMMAND_ID151;
    } else if (i == 152) {
        return CONFIG_EN_SPEECH_COMMAND_ID152;
    } else if (i == 153) {
        return CONFIG_EN_SPEECH_COMMAND_ID153;
    } else if (i == 154) {
        return CONFIG_EN_SPEECH_COMMAND_ID154;
    } else if (i == 155) {
        return CONFIG_EN_SPEECH_COMMAND_ID155;
    } else if (i == 156) {
        return CONFIG_EN_SPEECH_COMMAND_ID156;
    } else if (i == 157) {
        return CONFIG_EN_SPEECH_COMMAND_ID157;
    } else if (i == 158) {
        return CONFIG_EN_SPEECH_COMMAND_ID158;
    } else if (i == 159) {
        return CONFIG_EN_SPEECH_COMMAND_ID159;
    } else if (i == 160) {
        return CONFIG_EN_SPEECH_COMMAND_ID160;
    } else if (i == 161) {
        return CONFIG_EN_SPEECH_COMMAND_ID161;
    } else if (i == 162) {
        return CONFIG_EN_SPEECH_COMMAND_ID162;
    } else if (i == 163) {
        return CONFIG_EN_SPEECH_COMMAND_ID163;
    } else if (i == 164) {
        return CONFIG_EN_SPEECH_COMMAND_ID164;
    } else if (i == 165) {
        return CONFIG_EN_SPEECH_COMMAND_ID165;
    } else if (i == 166) {
        return CONFIG_EN_SPEECH_COMMAND_ID166;
    } else if (i == 167) {
        return CONFIG_EN_SPEECH_COMMAND_ID167;
    } else if (i == 168) {
        return CONFIG_EN_SPEECH_COMMAND_ID168;
    } else if (i == 169) {
        return CONFIG_EN_SPEECH_COMMAND_ID169;
    } else if (i == 170) {
        return CONFIG_EN_SPEECH_COMMAND_ID170;
    } else if (i == 171) {
        return CONFIG_EN_SPEECH_COMMAND_ID171;
    } else if (i == 172) {
        return CONFIG_EN_SPEECH_COMMAND_ID172;
    } else if (i == 173) {
        return CONFIG_EN_SPEECH_COMMAND_ID173;
    } else if (i == 174) {
        return CONFIG_EN_SPEECH_COMMAND_ID174;
    } else if (i == 175) {
        return CONFIG_EN_SPEECH_COMMAND_ID175;
    } else if (i == 176) {
        return CONFIG_EN_SPEECH_COMMAND_ID176;
    } else if (i == 177) {
        return CONFIG_EN_SPEECH_COMMAND_ID177;
    } else if (i == 178) {
        return CONFIG_EN_SPEECH_COMMAND_ID178;
    } else if (i == 179) {
        return CONFIG_EN_SPEECH_COMMAND_ID179;
    } else if (i == 180) {
        return CONFIG_EN_SPEECH_COMMAND_ID180;
    } else if (i == 181) {
        return CONFIG_EN_SPEECH_COMMAND_ID181;
    } else if (i == 182) {
        return CONFIG_EN_SPEECH_COMMAND_ID182;
    } else if (i == 183) {
        return CONFIG_EN_SPEECH_COMMAND_ID183;
    } else if (i == 184) {
        return CONFIG_EN_SPEECH_COMMAND_ID184;
    } else if (i == 185) {
        return CONFIG_EN_SPEECH_COMMAND_ID185;
    } else if (i == 186) {
        return CONFIG_EN_SPEECH_COMMAND_ID186;
    } else if (i == 187) {
        return CONFIG_EN_SPEECH_COMMAND_ID187;
    } else if (i == 188) {
        return CONFIG_EN_SPEECH_COMMAND_ID188;
    } else if (i == 189) {
        return CONFIG_EN_SPEECH_COMMAND_ID189;
    } else if (i == 190) {
        return CONFIG_EN_SPEECH_COMMAND_ID190;
    } else if (i == 191) {
        return CONFIG_EN_SPEECH_COMMAND_ID191;
    } else if (i == 192) {
        return CONFIG_EN_SPEECH_COMMAND_ID192;
    } else if (i == 193) {
        return CONFIG_EN_SPEECH_COMMAND_ID193;
    } else if (i == 194) {
        return CONFIG_EN_SPEECH_COMMAND_ID194;
    } else if (i == 195) {
        return CONFIG_EN_SPEECH_COMMAND_ID195;
    } else if (i == 196) {
        return CONFIG_EN_SPEECH_COMMAND_ID196;
    } else if (i == 197) {
        return CONFIG_EN_SPEECH_COMMAND_ID197;
    } else if (i == 198) {
        return CONFIG_EN_SPEECH_COMMAND_ID198;
    } else if (i == 199) {
        return CONFIG_EN_SPEECH_COMMAND_ID199;
    } else {
        return NULL;
    }
#else
    return NULL;
#endif
}

esp_mn_error_t *esp_mn_commands_update_from_sdkconfig(const esp_mn_iface_t *multinet,  model_iface_data_t *model_data)
{
#if defined CONFIG_SR_MN_CN_MULTINET6_QUANT || defined CONFIG_SR_MN_EN_MULTINET6_QUANT || defined CONFIG_SR_MN_CN_MULTINET6_AC_QUANT || CONFIG_SR_MN_EN_MULTINET7_QUANT || CONFIG_SR_MN_CN_MULTINET7_QUANT || CONFIG_SR_MN_CN_MULTINET7_AC_QUANT
    return NULL;
#endif

    esp_mn_commands_alloc(multinet, model_data);
    printf("esp_mn_commands_update_from_sdkconfig\n");
    int total_phrase_num = 0;
    int language_id = 1; // 0: Chinese, 1:English
#ifdef CONFIG_IDF_TARGET_ESP32
    language_id = 1;
#else
    if (strcmp(ESP_MN_CHINESE, multinet->get_language(model_data)) == 0) {
        language_id = 1;
    } else if (strcmp(ESP_MN_ENGLISH, multinet->get_language(model_data)) == 0) {
        language_id = 2;
    } else {
        ESP_LOGE(TAG, "Invalid language");
        return NULL;
    }
#endif

    for (int i = 0; i < ESP_MN_MAX_PHRASE_NUM; i++) {
        char *command_str = NULL;
        int command_str_len = 0;

        if (language_id == 1) {
            command_str = get_id_name_cn(i);
        } else if (language_id == 2) {
            command_str = get_id_name_en(i);
        } else {
            ESP_LOGE(TAG, "Invalid language");
        }

        if (command_str == NULL) {
            continue;
        }
        command_str_len = strlen(command_str);
        if (command_str_len <= 0) {
            continue;
        }

        char *command_str_temp = calloc(command_str_len + 1, 1);
        memcpy(command_str_temp, command_str, command_str_len);

        char *token = strtok(command_str_temp, ",");
        while (token != NULL) {
            if (total_phrase_num > ESP_MN_MAX_PHRASE_NUM) {
                ESP_LOGE(TAG, "The number of speech commands phrase must less than ESP_MN_MAX_PHRASE_NUM");
                free(command_str_temp);
                goto end;
            }

            esp_mn_commands_add(i, token);
            total_phrase_num++;
            token = strtok(NULL, ",");
        }

        free(command_str_temp);
    }
end:
    esp_mn_commands_print();

    return esp_mn_commands_update();
}

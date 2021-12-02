#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "mn_process_commands.h"

char *get_id_name(int i)
{
#if defined CONFIG_USE_MULTINET
    if (i == 0)
        return MN_SPEECH_COMMAND_ID0;
    else if (i == 1)
        return MN_SPEECH_COMMAND_ID1;
    else if (i == 2)
        return MN_SPEECH_COMMAND_ID2;
    else if (i == 3)
        return MN_SPEECH_COMMAND_ID3;
    else if (i == 4)
        return MN_SPEECH_COMMAND_ID4;
    else if (i == 5)
        return MN_SPEECH_COMMAND_ID5;
    else if (i == 6)
        return MN_SPEECH_COMMAND_ID6;
    else if (i == 7)
        return MN_SPEECH_COMMAND_ID7;
    else if (i == 8)
        return MN_SPEECH_COMMAND_ID8;
    else if (i == 9)
        return MN_SPEECH_COMMAND_ID9;
    else if (i == 10)
        return MN_SPEECH_COMMAND_ID10;
    else if (i == 11)
        return MN_SPEECH_COMMAND_ID11;
    else if (i == 12)
        return MN_SPEECH_COMMAND_ID12;
    else if (i == 13)
        return MN_SPEECH_COMMAND_ID13;
    else if (i == 14)
        return MN_SPEECH_COMMAND_ID14;
    else if (i == 15)
        return MN_SPEECH_COMMAND_ID15;
    else if (i == 16)
        return MN_SPEECH_COMMAND_ID16;
    else if (i == 17)
        return MN_SPEECH_COMMAND_ID17;
    else if (i == 18)
        return MN_SPEECH_COMMAND_ID18;
    else if (i == 19)
        return MN_SPEECH_COMMAND_ID19;
    else if (i == 20)
        return MN_SPEECH_COMMAND_ID20;
    else if (i == 21)
        return MN_SPEECH_COMMAND_ID21;
    else if (i == 22)
        return MN_SPEECH_COMMAND_ID22;
    else if (i == 23)
        return MN_SPEECH_COMMAND_ID23;
    else if (i == 24)
        return MN_SPEECH_COMMAND_ID24;
    else if (i == 25)
        return MN_SPEECH_COMMAND_ID25;
    else if (i == 26)
        return MN_SPEECH_COMMAND_ID26;
    else if (i == 27)
        return MN_SPEECH_COMMAND_ID27;
    else if (i == 28)
        return MN_SPEECH_COMMAND_ID28;
    else if (i == 29)
        return MN_SPEECH_COMMAND_ID29;
    else if (i == 30)
        return MN_SPEECH_COMMAND_ID30;
    else if (i == 31)
        return MN_SPEECH_COMMAND_ID31;
    else if (i == 32)
        return MN_SPEECH_COMMAND_ID32;
    else if (i == 33)
        return MN_SPEECH_COMMAND_ID33;
    else if (i == 34)
        return MN_SPEECH_COMMAND_ID34;
    else if (i == 35)
        return MN_SPEECH_COMMAND_ID35;
    else if (i == 36)
        return MN_SPEECH_COMMAND_ID36;
    else if (i == 37)
        return MN_SPEECH_COMMAND_ID37;
    else if (i == 38)
        return MN_SPEECH_COMMAND_ID38;
    else if (i == 39)
        return MN_SPEECH_COMMAND_ID39;
    else if (i == 40)
        return MN_SPEECH_COMMAND_ID40;
    else if (i == 41)
        return MN_SPEECH_COMMAND_ID41;
    else if (i == 42)
        return MN_SPEECH_COMMAND_ID42;
    else if (i == 43)
        return MN_SPEECH_COMMAND_ID43;
    else if (i == 44)
        return MN_SPEECH_COMMAND_ID44;
    else if (i == 45)
        return MN_SPEECH_COMMAND_ID45;
    else if (i == 46)
        return MN_SPEECH_COMMAND_ID46;
    else if (i == 47)
        return MN_SPEECH_COMMAND_ID47;
    else if (i == 48)
        return MN_SPEECH_COMMAND_ID48;
    else if (i == 49)
        return MN_SPEECH_COMMAND_ID49;
    else if (i == 50)
        return MN_SPEECH_COMMAND_ID50;
    else if (i == 51)
        return MN_SPEECH_COMMAND_ID51;
    else if (i == 52)
        return MN_SPEECH_COMMAND_ID52;
    else if (i == 53)
        return MN_SPEECH_COMMAND_ID53;
    else if (i == 54)
        return MN_SPEECH_COMMAND_ID54;
    else if (i == 55)
        return MN_SPEECH_COMMAND_ID55;
    else if (i == 56)
        return MN_SPEECH_COMMAND_ID56;
    else if (i == 57)
        return MN_SPEECH_COMMAND_ID57;
    else if (i == 58)
        return MN_SPEECH_COMMAND_ID58;
    else if (i == 59)
        return MN_SPEECH_COMMAND_ID59;
    else if (i == 60)
        return MN_SPEECH_COMMAND_ID60;
    else if (i == 61)
        return MN_SPEECH_COMMAND_ID61;
    else if (i == 62)
        return MN_SPEECH_COMMAND_ID62;
    else if (i == 63)
        return MN_SPEECH_COMMAND_ID63;
    else if (i == 64)
        return MN_SPEECH_COMMAND_ID64;
    else if (i == 65)
        return MN_SPEECH_COMMAND_ID65;
    else if (i == 66)
        return MN_SPEECH_COMMAND_ID66;
    else if (i == 67)
        return MN_SPEECH_COMMAND_ID67;
    else if (i == 68)
        return MN_SPEECH_COMMAND_ID68;
    else if (i == 69)
        return MN_SPEECH_COMMAND_ID69;
    else if (i == 70)
        return MN_SPEECH_COMMAND_ID70;
    else if (i == 71)
        return MN_SPEECH_COMMAND_ID71;
    else if (i == 72)
        return MN_SPEECH_COMMAND_ID72;
    else if (i == 73)
        return MN_SPEECH_COMMAND_ID73;
    else if (i == 74)
        return MN_SPEECH_COMMAND_ID74;
    else if (i == 75)
        return MN_SPEECH_COMMAND_ID75;
    else if (i == 76)
        return MN_SPEECH_COMMAND_ID76;
    else if (i == 77)
        return MN_SPEECH_COMMAND_ID77;
    else if (i == 78)
        return MN_SPEECH_COMMAND_ID78;
    else if (i == 79)
        return MN_SPEECH_COMMAND_ID79;
    else if (i == 80)
        return MN_SPEECH_COMMAND_ID80;
    else if (i == 81)
        return MN_SPEECH_COMMAND_ID81;
    else if (i == 82)
        return MN_SPEECH_COMMAND_ID82;
    else if (i == 83)
        return MN_SPEECH_COMMAND_ID83;
    else if (i == 84)
        return MN_SPEECH_COMMAND_ID84;
    else if (i == 85)
        return MN_SPEECH_COMMAND_ID85;
    else if (i == 86)
        return MN_SPEECH_COMMAND_ID86;
    else if (i == 87)
        return MN_SPEECH_COMMAND_ID87;
    else if (i == 88)
        return MN_SPEECH_COMMAND_ID88;
    else if (i == 89)
        return MN_SPEECH_COMMAND_ID89;
    else if (i == 90)
        return MN_SPEECH_COMMAND_ID90;
    else if (i == 91)
        return MN_SPEECH_COMMAND_ID91;
    else if (i == 92)
        return MN_SPEECH_COMMAND_ID92;
    else if (i == 93)
        return MN_SPEECH_COMMAND_ID93;
    else if (i == 94)
        return MN_SPEECH_COMMAND_ID94;
    else if (i == 95)
        return MN_SPEECH_COMMAND_ID95;
    else if (i == 96)
        return MN_SPEECH_COMMAND_ID96;
    else if (i == 97)
        return MN_SPEECH_COMMAND_ID97;
    else if (i == 98)
        return MN_SPEECH_COMMAND_ID98;
    else if (i == 99)
        return MN_SPEECH_COMMAND_ID99;
    else if (i == 100)
        return MN_SPEECH_COMMAND_ID100;
    else if (i == 101)
        return MN_SPEECH_COMMAND_ID101;
    else if (i == 102)
        return MN_SPEECH_COMMAND_ID102;
    else if (i == 103)
        return MN_SPEECH_COMMAND_ID103;
    else if (i == 104)
        return MN_SPEECH_COMMAND_ID104;
    else if (i == 105)
        return MN_SPEECH_COMMAND_ID105;
    else if (i == 106)
        return MN_SPEECH_COMMAND_ID106;
    else if (i == 107)
        return MN_SPEECH_COMMAND_ID107;
    else if (i == 108)
        return MN_SPEECH_COMMAND_ID108;
    else if (i == 109)
        return MN_SPEECH_COMMAND_ID109;
    else if (i == 110)
        return MN_SPEECH_COMMAND_ID110;
    else if (i == 111)
        return MN_SPEECH_COMMAND_ID111;
    else if (i == 112)
        return MN_SPEECH_COMMAND_ID112;
    else if (i == 113)
        return MN_SPEECH_COMMAND_ID113;
    else if (i == 114)
        return MN_SPEECH_COMMAND_ID114;
    else if (i == 115)
        return MN_SPEECH_COMMAND_ID115;
    else if (i == 116)
        return MN_SPEECH_COMMAND_ID116;
    else if (i == 117)
        return MN_SPEECH_COMMAND_ID117;
    else if (i == 118)
        return MN_SPEECH_COMMAND_ID118;
    else if (i == 119)
        return MN_SPEECH_COMMAND_ID119;
    else if (i == 120)
        return MN_SPEECH_COMMAND_ID120;
    else if (i == 121)
        return MN_SPEECH_COMMAND_ID121;
    else if (i == 122)
        return MN_SPEECH_COMMAND_ID122;
    else if (i == 123)
        return MN_SPEECH_COMMAND_ID123;
    else if (i == 124)
        return MN_SPEECH_COMMAND_ID124;
    else if (i == 125)
        return MN_SPEECH_COMMAND_ID125;
    else if (i == 126)
        return MN_SPEECH_COMMAND_ID126;
    else if (i == 127)
        return MN_SPEECH_COMMAND_ID127;
    else if (i == 128)
        return MN_SPEECH_COMMAND_ID128;
    else if (i == 129)
        return MN_SPEECH_COMMAND_ID129;
    else if (i == 130)
        return MN_SPEECH_COMMAND_ID130;
    else if (i == 131)
        return MN_SPEECH_COMMAND_ID131;
    else if (i == 132)
        return MN_SPEECH_COMMAND_ID132;
    else if (i == 133)
        return MN_SPEECH_COMMAND_ID133;
    else if (i == 134)
        return MN_SPEECH_COMMAND_ID134;
    else if (i == 135)
        return MN_SPEECH_COMMAND_ID135;
    else if (i == 136)
        return MN_SPEECH_COMMAND_ID136;
    else if (i == 137)
        return MN_SPEECH_COMMAND_ID137;
    else if (i == 138)
        return MN_SPEECH_COMMAND_ID138;
    else if (i == 139)
        return MN_SPEECH_COMMAND_ID139;
    else if (i == 140)
        return MN_SPEECH_COMMAND_ID140;
    else if (i == 141)
        return MN_SPEECH_COMMAND_ID141;
    else if (i == 142)
        return MN_SPEECH_COMMAND_ID142;
    else if (i == 143)
        return MN_SPEECH_COMMAND_ID143;
    else if (i == 144)
        return MN_SPEECH_COMMAND_ID144;
    else if (i == 145)
        return MN_SPEECH_COMMAND_ID145;
    else if (i == 146)
        return MN_SPEECH_COMMAND_ID146;
    else if (i == 147)
        return MN_SPEECH_COMMAND_ID147;
    else if (i == 148)
        return MN_SPEECH_COMMAND_ID148;
    else if (i == 149)
        return MN_SPEECH_COMMAND_ID149;
    else if (i == 150)
        return MN_SPEECH_COMMAND_ID150;
    else if (i == 151)
        return MN_SPEECH_COMMAND_ID151;
    else if (i == 152)
        return MN_SPEECH_COMMAND_ID152;
    else if (i == 153)
        return MN_SPEECH_COMMAND_ID153;
    else if (i == 154)
        return MN_SPEECH_COMMAND_ID154;
    else if (i == 155)
        return MN_SPEECH_COMMAND_ID155;
    else if (i == 156)
        return MN_SPEECH_COMMAND_ID156;
    else if (i == 157)
        return MN_SPEECH_COMMAND_ID157;
    else if (i == 158)
        return MN_SPEECH_COMMAND_ID158;
    else if (i == 159)
        return MN_SPEECH_COMMAND_ID159;
    else if (i == 160)
        return MN_SPEECH_COMMAND_ID160;
    else if (i == 161)
        return MN_SPEECH_COMMAND_ID161;
    else if (i == 162)
        return MN_SPEECH_COMMAND_ID162;
    else if (i == 163)
        return MN_SPEECH_COMMAND_ID163;
    else if (i == 164)
        return MN_SPEECH_COMMAND_ID164;
    else if (i == 165)
        return MN_SPEECH_COMMAND_ID165;
    else if (i == 166)
        return MN_SPEECH_COMMAND_ID166;
    else if (i == 167)
        return MN_SPEECH_COMMAND_ID167;
    else if (i == 168)
        return MN_SPEECH_COMMAND_ID168;
    else if (i == 169)
        return MN_SPEECH_COMMAND_ID169;
    else if (i == 170)
        return MN_SPEECH_COMMAND_ID170;
    else if (i == 171)
        return MN_SPEECH_COMMAND_ID171;
    else if (i == 172)
        return MN_SPEECH_COMMAND_ID172;
    else if (i == 173)
        return MN_SPEECH_COMMAND_ID173;
    else if (i == 174)
        return MN_SPEECH_COMMAND_ID174;
    else if (i == 175)
        return MN_SPEECH_COMMAND_ID175;
    else if (i == 176)
        return MN_SPEECH_COMMAND_ID176;
    else if (i == 177)
        return MN_SPEECH_COMMAND_ID177;
    else if (i == 178)
        return MN_SPEECH_COMMAND_ID178;
    else if (i == 179)
        return MN_SPEECH_COMMAND_ID179;
    else if (i == 180)
        return MN_SPEECH_COMMAND_ID180;
    else if (i == 181)
        return MN_SPEECH_COMMAND_ID181;
    else if (i == 182)
        return MN_SPEECH_COMMAND_ID182;
    else if (i == 183)
        return MN_SPEECH_COMMAND_ID183;
    else if (i == 184)
        return MN_SPEECH_COMMAND_ID184;
    else if (i == 185)
        return MN_SPEECH_COMMAND_ID185;
    else if (i == 186)
        return MN_SPEECH_COMMAND_ID186;
    else if (i == 187)
        return MN_SPEECH_COMMAND_ID187;
    else if (i == 188)
        return MN_SPEECH_COMMAND_ID188;
    else if (i == 189)
        return MN_SPEECH_COMMAND_ID189;
    else if (i == 190)
        return MN_SPEECH_COMMAND_ID190;
    else if (i == 191)
        return MN_SPEECH_COMMAND_ID191;
    else if (i == 192)
        return MN_SPEECH_COMMAND_ID192;
    else if (i == 193)
        return MN_SPEECH_COMMAND_ID193;
    else if (i == 194)
        return MN_SPEECH_COMMAND_ID194;
    else if (i == 195)
        return MN_SPEECH_COMMAND_ID195;
    else if (i == 196)
        return MN_SPEECH_COMMAND_ID196;
    else if (i == 197)
        return MN_SPEECH_COMMAND_ID197;
    else if (i == 198)
        return MN_SPEECH_COMMAND_ID198;
    else if (i == 199)
        return MN_SPEECH_COMMAND_ID199;
    else
        return NULL;
#else
    return NULL;
#endif
}

void reset_speech_commands_v1(model_iface_data_t *model_data, char* command_str, char *err_phrase_id);
void reset_speech_commands_v2(model_iface_data_t *model_data, char* command_str, char *err_phrase_id);

void reset_speech_commands(model_iface_data_t *model_data, char* command_str, char *err_phrase_id)
{
#if CONFIG_SINGLE_RECOGNITION
    reset_speech_commands_v1(model_data, command_str, err_phrase_id);
#elif CONFIG_SR_CHINESE && CONFIG_CONTINUOUS_RECOGNITION
    reset_speech_commands_v2(model_data, command_str, err_phrase_id);
#endif
}
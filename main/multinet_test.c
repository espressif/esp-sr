#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "multinet_test.h"
#include "dakaidiandeng.h"

static const esp_mn_iface_t *multinet = &MULTINET_MODEL;

void multinetTask(void *arg)
{
    model_iface_data_t *model_data = arg;
    int frequency = multinet->get_samp_rate(model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data);
    int16_t *buffer = malloc(audio_chunksize * sizeof(int16_t));
    assert(buffer);
    int chunks = 0;
    while (1) {
        if ((chunks+1)*audio_chunksize*sizeof(int16_t) <= sizeof(dakaidiandeng)) {
            memcpy(buffer, dakaidiandeng+chunks*audio_chunksize*sizeof(int16_t), audio_chunksize * sizeof(int16_t));   
        } else {
            memset(buffer, 0, audio_chunksize*sizeof(int16_t));
        }
        int commend_id = multinet->detect(model_data, buffer);
        chunks++;
        if (chunks == 200 || commend_id > -1) {
            if (commend_id > -1) {
                printf("MN test successfully, Commands ID: %d.\n", commend_id);
            } else {
                printf("can not recognize any speech commands\n");
            }
            break;
        }
    }
    printf("TEST2 FINISHED\n\n");
    vTaskDelete(NULL);
}

void add_speech_commands(esp_mn_iface_t *multinet, model_iface_data_t *model_data)
{
    char *phrase_spelling[20];
    phrase_spelling[0] = "d,a,k,ai,k,ong,ti,ao";
    phrase_spelling[1] = "gu,an,b,i,k,ong,ti,ao";
    phrase_spelling[2] = "z,eng,d,a,f,eng,s,u";
    phrase_spelling[3] = "ji,an,xi,ao,f,eng,s,u";
    phrase_spelling[4] = "sh,eng,g,ao,y,i,d,u";
    phrase_spelling[5] = "ji,ang,d,i,y,i,d,u";
    phrase_spelling[6] = "zh,i,r,e,m,o,sh,i";
    phrase_spelling[7] = "zh,i,l,eng,m,o,sh,i";
    phrase_spelling[8] = "s,ong,f,eng,m,o,sh,i";
    phrase_spelling[9] = "j,ie,n,eng,m,o,sh,i";

    phrase_spelling[10] = "gu,an,b,i,j,ie,n,eng,m,o,sh,i";
    phrase_spelling[11] = "ch,u,sh,i,m,o,sh,i";
    phrase_spelling[12] = "gu,an,b,i,ch,u,sh,i";
    phrase_spelling[13] = "d,a,k,ai,l,an,y,a";
    phrase_spelling[14] = "gu,an,b,i,l,an,y,a";
    phrase_spelling[15] = "b,o,f,ang,g,e,q,u";
    phrase_spelling[16] = "z,an,t,ing,b,o,f,ang";
    phrase_spelling[17] = "d,ing,sh,i,y,i,xi,ao,sh,i";
    phrase_spelling[18] = "d,a,k,ai,di,an,d,eng";
    phrase_spelling[19] = "gu,an,b,i,di,an,d,eng";

    printf("start to add commands:\n");
    for (int i = 0; i < 20; i++) {
        printf("commend %d: %s\n",i+1, phrase_spelling[i]);
        multinet->add_speech_commands(model_data, i, phrase_spelling[i], NULL);
    }
}

void multinet_test()
{
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("Start free RAM size: %d\n", start_size);

    //Initialize multinet model
    model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF);
    add_speech_commands(multinet, model_data);

    //define_speech_commands(multinet, model_data);
    int audio_chunksize = multinet->get_samp_chunksize(model_data);
    printf("multinet RAM size: %d\n, current RAM size after multinet init: %d\n",
           start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT));

    xTaskCreatePinnedToCore(&multinetTask, "multinet", 2 * 1024, (void*)model_data, 5, NULL, 0);
}

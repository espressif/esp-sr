#!/bin/bash

function convert_md2rst(){
    for files in $1/$2/*
    do
        filename="$(basename -- $files)"
        echo $filename
        fname="${filename%.*}"
        echo $fname
        echo "converting $fname"
        pandoc $1/$2/$filename -f markdown -t rst -s -o "$1/$2/${fname}".rst
    done
}

convert_md2rst en acoustic_algorithm
convert_md2rst en audio_front_end
convert_md2rst en flash_model
convert_md2rst en performance_test
convert_md2rst en speech_command_recognition
convert_md2rst en wake_word_engine

convert_md2rst zh_cn acoustic_algorithm
convert_md2rst zh_cn audio_front_end
convert_md2rst zh_cn flash_model
convert_md2rst zh_cn performance_test
convert_md2rst zh_cn speech_command_recognition
convert_md2rst zh_cn wake_word_engine
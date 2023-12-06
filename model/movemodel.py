import io
import os
import argparse
import shutil
import math
import sys
sys.dont_write_bytecode = True
from pack_model import pack_models

def calculate_total_size(folder_path):
    total_size = 0
    for file_name in os.listdir(folder_path):
        path = os.path.join(folder_path, file_name)
        if os.path.isdir(path):
            total_size = total_size + calculate_total_size(path)
        if os.path.isfile(path):
            total_size = total_size + os.path.getsize(path)
    return total_size

def copy_wakenet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy wakenet model from model_path to target_path based on sdkconfig
    """
    with io.open(sdkconfig_path, "r") as f:
        models_string = ''
        for label in f:
            label = label.strip("\n")
            if 'CONFIG_SR_WN' in label and label[0] != '#':
                models_string += label

    models = []
    if "CONFIG_SR_WN_WN7Q8_XIAOAITONGXUE" in models_string:
        models.append('wn7q8_xiaoaitongxue')
    if "CONFIG_SR_WN_WN7_XIAOAITONGXUE" in models_string:
        models.append('wn7_xiaoaitongxue')
    if "CONFIG_SR_WN_WN8_HILEXIN" in models_string:
        models.append('wn8_hilexin')
    if "CONFIG_SR_WN_WN8_ALEXA" in models_string:
        models.append('wn8_alexa')
    if "CONFIG_SR_WN_WN8_HIESP" in models_string:
        models.append('wn8_hiesp')
    if "CONFIG_SR_WN_WN9_XIAOAITONGXUE" in models_string:
        models.append('wn9_xiaoaitongxue')
    if "CONFIG_SR_WN_WN9_HILEXIN" in models_string:
        models.append('wn9_hilexin')
    if "CONFIG_SR_WN_WN9_ALEXA" in models_string:
        models.append('wn9_alexa')
    if "CONFIG_SR_WN_WN9_HIESP" in models_string:
        models.append('wn9_hiesp')
    if "CONFIG_SR_WN_WN9_HIMFIVE" in models_string:
        models.append('wn9_himfive')
    if "CONFIG_SR_WN_WN9_NIHAOXIAOZHI" in models_string:
        models.append('wn9_nihaoxiaozhi')
    if "CONFIG_SR_WN_WN9_JARVIS_TTS" in models_string:
        models.append('wn9_jarvis_tts')
    if "CONFIG_SR_WN_WN9_COMPUTER_TTS" in models_string:
        models.append('wn9_computer_tts')
    if "CONFIG_SR_WN_WN9_CUSTOMWORD" in models_string:
        models.append('wn9_customword')

    for item in models:
        shutil.copytree(model_path + '/wakenet_model/' + item, target_path+'/'+item)

def copy_multinet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy multinet model from model_path to target_path based on sdkconfig
    """
    with io.open(sdkconfig_path, "r") as f:
        models_string = ''
        for label in f:
            label = label.strip("\n")
            if 'CONFIG_SR_MN' in label and label[0] != '#':
                models_string += label

    models = []
    if "CONFIG_SR_MN_CN_MULTINET3_SINGLE_RECOGNITION" in models_string:
        models.append('mn3_cn')
    elif "CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION_QUANT8" in models_string:
        models.append('mn4q8_cn')
    elif "CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION" in models_string:
        models.append('mn4_cn')
    elif "CONFIG_SR_MN_CN_MULTINET5_RECOGNITION_QUANT8" in models_string:
        models.append('mn5q8_cn')
    elif "CONFIG_SR_MN_CN_MULTINET6_QUANT" in models_string:
        models.append('mn6_cn')
    elif "CONFIG_SR_MN_CN_MULTINET6_AC_QUANT" in models_string:
        models.append('mn6_cn_ac')
    elif "CONFIG_SR_MN_CN_MULTINET7_QUANT" in models_string:
        models.append('mn7_cn')
    elif "CONFIG_SR_MN_CN_MULTINET7_AC_QUANT" in models_string:
        models.append('mn7_cn_ac')

    if "CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION_QUANT8" in models_string:
        models.append('mn5q8_en')
    elif "CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION" in models_string:
        models.append('mn5_en')
    elif "CONFIG_SR_MN_EN_MULTINET6_QUANT" in models_string:
        models.append('mn6_en')
    elif "CONFIG_SR_MN_EN_MULTINET7_QUANT" in models_string:
        models.append('mn7_en')

    if "MULTINET6" in models_string or "MULTINET7" in models_string:
        models.append('fst')

    for item in models:
        shutil.copytree(model_path + '/multinet_model/' + item, target_path+'/'+item)


def copy_nsnet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy nsnet model from model_path to target_path based on sdkconfig
    """
    with io.open(sdkconfig_path, "r") as f:
        models_string = ''
        for label in f:
            label = label.strip("\n")
            if 'CONFIG_SR_NSN' in label and label[0] != '#':
                models_string += label

    models = []
    if "CONFIG_SR_NSN_NSNET1" in models_string:
        models.append('nsnet1')

    for item in models:
        shutil.copytree(model_path + '/nsnet_model/' + item, target_path+'/'+item)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Model generator tool')
    parser.add_argument('-d1', '--sdkconfig_path')
    parser.add_argument('-d2', '--model_path')
    parser.add_argument('-d3', '--build_path')
    args = parser.parse_args()

    sdkconfig_path = args.sdkconfig_path
    model_path = args.model_path + '/model'
    target_path = args.build_path + '/srmodels'
    image_file = "srmodels.bin"

    if os.path.exists(target_path):
        shutil.rmtree(target_path)
    os.makedirs(target_path)

    copy_multinet_from_sdkconfig(model_path, sdkconfig_path, target_path)
    copy_wakenet_from_sdkconfig(model_path, sdkconfig_path, target_path)
    copy_nsnet_from_sdkconfig(model_path, sdkconfig_path, target_path)
    pack_models(target_path, image_file)
    total_size = os.path.getsize(os.path.join(target_path, image_file))
    recommended_size = int(math.ceil(total_size/1024))
    print("Recommended model partition size: %dK" % (recommended_size))

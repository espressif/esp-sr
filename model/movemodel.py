import argparse
import math
import os
import shutil
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

def find_dirs_with_model_info(root_path):
    result_dirs = []

    for dirpath, dirnames, filenames in os.walk(root_path):
        if '_MODEL_INFO_' in filenames:
            file_path = os.path.join(dirpath, '_MODEL_INFO_')
            if os.path.getsize(file_path) > 0:
                result_dirs.append(dirpath)

    return result_dirs

def copy_models_from_sdkconfig_path(sdkconfig_path, target_path):
    """
    Copy wakenet model from model_path to target_path based on sdkconfig
    """
    models = []
    extra_model_path = None
    with open(sdkconfig_path, "r") as f:
        for label in f:
            label = label.strip("\n")
            if 'ESP_SR_EXTERNAL_MODEL_PATH' in label:
                extra_model_path = label.split("=")[1].strip('"')

    if extra_model_path:
        if os.path.exists(extra_model_path):
            model_dirs = find_dirs_with_model_info(extra_model_path)
            for model in model_dirs:
                model_name = os.path.basename(model)
                models.append(model_name)
                shutil.copytree(model, target_path+'/'+model_name, dirs_exist_ok=True)

    return extra_model_path, models


def copy_wn10_model(model_path, target_path, target):
    pie_versions = {
        "esp32s3": "p1",
        "esp32p4": "p2",
        "esp32s31": "p2",
    }
    if target not in pie_versions:
        raise ValueError(f"Invalid target for WN10 model: {target}")

    pie_version = pie_versions[target]
    selected_files = {
        f"wn10_data_{pie_version}": "wn10_data",
        f"_MODEL_INFO_{pie_version}": "_MODEL_INFO_",
    }

    os.makedirs(target_path)
    for file_name in os.listdir(model_path):
        source = os.path.join(model_path, file_name)
        if file_name in selected_files:
            destination = os.path.join(target_path, selected_files[file_name])
        elif file_name.endswith("_p1") or file_name.endswith("_p2"):
            continue
        else:
            destination = os.path.join(target_path, file_name)

        if os.path.isdir(source):
            shutil.copytree(source, destination)
        else:
            shutil.copy2(source, destination)


def copy_wakenet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy wakenet model from model_path to target_path based on sdkconfig
    """
    models = []
    target = None
    with open(sdkconfig_path, "r") as f:
        for label in f:
            label = label.strip("\n")
            if label.startswith("CONFIG_IDF_TARGET="):
                target = label.split("=", 1)[1].strip('"')
            if 'CONFIG_SR_WN' in label and  '#' not in label[0]:
                if '_NONE' in label:
                    continue
                is_wn10 = '_WN10' in label
                if '=' in label:
                    label = label.split("=")[0]
                if '_MULTI' in label:
                    label = label[:-6]
                model_name = label.split("_SR_WN_")[-1].lower()
                models.append((model_name, is_wn10))

    for item, is_wn10 in models:
        wakeword_model_path =  model_path + '/wakenet_model/' + item
        if os.path.exists(wakeword_model_path):
            destination = target_path + '/' + item
            if is_wn10:
                copy_wn10_model(wakeword_model_path, destination, target)
            else:
                shutil.copytree(wakeword_model_path, destination)

def copy_multinet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy multinet model from model_path to target_path based on sdkconfig
    """
    with open(sdkconfig_path, "r") as f:
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
    with open(sdkconfig_path, "r") as f:
        models_string = ''
        for label in f:
            label = label.strip("\n")
            if 'CONFIG_SR_NSN' in label and label[0] != '#':
                models_string += label

    models = []
    if "CONFIG_SR_NSN_NSNET1" in models_string:
        models.append('nsnet1')
    if "CONFIG_SR_NSN_NSNET2" in models_string:
        models.append('nsnet2')
    if "CONFIG_SR_NSN_NSNET3" in models_string:
        models.append('nsnet3')

    for item in models:
        shutil.copytree(model_path + '/nsnet_model/' + item, target_path+'/'+item)

def copy_vadnet_from_sdkconfig(model_path, sdkconfig_path, target_path):
    """
    Copy vadnet model from model_path to target_path based on sdkconfig
    """
    with open(sdkconfig_path, "r") as f:
        models_string = ''
        for label in f:
            label = label.strip("\n")
            if 'CONFIG_SR_VADN' in label and label[0] != '#':
                models_string += label

    models = []
    if "CONFIG_SR_VADN_VADNET1_SMALL" in models_string:
        models.append('vadnet1_small')
    elif "CONFIG_SR_VADN_VADNET1_MEDIUM" in models_string:
        models.append('vadnet1_medium')
    
    for item in models:
        shutil.copytree(model_path + '/vadnet_model/' + item, target_path+'/'+item)

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
    copy_vadnet_from_sdkconfig(model_path, sdkconfig_path, target_path)
    extra_model_path, extra_models = copy_models_from_sdkconfig_path(sdkconfig_path, target_path)

    loaded_models = []
    if os.path.exists(target_path):
        loaded_models = sorted([d for d in os.listdir(target_path) if os.path.isdir(os.path.join(target_path, d))])

    pack_models(target_path, image_file)

    total_size = 0
    image_path = os.path.join(target_path, image_file)
    if os.path.exists(image_path):
        total_size = os.path.getsize(image_path)

    recommended_size = int(math.ceil(total_size / 1024.0)) + 1

    # ESP-SR Model Report
    print()
    print('ESP-SR Models Report')
    print('─' * 40)
    if extra_model_path:
        if os.path.exists(extra_model_path):
            print(f'  Models loaded from external path: {extra_model_path}')
            for model in extra_models:
                model_size = calculate_total_size(os.path.join(target_path, model))
                print(f'    - {model:<20} ({model_size / 1024.0:.2f} KB)')
        else:
            print(f'  External model path specified but does not exist: {extra_model_path}')
        print()

    if loaded_models:
        print('  Models loaded from esp-sr:')
        for model in loaded_models:
            if model in extra_models:
                continue
            model_size = calculate_total_size(os.path.join(target_path, model))
            print(f'    - {model:<20} ({model_size / 1024.0:.2f} KB)')
        print()
        print(f'  Recommended Partition Size: {recommended_size}K')
    else:
        print('  No speech models loaded.')

    print('─' * 40)
    print()

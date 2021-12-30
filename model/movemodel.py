import io
import os
import argparse
import shutil
def calculate_total_size(folder_path):
    total_size = 0
    for file_name in os.listdir(folder_path):
        path = os.path.join(folder_path, file_name)
        if os.path.isdir(path):
            total_size = total_size + calculate_total_size(path)
        if os.path.isfile(path):
            total_size = total_size + os.path.getsize(path)
    return total_size

if __name__ == '__main__':
    # input parameter
    parser = argparse.ArgumentParser(description='Model generator tool')
    parser.add_argument('-d1', '--project_path')
    parser.add_argument('-d2', '--model_path')
    args = parser.parse_args()

    sdkconfig_path = args.project_path + '/sdkconfig'
    model_path = args.model_path + '/model'

    print(sdkconfig_path)
    print(model_path)

with io.open(sdkconfig_path, "r") as f:
    WN_STRING = ''
    MN_STRING = ''
    for label in f:
        label = label.strip("\n")
        if 'CONFIG_SR_WN' in label and label[0] != '#':
            WN_STRING += label
        if 'CONFIG_SR_MN' in label and label[0] != '#':
            MN_STRING += label

wakenet_model = 'null'
if "CONFIG_SR_WN_WN7_XIAOAITONGXUE" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT8" in WN_STRING:
    wakenet_model = 'xiaoaitongxue7q8'
elif "CONFIG_SR_WN_WN7_XIAOAITONGXUE" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT" in WN_STRING:
    wakenet_model = 'xiaoaitongxue7'
elif "CONFIG_SR_WN_WN7_HILEXIN" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT8" in WN_STRING:
    wakenet_model = 'hilexin7q8'
elif "CONFIG_SR_WN_WN7_HILEXIN" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT" in WN_STRING:
    wakenet_model = 'hilexin7'
elif "CONFIG_SR_WN_WN7_ALEXA" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT8" in WN_STRING:
    wakenet_model = 'alexa7q8'
elif "CONFIG_SR_WN_WN7_ALEXA" in WN_STRING and "CONFIG_SR_WN_MODEL_WN7_QUANT" in WN_STRING:
    wakenet_model = 'alexa7'
elif "CONFIG_SR_WN_WN8_ALEXA" in WN_STRING and "CONFIG_SR_WN_MODEL_WN8_QUANT" in WN_STRING:
    wakenet_model = 'alexa8'
elif "CONFIG_SR_WN_WN8_HIESP" in WN_STRING and "CONFIG_SR_WN_MODEL_WN8_QUANT8" in WN_STRING:
    wakenet_model = 'hiesp8q8'
elif "CONFIG_SR_WN_WN8_HIESP" in WN_STRING and "CONFIG_SR_WN_MODEL_WN8_QUANT" in WN_STRING:
    wakenet_model = 'hiesp8'
else:
    print('choose no wakenet mode')

multinet_model = 'null'
if "CONFIG_SR_MN_CN_MULTINET3_SINGLE_RECOGNITION" in MN_STRING or "CONFIG_SR_MN_CN_MULTINET3_CONTINUOUS_RECOGNITION" in MN_STRING:
    multinet_model = 'mn3cn'
elif "CONFIG_SR_MN_CN_MULTINET4_SINGLE_RECOGNITION" in MN_STRING:
    multinet_model = 'mn4cn'
elif "CONFIG_SR_MN_CN_MULTINET4_5_SINGLE_RECOGNITION" in MN_STRING:
    multinet_model = 'mn4_5cn'
elif "CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION_QUANT8" in MN_STRING:
    multinet_model = 'mn5q8en'
elif "CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION" in MN_STRING:
    multinet_model = 'mn5en'
else:
    print('choose no multinet mode')

print(wakenet_model)
print(multinet_model)

target_model = model_path + '/target'

if os.path.exists(target_model):
    shutil.rmtree(target_model)
os.makedirs(target_model)
if wakenet_model != 'null':
    shutil.copytree(model_path + '/wakenet_model/' + wakenet_model, target_model+'/'+wakenet_model)
if multinet_model != 'null':
    shutil.copytree(model_path + '/multinet_model/' + multinet_model, target_model+'/'+multinet_model)

# os.system("cp %s %s" % (wakenet_model+'/_MODEL_INFO_', target_model))

total_size = calculate_total_size(target_model)
print("Recommended model partition size: ", str(int((total_size / 1024 + 500) / 4 ) * 4) + 'KB')
# Performance Test

## 1. AFE

### 1.1 Resource Occupancy(ESP32)

|algorithm Type|RAM|Average cpu loading(compute with 2 cores)| Frame Length|
|:---:|:---:|:---:|:---:|
|AEC(HIGH_PERF)|114 KB|11%|32 ms|
|NS|27 KB|5%|10 ms|
|AFE Layer|73 KB| | |

### 1.2 Resource Occupancy(ESP32S3)

|algorithm Type|RAM|Average cpu loading(compute with 2 cores)| Frame Length|
|:---:|:---:|:---:|:---:|
|AEC(LOW_COST)|152.3 KB|8%|32 ms|
|AEC(HIGH_PERF)|166 KB|11%|32 ms|
|BSS(LOW_COST)|198.7 KB|6%|64 ms|
|BSS(HIGH_PERF)|215.5 KB|7%|64 ms|
|NS|27 KB|5%|10 ms|
|MISO|56 KB|8%|16 ms|
|AFE Layer|227 KB| | |

## 2. WakeNet

### 2.1 Resource Occupancy(ESP32)

|Model Type|Parameter Num|RAM|Average Running Time per Frame| Frame Length|
|:---:|:---:|:---:|:---:|:---:|
|Quantised WakeNet5|41 K|15 KB|5.5 ms|30 ms|
|Quantised WakeNet5X2|165 K|20 KB|10.5 ms|30 ms|
|Quantised WakeNet5X3|371 K|24 KB|18 ms|30 ms|

### 2.2 Resource Occupancy(ESP32S3)

|Model Type|RAM|PSRAM|Average Running Time per Frame| Frame Length|
|:---:|:---:|:---:|:---:|:---:|
|Quantised WakeNet8 @ 2 channel|50 KB|1640 KB|10 ms|32 ms|
|Quantised WakeNet9 @ 2 channel|16 KB|320 KB|4 ms|32 ms|

### 2.3 Performance

|Distance| Quiet | Stationary Noise (SNR = 4 dB)| Speech Noise (SNR = 4 dB)| AEC Interruption (-10 dB)|
|:---:|:---:|:---:|:---:|:---:|
|1 m|98%|96%|94%|96%|
|3 m|98%|94%|92%|94%|

False triggering rate: 1 time in 12 hours

**Note**: We use the ESP32-S3-Korvo V4.0 development board and the WakeNet8(Alexa) model in our test. 

## 3. MultiNet

### 2.1 Resource Occupancy(ESP32)

|Model Type|Internal RAM|PSRAM|Average Running Time per Frame| Frame Length|
|:---:|:---:|:---:|:---:|:---:|
|MultiNet 2|13.3 KB|9KB|38 ms|30 ms|

### 2.2 Resource Occupancy(ESP32S3)

|Model Type|Internal RAM|PSRAM|Average Running Time per Frame| Frame Length|
|:---:|:---:|:---:|:---:|:---:|
|MultiNet 4|16.8KB|1866 KB|18 ms|32 ms|
|MultiNet 4 Q8|10.5 KB|1009 KB|11 ms|32 ms|
|MultiNet 5 Q8|14 KB||12 ms|32 ms|

### 2.3 Performance with AFE

|Model Type|Distance| Quiet | Stationary Noise (SNR = 4 dB)| Speech Noise (SNR = 4 dB)|
|:---:|:---:|:---:|:---:|:---:|:---:|
|MultiNet 4|3 m|98%|93%|92%|
|MultiNet 4 Q8|3 m|94%|92%|91%|

---
name: add-wake-word
description: >-
  Add a bundled WakeNet wake word to ESP-SR. Place the model under
  model/wakenet_model, add a Kconfig.projbuild entry, and regenerate
  wakeword_list.md. Use when the user asks to add, import, move, or register
  a wake word, WakeNet model, 唤醒词, or wakenet model.
---

# 添加唤醒词

把已经训练好的 WakeNet 模型登记进 ESP-SR。源目录里的权重文件保持原名，只改外层文件夹名，并补上 menuconfig 条目和唤醒词列表。

## 步骤

1. 把模型目录移到 `model/wakenet_model/<model_name>/`。
2. 在 `Kconfig.projbuild` 对应菜单里增加 `bool` 条目。
3. 若 `_MODEL_INFO_` 里的词和展示名不同，把映射加进 `tool/generate_wakeword_list.py` 的 `WAKE_WORD_ALIASES`。
4. 运行 `python3 tool/generate_wakeword_list.py`。不要手改 `wakeword_list.md`。

同一模型目录里每个 `_MODEL_INFO*` 解析出的唤醒词必须相同，否则列表脚本会失败。

## 文件夹名

`<model_name>` 全小写：`wn<版本><变体>_<词>`。

| 部分 | 取值 | 例子 |
|------|------|------|
| 版本 | `9`、`10` | `wn10_himuse` |
| 变体 | 空、`s`、`l` | `wn9s_hilexin`、`wn9l_xiaoaitongxue` |
| 词 | 小写标识，不含空格 | `shuang1er2`、`heypamet` |

中文词的文件夹名用带声调数字的拼音，例如 `tu2tu2ni3hao3`、`shuang1er2`。int16 量化模型在末尾加 `_int16`。

## 目录内容

不要重命名目录内部文件。构建时 `model/movemodel.py` 会按芯片挑选 WakeNet10 文件并改名。

WakeNet10 / WakeNet10s 需要成对的数据和说明文件：

| 文件 | 芯片 |
|------|------|
| `wn10_data_p1`、`_MODEL_INFO_p1` | ESP32-S3 |
| `wn10_data_p2`、`_MODEL_INFO_p2` | ESP32-P4、ESP32-S31 |
| `wn10_data_p0`、`_MODEL_INFO_p0` | 其余目标，含 ESP32-C5 |

只交付了 p1/p2 时，不要补造 p0。缺少某个后缀时，对应芯片的打包会失败。

WakeNet9 / WakeNet9l / WakeNet9s 使用单个 `_MODEL_INFO_`，连同原有数据文件原样放入。

`_MODEL_INFO_` 中唤醒词是倒数第三段，不能含 `_`。空格和 `/` 可以保留，例如 `Hi/Hey Muse`。同一目录的 p0/p1/p2 必须写成同一个词。

## Kconfig

符号名是 `SR_WN_` 加上文件夹名的大写。`model/movemodel.py` 用 `CONFIG_SR_WN_` 后面的部分转成小写，得到文件夹名。名字里带 `WN10` 的条目走 WakeNet10 的 p0/p1/p2 选择，含 `wn10s`。

菜单：

- `wn9s_*`、`wn10s_*`：`Load Multiple Wake Words (WakeNet9s)`
- `wn9_*`、`wn9l_*`、`wn10_*`：`Load Multiple Wake Words (WakeNet9 or WakeNet10)`

提示文字用展示名，括号里写文件夹名，`default n`。

```
config SR_WN_WN10_SHUANG1ER2
bool "双儿 (wn10_shuang1er2)"
default n

config SR_WN_WN10_HIMUSE
bool "Hi/Hey Muse (wn10_himuse)"
default n
```

## 唤醒词列表

`wakeword_list.md` 由 `tool/generate_wakeword_list.py` 扫描 `model/wakenet_model` 生成。展示名优先取 `WAKE_WORD_ALIASES`，键不区分大小写。元数据里已经是展示名时不必再加别名。

拼音或内部标识需要别名时，跟现有条目一样追加，例如 `"shuang1er2": "双儿"`、`"himuse": "Hi/Hey Muse"`、`"hai1le4xin1": "Hi,乐鑫"`。然后重新生成列表。

`README.md` 里的表只收录已测试的一小部分模型。用户没有要求时，不要把新词加进那张表。

# -*- coding:utf-8 -*-
import psutil

# 変数のメモリサイズを取得する関数
def print_varsize():
    import types
    print("{}{: >15}{}{: >10}{}".format('|','Variable Name','|','  Size','|'))
    print(" -------------------------- ")
    for k, v in globals().items():
        if hasattr(v, 'size') and not k.startswith('_') and not isinstance(v,types.ModuleType):
            print("{}{: >15}{}{: >10}{}".format('|',k,'|',str(v.size),'|'))
        elif hasattr(v, '__len__') and not k.startswith('_') and not isinstance(v,types.ModuleType):
            print("{}{: >15}{}{: >10}{}".format('|',k,'|',str(len(v)),'|'))

# メモリとCPUの利用情報を取得
memory = psutil.virtual_memory()
cpu_percent = psutil.cpu_percent(interval=1)

print('メモリ使用率：', memory.percent) # メモリ使用率： 60.0
print('CPU使用率：', cpu_percent) # CPU使用率 2.4

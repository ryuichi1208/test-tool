# -*- coding:utf-8 -*-
import psutil

# メモリとCPUの利用情報を取得
memory = psutil.virtual_memory()
cpu_percent = psutil.cpu_percent(interval=1)

print('メモリ使用率：', memory.percent) # メモリ使用率： 60.0
print('CPU使用率：', cpu_percent) # CPU使用率 2.4

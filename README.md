# uCShell_Demo
这个用来测试和学习FreeRTOS＆uCShell组件方演示。
uC-Shell 依赖的文件有uC-Lib和uC-CPU.
将此文件加入到项目中.
如果是用uC-OS基本不用更新定义.
如果是用freeRTOS则要更新uC-Lib里面的定义的OS临界操作．taskEXIT_CRITICAL()和taskENTER_CRITICAL()替换掉CPU_CRITICAL_ENTER()和CPU_CRITICAL_EXIT()

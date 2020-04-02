@echo off
echo 清除VS工程临时文件，减小工程大小
for /r %%i in (
*.aps *.idb *.ncb *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk
*.sdf *.dep *.ipch *.tlog *.exp *.hpj *.mdp *.plg *.clw *.user
*.log *Log.htm gis_log*.txt HZSet*.lib *.manifest *ReportDlg.resources
*CSCC.res
) do echo 删除"%%i" && del /f /q "%%i"
::  *.opt 保存部分项目配置
echo 清理完成！
pause>nul

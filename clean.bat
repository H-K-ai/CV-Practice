@echo off
echo ���VS������ʱ�ļ�����С���̴�С
for /r %%i in (
*.aps *.idb *.ncb *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk
*.sdf *.dep *.ipch *.tlog *.exp *.hpj *.mdp *.plg *.clw *.user
*.log *Log.htm gis_log*.txt HZSet*.lib *.manifest *ReportDlg.resources
*CSCC.res
) do echo ɾ��"%%i" && del /f /q "%%i"
::  *.opt ���沿����Ŀ����
echo ������ɣ�
pause>nul

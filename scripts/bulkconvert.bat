if "%1" == "" (
echo usage: %0 ^<version^>
exit /b
)

for %%i in (*.veg) do (
	python3 msvpvf.py -i "%%i" --version %1 --type veg
)

pause

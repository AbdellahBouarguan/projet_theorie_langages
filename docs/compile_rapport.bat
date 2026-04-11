@echo off
chcp 65001 > NUL
REM Se positionner dans le repertoire du script
cd /d "%~dp0"
cd latexRapport

echo Compilation du rapport en cours...

set COMPILER=pdflatex
where pdflatex > NUL 2>&1
if %errorlevel% neq 0 (
    if exist "%APPDATA%\TinyTeX\bin\windows\pdflatex.exe" (
        set COMPILER="%APPDATA%\TinyTeX\bin\windows\pdflatex.exe"
    ) else (
        echo Erreur : Le compilateur LaTeX pdflatex est introuvable.
        pause
        exit /b 1
    )
)

%COMPILER% -interaction=nonstopmode rapport.tex > NUL
%COMPILER% -interaction=nonstopmode rapport.tex > NUL

if exist rapport.pdf (
    move /Y rapport.pdf ..\rapport.pdf > NUL
    echo Succes : Le rapport PDF a ete genere dans le dossier docs\ 
) else (
    echo Erreur : La compilation a echoue. Aucun PDF n a ete genere.
)

pause

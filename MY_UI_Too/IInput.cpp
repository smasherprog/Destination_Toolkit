#include "PCH.h"
#include "IInput.h"

bool CapsLock = false;
bool ShiftDown =false;
bool CtrlDown =false;
bool NumLock = false;
bool Mouse_RButton_Down= false;
bool Mouse_LButton_Down = false;
bool Mouse_MButton_Down= false;
bool Key_Down = false;

int Old_MousePosx(0), Old_MousePosy(0), New_MousePosx(0), New_MousePosy(0), Delta_Mousex(0), Delta_Mousey(0);
int Mouse_Wheel_Delta(0);
char Current_Key=0;

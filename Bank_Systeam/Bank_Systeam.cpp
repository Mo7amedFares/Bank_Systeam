#include "clsLoginScreen.h"

int main()
{
	bool flag = true;
	do
	{
		flag = clsLoginScreen::ShowLoginScreen();
	} while (flag);
    
}
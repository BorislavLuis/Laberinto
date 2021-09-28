#include "joystick.h"

Joystick::Joystick(int i)
{
	id = getId(i);

	update();
}

void Joystick::update()
{
	present = glfwJoystickPresent(id);

	if (present)
	{
		name = glfwGetJoystickName(id);
		axis = glfwGetJoystickAxes(id, &axisCount);
		buttons = glfwGetJoystickButtons(id, &buttonCount);

	}
}

float Joystick::axisState(int axis)
{
	if (present)
	{
		return this->axis[axis];
	}
	return -1;
}

unsigned char Joystick::buttonState(int button)
{
	return present ? buttons[button]:GLFW_RELEASE;
}

int Joystick::getAxisCount()
{
	return axisCount;
}

int Joystick::getButtonCount()
{
	return buttonCount;
}

bool Joystick::isPresent()
{
	return present;
}

const char* Joystick::getName()
{
	return name;
}

int Joystick::getId(int i)
{
	return GLFW_JOYSTICK_1 + i;
}

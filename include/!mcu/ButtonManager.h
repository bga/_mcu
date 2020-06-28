#pragma once

namespace ButtonManager {
struct Base {
	virtual void onDown() {
		abstract;
	}
	virtual void onUp() {
		abstract;
	}
	virtual Bool isHold() {
		abstract;
		return no;
	}
	virtual void timerThread() {
		abstract;
	}
};

struct Press: Base {
	virtual void onPress() {
		abstract;
	}

	virtual void onDown() {
		this->onPress();
	}
};

struct Toggle: Base {
	virtual void onToggle() override  {
		abstract;
	}

	Bool isPressed;
	virtual void onDown() override  {
		this->onToggle();
	}
};

#if 0
struct Config {
	enum {
		multiPressDelay = msToTicksCount(2000),
		multiPressStep = msToTicksCount(500)
	}
};
#endif
template<typename ConfigArg> struct PressDelayPressRepeat: Base {
	typedef ConfigArg Config;
	Config config;

	virtual void onPress() {
		abstract;
	}

	TicksCount pressStartTicksCount;
	Bool isPressed;

	PressDelayPressRepeat() {
		this->isPressed = no;
	}
	virtual Bool isHold() override  {
		return this->isPressed;
	}

	virtual void onUp() override  {
		this->isPressed = no;
	}
	virtual void onDown() override  {
		this->pressStartTicksCount = getTicksCount() + config.multiPressDelay - config.multiPressStep;
		this->isPressed = yes;
		this->onPress();
	}

	virtual void timerThread() override  {
		if(this->isPressed == no) {
		}
		else {
			if(config.multiPressStep < (getTicksCount() - this->pressStartTicksCount)) {
				this->pressStartTicksCount += config.multiPressStep;
				this->onPress();
			}
		}
	}
};

#if 0
#if 0
struct Config {
	enum {
		multiPressDelay = msToTicksCount(2000),
		multiPressStep = msToTicksCount(500)
	}
};
#endif
template<typename ConfigArg> struct PressDelayMultiPress: Base {
	typedef ConfigArg Config;
	Config config;

	virtual void onPress() {
		abstract;
	}

	TicksCount pressStartTicksCount;
	enum State {
		State_released = 0,
		State_pressed  = 1,
		State_multiPressed = 2
	};

	enum State state = State_released;

	virtual void onUp() override  {
		this->state = State_released;
	}
	virtual void onDown() override  {
		this->pressStartTicksCount = getTicksCount() + config.multiPressDelay;
		this->state = State_pressed;
		this->onPress();
	}

	virtual void timerThread() override  {
		if(this->state == State_released) {
		}
		else {
			if(this->state < State_multiPressed && 0 < (getTicksCount() - this->pressStartTicksCount)) {
				this->state += 1;
				this->pressStartTicksCount += config.multiPressStep;
				this->onPress();
			}
		}
	}
};
#endif

#if 0
struct Config {
	enum {
		longPressDelay = msToTicksCount(2000)
	}
};
#endif
template<typename ConfigArg> struct PressDelayLongPress: Base {
	typedef ConfigArg Config;
	Config config;

	virtual void onPress() {
		abstract;
	}
	virtual void onLongPress() {
		abstract;
	}

	TicksCount pressStartTicksCount;
	enum State {
		State_released = 0,
		State_pressed  = 1,
		State_longPressed = 2
	};

	unsigned int state;

	virtual Bool isHold() override  {
		return this->state != State_released;
	}

	PressDelayLongPress() {
		this->state = State_released;
	}

	virtual void onUp() override  {
		this->state = State_released;
	}
	virtual void onDown() override  {
		this->pressStartTicksCount = getTicksCount();
		this->state = State_pressed;
		this->onPress();
	}

	virtual void timerThread() override  {
		if(this->state == State_released) {
		}
		else {
			if(this->state < State_longPressed && config.longPressDelay < (getTicksCount() - this->pressStartTicksCount)) {
				this->state += 1;
				this->onLongPress();
			}
		}
	}
};
} //# namespace ButtonManager



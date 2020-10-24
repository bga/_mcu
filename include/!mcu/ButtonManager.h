#pragma once

/*
  Copyright 2020 Bga <bga.email@gmail.com>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

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
		if(this->state == State_pressed) {
			this->state = State_released;
			this->onPress();
		}
		else {
			this->state = State_released;
		}
	}
	virtual void onDown() override  {
		this->pressStartTicksCount = getTicksCount();
		this->state = State_pressed;
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



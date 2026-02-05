#ifndef EVENT_LISTENER_HPP
#define EVENT_LISTENER_HPP

#ifndef __EMSCRIPTEN__
	#include <SFML/Graphics.hpp>
#else
	#include "compat/event.hpp"
#endif

class iEventListener {
	
public:
	
	virtual bool handle_event(const sf::Event&) = 0;
	
	virtual ~iEventListener() {}
	
};

#endif

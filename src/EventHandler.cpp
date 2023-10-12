#include "WebSocketService.h"

WebSocketService::EventHandler::EventHandler(
	const char *eventKey,
	MessageCallback callback,
	const char *channel,
	IdentityCallback identityCallback
): eventKey(eventKey),
	callback(callback),
	registered(true),
	channel(channel),
	identityCallback(identityCallback)
{};

WebSocketService::EventHandler::EventHandler(
	const EventHandler &other
) {
	this->eventKey = other.eventKey;
	this->callback = other.callback;
	this->registered = other.registered;
	this->channel = other.channel;
	this->identityCallback = other.identityCallback;
}

WebSocketService::EventHandler &WebSocketService::EventHandler::operator=(
	const WebSocketService::EventHandler &other
) {
	this->eventKey = other.eventKey;
	this->callback = other.callback;
	this->registered = other.registered;
	this->channel = other.channel;
	this->identityCallback = other.identityCallback;
	return *this;
}

bool WebSocketService::EventHandler::identify(
	const char *identity
) const {
	if (this->identityCallback != NULL) {
		return this->identityCallback(identity);
	}
	return true;
}

void WebSocketService::EventHandler::execute(
	const String &message
) {
	this->callback(message);
}

void WebSocketService::EventHandler::setCallback(
	MessageCallback callback
) {
	this->callback = callback;
}

bool WebSocketService::EventHandler::isEnabled() const {
	return this->registered;
}

bool WebSocketService::EventHandler::filter(
	const char *channel
) const {
	if (this->channel == NULL) {
		return true;
	}
	Serial.println("Filtering");
	return strcmp(this->channel, channel) == 0;
}